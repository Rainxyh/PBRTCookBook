#pragma once
#ifndef __PHOTONMAP_H__
#define __PHOTONMAP_H__

#include "Core/Vector.h"

struct Photon {
	vec3 Pos; //λ��
	vec3 Dir; //���䷽��
	vec3 power; //������ͨ������ɫֵ��ʾ
	int axis;
};

struct Nearestphotons {
	vec3 Pos;
	int max_photons, found;
	bool got_heap;
	float* dist2;
	Photon** photons;
	Nearestphotons() {
		max_photons = found = 0;
		got_heap = false;
		dist2 = NULL;
		photons = NULL;
	}
	~Nearestphotons() {
		delete[] dist2;
		delete[] photons;
	}
};

class PhotonMap {
public:
	PhotonMap();
	PhotonMap(int max);
	~PhotonMap();
	int maxPhotonNum;
	int PhotonNum;//��������
	Photon *mPhoton;
	void store(Photon pn);
	void MedianSplit(Photon* porg, int start, int end, int med, int axis);
	void balance();
	void PhotonMap::balanceSegment(Photon*, int, int, int);
	bool getPhoton(Photon &pn, int index) {
		if (index > maxPhotonNum) return false;
		else {
			pn = mPhoton[index];
			return true;
		}
	}
	void getNearestPhotons(Nearestphotons* np, int index);
	float getPhotonPosAxis(int index, int axis) {
		return mPhoton[index].Pos[axis];
	}
	vec3 getIrradiance(vec3 Pos, vec3 Norm, float max_dist, const int N);
	vec3 box_min, box_max;

};







#endif



