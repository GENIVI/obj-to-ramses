#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <vector>

namespace obj2ramses {
namespace ObjGeometry {

using std::vector;


struct vertex3f {
    float x, y, z;
};

struct tex_coord_3f {
    float u, v = 0.0f, w = 0.0f;
};

struct vertex_normal_3f {
    float x, y, z;
};

struct face {
    vector<unsigned> v, vt, vn;
    unsigned long len = v.size();
};

}}

#endif //GEOMETRY_H