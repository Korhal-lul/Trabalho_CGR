#ifndef BVH_H
#define BVH_H

#include "rtweekend.h"

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"

#include <algorithm>

class bvh_node : public hittable {
  public:
    bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {}

    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) {
        // Build the bounding box of the span of source objects.
        aabb bbox_temp;
        for (size_t object_index = start; object_index < end; object_index++) {
            bbox_temp = aabb(bbox_temp, bounding_box(objects[object_index]));
        }
        this->bbox_bounds = bbox_temp;

        int axis = longest_axis(bbox_temp);
        auto comparator = (axis == 0) ? box_x_compare
                        : (axis == 1) ? box_y_compare
                                      : box_z_compare;

        size_t object_span = end - start;

        if (object_span == 1) {
            left = right = objects[start];
        } else if (object_span == 2) {
            left = objects[start];
            right = objects[start+1];
        } else {
            std::sort(objects.begin() + start, objects.begin() + end, comparator);

            auto mid = start + object_span / 2;
            left = make_shared<bvh_node>(objects, start, mid);
            right = make_shared<bvh_node>(objects, mid, end);
        }
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        if (!bbox_bounds.hit(r, ray_t))
            return false;

        bool hit_left = left->hit(r, ray_t, rec);
        bool hit_right = right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

        return hit_left || hit_right;
    }

    void bbox(hit_record& rec) const override {
        rec.bbox_ptr = new aabb(this->bbox_bounds);
    }

  private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox_bounds;

    static bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
        aabb box_a = bounding_box(a);
        aabb box_b = bounding_box(b);
        return box_a.axis(axis).min < box_b.axis(axis).min;
    }

    static bool box_x_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 0);
    }

    static bool box_y_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 1);
    }

    static bool box_z_compare (const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
        return box_compare(a, b, 2);
    }

    static aabb bounding_box(const shared_ptr<hittable> object) {
        // Hack to get bounding box from any hittable
        hit_record rec;
        ray r(point3(0,0,0), vec3(1,0,0));
        object->bbox(rec);
        if (rec.bbox_ptr) {
            aabb result = *rec.bbox_ptr;
            delete rec.bbox_ptr;
            return result;
        }
        return aabb();
    }

    static int longest_axis(const aabb& box) {
        auto x_size = box.x.size();
        auto y_size = box.y.size();
        auto z_size = box.z.size();

        if (x_size > y_size && x_size > z_size)
            return 0;
        if (y_size > z_size)
            return 1;
        return 2;
    }
};

#endif
