
#pragma once
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>

namespace cgal_gdextension {

using Kernel = CGAL::Simple_cartesian<double>;
using Point3 = Kernel::Point_3;
using SurfaceMesh = CGAL::Surface_mesh<Point3>;

}
