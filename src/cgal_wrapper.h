#pragma once

#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/core/class_db.hpp>

using namespace godot;

class CGALWrapper : public Object {
    GDCLASS(CGALWrapper, Object);

protected:
    static void _bind_methods();

public:
    Ref<ArrayMesh> boolean_difference(Ref<ArrayMesh> a, Ref<ArrayMesh> b);
};