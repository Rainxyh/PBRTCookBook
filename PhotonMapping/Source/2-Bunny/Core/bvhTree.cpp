#include "Core\bvhTree.h"
#include "Core\TimeClockRandom.h"

int box_x_compare(const void *a, const void *b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;
	//if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
	//	return 0;//输出错误信息
	if (box_left.rmin().x() - box_right.rmin().x() < 0.0)return -1;
	else if (box_left.rmin().x() - box_right.rmin().x() == 0.0) {
		if (box_left.rmin().y() - box_right.rmin().y() < 0.0)return -1;
		else if (box_left.rmin().y() - box_right.rmin().y() == 0.0) {
			if (box_left.rmin().z() - box_right.rmin().z() < 0.0)return -1;
			else if (box_left.rmin().z() - box_right.rmin().z() == 0.0) {
				return -1;
			}
			else return 1;
		}
		else return 1;
	}
	else return 1;
	/*if (box_left.rmin().x() - box_right.rmin().x() < 0.0)return -1;
	else return 1;*/
}
int box_y_compare(const void *a, const void *b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;
	//if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
	//	return 0;//输出错误信息
	if (box_left.rmin().y() - box_right.rmin().y() < 0.0)return -1;
	else if (box_left.rmin().y() - box_right.rmin().y() == 0.0) {
		if (box_left.rmin().z() - box_right.rmin().z() < 0.0)return -1;
		else if (box_left.rmin().z() - box_right.rmin().z() == 0.0) {
			if (box_left.rmin().x() - box_right.rmin().x() < 0.0)return -1;
			else if (box_left.rmin().x() - box_right.rmin().x() == 0.0) {
				return -1;
			}
			else return 1;
		}
		else return 1;
	}
	else return 1;
	/*if (box_left.rmin().y() - box_right.rmin().y() < 0.0)return -1;
	else return 1;*/
}
int box_z_compare(const void *a, const void *b) {
	aabb box_left, box_right;
	hitable *ah = *(hitable**)a;
	hitable *bh = *(hitable**)b;
	//if (!ah->bounding_box(0, 0, box_left) || !bh->bounding_box(0, 0, box_right))
	//	return 0;//输出错误信息
	if (box_left.rmin().z() - box_right.rmin().z() < 0.0)return -1;
	else if (box_left.rmin().z() - box_right.rmin().z() == 0.0) {
		if (box_left.rmin().x() - box_right.rmin().x() < 0.0)return -1;
		else if (box_left.rmin().x() - box_right.rmin().x() == 0.0) {
			if (box_left.rmin().y() - box_right.rmin().y() < 0.0)return -1;
			else if (box_left.rmin().y() - box_right.rmin().y() == 0.0) {
				return -1;
			}
			else return 1;
		}
		else return 1;
	}
	else return 1;
	/*if (box_left.rmin().z() - box_right.rmin().z() < 0.0)return -1;
	else return 1;*/
}

bool bvh_node::hit(const Ray&r, float t_min, float t_max, hit_record &rec)const {
	if (box.hit(r, t_min, t_max)) {
		
		hit_record left_rec, right_rec;	
		bool hit_left = left->hit(r, t_min, t_max, left_rec);
		bool hit_right = right->hit(r, t_min, t_max, right_rec);
		if (hit_left && hit_right) {
			if (left_rec.t <= right_rec.t)
				rec = left_rec;
			else if (left_rec.t > right_rec.t)
				rec = right_rec;
			return true;
		}
		else if (hit_left) {
			rec = left_rec;
			return true;
		}
		else if (hit_right) {
			rec = right_rec;
			return true;
		}
		else
			return false;
	}
	else return false;
}
bool bvh_node::bounding_box(float t0, float t1, aabb& b)const {
	b = box;
	return true;
}
bvh_node::bvh_node(hitable **l, int n, float time0, float time1) {
	int axis = int(1 + 1 * getClockRandom());
	if (axis == 0) {
		//DebugText::getDebugText()->addContents("axis == 0");
		qsort(l, n, sizeof(hitable*), box_x_compare);
	}	
	else if (axis == 1) {
		//DebugText::getDebugText()->addContents("axis == 1");
		qsort(l, n, sizeof(hitable *), box_y_compare);
	}	
	else {
		//DebugText::getDebugText()->addContents("axis == 2");
		qsort(l, n, sizeof(hitable *), box_z_compare);
	}
		
	if (n == 1) {
		//DebugText::getDebugText()->addContents("n==1");
		left = right = l[0];
	}
	else if (n == 2) {
		//DebugText::getDebugText()->addContents("n=2");
		left = l[0];
		right = l[1];
	}
	else {
		//DebugText::getDebugText()->addContents("Left");
		left = new bvh_node(l, n / 2, time0, time1);
		//DebugText::getDebugText()->addContents("Right");
		right = new bvh_node(l + n / 2, n - n / 2, time0, time1);
	}
	//包围盒包裹的就是left和right两边的总的包围盒
	aabb box_left, box_right;
	if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right));
	box = surrounding_box(box_left, box_right);
}
































