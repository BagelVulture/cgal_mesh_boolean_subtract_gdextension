#include "cgal_wrapper.h"

#include <map>
#include <vector>

#include <CGAL/Polygon_mesh_processing/self_intersections.h>
#include <map>
#include <vector>
#include <CGAL/Polygon_mesh_processing/repair.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/boost/graph/iterator.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polygon_mesh_processing/orient_polygon_soup.h>
#include <CGAL/Polygon_mesh_processing/polygon_soup_to_polygon_mesh.h>

using namespace godot;

using Kernel = CGAL::Exact_predicates_exact_constructions_kernel;
using Point = Kernel::Point_3;
using CgalMesh = CGAL::Surface_mesh<Point>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;
using vertex_descriptor = CgalMesh::Vertex_index;

namespace PMP = CGAL::Polygon_mesh_processing;

void CGALWrapper::_bind_methods() {
    ClassDB::bind_method(D_METHOD("boolean_difference", "a", "b"),
        &CGALWrapper::boolean_difference);

    ClassDB::bind_method(D_METHOD("boolean_difference_shape", "a", "b"),
        &CGALWrapper::boolean_difference_shape);
}





// ----------------------------
// Helpers
// ----------------------------
static inline int64_t quantize_coord(double v, double scale = 1000000.0) {
    return (int64_t)std::llround(v * scale);
}

struct PointKey {
    int64_t x;
    int64_t y;
    int64_t z;

    bool operator<(const PointKey &other) const {
        if (x != other.x) return x < other.x;
        if (y != other.y) return y < other.y;
        return z < other.z;
    }
};

static inline PointKey make_point_key(const Vector3 &v) {
    return PointKey{
        quantize_coord((double)v.x),
        quantize_coord((double)v.y),
        quantize_coord((double)v.z)
    };
}

static inline Vector3 point_to_vector3(const Point &p) {
    return Vector3(
        (real_t)CGAL::to_double(p.x()),
        (real_t)CGAL::to_double(p.y()),
        (real_t)CGAL::to_double(p.z())
    );
}

static void print_face_points(const char *label, int64_t face_idx, const std::vector<Point> &pts) {
    if (pts.size() != 3) {
        UtilityFunctions::print(label, " face ", face_idx, " has ", (int64_t)pts.size(), " vertices");
        return;
    }

    UtilityFunctions::print(
        label, " face ", face_idx, ": ",
        "(",
        CGAL::to_double(pts[0].x()), ", ",
        CGAL::to_double(pts[0].y()), ", ",
        CGAL::to_double(pts[0].z()), ") ",
        "(",
        CGAL::to_double(pts[1].x()), ", ",
        CGAL::to_double(pts[1].y()), ", ",
        CGAL::to_double(pts[1].z()), ") ",
        "(",
        CGAL::to_double(pts[2].x()), ", ",
        CGAL::to_double(pts[2].y()), ", ",
        CGAL::to_double(pts[2].z()), ")"
    );
}






// ----------------------------
// Godot → CGAL
// ----------------------------
static CgalMesh godot_to_cgal(Ref<ArrayMesh> mesh) {
    CgalMesh out;

    if (mesh.is_null() || mesh->get_surface_count() == 0) {
        return out;
    }

    Array arrays = mesh->surface_get_arrays(0);

    PackedVector3Array verts = arrays[godot::Mesh::ARRAY_VERTEX];
    PackedInt32Array indices = arrays[godot::Mesh::ARRAY_INDEX];

    std::map<PointKey, CgalMesh::Vertex_index> vertex_map;

    auto get_or_create_vertex = [&](const Vector3 &v) -> CgalMesh::Vertex_index {
        PointKey key = make_point_key(v);
        auto it = vertex_map.find(key);
        if (it != vertex_map.end()) {
            return it->second;
        }

        CgalMesh::Vertex_index vi = out.add_vertex(Point(v.x, v.y, v.z));
        vertex_map[key] = vi;
        return vi;
    };

    if (indices.size() >= 3) {
        // Indexed triangle mesh path.
        for (int i = 0; i + 2 < indices.size(); i += 3) {
            int ia = indices[i];
            int ib = indices[i + 1];
            int ic = indices[i + 2];

            if (ia < 0 || ib < 0 || ic < 0 ||
                ia >= verts.size() || ib >= verts.size() || ic >= verts.size()) {
                UtilityFunctions::print("Skipping out-of-range face indices: ", (int64_t)ia, ", ", (int64_t)ib, ", ", (int64_t)ic);
                continue;
            }

            Vector3 va = verts[ia];
            Vector3 vb = verts[ib];
            Vector3 vc = verts[ic];

            CgalMesh::Vertex_index a = get_or_create_vertex(va);
            CgalMesh::Vertex_index b = get_or_create_vertex(vb);
            CgalMesh::Vertex_index c = get_or_create_vertex(vc);

            auto f = out.add_face(a, b, c);
            if (f == CgalMesh::null_face()) {
                UtilityFunctions::print("CGAL rejected a face during conversion");
            }
        }
    } else {
        // Non-indexed fallback: treat the vertex array as raw triangle soup.
        for (int i = 0; i + 2 < verts.size(); i += 3) {
            Vector3 va = verts[i];
            Vector3 vb = verts[i + 1];
            Vector3 vc = verts[i + 2];

            CgalMesh::Vertex_index a = get_or_create_vertex(va);
            CgalMesh::Vertex_index b = get_or_create_vertex(vb);
            CgalMesh::Vertex_index c = get_or_create_vertex(vc);

            auto f = out.add_face(a, b, c);
            if (f == CgalMesh::null_face()) {
                UtilityFunctions::print("CGAL rejected a soup face during conversion");
            }
        }
    }

    out.collect_garbage();
    return out;
}

