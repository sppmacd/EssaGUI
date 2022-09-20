#include <EssaUtil/Color.hpp>
#include <EssaUtil/Config.hpp>
#include <EssaUtil/Matrix.hpp>
#include <EssaUtil/Vector.hpp>
#include <LLGL/Core/Transform.hpp>
#include <LLGL/OpenGL/Error.hpp>
#include <LLGL/OpenGL/Extensions.hpp>
#include <LLGL/OpenGL/Projection.hpp>
#include <LLGL/OpenGL/Renderer.hpp>
#include <LLGL/OpenGL/Shader.hpp>
#include <LLGL/OpenGL/Transform.hpp>
#include <LLGL/OpenGL/Vertex.hpp>
#include <LLGL/OpenGL/VertexArray.hpp>
#include <LLGL/Window/Window.hpp>
#include <fmt/ostream.h>

auto VertexShader =
    R"~~~(
#version 410 core

layout(location=0) in vec2 position;
layout(location=1) in vec4 color;
out vec4 f_color;
uniform mat4 matrix;

void main()
{
    f_color = color;
    gl_Position = matrix * vec4(position,0,1);
}
)~~~";

auto FragmentShader =
    R"~~~(
#version 410 core

in vec4 f_color;
uniform vec4 otherColor;

void main()
{
    gl_FragColor = f_color + otherColor;
}    
)~~~";

struct MyShader : public llgl::Shader {
    using Vertex = llgl::Vertex<Util::Vector2f, Util::Colorf>;

    llgl::Uniform<Util::Color> other_color { "otherColor" };
    llgl::Uniform<Util::Matrix4x4f> matrix { "matrix" };

    auto uniforms() {
        return llgl::UniformList { other_color, matrix };
    }

    std::string_view source(llgl::ShaderType type) {
        switch (type) {
        case llgl::ShaderType::Fragment:
            return FragmentShader;
        case llgl::ShaderType::Vertex:
            return VertexShader;
        }
        ESSA_UNREACHABLE;
    }
};

int main() {
    llgl::Window window { { 400, 400 }, "test" };
    llgl::opengl::enable_debug_output();

    llgl::VertexArray<MyShader::Vertex> varr;
    MyShader::Vertex verts[] {
        { { 50, 50 }, Util::Colors::Red },
        { { 100, 50 }, Util::Colors::Green },
        { { 50, 100 }, Util::Colors::Blue }
    };
    varr.upload_vertices(verts);

    llgl::Renderer renderer { 0 };

    MyShader shader;
    shader.other_color = Util::Colors::Red;
    shader.matrix = llgl::Transform {}.rotate_z(0.1).matrix() * llgl::Projection::ortho({ { 0, 0, 400, 400 } }, {}).matrix();

    while (true) {
        llgl::Event event;
        while (window.poll_event(event)) {
            ;
        }
        llgl::set_viewport({ 0, 0, 400, 400 });
        renderer.clear();
        renderer.draw_vertices(varr, llgl::DrawState { shader, llgl::PrimitiveType::Triangles });
        window.display();
    }
}