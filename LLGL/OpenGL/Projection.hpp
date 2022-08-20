#pragma once

#include <EssaUtil/Matrix.hpp>
#include <LLGL/OpenGL/Transform.hpp>
#include <cassert>

namespace llgl {

class Projection {
public:
    static Projection ortho(opengl::OrthoArgs, Util::Recti viewport);
    static Projection perspective(opengl::PerspectiveArgs, Util::Recti viewport);

    Projection() = default;

    Projection(Util::Matrix4x4f matrix, Util::Recti viewport)
        : m_matrix(matrix)
        , m_viewport(viewport) { }

    Util::Recti viewport() const { return m_viewport; }
    Util::Matrix4x4f matrix() const { return m_matrix; }

private:
    Util::Matrix4x4f m_matrix;
    Util::Recti m_viewport;
};

}
