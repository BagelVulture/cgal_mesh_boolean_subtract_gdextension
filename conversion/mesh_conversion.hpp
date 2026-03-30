
#pragma once
#include "../include/cgal_types.hpp"
#include <vector>
#include <array>

namespace cgal_gdextension {

SurfaceMesh arrays_to_surface_mesh(
    const std::vector<std::array<double,3>>& vertices,
    const std::vector<std::array<int,3>>& faces);

void surface_mesh_to_arrays(
    const SurfaceMesh& mesh,
    std::vector<std::array<double,3>>& vertices,
    std::vector<std::array<int,3>>& faces);

}
