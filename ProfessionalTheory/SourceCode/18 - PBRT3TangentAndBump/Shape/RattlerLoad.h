#ifndef __RattlerLoad_h__
#define __RattlerLoad_h__

#include "Core/Geometry.h"
#include "Shape/Triangle.h"
#include "Texture/Texture.h"
#include "Texture/ImageTexture.h"
#include "Core/Primitive.h"

#include <string>
#include <iostream>
#include <fstream>

namespace Feimos
{

	class RattlerLoad
	{
	public:
		Point3f minPos, maxPos;
		std::vector<std::string> meshFiles;
		std::string modelDir;
		bool modelReadFlag;
		long long MeshNumInModel = 0;

		RattlerLoad(std::string fileDir, const Transform &ObjectToWorld, std::vector<std::shared_ptr<Feimos::Primitive>> &prims, const MediumInterface &mediumInterface);
	};

}

#endif
