#include "RattlerLoad.h"
#include "Material/MatteMaterial.h"
#include "Material/PlasticMaterial.h"
#include "Texture/ConstantTexture.h"
#include "Texture/ImageTexture.h"

namespace Feimos
{

	inline std::shared_ptr<Texture<float>> getAlphaMaskTexture(std::string filename)
	{
		std::unique_ptr<TextureMapping2D> map = std::make_unique<UVMapping2D>(1.f, 1.f, 0.f, 0.f);
		ImageWrap wrapMode = ImageWrap::Repeat;
		bool trilerp = false;
		float maxAniso = 8.f;
		float scale = 1.f;
		bool gamma = false; // 如果是tga和png就是true;
		std::shared_ptr<Texture<float>> alphaMask = std::make_shared<ImageTexture<float, float>>(std::move(map), filename, trilerp,
																								 maxAniso, wrapMode, scale, gamma);
		return alphaMask;
	}
	inline std::shared_ptr<Texture<float>> getBumpTexture(std::string filename)
	{
		std::unique_ptr<TextureMapping2D> map = std::make_unique<UVMapping2D>(1.f, 1.f, 0.f, 0.f);
		ImageWrap wrapMode = ImageWrap::Repeat;
		bool trilerp = false;
		float maxAniso = 8.f;
		float scale = 1.f;
		bool gamma = false; // 如果是tga和png就是true;
		std::shared_ptr<Texture<float>> bump = std::make_shared<ImageTexture<float, float>>(std::move(map), filename, trilerp,
																							maxAniso, wrapMode, scale, gamma);
		return bump;
	}

