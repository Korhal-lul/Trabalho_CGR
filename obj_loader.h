#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "rtweekend.h"
#include "hittable_list.h"
#include "triangle.h"
#include "material.h"

#include <fstream>
#include <sstream>
#include <vector>
#include <string>

class obj_loader {
  public:
    static hittable_list load(const std::string& filename, shared_ptr<material> mat) {
        hittable_list result;
        std::vector<point3> vertices;
        std::ifstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            return result;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Remove leading/trailing whitespace
            line = trim(line);

            if (line.empty() || line[0] == '#') {
                continue; // Skip empty lines and comments
            }

            std::istringstream iss(line);
            std::string type;
            iss >> type;

            if (type == "v") {
                // Vertex position
                double x, y, z;
                iss >> x >> y >> z;
                vertices.push_back(point3(x, y, z));
            } 
            else if (type == "f") {
                // Face (we support triangulated faces)
                std::vector<int> indices;
                std::string vertex_str;

                while (iss >> vertex_str) {
                    // Handle formats: v, v/vt, v//vn, v/vt/vn
                    int v_index = std::stoi(split_face_index(vertex_str, 0));
                    indices.push_back(v_index - 1); // OBJ indices are 1-based
                }

                // Triangulate if necessary (assume triangles or quads)
                for (size_t i = 1; i + 1 < indices.size(); i++) {
                    int idx0 = indices[0];
                    int idx1 = indices[i];
                    int idx2 = indices[i + 1];

                    if (idx0 >= 0 && idx0 < (int)vertices.size() &&
                        idx1 >= 0 && idx1 < (int)vertices.size() &&
                        idx2 >= 0 && idx2 < (int)vertices.size()) {
                        
                        result.add(make_shared<triangle>(
                            vertices[idx0],
                            vertices[idx1],
                            vertices[idx2],
                            mat
                        ));
                    }
                }
            }
        }

        file.close();
        return result;
    }

  private:
    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, (last - first + 1));
    }

    static std::string split_face_index(const std::string& vertex_str, int index) {
        std::istringstream iss(vertex_str);
        std::string token;
        int count = 0;
        
        while (std::getline(iss, token, '/')) {
            if (count == index) {
                return token;
            }
            count++;
        }
        
        return "";
    }
};

#endif
