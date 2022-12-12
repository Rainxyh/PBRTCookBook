#include "Core\BoundingBox.h"



aabb surrounding_box(aabb box0, aabb box1) {
	vec3 small(fmin(box0.rmin().x(), box1.rmin().x()),
		fmin(box0.rmin().y(), box1.rmin().y()),
		fmin(box0.rmin().z(), box1.rmin().z()));
	vec3 big(fmax(box0.rmax().x(), box1.rmax().x()),
		fmax(box0.rmax().y(), box1.rmax().y()),
		fmax(box0.rmax().z(), box1.rmax().z()));
	return aabb(small,big);
}























