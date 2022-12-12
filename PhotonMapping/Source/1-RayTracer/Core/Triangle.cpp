#include "Core\Triangle.h"

bool triangle::hit(const Ray&r, float t_min, float t_max, hit_record&rec)const {
	vec3 E1 = V1 - V0;
	// E2
	vec3 E2 = V2 - V0;
	// P
	vec3 P = cross(r.direction(), E2);
	// determinant
	float det = dot(E1, P);
	// keep det > 0, modify T accordingly
	vec3 T;
	if (det >0) {
		T = r.origin() - V0;
	}
	else {
		T = V0 - r.origin();
		det = -det;
	}
	// If determinant is near zero, ray lies in plane of triangle
	if (det < 0.0001f)
		return false;
	// Calculate u and make sure u <= 1
	rec.texU = dot(T, P);
	if (rec.texU < 0.0f || rec.texU > det)
		return false;
	// Q
	vec3 Q = cross(T, E1);
	// Calculate v and make sure u + v <= 1
	rec.texV = dot(r.direction(), Q);
	if (rec.texV < 0.0f || rec.texU + rec.texV > det)
		return false;
	// Calculate t, scale parameters, ray intersects triangle
	float t;
	t = dot(E2, Q);
	float fInvDet = 1.0f / det;
	t *= fInvDet;
	rec.texU *= fInvDet;
	rec.texV *= fInvDet;
	//纹理坐标的表示方法：V0 + u(V1-V0) + v(V2-V0);就可以计算出纹理位置
	if (t < t_max && t > t_min) {
		rec.t = t;
		rec.p = r.pointAtParameter(rec.t);
		rec.normal = NormalV0 + rec.texU * (NormalV1 - NormalV0) + rec.texV * (NormalV2 - NormalV0);
		rec.mat_ptr = matral;
		return true;
	}
	return false;
}
bool triangle::bounding_box(float t0, float t1, aabb&box)const {
	vec3 minV = vec3(ffmin(V0.x(), V1.x(), V2.x()), ffmin(V0.y(), V1.y(), V2.y()), ffmin(V0.z(), V1.z(), V2.z()));
	vec3 maxV = vec3(ffmax(V0.x(), V1.x(), V2.x()), ffmax(V0.y(), V1.y(), V2.y()), ffmax(V0.z(), V1.z(), V2.z()));
	//防止变成一个片段
	minV[0] -= 0.01; minV[1] -= 0.01; minV[2] -= 0.01;
	maxV[0] += 0.01; maxV[1] += 0.01; maxV[2] += 0.01;
	box = aabb(minV, maxV);
	//DebugText::getDebugText()->addContents("V0:" + QString::number(V0.x()) + "," + QString::number(V0.y()) + "," + QString::number(V0.z()));
	//DebugText::getDebugText()->addContents("V1:" + QString::number(V1.x()) + "," + QString::number(V1.y()) + "," + QString::number(V1.z()));
	//DebugText::getDebugText()->addContents("V2:" + QString::number(V2.x()) + "," + QString::number(V2.y()) + "," + QString::number(V2.z()));
	//DebugText::getDebugText()->addContents("minV:" + QString::number(minV.x()) + "," + QString::number(minV.y()) + "," + QString::number(minV.z()));
	//DebugText::getDebugText()->addContents("maxV:" + QString::number(maxV.x()) + "," + QString::number(maxV.y()) + "," + QString::number(maxV.z()));
	return true;
}















