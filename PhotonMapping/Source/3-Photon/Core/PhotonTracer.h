#pragma once
#ifndef __PHOTONTRACER_H__
#define __PHOTONTRACER_H__


#include "Core\Ray.h"
#include "Core\Hitable.h"
#include "Core\Sphere.h"
#include "Core\Triangle.h"
#include "Core\HitableList.h"
#include "Core\Camera.h"
#include "Core\bvhTree.h"
#include "Core\rectangle.h"
#include "Core\Box.h"
#include "Core\change.h"
#include "Core\Constant_medium.h"
#include "Core\PDF.h"

#include "Core/PhotonMap.h"
#include <cmath>

void traceGlobalPhoton(const Ray&r, hitable *world, int depth, vec3 Power, PhotonMap* mPhotonMap) {
	//记录击中信息，例如击中点，表面向量，纹理坐标等
	hit_record hrec;
	if (world->hit(r, 0.001, FLT_MAX, hrec)) {

		/*Photon pn;
		pn.Pos = hrec.p;
		pn.Dir = r.direction();
		pn.power = Power;
		mPhotonMap->store(pn);*/

		//记录当前点散射信息，比如是否为镜面反射等
		scatter_record srec;
		if (depth < 4 && hrec.mat_ptr->scatter(r, hrec, srec)) {
			if (srec.is_specular) {
				traceGlobalPhoton(srec.specular_ray, world, depth + 1, Power, mPhotonMap);
			}
			else {
				//if (depth == 0) return;
				//else 
				{
					Photon pn;
					pn.Pos = hrec.p;
					pn.Dir = r.direction();
					pn.power = Power;
					mPhotonMap->store(pn);
				}
			}
		}	
	}
}
void traceCausticsPhoton(const Ray&r, hitable *world, int depth, vec3 Power, PhotonMap* mPhotonMap) {
	//记录击中信息，例如击中点，表面向量，纹理坐标等
	hit_record hrec;
	if (world->hit(r, 0.001, FLT_MAX, hrec)) {

		/*Photon pn;
		pn.Pos = hrec.p;
		pn.Dir = r.direction();
		pn.power = Power;
		mPhotonMap->store(pn);*/

		//记录当前点散射信息，比如是否为镜面反射等
		scatter_record srec;
		if (depth < 4 && hrec.mat_ptr->scatter(r, hrec, srec)) {
			if (srec.is_specular) {
				traceCausticsPhoton(srec.specular_ray, world, depth + 1, Power, mPhotonMap);
			}
			else {
				if (depth == 0) return;
				else 
				{
					Photon pn;
					pn.Pos = hrec.p;
					pn.Dir = r.direction();
					pn.power = Power;
					mPhotonMap->store(pn);
				}
			}
		}
	}
}

PhotonMap * mGlobalPhotonMap;
PhotonMap * mCausticsPhotonMap;

void  worldInit_PhotonMap(hitable * light_shape, hitable* world) {
	
	vec3 Power(27.0f, 27.0f, 27.0f);
	vec3 Origin, Dir;
	float PowScale;

	mGlobalPhotonMap = new PhotonMap(100000);
	while(mGlobalPhotonMap->PhotonNum < 100000) {
		light_shape->generatePhoton(Origin, Dir, PowScale);
		Ray r(Origin, Dir);
		traceGlobalPhoton(r, world, 0, PowScale*Power, mGlobalPhotonMap);
	}
	mGlobalPhotonMap->balance();

	// 只捕捉焦散光子
	/*mCausticsPhotonMap = new PhotonMap(10000);
	while (mCausticsPhotonMap->PhotonNum < 10000) {
		light_shape->generatePhoton(Origin, Dir, PowScale);
		Ray r(Origin, Dir);
		traceCausticsPhoton(r, world, 0, PowScale * Power * vec3(0.87, 0.49, 0.173), mCausticsPhotonMap);
	}
	mCausticsPhotonMap->balance();*/


	/*
	for (int i = 1; i <= mPhotonMap->PhotonNum; i++) {
		Photon pn;
		mPhotonMap->getPhoton(pn,i);
		//DebugText::getDebugText()->addContents(QString::number(pn.Pos.x)+" "+ QString::number(pn.Pos.y)+" "+ QString::number(pn.Pos.z));  
		//DebugText::getDebugText()->addContents(QString::number(pn.Pos.x + pn.Dir.x) + " " +
		//	QString::number(pn.Pos.y + pn.Dir.y) + " " + QString::number(pn.Pos.z + pn.Dir.z));
	}
	//DebugText::getDebugText()->addContents("");*/

	/*
	// 输出全部光子
	for (int i = 1; i <= mPhotonMap->PhotonNum; i++) {
		Photon pn;
		mPhotonMap->getPhoton(pn, i);
		//DebugText::getDebugText()->addContents(QString::number(pn.Pos.x) + " " + QString::number(pn.Pos.y) + " " +   \
			QString::number(pn.Pos.z) + " axis:" + QString::number(pn.axis));
	}
	//DebugText::getDebugText()->addContents("");

	Nearestphotons npn;
	npn.Pos = vec3(3.0,0.0,2.0);
	npn.max_photons = 100;
	npn.dist2 = new float[100 + 1];
	npn.photons = new Photon*[100 + 1];
	npn.dist2[0] = 0.6 * 0.6;
	//DebugText::getDebugText()->addContents(QString::number(npn.Pos.x) + " " + QString::number(npn.Pos.y) + " " +   \
		QString::number(npn.Pos.z));
	//输出最近的N个光子
	mPhotonMap->getNearestPhotons(&npn, 1);
	for (int i = 1; i <= npn.found; i++) {
		Photon pn;
		pn = *npn.photons[i];
		//DebugText::getDebugText()->addContents(QString::number(pn.Pos.x) + " " + QString::number(pn.Pos.y) + " " +   \
			QString::number(pn.Pos.z));
	}*/


}





#endif








