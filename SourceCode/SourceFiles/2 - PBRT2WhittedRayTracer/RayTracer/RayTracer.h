#ifndef __RayTracer_h__
#define __RayTracer_h__

#include "material.h"
#include "Rectangle.h"
#include "change.h"
#include "vec3.h"
#include "bvh_node.h"
#include "hitable.h"

//定义光源的形状和光强度
material *lightM = new diffuse_light(new constant_texture(vec3(12.0f, 12.0f, 12.0f)));
hitable *light_shape = new xz_rect(2.23f, 3.33f, 2.23f, 3.33f, 5.54f, lightM);
//定义康奈尔盒场景
hitable *cornell_box()
{
	hitable **list = new hitable *[1000];
	int index = 0;
	material *LeftWall = new lambertian(new constant_texture(vec3(0.63f, 0.065f, 0.05f)));
	material *RightWall = new lambertian(new constant_texture(vec3(0.12f, 0.45f, 0.15f)));
	material *white = new lambertian(new constant_texture(vec3(0.725f, 0.710f, 0.680f)));
	material *mirror = new metal(vec3(0.725f, 0.710f, 0.680f), 0.0f);
	material *Box = new lambertian(new constant_texture(vec3(0.725f, 0.710f, 0.680f)));
	//右左墙
	list[index++] = new flip_normals(new yz_rect(0.f, 5.55f, 0.0f, 5.55f, 5.55f, LeftWall));
	list[index++] = new yz_rect(0.f, 5.55f, 0.0f, 5.55f, 0.f, RightWall);
	//光 天花板 地板
	list[index++] = new flip_normals(light_shape);
	list[index++] = new flip_normals(new xz_rect(0.f, 5.55f, 0.0f, 5.55f, 5.55f, white));
	list[index++] = new xz_rect(0.f, 5.55f, 0.0f, 5.55f, 0.f, white);
	//前墙 后墙
	list[index++] = new flip_normals(new xy_rect(0.f, 5.55f, 0.f, 5.55f, 5.55f, white));
	//矮箱子和高箱子
	hitable *shortBox = new translate(new rotate_y(new box(vec3(0.f, 0.0f, 0.f), vec3(1.45f, 1.45f, 1.45f), white), -18.f), vec3(1.20f, 0.f, 1.15f));
	hitable *tallBox = new translate(new rotate_y(new box(vec3(0.f, 0.f, 0.f), vec3(1.45f, 3.10f, 1.25f), mirror), 25.f), vec3(2.8f, 0.f, 2.85f));
	list[index++] = shortBox;
	list[index++] = tallBox;
	//建立BVH树
	// return new hitable_list(list, index);
	return new bvh_node(list, index, 0.0f, 1.0f);
}

vec3 getLight(const ray &r, hitable *world)
{
	hit_record hrec;
	scatter_record srec;
	if (world->hit(r, 0.001f, 100000.0f, hrec))
	{
		//计算发出的光。如果是非光源物体，emitted就是vec3(0.f)
		vec3 emitted = hrec.mat_ptr->emitted(r, hrec, hrec.u, hrec.v, hrec.p);
		return emitted;
	}
	else
	{
		return vec3(0, 0, 0);
	}
}

vec3 calSpecularLi(const ray &r, hitable *world, int depth);
vec3 WhittedRT(const ray &r, hitable *world, int depth)
{
	vec3 finalColor(0.0, 0.0, 0.0);
	ray tempR = r;
	hit_record hrec;
	scatter_record srec;
	depth++;
	if (world->hit(tempR, 0.001f, 100000.0f, hrec))
	{
		vec3 emitted = hrec.mat_ptr->emitted(tempR, hrec, hrec.u, hrec.v, hrec.p);
		//击中了物体
		if (hrec.mat_ptr->scatter(tempR, hrec, srec))
		{
			if (srec.is_specular)
			{
				finalColor = calSpecularLi(srec.specular_ray, world, depth + 1);
			}
			else
			{
				//计算对光采样的pdf
				hitable_pdf p(light_shape, hrec.p);
				ray scattered = ray(hrec.p, p.generate(), r.time());
				//对光重要性采样的pdf
				float pdf_val = p.value(scattered.direction());
				//计算散射pdf
				float mpdf = hrec.mat_ptr->scattering_pdf(r, hrec, scattered);
				//计算光照强度
				vec3 Li = getLight(scattered, world);
				finalColor = srec.albedo * mpdf * Li / pdf_val;
			}
		}
		//击中了光源
		else
		{
			finalColor = emitted;
		}
	}
	return finalColor;
}

const int maxDepth = 5;
vec3 calSpecularLi(const ray &r, hitable *world, int depth)
{
	if (depth > maxDepth)
		return vec3(0.0, 0.0, 0.0);
	return WhittedRT(r, world, depth);
}

inline vec3 de_nan(const vec3 &c)
{
	vec3 temp = c;
	if (!(temp[0] == temp[0]))
		temp[0] = 0;
	if (!(temp[1] == temp[1]))
		temp[1] = 0;
	if (!(temp[2] == temp[2]))
		temp[2] = 0;
	return temp;
}

vec3 HDRtoLDR(const vec3 &col, float exposure)
{
	exposure = 1.0f - exposure;
	float invExposure = 1.0f / exposure;
	float gamma = 2.2f;
	// HDR修正
	vec3 temp_c;
	temp_c[0] = 1.0f - std::exp(-col[0] * invExposure);
	// temp_cx = powf(temp_cx, 1.0 / gamma);
	temp_c[1] = 1.0f - std::exp(-col[1] * invExposure);
	// temp_cy = powf(temp_cy, 1.0 / gamma);
	temp_c[2] = 1.0f - std::exp(-col[2] * invExposure);
	// temp_cz = powf(temp_cz, 1.0 / gamma);
	return temp_c;
}

#endif
