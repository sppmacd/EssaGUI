#pragma once

#include "Widget.hpp"
#include <LLGL/OpenGL/Projection.hpp>
#include <LLGL/Renderer/Camera.hpp>

namespace GUI {

class WorldView;

// This class ensures that everything in the scope will be drawn using
// the world states (that is, in 3D, with depth enabled). This must be
// used when drawing actual 3D things, otherwise the default widget
// states will be used.
class WorldDrawScope {
public:
    // Can be used for doing multiple layers.
    enum class ClearDepth {
        Yes,
        No
    };

    explicit WorldDrawScope(WorldView const& view, ClearDepth = ClearDepth::No);
    ~WorldDrawScope();

    static void verify();
    static WorldDrawScope const* current();

private:
    static WorldDrawScope const* s_current;

    WorldView const& m_world_view;
    WorldDrawScope const* m_parent = nullptr;
    llgl::Projection m_previous_projection;
};

class WorldView : public Widget {
private:
    friend WorldDrawScope;

    virtual llgl::Camera camera() const = 0;
    virtual void draw(GUI::Window&) const = 0;
};

}
