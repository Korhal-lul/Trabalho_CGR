#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"
#include "interval.h"

class aabb {
  public:
    interval x, y, z;

    aabb() {} // Default aabb is empty

    aabb(const point3& a, const point3& b) {
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
        // particular minimum/maximum coordinate order.

        x = interval(fmin(a.x(), b.x()), fmax(a.x(), b.x()));
        y = interval(fmin(a.y(), b.y()), fmax(a.y(), b.y()));
        z = interval(fmin(a.z(), b.z()), fmax(a.z(), b.z()));
    }

    aabb(const aabb& box0, const aabb& box1) {
        x = interval(fmin(box0.x.min, box1.x.min), fmax(box0.x.max, box1.x.max));
        y = interval(fmin(box0.y.min, box1.y.min), fmax(box0.y.max, box1.y.max));
        z = interval(fmin(box0.z.min, box1.z.min), fmax(box0.z.max, box1.z.max));
    }

    const interval& axis(int n) const {
        if (n == 0) return x;
        if (n == 1) return y;
        return z;
    }

    bool hit(const ray& r, interval ray_t) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1.0 / r.direction()[a];
            auto orig = r.origin()[a];

            auto t0 = (axis(a).min - orig) * invD;
            auto t1 = (axis(a).max - orig) * invD;

            if (invD < 0.0)
                std::swap(t0, t1);

            if (t0 > ray_t.min) ray_t.min = t0;
            if (t1 < ray_t.max) ray_t.max = t1;

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }
};

#endif
