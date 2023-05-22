#include "DraggableView2D.hpp"

namespace GUI {

llgl::Transform DraggableView2D::transform() const {
    return llgl::Transform {}.translate_2d((raw_size().cast<float>().to_vector() / 2.f).rounded()).scale(m_zoom).translate_2d(-m_offset);
}

llgl::Transform DraggableView2D::inverse_transform() const {
    return llgl::Transform {}
        .translate_2d(m_offset)
        .scale(1 / m_zoom)
        .translate_2d(-(raw_size().cast<float>().to_vector() / 2.f).rounded());
}

Util::Rectf DraggableView2D::visible_area() const {
    return { m_offset.x() - raw_size().x() / 2.f / zoom(), m_offset.y() - raw_size().y() / 2.f / zoom(), raw_size().x() / zoom(),
             raw_size().y() / zoom() };
}

Util::Point2f DraggableView2D::screen_to_world(Util::Point2i const& screen) const {
    return inverse_transform().transform_point_2d(screen.cast<float>());
}

Util::Point2i DraggableView2D::world_to_screen(Util::Point2f const& world) const {
    return transform().transform_point_2d(world).cast<int>();
}

Widget::EventHandlerResult DraggableView2D::on_mouse_scroll(Event::MouseScroll const& event) {
    if (m_zoom_enabled) {
        if (event.delta() > 0) {
            m_zoom *= 2;
            auto delta = (event.local_position() - raw_size().to_vector() / 2).cast<float>() / m_zoom;
            m_offset += delta.to_vector();
        }
        else {
            m_zoom /= 2;
            auto delta = (event.local_position() - raw_size().to_vector() / 2).cast<float>() / m_zoom;
            m_offset -= delta.to_vector() / 2.f;
        }
    }
    return Widget::EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult DraggableView2D::on_mouse_button_press(Event::MouseButtonPress const& event) {
    if (event.button() != m_pan_button) {
        return Widget::EventHandlerResult::NotAccepted;
    }
    m_drag_start_mouse = event.local_position();
    m_drag_start_offset = m_offset;
    m_dragging = true;
    return EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult DraggableView2D::on_mouse_button_release(Event::MouseButtonRelease const& event) {
    if (event.button() != m_pan_button) {
        return Widget::EventHandlerResult::NotAccepted;
    }
    m_dragging = false;
    m_actually_dragging = false;
    return Widget::EventHandlerResult::NotAccepted;
}

Widget::EventHandlerResult DraggableView2D::on_mouse_move(Event::MouseMove const& event) {
    auto delta = event.local_position() - m_drag_start_mouse;
    if (m_dragging && delta.length_squared() > 400) {
        m_actually_dragging = true;
    }
    if (m_actually_dragging) {
        m_offset = m_drag_start_offset - delta.cast<float>() / m_zoom;
    }
    return EventHandlerResult::NotAccepted;
}

}