// ----------------------------
// CGAL → Godot
// ----------------------------
static Ref<ArrayMesh> cgal_to_godot(const CgalMesh &mesh) {
    PackedVector3Array verts;
    PackedInt32Array indices;

    std::map<CgalMesh::Vertex_index, int> index_map;

    int idx = 0;
    for (auto v : mesh.vertices()) {
        const Point &p = mesh.point(v);
        verts.push_back(point_to_vector3(p));
        index_map[v] = idx++;
    }

    for (auto f : mesh.faces()) {
        std::vector<int> face;
        for (auto v : CGAL::vertices_around_face(mesh.halfedge(f), mesh)) {
            face.push_back(index_map[v]);
        }

        if (face.size() == 3) {
            indices.push_back(face[0]);
            indices.push_back(face[1]);
            indices.push_back(face[2]);
        }
    }

    Array arrays;
    arrays.resize(godot::Mesh::ARRAY_MAX);
    arrays[godot::Mesh::ARRAY_VERTEX] = verts;
    arrays[godot::Mesh::ARRAY_INDEX] = indices;

    Ref<ArrayMesh> out = memnew(ArrayMesh);
    out->add_surface_from_arrays(godot::Mesh::PRIMITIVE_TRIANGLES, arrays);

    return out;
}

// ----------------------------
// Boolean Difference (Mesh) with Validation
// ----------------------------
Ref<ArrayMesh> CGALWrapper::boolean_difference(Ref<ArrayMesh> a, Ref<ArrayMesh> b) {
    CgalMesh result;

    try {
        if (a.is_null() || b.is_null()) {
            UtilityFunctions::print("Null mesh passed");
            return cgal_to_godot(result);
        }

        if (a->get_surface_count() == 0 || b->get_surface_count() == 0) {
            UtilityFunctions::print("Empty mesh");
            return cgal_to_godot(result);
        }

        CgalMesh ma = godot_to_cgal(a);
        CgalMesh mb = godot_to_cgal(b);

        if (!CGAL::is_triangle_mesh(ma) || !CGAL::is_triangle_mesh(mb)) {
            UtilityFunctions::print("Input is not a triangle mesh");
            return cgal_to_godot(result);
        }

        if (PMP::does_self_intersect(ma) || PMP::does_self_intersect(mb)) {
            UtilityFunctions::print("Input mesh self-intersects");
            return cgal_to_godot(result);
        }

        if (!CGAL::is_closed(ma) || !CGAL::is_closed(mb)) {
            UtilityFunctions::print("Input mesh is not closed");
            return cgal_to_godot(result);
        }

        if (CGAL::is_closed(ma) && !PMP::is_outward_oriented(ma)) {
            PMP::reverse_face_orientations(ma);
        }

        if (CGAL::is_closed(mb) && !PMP::is_outward_oriented(mb)) {
            PMP::reverse_face_orientations(mb);
        }

        PMP::corefine_and_compute_difference(ma, mb, result);
        return cgal_to_godot(result);

    } catch (const std::exception &e) {
        UtilityFunctions::print("CGAL exception: ", e.what());
    } catch (...) {
        UtilityFunctions::print("Unknown CGAL crash");
    }

    return cgal_to_godot(result);
}

// ----------------------------
// Boolean Difference (Shape)
// ----------------------------
Ref<ArrayMesh> CGALWrapper::boolean_difference_shape(
    Ref<ConcavePolygonShape3D> a,
    Ref<ConcavePolygonShape3D> b
) {
    PackedVector3Array fa = a->get_faces();
    PackedVector3Array fb = b->get_faces();

    Ref<ArrayMesh> ma = memnew(ArrayMesh);
    Ref<ArrayMesh> mb = memnew(ArrayMesh);

    Array arr_a;
    arr_a.resize(godot::Mesh::ARRAY_MAX);
    arr_a[godot::Mesh::ARRAY_VERTEX] = fa;
    ma->add_surface_from_arrays(godot::Mesh::PRIMITIVE_TRIANGLES, arr_a);

    Array arr_b;
    arr_b.resize(godot::Mesh::ARRAY_MAX);
    arr_b[godot::Mesh::ARRAY_VERTEX] = fb;
    mb->add_surface_from_arrays(godot::Mesh::PRIMITIVE_TRIANGLES, arr_b);

    return boolean_difference(ma, mb);
}