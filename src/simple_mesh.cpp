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

	// Initialize normals to zero
	std::vector<Vec3f> normals(result.attributes.positions.size(), Vec3f{ 0.0f, 0.0f, 0.0f });
	
	for (auto const& shape : result.shapes) {
		for (std::size_t i = 0; i < shape.mesh.indices.size(); ++i)
		{
			auto const& idx = shape.mesh.indices[i];

			ret.positions.emplace_back(Vec3f{
				result.attributes.positions[idx.position_index * 3 + 0],
				result.attributes.positions[idx.position_index * 3 + 1],
				result.attributes.positions[idx.position_index * 3 + 2]
				});

			auto const& mat = result.materials[shape.mesh.material_ids[i / 3]];
			ret.colors.emplace_back(Vec3f{
				mat.ambient[0],
				mat.ambient[1],
				mat.ambient[2]
				});

			//load normals
			ret.normals.emplace_back(Vec3f{
				result.attributes.normals[idx.normal_index * 3 + 0],
				result.attributes.normals[idx.normal_index * 3 + 1],
				result.attributes.normals[idx.normal_index * 3 + 2]
				});

			//load texcoords
			/*ret.texcoords.emplace_back(Vec2f{
				result.attributes.texcoords[idx.texcoord_index * 2 + 0],
				result.attributes.texcoords[idx.texcoord_index * 2 + 1]
				});*/
		}
	}
	return ret;
}

SimpleMeshData concatenate(SimpleMeshData aM, SimpleMeshData const& aN)
{
	aM.positions.insert(aM.positions.end(), aN.positions.begin(), aN.positions.end());
	aM.colors.insert(aM.colors.end(), aN.colors.begin(), aN.colors.end());
	aM.normals.insert(aM.normals.end(), aN.normals.begin(), aN.normals.end());
	aM.texcoords.insert(aM.texcoords.end(), aN.texcoords.begin(), aN.texcoords.end());
	return aM;
}

namespace embree {

/* all features required by this tutorial */
#define FEATURE_MASK \
  RTC_FEATURE_FLAG_TRIANGLE

RTCScene g_scene = nullptr;
sceneData data;

/* adds a cube to the scene */
unsigned int addCube(RTCScene scene_i)
{
	/* create a triangulated cube with 12 triangles and 8 vertices */
	RTCGeometry mesh = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_TRIANGLE);

	/* create face and vertex color arrays */
	data.face_colors = (Vec3fa*)alignedUSMMalloc((12) * sizeof(Vec3fa), 16);
	data.vertex_colors = (Vec3fa*)alignedUSMMalloc((8) * sizeof(Vec3fa), 16);

	/* set vertices and vertex colors */
	Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), 8);
	data.vertex_colors[0] = Vec3fa(0, 0, 0); vertices[0].x = -1; vertices[0].y = -1; vertices[0].z = -1;
	data.vertex_colors[1] = Vec3fa(0, 0, 1); vertices[1].x = -1; vertices[1].y = -1; vertices[1].z = +1;
	data.vertex_colors[2] = Vec3fa(0, 1, 0); vertices[2].x = -1; vertices[2].y = +1; vertices[2].z = -1;
	data.vertex_colors[3] = Vec3fa(0, 1, 1); vertices[3].x = -1; vertices[3].y = +1; vertices[3].z = +1;
	data.vertex_colors[4] = Vec3fa(1, 0, 0); vertices[4].x = +1; vertices[4].y = -1; vertices[4].z = -1;
	data.vertex_colors[5] = Vec3fa(1, 0, 1); vertices[5].x = +1; vertices[5].y = -1; vertices[5].z = +1;
	data.vertex_colors[6] = Vec3fa(1, 1, 0); vertices[6].x = +1; vertices[6].y = +1; vertices[6].z = -1;
	data.vertex_colors[7] = Vec3fa(1, 1, 1); vertices[7].x = +1; vertices[7].y = +1; vertices[7].z = +1;

	/* set triangles and face colors */
	int tri = 0;
	Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), 12);

	// left side
	data.face_colors[tri] = Vec3fa(1, 0, 0); triangles[tri].v0 = 0; triangles[tri].v1 = 1; triangles[tri].v2 = 2; tri++;
	data.face_colors[tri] = Vec3fa(1, 0, 0); triangles[tri].v0 = 1; triangles[tri].v1 = 3; triangles[tri].v2 = 2; tri++;

	// right side
	data.face_colors[tri] = Vec3fa(0, 1, 0); triangles[tri].v0 = 4; triangles[tri].v1 = 6; triangles[tri].v2 = 5; tri++;
	data.face_colors[tri] = Vec3fa(0, 1, 0); triangles[tri].v0 = 5; triangles[tri].v1 = 6; triangles[tri].v2 = 7; tri++;

	// bottom side
	data.face_colors[tri] = Vec3fa(0.5f);  triangles[tri].v0 = 0; triangles[tri].v1 = 4; triangles[tri].v2 = 1; tri++;
	data.face_colors[tri] = Vec3fa(0.5f);  triangles[tri].v0 = 1; triangles[tri].v1 = 4; triangles[tri].v2 = 5; tri++;

	// top side
	data.face_colors[tri] = Vec3fa(1.0f);  triangles[tri].v0 = 2; triangles[tri].v1 = 3; triangles[tri].v2 = 6; tri++;
	data.face_colors[tri] = Vec3fa(1.0f);  triangles[tri].v0 = 3; triangles[tri].v1 = 7; triangles[tri].v2 = 6; tri++;

	// front side
	data.face_colors[tri] = Vec3fa(0, 0, 1); triangles[tri].v0 = 0; triangles[tri].v1 = 2; triangles[tri].v2 = 4; tri++;
	data.face_colors[tri] = Vec3fa(0, 0, 1); triangles[tri].v0 = 2; triangles[tri].v1 = 6; triangles[tri].v2 = 4; tri++;

	// back side
	data.face_colors[tri] = Vec3fa(1, 1, 0); triangles[tri].v0 = 1; triangles[tri].v1 = 5; triangles[tri].v2 = 3; tri++;
	data.face_colors[tri] = Vec3fa(1, 1, 0); triangles[tri].v0 = 3; triangles[tri].v1 = 5; triangles[tri].v2 = 7; tri++;

	rtcSetGeometryVertexAttributeCount(mesh, 1);
	rtcSetSharedGeometryBuffer(mesh, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, data.vertex_colors, 0, sizeof(Vec3fa), 8);

	rtcCommitGeometry(mesh);
	unsigned int geomID = rtcAttachGeometry(scene_i, mesh);
	rtcReleaseGeometry(mesh);
	return geomID;
}

