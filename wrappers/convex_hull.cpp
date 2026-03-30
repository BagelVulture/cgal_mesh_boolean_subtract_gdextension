
#include <CGAL/convex_hull_3.h>
#include "../include/cgal_types.hpp"
#include <vector>

namespace godot_cgal {

SurfaceMesh convex_hull(const std::vector<Point3> &points){
    SurfaceMesh mesh;
    CGAL::convex_hull_3(points.begin(),points.end(),mesh);
    return mesh;
}

}
