#pragma once

#include "Extensions.hpp"
#include "OpenGL.hpp"
#include "Texture.hpp"
#include <EssaUtil/Matrix.hpp>
#include <bits/utility.h>
#include <concepts>
#include <fmt/core.h>
#include <string_view>
#include <tuple>
#include <type_traits>

namespace llgl {

enum class ShaderType {
    Fragment,
    Vertex
};

struct TextureUnit {
    int id = 0;
    llgl::Texture const* texture = nullptr;
};

namespace Detail {

inline void set_uniform(int location, TextureUnit const& tu) {
    glActiveTexture(GL_TEXTURE0 + tu.id);
    llgl::Texture::bind(tu.texture);
    glUniform1i(location, tu.id);
}

inline void set_uniform(int location, Util::Color const& color) {
    Util::Colorf colorf { color };
    glUniform4f(location, colorf.r, colorf.g, colorf.b, colorf.a);
}

inline void set_uniform(int location, Util::Matrix4x4f const& matrix) {
    glUniformMatrix4fv(location, 1, true, matrix.raw_data());
}

inline void set_uniform(int location, Util::Vector2f const& vec) {
    glUniform2f(location, vec.x(), vec.y());
}

inline void set_uniform(int location, Util::Vector3f const& vec) {
    glUniform3f(location, vec.x(), vec.y(), vec.z());
}

inline void set_uniform(int location, int value) {
    glUniform1i(location, value);
}

inline void set_uniform(int location, bool value) {
    glUniform1i(location, value);
}

inline void set_uniform(int location, float value) {
    glUniform1f(location, value);
}

} // namespace Detail

namespace Detail {

template<class T1, size_t... Id1, class T2, size_t... Id2>
decltype(auto) concat_tuples_impl(T1 const& t1, std::index_sequence<Id1...>,
    T2 const& t2,
    std::index_sequence<Id2...>) noexcept {
    return std::make_tuple(get<Id1>(t1)..., get<Id2>(t2)...);
}

template<class... T1, class... T2>
auto concat_tuples(std::tuple<T1...> const& t1, std::tuple<T2...> const& t2) {
    return concat_tuples_impl(t1, std::make_index_sequence<sizeof...(T1)>(), t2,
        std::make_index_sequence<sizeof...(T2)>());
}

} // namespace Detail

template<class... Members>
class UniformMapping {
public:
    UniformMapping(std::pair<const char*, Members>&&... pairs)
        : m_pairs(std::move(pairs)...) { }

    void bind(int program, auto const& uniforms) {
        bind_impl(program, uniforms, std::make_index_sequence<sizeof...(Members)>());
    }

    template<class... Members2>
    auto operator|(UniformMapping<Members2...> const& other) const {
        return UniformMapping<Members..., Members2...>(Detail::concat_tuples(m_pairs, other.m_pairs));
    }

private:
    template<class... M2>
    friend class UniformMapping;

    template<size_t... Idx>
    void bind_impl(int program, auto const& uniforms, std::index_sequence<Idx...>) {
        ((Detail::set_uniform(glGetUniformLocation(program, std::get<Idx>(m_pairs).first), uniforms.*std::get<Idx>(m_pairs).second)), ...);
    }

    explicit UniformMapping(std::tuple<std::pair<const char*, Members>...> pairs)
        : m_pairs(std::move(pairs)) { }

    std::tuple<std::pair<const char*, Members>...> m_pairs;
};

template<class... Members>
UniformMapping<Members...> make_uniform_mapping(std::pair<const char*, Members>&&... members) {
    return { std::move(members)... };
}

class Shader;

template<class T>
concept ShaderImpl = std::is_base_of_v<Shader, T> && requires(T t, ShaderType st) {
    typename T::Vertex;
    typename T::Uniforms;
    { t.source(st) } -> std::convertible_to<std::string_view>;
    { std::declval<typename T::Uniforms>().mapping() };
};

class Shader {
public:
    ~Shader() {
        if (m_program)
            glDeleteProgram(m_program);
    }

    unsigned program() const { return m_program; }
    void set_program(unsigned p) { m_program = p; }

private:
    unsigned m_program = 0;
};

namespace Detail {

unsigned compile_shader(GLenum type, std::string_view source);

template<class... Args>
requires(std::is_same_v<Args, unsigned>&&...) unsigned link_shader(
    Args... ids) {
    assert((ids && ...));
    std::cout << "Program: Linking " << sizeof...(Args) << " shader objects"
              << std::endl;
    auto id = glCreateProgram();
    (glAttachShader(id, ids), ...);

    // TODO: Attrib locations if supported
    glLinkProgram(id);
    GLint success = 0;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        GLint max_length = 0;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &max_length);
        std::string error_message;
        error_message.resize(max_length);
        glGetProgramInfoLog(id, max_length, &max_length, error_message.data());
        std::cout << "Program: Failed to link shader program: " << error_message
                  << std::endl;
        glDeleteProgram(id);
        return 0;
    }

    (glDetachShader(id, ids), ...);
    return id;
}

template<ShaderImpl Shader>
inline void bind_shader(Shader& shader, typename Shader::Uniforms const& uniforms) {
    if (!shader.program()) {
        auto fragment_shader = compile_shader(GL_FRAGMENT_SHADER, shader.source(ShaderType::Fragment));
        auto vertex_shader = compile_shader(GL_VERTEX_SHADER, shader.source(ShaderType::Vertex));
        shader.set_program(link_shader(fragment_shader, vertex_shader));
    }

    glUseProgram(shader.program());
    using Uniforms = typename Shader::Uniforms;
    Uniforms::mapping().bind(shader.program(), uniforms);
}

} // namespace Detail

} // namespace llgl
