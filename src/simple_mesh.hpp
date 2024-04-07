#ifndef SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9


// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#include "../thirdparty/embree/tutorials/common/tutorial/tutorial_device.h"
#include "../thirdparty/embree/common/math/vec3fa.h"
#include <vector>

#include "util/vmlib/vec3.hpp"
#include "util/vmlib/vec2.hpp"

struct SimpleMeshData
{
	std::vector<Vec3f> positions;
	std::vector<Vec3f> colors;
	std::vector<Vec3f> normals;
	std::vector<Vec2f> texcoords;
	//GLuint vao;
};

SimpleMeshData load_wavefront_obj(char const* aPath);
SimpleMeshData concatenate(SimpleMeshData, SimpleMeshData const&);

// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

namespace embree {

	struct sceneData
	{
		RTCScene g_scene;
		Vec3fa* face_colors;
		Vec3fa* vertex_colors;
	};

	inline void sceneData_Constructor(sceneData* This)
	{
		This->g_scene = nullptr;
		This->face_colors = nullptr;
		This->vertex_colors = nullptr;
	}

	inline void sceneData_Destructor(sceneData* This)
	{
		rtcReleaseScene(This->g_scene); This->g_scene = nullptr;
		alignedUSMFree(This->face_colors); This->face_colors = nullptr;
		alignedUSMFree(This->vertex_colors); This->vertex_colors = nullptr;
	}

} // namespace embree

#endif // SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9