#include "Core/Scene.h"


namespace Feimos {
static long long nIntersectionTests = 0;
static long long nShadowTests = 0;

// Scene Method Definitions
bool Scene::Intersect(const Ray &ray, SurfaceInteraction *isect) const {
	++nIntersectionTests;
	return aggregate->Intersect(ray, isect);
}

bool Scene::IntersectP(const Ray &ray) const {
	++nShadowTests;
	return aggregate->IntersectP(ray);
}






}









