#ifndef __ModelSet_h__
#define __ModelSet_h__

#include "Shape/Shape.h"
#include "Shape/RattlerLoad.h"
#include "Shape/Triangle.h"
#include "Shape/plyRead.h"

#include "Material/MaterialSet.h"

namespace Feimos
{

	inline void getDiffuseCornellBox(const Transform &tri_ConBox2World, std::vector<std::shared_ptr<Feimos::Primitive>> &prims, const MediumInterface &mediumInterface)
	{

		Feimos::Spectrum whiteColor;
		whiteColor[0] = 0.91;
		whiteColor[1] = 0.91;
		whiteColor[2] = 0.91;
		Feimos::Spectrum redWallColor;
		redWallColor[0] = 0.9;
		redWallColor[1] = 0.1;
		redWallColor[2] = 0.17;
		Feimos::Spectrum blueWallColor;
		blueWallColor[0] = 0.14;
		blueWallColor[1] = 0.21;
		blueWallColor[2] = 0.87;
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdWhite = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(whiteColor);
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdRed = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(redWallColor);
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdBlue = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(blueWallColor);

		std::shared_ptr<Feimos::Material> whiteWallMaterial = getMatteMaterial(KdWhite);
		std::shared_ptr<Feimos::Material> redWallMaterial = getMatteMaterial(KdRed);
		std::shared_ptr<Feimos::Material> blueWallMaterial = getMatteMaterial(KdBlue);

		// 墙和地板
		const int nTrianglesWall = 2 * 5;
		int vertexIndicesWall[nTrianglesWall * 3];
		for (int i = 0; i < nTrianglesWall * 3; i++)
			vertexIndicesWall[i] = i;
		const int nVerticesWall = nTrianglesWall * 3;
		const float length_Wall = 1.0f;
		Feimos::Point3f P_Wall[nVerticesWall] = {
			// 底座
			Feimos::Point3f(0.f, 0.f, length_Wall), Feimos::Point3f(length_Wall, 0.f, length_Wall), Feimos::Point3f(0.f, 0.f, 0.f),
			Feimos::Point3f(length_Wall, 0.f, length_Wall), Feimos::Point3f(length_Wall, 0.f, 0.f), Feimos::Point3f(0.f, 0.f, 0.f),
			// 天花板
			Feimos::Point3f(0.f, length_Wall, length_Wall), Feimos::Point3f(0.f, length_Wall, 0.f), Feimos::Point3f(length_Wall, length_Wall, length_Wall),
			Feimos::Point3f(length_Wall, length_Wall, length_Wall), Feimos::Point3f(0.f, length_Wall, 0.f), Feimos::Point3f(length_Wall, length_Wall, 0.f),
			// 后墙
			Feimos::Point3f(0.f, 0.f, 0.f), Feimos::Point3f(length_Wall, 0.f, 0.f), Feimos::Point3f(length_Wall, length_Wall, 0.f),
			Feimos::Point3f(0.f, 0.f, 0.f), Feimos::Point3f(length_Wall, length_Wall, 0.f), Feimos::Point3f(0.f, length_Wall, 0.f),
			// 右墙
			Feimos::Point3f(0.f, 0.f, 0.f), Feimos::Point3f(0.f, length_Wall, length_Wall), Feimos::Point3f(0.f, 0.f, length_Wall),
			Feimos::Point3f(0.f, 0.f, 0.f), Feimos::Point3f(0.f, length_Wall, 0.f), Feimos::Point3f(0.f, length_Wall, length_Wall),
			// 左墙
			Feimos::Point3f(length_Wall, 0.f, 0.f), Feimos::Point3f(length_Wall, length_Wall, length_Wall), Feimos::Point3f(length_Wall, 0.f, length_Wall),
			Feimos::Point3f(length_Wall, 0.f, 0.f), Feimos::Point3f(length_Wall, length_Wall, 0.f), Feimos::Point3f(length_Wall, length_Wall, length_Wall)};
		Feimos::Transform tri_World2ConBox = Feimos::Inverse(tri_ConBox2World);
		std::shared_ptr<Feimos::TriangleMesh> meshConBox = std::make_shared<Feimos::TriangleMesh>(tri_ConBox2World, nTrianglesWall, vertexIndicesWall, nVerticesWall, P_Wall, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
		std::vector<std::shared_ptr<Feimos::Shape>> trisConBox;
		for (int i = 0; i < nTrianglesWall; ++i)
			trisConBox.push_back(std::make_shared<Feimos::Triangle>(&tri_ConBox2World, &tri_World2ConBox, false, meshConBox, i));

		// 将物体填充到基元
		for (int i = 0; i < nTrianglesWall; ++i)
		{
			if (i == 6 || i == 7)
				prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisConBox[i], redWallMaterial, nullptr, mediumInterface));
			else if (i == 8 || i == 9)
				prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisConBox[i], blueWallMaterial, nullptr, mediumInterface));
			else
				prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisConBox[i], whiteWallMaterial, nullptr, mediumInterface));
		}
	}

	inline void getDiffuseFloor(const Transform &tri_Floor2World, const float length_Wall, std::shared_ptr<Feimos::Material> material, std::vector<std::shared_ptr<Feimos::Primitive>> &prims, const MediumInterface &mediumInterface)
	{
		// 地板
		const int nTrianglesWall = 2;
		int vertexIndicesWall[nTrianglesWall * 3];
		for (int i = 0; i < nTrianglesWall * 3; i++)
			vertexIndicesWall[i] = i;
		const int nVerticesWall = nTrianglesWall * 3;
		Feimos::Point3f P_Wall[nVerticesWall] = {
			Feimos::Point3f(0.f, 0.f, length_Wall),
			Feimos::Point3f(length_Wall, 0.f, length_Wall),
			Feimos::Point3f(0.f, 0.f, 0.f),
			Feimos::Point3f(length_Wall, 0.f, length_Wall),
			Feimos::Point3f(length_Wall, 0.f, 0.f),
			Feimos::Point3f(0.f, 0.f, 0.f),
		};

		Feimos::Point2f UV[nVerticesWall] = {
			Feimos::Point2f(0.f, 1.f), Feimos::Point2f(1.f, 1.f), Feimos::Point2f(0.f, 0.f),
			Feimos::Point2f(1.f, 1.f), Feimos::Point2f(1.f, 0.f), Feimos::Point2f(0.f, 0.f)};
		Feimos::Transform tri_World2Floor = Feimos::Inverse(tri_Floor2World);
		std::shared_ptr<Feimos::TriangleMesh> meshFloor = std::make_shared<Feimos::TriangleMesh>(tri_Floor2World, nTrianglesWall, vertexIndicesWall, nVerticesWall, P_Wall, nullptr, nullptr, UV, nullptr, nullptr, nullptr);
		std::vector<std::shared_ptr<Feimos::Shape>> trisFloor;
		for (int i = 0; i < nTrianglesWall; ++i)
			trisFloor.push_back(std::make_shared<Feimos::Triangle>(&tri_Floor2World, &tri_World2Floor, false, meshFloor, i));

		// 将物体填充到基元
		for (int i = 0; i < nTrianglesWall; ++i)
		{
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisFloor[i], material, nullptr, mediumInterface));
		}
	}

	inline void getDragon(Transform tri_Object2World, std::shared_ptr<Feimos::Material> material, std::vector<std::shared_ptr<Feimos::Primitive>> &prims, const MediumInterface &mediumInterface)
	{
		std::shared_ptr<Feimos::TriangleMesh> mesh;
		std::vector<std::shared_ptr<Feimos::Shape>> tris;

		Feimos::Transform tri_World2Object = Inverse(tri_Object2World);

		Feimos::plyInfo plyi("../../Resources/dragon.3d");
		mesh = std::make_shared<Feimos::TriangleMesh>(tri_Object2World, plyi.nTriangles, plyi.vertexIndices, plyi.nVertices, plyi.vertexArray, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
		tris.reserve(plyi.nTriangles);

		for (int i = 0; i < plyi.nTriangles; ++i)
			tris.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World, &tri_World2Object, false, mesh, i));

		for (int i = 0; i < plyi.nTriangles; ++i)
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(tris[i], material, nullptr, mediumInterface));
		plyi.Release();
	}

	inline void getMovingDragon(AnimatedTransform animatedTrans, Transform tri_Object2World, std::shared_ptr<Feimos::Material> material, std::vector<std::shared_ptr<Feimos::Primitive>> &prims, const MediumInterface &mediumInterface)
	{
		std::shared_ptr<Feimos::TriangleMesh> mesh;
		std::vector<std::shared_ptr<Feimos::Shape>> tris;

		Feimos::Transform tri_World2Object = Inverse(tri_Object2World);

		Feimos::plyInfo plyi("../../Resources/dragon.3d");
		mesh = std::make_shared<Feimos::TriangleMesh>(tri_Object2World, plyi.nTriangles, plyi.vertexIndices, plyi.nVertices, plyi.vertexArray, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr);
		tris.reserve(plyi.nTriangles);

		for (int i = 0; i < plyi.nTriangles; ++i)
			tris.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World, &tri_World2Object, false, mesh, i));

		for (int i = 0; i < plyi.nTriangles; ++i)
		{
			std::shared_ptr<Primitive> geoPrim = std::make_shared<Feimos::GeometricPrimitive>(tris[i], material, nullptr, mediumInterface);
			std::shared_ptr<TransformedPrimitive> aniPrim = std::make_shared<TransformedPrimitive>(geoPrim, animatedTrans);
			prims.push_back(aniPrim);
		}

		plyi.Release();
	}

	// 纹理和顶点对应关系可能存在问题
	inline void getBox(Feimos::Transform &tri_Object2World, float xlength, float ylength, float zlength,
					   std::vector<std::shared_ptr<Feimos::Primitive>> &prims, const std::shared_ptr<Feimos::Material> &mat, const Feimos::MediumInterface &mediumInterface)
	{

		// 墙和地板
		const int nTrianglesBox = 2 * 6;
		int vertexIndicesWall[nTrianglesBox * 3];
		for (int i = 0; i < nTrianglesBox * 3; i++)
			vertexIndicesWall[i] = i;
		const int nVerticesBox = nTrianglesBox * 3;
		float halfX = 0.5 * xlength, halfY = 0.5 * ylength, halfZ = 0.5 * zlength;
		Feimos::Point3f P_box[nVerticesBox] = {
			// 底板
			Feimos::Point3f(-halfX, -halfY, halfZ), Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(halfX, -halfY, halfZ),
			Feimos::Point3f(halfX, -halfY, halfZ), Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(halfX, -halfY, -halfZ),
			// 顶板
			Feimos::Point3f(-halfX, halfY, halfZ), Feimos::Point3f(halfX, halfY, halfZ), Feimos::Point3f(-halfX, halfY, -halfZ),
			Feimos::Point3f(halfX, halfY, halfZ), Feimos::Point3f(halfX, halfY, -halfZ), Feimos::Point3f(-halfX, halfY, -halfZ),
			// 后板
			Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(halfX, halfY, -halfZ), Feimos::Point3f(halfX, -halfY, -halfZ),
			Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(-halfX, halfY, -halfZ), Feimos::Point3f(halfX, halfY, -halfZ),
			// 前板
			Feimos::Point3f(-halfX, -halfY, halfZ), Feimos::Point3f(halfX, -halfY, halfZ), Feimos::Point3f(halfX, halfY, halfZ),
			Feimos::Point3f(-halfX, -halfY, halfZ), Feimos::Point3f(halfX, halfY, halfZ), Feimos::Point3f(-halfX, halfY, halfZ),
			// 右板
			Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(-halfX, -halfY, halfZ), Feimos::Point3f(-halfX, halfY, halfZ),
			Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(-halfX, halfY, halfZ), Feimos::Point3f(-halfX, halfY, -halfZ),
			// 左板
			Feimos::Point3f(halfX, -halfY, -halfZ), Feimos::Point3f(halfX, halfY, halfZ), Feimos::Point3f(halfX, -halfY, halfZ),
			Feimos::Point3f(halfX, -halfY, -halfZ), Feimos::Point3f(halfX, halfY, -halfZ), Feimos::Point3f(halfX, halfY, halfZ)};
		const float uv_l = 10.f;
		Feimos::Point2f UV_box[nVerticesBox] = {
			// 底板
			Feimos::Point2f(0.f, uv_l), Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l),
			Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, 0.f),
			// 顶板
			Feimos::Point2f(0.f, uv_l), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(0.f, 0.f),
			Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(uv_l, 0.f), Feimos::Point2f(0.f, 0.f),
			// 后板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(uv_l, 0.f),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(0.f, uv_l), Feimos::Point2f(uv_l, uv_l),
			// 前板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, 0.f), Feimos::Point2f(uv_l, uv_l),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(0.f, uv_l),
			// 右板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(0.f, uv_l),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, 0.f), Feimos::Point2f(uv_l, uv_l),
			// 左板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(0.f, uv_l), Feimos::Point2f(uv_l, uv_l),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(uv_l, 0.f)

		};

		std::shared_ptr<Feimos::TriangleMesh> meshBox = std::make_shared<Feimos::TriangleMesh>(tri_Object2World, nTrianglesBox, vertexIndicesWall, nVerticesBox, P_box, nullptr, nullptr, UV_box, nullptr, nullptr, nullptr);

		Feimos::Transform tri_World2Object = Inverse(tri_Object2World);
		std::vector<std::shared_ptr<Feimos::Shape>> trisBox;
		for (int i = 0; i < 12; ++i)
			trisBox.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World, &tri_World2Object, false, meshBox, i));

		for (int i = 0; i < trisBox.size(); ++i)
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisBox[i], mat, nullptr, mediumInterface));
	}

	// 纹理和顶点对应关系可能存在问题
	inline void getMovingBox(AnimatedTransform animatedTrans, Feimos::Transform &tri_Object2World, float xlength, float ylength, float zlength,
							 std::vector<std::shared_ptr<Feimos::Primitive>> &prims, const std::shared_ptr<Feimos::Material> &mat, const Feimos::MediumInterface &mediumInterface)
	{

		// 墙和地板
		const int nTrianglesBox = 2 * 6;
		int vertexIndicesWall[nTrianglesBox * 3];
		for (int i = 0; i < nTrianglesBox * 3; i++)
			vertexIndicesWall[i] = i;
		const int nVerticesBox = nTrianglesBox * 3;
		float halfX = 0.5 * xlength, halfY = 0.5 * ylength, halfZ = 0.5 * zlength;
		Feimos::Point3f P_box[nVerticesBox] = {
			// 底板
			Feimos::Point3f(-halfX, -halfY, halfZ), Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(halfX, -halfY, halfZ),
			Feimos::Point3f(halfX, -halfY, halfZ), Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(halfX, -halfY, -halfZ),
			// 顶板
			Feimos::Point3f(-halfX, halfY, halfZ), Feimos::Point3f(halfX, halfY, halfZ), Feimos::Point3f(-halfX, halfY, -halfZ),
			Feimos::Point3f(halfX, halfY, halfZ), Feimos::Point3f(halfX, halfY, -halfZ), Feimos::Point3f(-halfX, halfY, -halfZ),
			// 后板
			Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(halfX, halfY, -halfZ), Feimos::Point3f(halfX, -halfY, -halfZ),
			Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(-halfX, halfY, -halfZ), Feimos::Point3f(halfX, halfY, -halfZ),
			// 前板
			Feimos::Point3f(-halfX, -halfY, halfZ), Feimos::Point3f(halfX, -halfY, halfZ), Feimos::Point3f(halfX, halfY, halfZ),
			Feimos::Point3f(-halfX, -halfY, halfZ), Feimos::Point3f(halfX, halfY, halfZ), Feimos::Point3f(-halfX, halfY, halfZ),
			// 右板
			Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(-halfX, -halfY, halfZ), Feimos::Point3f(-halfX, halfY, halfZ),
			Feimos::Point3f(-halfX, -halfY, -halfZ), Feimos::Point3f(-halfX, halfY, halfZ), Feimos::Point3f(-halfX, halfY, -halfZ),
			// 左板
			Feimos::Point3f(halfX, -halfY, -halfZ), Feimos::Point3f(halfX, halfY, halfZ), Feimos::Point3f(halfX, -halfY, halfZ),
			Feimos::Point3f(halfX, -halfY, -halfZ), Feimos::Point3f(halfX, halfY, -halfZ), Feimos::Point3f(halfX, halfY, halfZ)};
		const float uv_l = 10.f;
		Feimos::Point2f UV_box[nVerticesBox] = {
			// 底板
			Feimos::Point2f(0.f, uv_l), Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l),
			Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, 0.f),
			// 顶板
			Feimos::Point2f(0.f, uv_l), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(0.f, 0.f),
			Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(uv_l, 0.f), Feimos::Point2f(0.f, 0.f),
			// 后板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(uv_l, 0.f),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(0.f, uv_l), Feimos::Point2f(uv_l, uv_l),
			// 前板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, 0.f), Feimos::Point2f(uv_l, uv_l),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(0.f, uv_l),
			// 右板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(0.f, uv_l),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, 0.f), Feimos::Point2f(uv_l, uv_l),
			// 左板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(0.f, uv_l), Feimos::Point2f(uv_l, uv_l),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(uv_l, 0.f)

		};

		std::shared_ptr<Feimos::TriangleMesh> meshBox = std::make_shared<Feimos::TriangleMesh>(tri_Object2World, nTrianglesBox, vertexIndicesWall, nVerticesBox, P_box, nullptr, nullptr, UV_box, nullptr, nullptr, nullptr);

		Feimos::Transform tri_World2Object = Inverse(tri_Object2World);
		std::vector<std::shared_ptr<Feimos::Shape>> trisBox;
		for (int i = 0; i < 12; ++i)
			trisBox.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World, &tri_World2Object, false, meshBox, i));

		std::vector<std::shared_ptr<Feimos::Primitive>> primsObj;
		for (int i = 0; i < trisBox.size(); ++i)
		{
			std::shared_ptr<Primitive> geoPrim = std::make_shared<Feimos::GeometricPrimitive>(trisBox[i], mat, nullptr, mediumInterface);
			std::shared_ptr<TransformedPrimitive> aniPrim = std::make_shared<TransformedPrimitive>(geoPrim, animatedTrans);
			primsObj.push_back(aniPrim);
		}
		std::shared_ptr<Feimos::Aggregate> aggregate = std::make_unique<Feimos::BVHAccel>(primsObj, 1);
		prims.push_back(aggregate);
	}

	// 纹理和顶点对应关系可能存在问题
	inline void getMediumBox(Feimos::Transform &tri_Object2World, float pMin[3], float pMax[3],
							 std::vector<std::shared_ptr<Feimos::Primitive>> &prims, const std::shared_ptr<Feimos::Material> &mat, const Feimos::MediumInterface &mediumInterface)
	{

		// 墙和地板
		const int nTrianglesBox = 2 * 6;
		int vertexIndicesWall[nTrianglesBox * 3];
		for (int i = 0; i < nTrianglesBox * 3; i++)
			vertexIndicesWall[i] = i;
		const int nVerticesBox = nTrianglesBox * 3;
		Feimos::Point3f P_box[nVerticesBox] = {
			// 底板
			Feimos::Point3f(pMin[0], pMin[1], pMax[2]), Feimos::Point3f(pMin[0], pMin[1], pMin[2]), Feimos::Point3f(pMax[0], pMin[1], pMax[2]),
			Feimos::Point3f(pMax[0], pMin[1], pMax[2]), Feimos::Point3f(pMin[0], pMin[1], pMin[2]), Feimos::Point3f(pMax[0], pMin[1], pMin[2]),
			// 顶板
			Feimos::Point3f(pMin[0], pMax[1], pMax[2]), Feimos::Point3f(pMax[0], pMax[1], pMax[2]), Feimos::Point3f(pMin[0], pMax[1], pMin[2]),
			Feimos::Point3f(pMax[0], pMax[1], pMax[2]), Feimos::Point3f(pMax[0], pMax[1], pMin[2]), Feimos::Point3f(pMin[0], pMax[1], pMin[2]),
			// 后板
			Feimos::Point3f(pMin[0], pMin[1], pMin[2]), Feimos::Point3f(pMax[0], pMax[1], pMin[2]), Feimos::Point3f(pMax[0], pMin[1], pMin[2]),
			Feimos::Point3f(pMin[0], pMin[1], pMin[2]), Feimos::Point3f(pMin[0], pMax[1], pMin[2]), Feimos::Point3f(pMax[0], pMax[1], pMin[2]),
			// 前板
			Feimos::Point3f(pMin[0], pMin[1], pMax[2]), Feimos::Point3f(pMax[0], pMin[1], pMax[2]), Feimos::Point3f(pMax[0], pMax[1], pMax[2]),
			Feimos::Point3f(pMin[0], pMin[1], pMax[2]), Feimos::Point3f(pMax[0], pMax[1], pMax[2]), Feimos::Point3f(pMin[0], pMax[1], pMax[2]),
			// 右板
			Feimos::Point3f(pMin[0], pMin[1], pMin[2]), Feimos::Point3f(pMin[0], pMin[1], pMax[2]), Feimos::Point3f(pMin[0], pMax[1], pMax[2]),
			Feimos::Point3f(pMin[0], pMin[1], pMin[2]), Feimos::Point3f(pMin[0], pMax[1], pMax[2]), Feimos::Point3f(pMin[0], pMax[1], pMin[2]),
			// 左板
			Feimos::Point3f(pMax[0], pMin[1], pMin[2]), Feimos::Point3f(pMax[0], pMax[1], pMax[2]), Feimos::Point3f(pMax[0], pMin[1], pMax[2]),
			Feimos::Point3f(pMax[0], pMin[1], pMin[2]), Feimos::Point3f(pMax[0], pMax[1], pMin[2]), Feimos::Point3f(pMax[0], pMax[1], pMax[2])};
		const float uv_l = 10.f;
		Feimos::Point2f UV_box[nVerticesBox] = {
			// 底板
			Feimos::Point2f(0.f, uv_l), Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l),
			Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, 0.f),
			// 顶板
			Feimos::Point2f(0.f, uv_l), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(0.f, 0.f),
			Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(uv_l, 0.f), Feimos::Point2f(0.f, 0.f),
			// 后板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(uv_l, 0.f),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(0.f, uv_l), Feimos::Point2f(uv_l, uv_l),
			// 前板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, 0.f), Feimos::Point2f(uv_l, uv_l),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(0.f, uv_l),
			// 右板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(0.f, uv_l), Feimos::Point2f(uv_l, uv_l),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(uv_l, 0.f),
			// 左板
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(0.f, uv_l), Feimos::Point2f(uv_l, uv_l),
			Feimos::Point2f(0.f, 0.f), Feimos::Point2f(uv_l, uv_l), Feimos::Point2f(uv_l, 0.f)

		};

		std::shared_ptr<Feimos::TriangleMesh> meshBox = std::make_shared<Feimos::TriangleMesh>(tri_Object2World, nTrianglesBox, vertexIndicesWall, nVerticesBox, P_box, nullptr, nullptr, UV_box, nullptr, nullptr, nullptr);

		Feimos::Transform tri_World2Object = Inverse(tri_Object2World);
		std::vector<std::shared_ptr<Feimos::Shape>> trisBox;
		for (int i = 0; i < 12; ++i)
			trisBox.push_back(std::make_shared<Feimos::Triangle>(&tri_Object2World, &tri_World2Object, false, meshBox, i));

		for (int i = 0; i < trisBox.size(); ++i)
			prims.push_back(std::make_shared<Feimos::GeometricPrimitive>(trisBox[i], mat, nullptr, mediumInterface));
	}

}

#endif