	inline std::shared_ptr<Material> getDiffuseMaterial(std::shared_ptr<Texture<float>> bumpMap)
	{
		Feimos::Spectrum whiteColor;
		whiteColor[0] = 0.62f;
		whiteColor[1] = 0.62f;
		whiteColor[2] = 0.62f;
		std::shared_ptr<Feimos::Texture<Feimos::Spectrum>> KdWhite = std::make_shared<Feimos::ConstantTexture<Feimos::Spectrum>>(whiteColor);
		std::shared_ptr<Feimos::Texture<float>> sigma = std::make_shared<Feimos::ConstantTexture<float>>(0.0f);
		// 材质
		return std::make_shared<Feimos::MatteMaterial>(KdWhite, sigma, bumpMap);
	}
	inline std::shared_ptr<Material> getDiffuseMaterial(std::string filename, std::shared_ptr<Texture<float>> bumpMap)
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
		return std::make_shared<MatteMaterial>(Kt, sigmaRed, bumpMap);
	}
	inline std::shared_ptr<Material> getPlasticMaterial(std::string diffFilename, std::string specFilename, std::shared_ptr<Texture<float>> bumpMap)
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
		return std::make_shared<PlasticMaterial>(plasticKd, plasticKr, plasticRoughness, bumpMap, false);
	}

	RattlerLoad::RattlerLoad(std::string fileDir, const Transform &ObjectToWorld, std::vector<std::shared_ptr<Feimos::Primitive>> &prims, const MediumInterface &mediumInterface)
	{

		modelReadFlag = true;

		std::string filePath = fileDir + "/ModelInfo.rattler";
		modelDir = fileDir;

		std::ifstream file(filePath);
		if (!file)
		{
			modelReadFlag = false;
			return;
		}
		std::string name;
		while (file >> name)
		{
			if (name == "minPos")
				file >> minPos.x >> minPos.y >> minPos.z;
			else if (name == "maxPos")
				file >> maxPos.x >> maxPos.y >> maxPos.z;
			else if (name == "MeshFile")
			{
				file >> name;
				meshFiles.push_back(name);
			}
		}
		file.close();

		// 依次处理每个面片
		for (int meshID = 0; meshID < meshFiles.size(); meshID++)
		{

			std::string curFilePath = fileDir + meshFiles[meshID];

			std::ifstream curFile(curFilePath);
			if (!curFile)
			{
				modelReadFlag = false;
				continue;
			}
			int VertexNum = 0, FaceNum = 0, VertIndexNum = 0, NormNum = 0, UVNum = 0, TanNum = 0, BiTanNum = 0;
			Point3f meshMinPos, meshMaxPos;

			std::string diffuseMap = "NoFile_Rattler", specularMap = "NoFile_Rattler", alphaMap = "NoFile_Rattler", bumpMap = "NoFile_Rattler";

			std::string cname;
			while (curFile >> cname)
			{
				if (cname == "VertexNum")
					curFile >> VertexNum;
				else if (cname == "FaceNum")
					curFile >> FaceNum;
				else if (cname == "VertIndexNum")
					curFile >> VertIndexNum;
				else if (cname == "NormNum")
					curFile >> NormNum;
				else if (cname == "UVNum")
					curFile >> UVNum;
				else if (cname == "TanNum")
					curFile >> TanNum;
				else if (cname == "BiTanNum")
					curFile >> BiTanNum;
				else if (cname == "minPos")
					curFile >> meshMinPos.x >> meshMinPos.y >> meshMinPos.z;
				else if (cname == "maxPos")
					curFile >> meshMaxPos.x >> meshMaxPos.y >> meshMaxPos.z;
				else if (cname == "Diffuse")
					curFile >> diffuseMap;
				else if (cname == "Specular")
					curFile >> specularMap;
				else if (cname == "Alpha")
					curFile >> alphaMap;
				else if (cname == "Bump")
					curFile >> bumpMap;
				else if (cname == "data")
					break;
			}

			if (VertexNum == 0 || VertIndexNum == 0)
				modelReadFlag = false;

			Point3f *vertexArray = new Point3f[VertexNum];
			int *vertexIndices = new int[FaceNum * 3];
			Normal3f *NormArray = new Normal3f[NormNum];
			Point2f *uvArray = new Point2f[UVNum];
			Vector3f *TangArray;
			if (TanNum == 0)
				TangArray = nullptr;
			else
				TangArray = new Vector3f[TanNum];

			int tVertexNum = 0, tFaceNum = 0, tNormNum = 0, tUVNum = 0, tTanNum = 0, tBiTanNum = 0;
			while (curFile >> cname)
			{
				if (cname == "Vetx")
					curFile >> vertexArray[tVertexNum].x >> vertexArray[tVertexNum].y >> vertexArray[tVertexNum++].z;
				else if (cname == "FaceVetxIdx")
				{
					curFile >> vertexIndices[tFaceNum * 3 + 0] >> vertexIndices[tFaceNum * 3 + 1] >> vertexIndices[tFaceNum * 3 + 2];
					tFaceNum++;
				}
				else if (cname == "Norm")
					curFile >> NormArray[tNormNum].x >> NormArray[tNormNum].y >> NormArray[tNormNum++].z;
				else if (cname == "UV")
					curFile >> uvArray[tUVNum].x >> uvArray[tUVNum++].y;
				else if (cname == "Tan")
					curFile >> TangArray[tTanNum].x >> TangArray[tTanNum].y >> TangArray[tTanNum++].z;
				else if (cname == "BiTan")
				{
					// PBRT不需要副切线
					tBiTanNum++;
				}
			}
			if ((VertexNum != tVertexNum) || (FaceNum != tFaceNum))
				modelReadFlag = false;

			curFile.close();

			if (modelReadFlag)
			{

				Transform World2Object = Inverse(ObjectToWorld);

				std::string diffFilename = modelDir + "/" + diffuseMap;
				std::string specFilename = modelDir + "/" + specularMap;
				std::string alphaFilename = modelDir + "/" + alphaMap;
				std::string bumpFilename = modelDir + "/" + bumpMap;

				std::shared_ptr<Material> curMaterial;
				std::shared_ptr<Texture<float>> alphaTexture;
				std::shared_ptr<Texture<float>> bumpTexture;

				if (alphaMap != "NoFile_Rattler")
				{
					alphaTexture = getAlphaMaskTexture(alphaFilename);
				}
				if (bumpMap != "NoFile_Rattler")
					bumpTexture = getBumpTexture(bumpFilename);
				else
					bumpTexture = std::make_shared<ConstantTexture<float>>(0.0f);

				if (diffuseMap == "NoFile_Rattler")
				{
					// 生成灰白常量纹理
					curMaterial = getDiffuseMaterial(bumpTexture);
				}
				else
				{
					if (specularMap == "NoFile_Rattler")
					{
						// 生成漫反射纹理材质
						curMaterial = getDiffuseMaterial(diffFilename, bumpTexture);
					}
					else
					{
						// 生成塑料纹理材质
						// curMaterial = getDiffuseMaterial(diffFilename, bumpTexture);
						curMaterial = getPlasticMaterial(diffFilename, specFilename, bumpTexture);
					}
				}

				std::shared_ptr<TriangleMesh> trimesh =
					std::make_shared<TriangleMesh>(ObjectToWorld, FaceNum, vertexIndices, VertexNum, vertexArray, TangArray, NormArray, uvArray, alphaTexture, nullptr, nullptr);

				if (mediumInterface.inside == nullptr && mediumInterface.outside == nullptr)
				{
					for (int j = 0; j < trimesh->nTriangles; ++j)
					{
						prims.push_back(std::make_shared<GeometricPrimitive>(
							std::make_shared<Triangle>(&ObjectToWorld, &World2Object, false, trimesh, j),
							curMaterial, nullptr, mediumInterface));
					}
				}
				else
				{
					for (int j = 0; j < trimesh->nTriangles; ++j)
					{
						prims.push_back(std::make_shared<GeometricPrimitive>(
							std::make_shared<Triangle>(&ObjectToWorld, &World2Object, false, trimesh, j),
							nullptr, nullptr, mediumInterface));
					}
				}
			}

			MeshNumInModel += tFaceNum;

			delete[] vertexArray;
			delete[] vertexIndices;
			delete[] NormArray;
			delete[] uvArray;
			delete[] TangArray;
		}
	}

}
