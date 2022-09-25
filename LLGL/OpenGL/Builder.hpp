#pragma once

#include "LLGL/OpenGL/PrimitiveType.hpp"
#include <LLGL/OpenGL/MappedVertex.hpp>
#include <LLGL/OpenGL/Renderer.hpp>
#include <LLGL/OpenGL/VertexArray.hpp>

namespace llgl {

struct RenderRange {
    size_t first;
    size_t size;
    llgl::PrimitiveType type;
};

// A class which simplifies generating VAOs and abstract away
// all modifications to it
template<class Vertex, class RR>
requires std::is_base_of_v<RenderRange, RR>
class Builder {
public:
    using MappedVertex = llgl::MappedVertex<Vertex>;
    using StoredRenderRange = RR;

    virtual ~Builder() = default;

    void add(std::initializer_list<Vertex> v) {
        m_vertices.insert(m_vertices.begin(), v.begin(), v.end());
        set_modified();
    }

    void add(Vertex v) {
        m_vertices.push_back(std::move(v));
        set_modified();
    }

    void render(llgl::Renderer& renderer) const {
        if (m_was_modified) {
            m_vao.upload_vertices(m_vertices);
        }
        if (m_ranges.empty()) {
            render_range(renderer, m_vao, { 0, m_vertices.size(), llgl::PrimitiveType::Triangles });
        }
        else {
            for (auto const& range : m_ranges) {
                render_range(renderer, m_vao, range);
            }
        }
    }

protected:
    static Vertex create_vertex(auto position, auto color, auto tex_coord, auto normal) {
        Vertex vertex;
        llgl::MappedVertex<Vertex> mapped { vertex };
        if constexpr (MappedVertex::HasPosition)
            mapped.set_position(position);
        if constexpr (MappedVertex::HasColor)
            mapped.set_color(color);
        if constexpr (MappedVertex::HasTexCoord)
            mapped.set_tex_coord(tex_coord);
        if constexpr (MappedVertex::HasNormal)
            mapped.set_normal(normal);
        return vertex;
    }

    void set_modified() { m_was_modified = true; }

    template<class... Args>
    void add_render_range_for_last_vertices(size_t count, llgl::PrimitiveType pt, Args&&... args) {
        m_ranges.push_back(StoredRenderRange { m_vertices.size() - count, count, pt, std::forward<Args>(args)... });
    }

private:
    virtual void render_range(llgl::Renderer&, llgl::VertexArray<Vertex> const&, StoredRenderRange const&) const {};

    mutable llgl::VertexArray<Vertex> m_vao;
    std::vector<Vertex> m_vertices;
    mutable bool m_was_modified = false;
    std::vector<StoredRenderRange> m_ranges;
};

}
