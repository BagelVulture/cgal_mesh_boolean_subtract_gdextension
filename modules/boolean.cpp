
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include "../include/cgal_types.hpp"

namespace PMP = CGAL::Polygon_mesh_processing;

namespace cgal_gdextension {

SurfaceMesh mesh_union(SurfaceMesh &a, SurfaceMesh &b){
    SurfaceMesh out;
    PMP::corefine_and_compute_union(a,b,out);
    return out;
}

SurfaceMesh mesh_difference(SurfaceMesh &a, SurfaceMesh &b){
    SurfaceMesh out;
    PMP::corefine_and_compute_difference(a,b,out);
    return out;
}

SurfaceMesh mesh_intersection(SurfaceMesh &a, SurfaceMesh &b){
    SurfaceMesh out;
    PMP::corefine_and_compute_intersection(a,b,out);
    return out;
}

}
