
#define TASKING_TBB
#define __SSE2__

#include "simple_mesh.hpp"
#include <rapidobj/rapidobj.hpp>
#include <stb_image.h>
#include <embree3/rtcore.h>
#include <iostream>
#include <unordered_map>
#include <vector>

SimpleMeshData load_wavefront_obj(char const* aPath) {

    auto result = rapidobj::ParseFile(aPath);
    if (result.error) {
        std::cerr << "Error parsing OBJ file: " << result.error.code.message() << '\n';
        std::cerr << "Error at line: " << result.error.line << '\n';  // If available
       // std::cout << "Attempting to load MTL file from: " << resolvedPath << std::endl;
        return {};
    }

    rapidobj::Triangulate(result);
    SimpleMeshData ret;

    std::unordered_map<int, int> positionIndexMapping; // Maps original position indices to sequential indices
    std::unordered_map<int, int> normalIndexMapping;
    std::vector<Vec3f> sequentialPositions; // Stores vertex positions in the order they're encountered
    std::vector<Vec3f> sequentialNormals;

    for (const auto& shape : result.shapes) {
        //for (const auto& idx : shape.mesh.indices) {
        for (int i = 0; i < shape.mesh.indices.size(); i++) {
            int positionIndex = -1;
            const auto& idx = shape.mesh.indices[i];
            // Process position index
            if (positionIndexMapping.find(idx.position_index) == positionIndexMapping.end()) {
                positionIndex = sequentialPositions.size();
                positionIndexMapping[idx.position_index] = positionIndex;

                // Add the corresponding vertex position
                sequentialPositions.emplace_back(Vec3f{
                    result.attributes.positions[idx.position_index * 3 + 0],
                    result.attributes.positions[idx.position_index * 3 + 1],
                    result.attributes.positions[idx.position_index * 3 + 2]
                    });
            }
            else {
                //std::cout << "Index already processed: " << idx.position_index << std::endl;
                positionIndex = positionIndexMapping[idx.position_index];
            }

            // Process normal index, ensure it's associated with the correct vertex index
            if (idx.normal_index != -1) {
                if (normalIndexMapping.find(idx.normal_index) == normalIndexMapping.end()) {
                    normalIndexMapping[idx.normal_index] = positionIndex; // Use positionIndex to align normals
                    sequentialNormals.resize(sequentialPositions.size()); // Ensure normals array is large enough
                    sequentialNormals[positionIndex] = Vec3f{
                        result.attributes.normals[idx.normal_index * 3 + 0],
                        result.attributes.normals[idx.normal_index * 3 + 1],
                        result.attributes.normals[idx.normal_index * 3 + 2]
                    };
                }
            }

            // Always use the position index for indices vector
            ret.indices.push_back(positionIndex);
        }
    }

    ret.positions = sequentialPositions;
    ret.normals = sequentialNormals;

    std::cout << "Processed " << ret.indices.size() << " indices, "
        << ret.positions.size() << " positions, and "
        << ret.normals.size() << " normals.\n"; 
    return ret;
}

SimpleMeshData concatenate(SimpleMeshData aM, SimpleMeshData const& aN)
{
	aM.positions.insert(aM.positions.end(), aN.positions.begin(), aN.positions.end());
	aM.colors.insert(aM.colors.end(), aN.colors.begin(), aN.colors.end());
	aM.normals.insert(aM.normals.end(), aN.normals.begin(), aN.normals.end());
	aM.texcoords.insert(aM.texcoords.end(), aN.texcoords.begin(), aN.texcoords.end());
	aM.indices.insert(aM.indices.end(), aN.indices.begin(), aN.indices.end());
	return aM;
}



