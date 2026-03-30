
#include <CGAL/Polygon_mesh_processing/remesh.h>
#include "../include/cgal_types.hpp"

namespace PMP = CGAL::Polygon_mesh_processing;

namespace godot_cgal {

void isotropic_remesh(SurfaceMesh &mesh,double target_length){
    PMP::isotropic_remeshing(
        faces(mesh),
        target_length,
        mesh
    );
}

}
