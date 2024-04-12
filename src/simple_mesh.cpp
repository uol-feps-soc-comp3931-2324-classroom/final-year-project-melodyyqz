
#define TASKING_TBB
#define __SSE2__

#include "simple_mesh.hpp"
#include <rapidobj/rapidobj.hpp>
#include <stb_image.h>
#include "../thirdparty/embree/include/embree4/rtcore.h"
#include <iostream>
#include <unordered_map>
#include <vector>

SimpleMeshData load_wavefront_obj(char const* aPath) {
    auto result = rapidobj::ParseFile(aPath);
    if (result.error) {
        std::cout << result.error.code.message() << '\n';
        // Handle the error appropriately
        // For example, return an empty SimpleMeshData object or throw an exception
    }

    rapidobj::Triangulate(result);
    SimpleMeshData ret;

    std::unordered_map<int, int> positionIndexMapping; // Maps original position indices to sequential indices
    std::vector<Vec3f> sequentialPositions; // Stores vertex positions in the order they're encountered

    for (const auto& shape : result.shapes) {
        for (const auto& idx : shape.mesh.indices) {
            // Process position index
            if (positionIndexMapping.find(idx.position_index) == positionIndexMapping.end()) {
                // This index hasn't been seen before; add it to the mapping
                int newSequentialIndex = sequentialPositions.size();
                positionIndexMapping[idx.position_index] = newSequentialIndex;

                // Add the corresponding vertex position to sequentialPositions
                sequentialPositions.emplace_back(Vec3f{
                    result.attributes.positions[idx.position_index * 3 + 0],
                    result.attributes.positions[idx.position_index * 3 + 1],
                    result.attributes.positions[idx.position_index * 3 + 2]
                    });
            }

            // Use the new sequential index for this vertex in the indices vector
            ret.indices.push_back(positionIndexMapping[idx.position_index]);
        }
    }

    // After processing all indices, ret.positions should contain the reindexed positions
    ret.positions = sequentialPositions;

    // Continue to process normals and any other attributes similarly, if necessary

    std::cout << "Processed " << ret.indices.size() << " indices and " << ret.positions.size() << " positions.\n";
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



