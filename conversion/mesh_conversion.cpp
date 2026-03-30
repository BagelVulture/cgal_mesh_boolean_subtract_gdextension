
#include "mesh_conversion.hpp"
#include <unordered_map>

namespace cgal_gdextension {

SurfaceMesh arrays_to_surface_mesh(
    const std::vector<std::array<double,3>>& vertices,
    const std::vector<std::array<int,3>>& faces){

    SurfaceMesh mesh;
    std::vector<SurfaceMesh::Vertex_index> vmap;

    for(auto &v:vertices)
        vmap.push_back(mesh.add_vertex(Point3(v[0],v[1],v[2])));

    for(auto &f:faces)
        mesh.add_face(vmap[f[0]],vmap[f[1]],vmap[f[2]]);

    return mesh;
}

void surface_mesh_to_arrays(
    const SurfaceMesh& mesh,
    std::vector<std::array<double,3>>& vertices,
    std::vector<std::array<int,3>>& faces){

    std::unordered_map<SurfaceMesh::Vertex_index,int> map;
    int i=0;

    for(auto v:mesh.vertices()){
        auto p=mesh.point(v);
        vertices.push_back({p.x(),p.y(),p.z()});
        map[v]=i++;
    }

    for(auto f:mesh.faces()){
        std::vector<int> face;
        for(auto v:CGAL::vertices_around_face(mesh.halfedge(f),mesh))
            face.push_back(map[v]);

        if(face.size()>=3)
            faces.push_back({face[0],face[1],face[2]});
    }
}

}
