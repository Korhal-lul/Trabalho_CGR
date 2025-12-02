#ifndef MESH_H
#define MESH_H

#include "hittable_list.h"
#include "triangle.h"
#include "tiny_obj_loader.h"

class mesh : public hittable {
public:
    hittable_list tris;

    mesh(const std::string& filename,
         shared_ptr<material> mat,
         double scale = 1.0,
         const vec3& translate = vec3(0,0,0))
    {
        load_obj(filename, mat, scale, translate);
    }

    bool hit(const ray& r, interval t_range, hit_record& rec) const override {
        return tris.hit(r, t_range, rec);
    }

private:
    void load_obj(const std::string& filename,
                  shared_ptr<material> mat,
                  double scale,
                  const vec3& T)
    {
        tinyobj::ObjReader reader;
        tinyobj::ObjReaderConfig config;

        config.triangulate = true;

        if (!reader.ParseFromFile(filename, config)) {
            throw std::runtime_error("Failed to load OBJ: " + filename);
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();

        for (const auto& shape : shapes) {
            size_t index_offset = 0;

            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                int fv = shape.mesh.num_face_vertices[f]; // always 3

                tinyobj::index_t idx0 = shape.mesh.indices[index_offset + 0];
                tinyobj::index_t idx1 = shape.mesh.indices[index_offset + 1];
                tinyobj::index_t idx2 = shape.mesh.indices[index_offset + 2];

                point3 v0(
                    attrib.vertices[3 * idx0.vertex_index + 0],
                    attrib.vertices[3 * idx0.vertex_index + 1],
                    attrib.vertices[3 * idx0.vertex_index + 2]
                );

                point3 v1(
                    attrib.vertices[3 * idx1.vertex_index + 0],
                    attrib.vertices[3 * idx1.vertex_index + 1],
                    attrib.vertices[3 * idx1.vertex_index + 2]
                );

                point3 v2(
                    attrib.vertices[3 * idx2.vertex_index + 0],
                    attrib.vertices[3 * idx2.vertex_index + 1],
                    attrib.vertices[3 * idx2.vertex_index + 2]
                );

                // Apply scale + translation
                v0 = v0 * scale + T;
                v1 = v1 * scale + T;
                v2 = v2 * scale + T;

                tris.add(make_shared<triangle>(v0, v1, v2, mat));

                index_offset += fv;
            }
        }
    }
};

#endif
