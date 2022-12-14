#include "Core/Constant_medium.h"

bool constant_medium::hit(const Ray &r, float t_min, float t_max, hit_record &rec) const
{
	hit_record rec1, rec2;
	if (boundary->hit(r, -FLT_MAX, FLT_MAX, rec1))
	{
		if (boundary->hit(r, rec1.t + 0.0001, FLT_MAX, rec2))
		{
			if (rec1.t < t_min)
				rec1.t = t_min;
			if (rec2.t > t_max)
				rec2.t = t_max;
			if (rec1.t >= rec2.t)
				return false; // ò�Ƹ��������ᴥ�������Լ����Ҳ�ǲ��ᴥ��
			if (rec1.t < 0)
				rec1.t = 0;
			float distance_inside_boundary = (rec2.t - rec1.t) * r.direction().length();
			float hit_distance = -(1.0f / density) * log(getClockRandom());
			if (hit_distance < distance_inside_boundary)
			{
				rec.t = rec1.t + hit_distance / r.direction().length();
				rec.p = r.pointAtParameter(rec.t);
				rec.normal = vec3(1, 0, 0); // �����������У������ò��š�
				rec.mat_ptr = phase_function;
				return true;
			}
		}
	}
	return false;
}
