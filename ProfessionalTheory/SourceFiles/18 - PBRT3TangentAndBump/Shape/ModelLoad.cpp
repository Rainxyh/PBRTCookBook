#include "Shape/ModelLoad.h"
#include "Core/Geometry.h"
#include "Core/Transform.h"
#include "Core/Primitive.h"
#include "Texture/ConstantTexture.h"
#include "Material/MatteMaterial.h"
#include "Material/PlasticMaterial.h"

namespace Feimos
{

	std::shared_ptr<TriangleMesh> ModelLoad::processMesh(aiMesh *mesh, const aiScene *scene, const Transform &ObjectToWorld)
	{
		long nVertices = mesh->mNumVertices;
		long nTriangles = mesh->mNumFaces;
		int *vertexIndices = new int[nTriangles * 3];
		Point3f *P = new Point3f[nVertices];
		Vector3f *S = nullptr; // new Vector3f[nVertices];
		Normal3f *N = new Normal3f[nVertices];
		Point2f *uv = new Point2f[nVertices];
		int *faceIndices = nullptr; // 第几个面的编号

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			// 顶点
			P[i].x = mesh->mVertices[i].x;
			P[i].y = mesh->mVertices[i].y;
			P[i].z = mesh->mVertices[i].z;
			// 法向量
			if (mesh->HasNormals())
			{
				N[i].x = mesh->mNormals[i].x;
				N[i].y = mesh->mNormals[i].y;
				N[i].z = mesh->mNormals[i].z;
			}
			// 纹理和切线向量
			if (mesh->mTextureCoords[0])
			{
				uv[i].x = mesh->mTextureCoords[0][i].x;
				uv[i].y = mesh->mTextureCoords[0][i].y;
			}
		}
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
				vertexIndices[3 * i + j] = face.mIndices[j];
		}
		if (!mesh->HasNormals())
		{
			delete[] N;
			N = nullptr;
		}
		if (!mesh->mTextureCoords[0])
		{
			delete[] uv;
			uv = nullptr;
		}
		std::shared_ptr<TriangleMesh> trimesh =
			std::make_shared<TriangleMesh>(ObjectToWorld, nTriangles, vertexIndices, nVertices, P, S, N, uv, nullptr, nullptr, faceIndices);
		// 加载纹理，只加载一个
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		int count = material->GetTextureCount(aiTextureType_DIFFUSE);
		if (count == 0)
		{
			diffTexName.push_back("");
		}
		else
		{
			for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
			{
				aiString str;
				material->GetTexture(aiTextureType_DIFFUSE, i, &str);
				std::string filename = str.C_Str();
				diffTexName.push_back(filename);
				break;
			}
		}
		count = material->GetTextureCount(aiTextureType_SPECULAR);
		if (count == 0)
		{
			specTexName.push_back("");
		}
		else
		{
			for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); i++)
			{
				aiString str;
				material->GetTexture(aiTextureType_SPECULAR, i, &str);
				std::string filename = str.C_Str();
				specTexName.push_back(filename);
				break;
			}
		}

		delete[] vertexIndices;
		delete[] P;
		delete[] S;
		delete[] N;
		delete[] uv;
		return trimesh;
	}
	void ModelLoad::processNode(aiNode *node, const aiScene *scene, const Transform &ObjectToWorld)
	{
		// 处理节点所有的网格（如果有的话）
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(processMesh(mesh, scene, ObjectToWorld));
		}
		// 接下来对它的子节点重复这一过程
		for (unsigned int i = 0; i < node->mNumChildren; i++)
			processNode(node->mChildren[i], scene, ObjectToWorld);
	}
	void ModelLoad::loadModel(std::string path, const Transform &ObjectToWorld)
	{
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
			return;
		directory = path.substr(0, path.find_last_of('/'));
		processNode(scene->mRootNode, scene, ObjectToWorld);
	}

	inline std::shared_ptr<Material> getDiffuseMaterial(std::string filename)
	{
		std::unique_ptr<TextureMapping2D> map = std::make_unique<UVMapping2D>(1.f, 1.f, 0.f, 0.f);
		ImageWrap wrapMode = ImageWrap::Repeat;
		bool trilerp = false;
		float maxAniso = 8.f;
		float scale = 1.f;
		bool gamma = false; // 如果是tga和png就是true;
		std::shared_ptr<Texture<Spectrum>> Kt = std::make_shared<ImageTexture<RGBSpectrum, Spectrum>>(std::move(map), filename, trilerp,
																									  maxAniso, wrapMode, scale, gamma);
		std::shared_ptr<Texture<float>> sigmaRed = std::make_shared<ConstantTexture<float>>(0.0f);
		std::shared_ptr<Texture<float>> bumpMap = std::make_shared<ConstantTexture<float>>(0.0f);
		return std::make_shared<MatteMaterial>(Kt, sigmaRed, bumpMap);
	}
	inline std::shared_ptr<Material> getPlasticMaterial(std::string diffFilename, std::string specFilename)
	{
		std::unique_ptr<TextureMapping2D> map1 = std::make_unique<UVMapping2D>(1.f, 1.f, 0.f, 0.f);
		std::unique_ptr<TextureMapping2D> map2 = std::make_unique<UVMapping2D>(1.f, 1.f, 0.f, 0.f);
		ImageWrap wrapMode = ImageWrap::Repeat;
		bool trilerp = false;
		float maxAniso = 8.f;
		float scale = 1.f;
		bool gamma = false; // 如果是tga和png就是true;
		std::shared_ptr<Texture<Spectrum>> plasticKd = std::make_shared<ImageTexture<RGBSpectrum, Spectrum>>(std::move(map1), diffFilename, trilerp,
																											 maxAniso, wrapMode, scale, gamma);
		std::shared_ptr<Texture<Spectrum>> plasticKr = std::make_shared<ImageTexture<RGBSpectrum, Spectrum>>(std::move(map2), specFilename, trilerp,
																											 maxAniso, wrapMode, scale, gamma);
		std::shared_ptr<Texture<float>> plasticRoughness = std::make_shared<ConstantTexture<float>>(0.8f);
		std::shared_ptr<Texture<float>> bumpMap = std::make_shared<ConstantTexture<float>>(0.0f);
		return std::make_shared<PlasticMaterial>(plasticKd, plasticKr, plasticRoughness, bumpMap, false);
	}

	void ModelLoad::buildNoTextureModel(Transform &tri_Object2World, const MediumInterface &mediumInterface,
										std::vector<std::shared_ptr<Primitive>> &prims, std::shared_ptr<Material> material)
	{

		std::vector<std::shared_ptr<Shape>> trisObj;
		Transform tri_World2Object = Inverse(tri_Object2World);
		for (int i = 0; i < meshes.size(); i++)
		{
			for (int j = 0; j < meshes[i]->nTriangles; ++j)
			{
				std::shared_ptr<TriangleMesh> meshPtr = meshes[i];
				trisObj.push_back(std::make_shared<Triangle>(&tri_Object2World, &tri_World2Object, false, meshPtr, j));
			}
		}
		// 将物体填充到基元
		for (int i = 0; i < trisObj.size(); ++i)
			prims.push_back(std::make_shared<GeometricPrimitive>(trisObj[i], material, nullptr, mediumInterface));

		meshes.clear();
		diffTexName.clear();
		specTexName.clear();
		directory = "";
	}

	void ModelLoad::buildTextureModel(Transform &tri_Object2World, const MediumInterface &mediumInterface,
									  std::vector<std::shared_ptr<Primitive>> &prims)
	{
		std::vector<std::shared_ptr<Shape>> trisObj;
		Transform tri_World2Object = Inverse(tri_Object2World);
		for (int i = 0; i < meshes.size(); i++)
		{
			std::string diffFilename = directory + "/" + diffTexName[i];
			std::string specFilename = directory + "/" + specTexName[i];
			std::shared_ptr<Material> diffMaterial = getDiffuseMaterial(diffFilename);
			std::shared_ptr<Material> MetalMaterial = getPlasticMaterial(diffFilename, specFilename);

			if (mediumInterface.inside == nullptr && mediumInterface.outside == nullptr)
			{
				for (int j = 0; j < meshes[i]->nTriangles; ++j)
				{
					std::shared_ptr<TriangleMesh> meshPtr = meshes[i];
					prims.push_back(std::make_shared<GeometricPrimitive>(
						std::make_shared<Triangle>(&tri_Object2World, &tri_World2Object, false, meshPtr, j),
						MetalMaterial, nullptr, mediumInterface));
				}
			}
			else
			{
				for (int j = 0; j < meshes[i]->nTriangles; ++j)
				{
					std::shared_ptr<TriangleMesh> meshPtr = meshes[i];

					prims.push_back(std::make_shared<GeometricPrimitive>(
						std::make_shared<Triangle>(&tri_Object2World, &tri_World2Object, false, meshPtr, j),
						nullptr, nullptr, mediumInterface));
				}
			}
		}
		meshes.clear();
		diffTexName.clear();
		specTexName.clear();
		directory = "";
	}

}
