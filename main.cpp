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

// Auto camera globals (set when scene bbox is available)
static point3 AUTO_CAM_POS = point3(0,0,0);
static point3 AUTO_CAM_LOOKAT = point3(0,0,0);
static bool AUTO_CAM_SET = false;

#define USE_OBJ true  // true = usar .obj | false = usar teste com esfera

int main() {
    hittable_list world;

    #if USE_OBJ
        std::cout << "*-*-*-*-*-* Modo: carregando arquivo obj *-*-*-*-*-*" << std::endl;
        std::string obj_file = "caneca.obj";
        auto material_object = make_shared<lambertian>(color(0.8, 0.8, 0.0));
        
        std::cout << "Carregando arquivo: " << obj_file << std::endl;
        hittable_list obj_world = obj_loader::load(obj_file, material_object);
        
        std::cout << "Triangulos carregados: " << obj_world.objects.size() << std::endl;
        
        if (obj_world.objects.size() > 0) {
            std::cout << "BVH compilado" << std::endl;
        } else {
            std::cout << "ERRO: Nenhum triangulo carregado!" << std::endl;
            return 1;
        }

        hit_record bbox_rec;
        world.bbox(bbox_rec);
        if (bbox_rec.bbox_ptr) {
            aabb scene_box = *bbox_rec.bbox_ptr;
            auto min_x = scene_box.x.min; auto max_x = scene_box.x.max;
            auto min_y = scene_box.y.min; auto max_y = scene_box.y.max;
            auto min_z = scene_box.z.min; auto max_z = scene_box.z.max;
            point3 center((min_x+max_x)/2.0, (min_y+max_y)/2.0, (min_z+max_z)/2.0);
            double dx = max_x - min_x;
            double dy = max_y - min_y;
            double dz = max_z - min_z;
            double radius = 0.5 * std::sqrt(dx*dx + dy*dy + dz*dz);

            vec3 view_dir = unit_vector(vec3(-0.3, 0.25, 1.0));
            double distance = radius * 3.0 + 1.0;
            point3 cam_pos = center + view_dir * distance;

            std::cout << "Scene center: (" << center.x() << ", " << center.y() << ", " << center.z() << ")\n";
            std::cout << "Scene radius ~ " << radius << " ; setting camera at distance " << distance << std::endl;

            // Save computed camera values in static locals so we can read them later
            AUTO_CAM_POS = cam_pos;
            AUTO_CAM_LOOKAT = center;
            AUTO_CAM_SET = true;

            // Expose via global pointers (cheaper than redesign)
            // We'll store pointers to these static variables in global variables
        }
    #else
        std::cout << "*-*-*-*-*-* Modo: teste com esferas *-*-*-*-*-*" << std::endl;
        
        auto material_red = make_shared<lambertian>(color(1.0, 0.2, 0.2));
        world.add(make_shared<sphere>(point3(-0.5, 0, -2), 0.5, material_red));
        
        auto material_blue = make_shared<lambertian>(color(0.2, 0.2, 1.0));
        world.add(make_shared<sphere>(point3(0.5, 0, -2), 0.5, material_blue));
        
        auto material_ground = make_shared<lambertian>(color(0.5, 0.5, 0.5));
        world.add(make_shared<sphere>(point3(0, -100.5, -2), 100.0, material_ground));
        
        std::cout << "Objetos adicionados: " << world.objects.size() << std::endl;
    #endif

    camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 800;          // maior -> + qualidade
    cam.samples_per_pixel = 100;          // maior -> - ruído
    cam.max_depth         = 20;           // maior -> mais reflexões

    #if USE_OBJ
        cam.vfov = 40;
        if (AUTO_CAM_SET) {
            cam.lookfrom = AUTO_CAM_POS;
            cam.lookat = AUTO_CAM_LOOKAT;
            // recompute focus distance as distance from lookfrom to lookat
            cam.focus_dist = (cam.lookfrom - cam.lookat).length();
            std::cout << "Using auto camera: lookfrom=(" << cam.lookfrom.x() << "," << cam.lookfrom.y() << "," << cam.lookfrom.z() << ")\n";
        } else {
            cam.lookfrom = point3(-2, 2.5, 0);   // fallback
            cam.lookat   = point3(0, 1.5, 0);
            cam.focus_dist = 10.0;
        }
    #else
        cam.vfov     = 90;
        cam.lookfrom = point3(0, 0, 0);
        cam.lookat   = point3(0, 0, -1);
        cam.focus_dist = 1.0;
    #endif

    cam.vup           = vec3(0, 1, 0);
    cam.defocus_angle = 0.0;

    cam.render(world);
    std::cout << "Resultado salvo em: output.png" << std::endl;
}
