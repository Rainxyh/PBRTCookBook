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

void traceGlobalPhoton(const Ray&r, hitable *world, int depth, vec3 Power, PhotonMap* mPhotonMap);
void traceCausticsPhoton(const Ray&r, hitable *world, int depth, vec3 Power, PhotonMap* mPhotonMap);

extern PhotonMap * mGlobalPhotonMap;
extern PhotonMap * mCausticsPhotonMap;

void  worldInit_PhotonMap(hitable * light_shape, hitable* world);

vec3 color_PMPT(const Ray&r, hitable *world, int depth);




#endif