/* adds a ground plane to the scene */
unsigned int addGroundPlane(RTCScene scene_i)
{
	/* create a triangulated plane with 2 triangles and 4 vertices */
	RTCGeometry mesh = rtcNewGeometry(g_device, RTC_GEOMETRY_TYPE_TRIANGLE);

	/* set vertices */
	Vertex* vertices = (Vertex*)rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(Vertex), 4);
	vertices[0].x = -10; vertices[0].y = -2; vertices[0].z = -10;
	vertices[1].x = -10; vertices[1].y = -2; vertices[1].z = +10;
	vertices[2].x = +10; vertices[2].y = -2; vertices[2].z = -10;
	vertices[3].x = +10; vertices[3].y = -2; vertices[3].z = +10;

	/* set triangles */
	Triangle* triangles = (Triangle*)rtcSetNewGeometryBuffer(mesh, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, sizeof(Triangle), 2);
	triangles[0].v0 = 0; triangles[0].v1 = 1; triangles[0].v2 = 2;
	triangles[1].v0 = 1; triangles[1].v1 = 3; triangles[1].v2 = 2;

	rtcCommitGeometry(mesh);
	unsigned int geomID = rtcAttachGeometry(scene_i, mesh);
	rtcReleaseGeometry(mesh);
	return geomID;
}
/* add armadillo mesh*/

/* renders single screen tile */
//void renderTileStandard(const RTCDevice device, const RTCScene scene, const RTCRayHit* __restrict__ rays, const unsigned int N, const unsigned int tileID, const unsigned int numTilesX, const unsigned int numTilesY)
//{
	/* iterate over all rays */
	//for (unsigned int i = 0; i < N; i++)
	//{
	//RTCRayHit ray = rays[i];
	//RTCIntersectContext context;
	//rtcInitIntersectContext(&context);

	/* intersect ray with scene */
	//rtcIntersect1(scene, &context, &ray);

	/* shade background */
	//if (ray.hit.geomID == RTC_INVALID_GEOMETRY_ID)
	//{
		/**/
	//}
	//else
	//{
		/* shade hit point */
		/**/
	//}
	//}

//}
extern "C" void device_init(char* cfg)
{
	sceneData_Constructor(&data);

}
void renderPixelStandard(const sceneData& data,
		int x, int y,
		int* pixels,
		const unsigned int width,
		const unsigned int height,
		const float time,
		const ISPCCamera& camera, RayStats& stats)
{
	// Initialize ray
	Ray ray(Vec3fa(camera.xfm.p), Vec3fa(normalize(x * camera.xfm.l.vx + y * camera.xfm.l.vy + camera.xfm.l.vz)), 0.0f, inf);
	/* intersect ray with scene */
	RTCIntersectArguments iargs;
	rtcInitIntersectArguments(&iargs);
	iargs.feature_mask = (RTCFeatureFlags)(FEATURE_MASK);
	rtcIntersect1(data.g_scene, RTCRayHit_(ray), &iargs);
	RayStats_addRay(stats);

	/* shade pixels */
	Vec3fa color = Vec3fa(0.0f);
	if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		Vec3fa diffuse = data.face_colors[ray.primID];
		color = color + diffuse * 0.5f;
		Vec3fa lightDir = normalize(Vec3fa(-1, -1, -1));

		/* initialize shadow ray */
		Ray shadow(ray.org + ray.tfar * ray.dir, neg(lightDir), 0.001f, inf, 0.0f);

		/* trace shadow ray */
		RTCOccludedArguments sargs;
		rtcInitOccludedArguments(&sargs);
		sargs.feature_mask = (RTCFeatureFlags)(FEATURE_MASK);
		rtcOccluded1(data.g_scene, RTCRay_(shadow), &sargs);
		RayStats_addShadowRay(stats);

		/* add light contribution */
		if (shadow.tfar >= 0.0f)
			color = color + diffuse * clamp(-dot(lightDir, normalize(ray.Ng)), 0.0f, 1.0f);
	}

	/* write color to framebuffer */
	unsigned int r = (unsigned int)(255.0f * clamp(color.x, 0.0f, 1.0f));
	unsigned int g = (unsigned int)(255.0f * clamp(color.y, 0.0f, 1.0f));
	unsigned int b = (unsigned int)(255.0f * clamp(color.z, 0.0f, 1.0f));
	pixels[y * width + x] = (b << 16) + (g << 8) + r;
}

} // namespace embree