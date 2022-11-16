#pragma once
#ifndef __ModelLoad_h__
#define __ModelLoad_h__


#include "Core\FeimosRender.h"
#include "Shape\Triangle.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "Texture\ImageTexture.h"
#include "Media\Medium.h"

#include <memory>
#include <string>
#include <vector>

namespace Feimos {


class ModelLoad {
public:
	//  模型数据  
	std::vector<std::shared_ptr<TriangleMesh>> meshes;
	std::vector<std::string> diffTexName,specTexName; //纹理序列
	std::string directory;
	//  函数  
	void loadModel(std::string path, const Transform &ObjectToWorld);
	void processNode(aiNode *node, const aiScene *scene, const Transform &ObjectToWorld);
	std::shared_ptr<TriangleMesh> processMesh(aiMesh *mesh, const aiScene *scene, const Transform &ObjectToWorld);
	void buildNoTextureModel(Transform& tri_Object2World, const MediumInterface &mediumInterface,
		std::vector<std::shared_ptr<Primitive>> &prims, std::shared_ptr<Material> material);
	void ModelLoad::buildTextureModel(Transform& tri_Object2World, const MediumInterface &mediumInterface,
		std::vector<std::shared_ptr<Primitive>> &prims);
};



}














#endif


