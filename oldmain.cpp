#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "triangle.h"
#include "material.h"
#include "obj_loader.h"
#include "bvh.h"


int main() {
    hittable_list world;

    auto material_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
    auto material_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<dielectric>(1.50);
    auto material_bubble = make_shared<dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<sphere>(point3( 0.0, -101, -1.0), 100.0, material_ground));
    world.add(make_shared<sphere>(point3( 0.0,    0.0, -1.2),   1, material_center));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, 0),   1, material_left));
    world.add(make_shared<sphere>(point3(-1.0,    0.0, 0),   0.9, material_bubble));
    world.add(make_shared<sphere>(point3( 1.0,    0.0, -2.0),   1, material_right));

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 500;
    cam.samples_per_pixel = 200;
    cam.max_depth         = 50;


    cam.vfov     = 30;
    cam.lookfrom = point3(-2,3,1.7);
    cam.lookat   = point3(0,0,-1);
    cam.vup      = vec3(0,1,0);

    cam.render(world);
}