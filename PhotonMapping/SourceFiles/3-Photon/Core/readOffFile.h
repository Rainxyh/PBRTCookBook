#pragma once
#ifndef __readOffFile_h__
#define __readOffFile_h__
#include <iostream>
#include <fstream>
#include <string>

#include "Core/Triangle.h"
#include "Core/bvhTree.h"

class offRead
{
public:
	int vertexs, faces, normals;
	vec3 *vertexArray;
	vec3 *normalArray;
	bvh_node *m_bvh;
	offRead(material *mtrl)
	{
		std::fstream f("../../Resources/bunny.obj");
		std::string ed;
		f >> vertexs >> normals >> faces;
		vertexArray = new vec3[vertexs];
		for (int i = 0; i < vertexs; i++)
		{
			f >> ed;
			f >> vertexArray[i][0];
			f >> vertexArray[i][1];
			f >> vertexArray[i][2];
		}
		normalArray = new vec3[normals];
		for (int i = 0; i < normals; i++)
		{
			f >> ed;
			f >> normalArray[i][0];
			f >> normalArray[i][1];
			f >> normalArray[i][2];
		}
		hitable **list = new hitable *[faces];
		int index = 0;
		int num, v0, v1, v2, n0, n1, n2;
		float scale = 0.001;
		vec3 trans(1.9, 1.3, 2.3);
		for (int i = 0; i < faces; i++)
		{
			f >> ed >> v0 >> n0 >> v1 >> n1 >> v2 >> n2;
			list[index++] = new triangle(scale * vertexArray[v0 - 1] + trans, scale * vertexArray[v1 - 1] + trans,
										 scale * vertexArray[v2 - 1] + trans, normalArray[n0 - 1], normalArray[n1 - 1], normalArray[n2 - 1], mtrl);
		}
		m_bvh = new bvh_node(list, index, 0.0, 1.0);
		f.close();
	}
};

extern offRead *myOffRead;

#endif
