#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"
#include "vec3.h"
#include "aabb.h"

class triangle : public hittable {
  public:
    // Constructor without smooth normals (compute from geometry)
    triangle(const point3& v0, const point3& v1, const point3& v2, shared_ptr<material> mat)
        : v0(v0), v1(v1), v2(v2), mat(mat), 
          n0(vec3(0,0,0)), n1(vec3(0,0,0)), n2(vec3(0,0,0)), 
          use_smooth_normals(false) {}
    
    // Constructor with smooth normals (from OBJ vn)
    triangle(const point3& v0, const point3& v1, const point3& v2, shared_ptr<material> mat,
             const vec3& n0, const vec3& n1, const vec3& n2)
        : v0(v0), v1(v1), v2(v2), mat(mat), 
          n0(n0), n1(n1), n2(n2),
          use_smooth_normals(!(n0.length() < 0.001 && n1.length() < 0.001 && n2.length() < 0.001)) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // Möller-Trumbore ray-triangle intersection algorithm
        const double EPSILON = 1e-8;
        
        vec3 edge1 = v1 - v0;
        vec3 edge2 = v2 - v0;
        vec3 ray_cross_e2 = cross(r.direction(), edge2);
        double det = dot(edge1, ray_cross_e2);

        if (std::fabs(det) < EPSILON) {
            return false; // Ray is parallel to triangle
        }

        double inv_det = 1.0 / det;
        vec3 s = r.origin() - v0;
        double u = inv_det * dot(s, ray_cross_e2);

        if (u < 0.0 || u > 1.0) {
            return false;
        }

        vec3 s_cross_e1 = cross(s, edge1);
        double v = inv_det * dot(r.direction(), s_cross_e1);

        if (v < 0.0 || u + v > 1.0) {
            return false;
        }

        double t = inv_det * dot(edge2, s_cross_e1);

        if (!ray_t.surrounds(t)) {
            return false;
        }

        rec.t = t;
        rec.p = r.at(rec.t);
        
        // Use smooth normals if available, otherwise compute from geometry
        vec3 outward_normal;
        if (use_smooth_normals) {
            // Interpolate smooth normals using barycentric coordinates
            double w = 1.0 - u - v;
            outward_normal = unit_vector(w * n0 + u * n1 + v * n2);
        } else {
            // Compute face normal using cross product
            outward_normal = unit_vector(cross(edge1, edge2));
        }
        
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

    void bbox(hit_record& rec) const override {
        aabb box0(v0, v1);
        aabb box1(v0, v2);
        rec.bbox_ptr = new aabb(box0, box1);
    }
    
  private:
    point3 v0, v1, v2;
    vec3 n0, n1, n2;  // Smooth normals for each vertex
    shared_ptr<material> mat;
    bool use_smooth_normals;
};

#endif
