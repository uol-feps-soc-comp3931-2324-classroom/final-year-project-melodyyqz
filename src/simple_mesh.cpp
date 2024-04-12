
#define TASKING_TBB
#define __SSE2__

#include "simple_mesh.hpp"
#include <rapidobj/rapidobj.hpp>
#include <stb_image.h>
#include "../thirdparty/embree/include/embree4/rtcore.h"
#include <iostream>


SimpleMeshData load_wavefront_obj(char const* aPath)
{
	auto result = rapidobj::ParseFile(aPath);
	if (result.error)
	{
		std::cout << result.error.code.message() << '\n';
		// do some kind of return here
	}
	rapidobj::Triangulate(result);
	SimpleMeshData ret;

	auto num_triangles = size_t();

	for (const auto& shape : result.shapes) {
		num_triangles += shape.mesh.num_face_vertices.size();
	}

	std::cout << "Shapes:    " << result.shapes.size() << '\n';
	std::cout << "Triangles: " << num_triangles << '\n';
	std::cout << "Normals:   " << result.attributes.normals.size() / 3 << '\n' << std::endl;

	// Initialize normals to zero
	std::vector<Vec3f> normals(result.attributes.positions.size(), Vec3f{ 0.0f, 0.0f, 0.0f });
	
	for (auto const& shape : result.shapes) {
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];

			// store vertex indices
			ret.indices.push_back((GLuint)(idx.position_index));
			//printf("index: %d\n", idx.position_index);

			ret.positions.emplace_back(Vec3f{
				result.attributes.positions[i * 3 + 0],
				result.attributes.positions[i * 3 + 1],
				result.attributes.positions[i * 3 + 2]
				});

			// load normals (adjust to be 0-based)
			ret.normals.emplace_back(Vec3f{
				result.attributes.normals[(idx.normal_index) * 3 + 0],
				result.attributes.normals[(idx.normal_index) * 3 + 1],
				result.attributes.normals[(idx.normal_index) * 3 + 2]
				});

			//load texcoords
			/*ret.texcoords.emplace_back(Vec2f{
				result.attributes.texcoords[idx.texcoord_index * 2 + 0],
				result.attributes.texcoords[idx.texcoord_index * 2 + 1]
				});*/

			// load indices
			//ret.indices.push_back(ret.indices.size() - 1);
		}
	}
	printf("read in %d indices\n", ret.indices.size());
	printf("read in %d positions\n", ret.positions.size());
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



