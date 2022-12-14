#ifndef __LightSet_h__
#define __LightSet_h__

#include "Light/Light.h"
#include "Light/DiffuseLight.h"
#include "Light/InfiniteAreaLight.h"
#include "Light/SkyBoxLight.h"

namespace Feimos
{

	inline void getSkyBox(std::string filename,
						  std::vector<std::shared_ptr<Feimos::Light>> &lights,
						  Transform SkyBoxToWorld = Transform(),
						  float SkyBoxRadius = 10.0f, Point3f SkyBoxCenter = Point3f(0.f, 0.f, 0.f))
	{

		std::shared_ptr<Feimos::Light> skyBoxLight =
			std::make_shared<Feimos::SkyBoxLight>(SkyBoxToWorld, SkyBoxCenter, SkyBoxRadius, filename.c_str(), 1);
		lights.push_back(skyBoxLight);
	}

	inline void getInfiniteLight(std::string filename,
								 std::vector<std::shared_ptr<Feimos::Light>> &lights,
								 Transform InfinityLightToWorld = Transform(), Spectrum power = Spectrum(0.5f),
								 float SkyBoxRadius = 10.0f, Point3f SkyBoxCenter = Point3f(0.f, 0.f, 0.f))
	{

		std::shared_ptr<Feimos::Light> infinityLight =
			std::make_shared<Feimos::InfiniteAreaLight>(InfinityLightToWorld, power, 10, filename);
		lights.push_back(infinityLight);
	}

	inline void getAreaLight(
		Transform tri_Object2World_AreaLight,
		std::vector<std::shared_ptr<Feimos::Light>> &lights,
		std::vector<std::shared_ptr<Feimos::Primitive>> &prims,
		const MediumInterface &mediumInterface,
		std::shared_ptr<Feimos::Material> material, Spectrum power)
	{
		// 定义面光源
		int nTrianglesAreaLight = 2;						// 面光源数（三角形数）
		int vertexIndicesAreaLight[6] = {0, 1, 2, 3, 4, 5}; // 面光源顶点索引
		int nVerticesAreaLight = 6;							// 面光源顶点数
		const float yPos_AreaLight = 0.0;
		Feimos::Point3f P_AreaLight[6] = {Feimos::Point3f(-1.f, 0.0, 1.f), Feimos::Point3f(-1.f, 0.0, -1.f), Feimos::Point3f(1.f, 0.0, 1.f), Feimos::Point3f(1.f, 0.0, 1.f), Feimos::Point3f(-1.f, 0.0, -1.f), Feimos::Point3f(1.f, 0.0, -1.f)};
		// 面光源的变换矩阵
		Feimos::Transform tri_World2Object_AreaLight = Feimos::Inverse(tri_Object2World_AreaLight);
		// 构造三角面片集
		std::shared_ptr<Feimos::TriangleMesh> meshAreaLight = std::make_shared<Feimos::TriangleMesh>(tri_Object2World_AreaLight, nTrianglesAreaLight, vertexIndicesAreaLight, nVerticesAreaLight, P_AreaLight, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
		std::vector<std::shared_ptr<Feimos::Shape>> trisAreaLight;
		// 生成三角形数组
		for (int i = 0; i < nTrianglesAreaLight; ++i)
			trisAreaLight.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World_AreaLight, &tri_World2Object_AreaLight, false, meshAreaLight, i));
		// 填充光源类物体到基元
		for (int i = 0; i < nTrianglesAreaLight; ++i)
		{
			std::shared_ptr<Feimos::AreaLight> area =
				std::make_shared<Feimos::DiffuseAreaLight>(tri_Object2World_AreaLight, mediumInterface, power, 5, trisAreaLight[i], false);
			lights.push_back(area);
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisAreaLight[i], material, area, mediumInterface));
		}
	}

}

#endif
