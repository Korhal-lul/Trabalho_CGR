#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "aabb.h"

#include <vector>
#include "rtweekend.h"

class hittable_list : public hittable {
  public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects) {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    void bbox(hit_record& rec) const override {
        if (objects.empty()) {
            rec.bbox_ptr = nullptr;
            return;
        }

        hit_record temp_rec;
        aabb output_box;
        
        for (const auto& object : objects) {
            object->bbox(temp_rec);
            if (temp_rec.bbox_ptr) {
                output_box = aabb(output_box, *temp_rec.bbox_ptr);
                delete temp_rec.bbox_ptr;
            }
        }
        
        rec.bbox_ptr = new aabb(output_box);
    }
};

#endif