
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include "../include/cgal_types.hpp"

namespace PMP = CGAL::Polygon_mesh_processing;

namespace godot_cgal {

SurfaceMesh boolean_union(SurfaceMesh &a, SurfaceMesh &b){
    SurfaceMesh out;
    PMP::corefine_and_compute_union(a,b,out);
    return out;
}

SurfaceMesh boolean_difference(SurfaceMesh &a, SurfaceMesh &b){
    SurfaceMesh out;
    PMP::corefine_and_compute_difference(a,b,out);
    return out;
}

SurfaceMesh boolean_intersection(SurfaceMesh &a, SurfaceMesh &b){
    SurfaceMesh out;
    PMP::corefine_and_compute_intersection(a,b,out);
    return out;
}

}
