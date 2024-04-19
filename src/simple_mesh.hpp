#ifndef SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9
#define SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9


// Copyright 2009-2021 Intel Corporation
// SPDX-License-Identifier: Apache-2.0
#include <vector>
#include <glad/glad.h>

#include "util/vmlib/vec3.hpp"
#include "util/vmlib/vec2.hpp"

struct SimpleMeshData
{
	std::vector<Vec3f> positions;
	std::vector<Vec3f> colors;
	std::vector<Vec3f> normals;
	std::vector<Vec2f> texcoords;
	std::vector<GLuint> indices;
};

SimpleMeshData load_wavefront_obj(char const* aPath);
SimpleMeshData concatenate(SimpleMeshData, SimpleMeshData const&);

#endif // SIMPLE_MESH_HPP_C6B749D6_C83B_434C_9E58_F05FC27FEFC9