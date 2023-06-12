#include "ScrollableContainer.hpp"

#include <Essa/GUI/EML/Loader.hpp>

namespace GUI {

ScrollableContainer::ScrollableContainer() {
    on_scroll = [&]() { m_widget->set_raw_position(scroll_offset().to_point()); };
}

Util::Size2i ScrollableContainer::content_size() const { return m_widget->total_size(); }

Widget::EventHandlerResult ScrollableContainer::do_handle_event(Event const& event) {
    if (Widget::do_handle_event(event) == EventHandlerResult::Accepted) {
        return EventHandlerResult::Accepted;
    }
    return m_widget->do_handle_event(event.relativized(raw_position().to_vector()));
}

void ScrollableContainer::draw(Gfx::Painter& painter) const {
    m_widget->do_draw(painter);
    ScrollableWidget::draw_scrollbar(painter);
}

void ScrollableContainer::do_relayout() {
    Widget::do_relayout();

    // 1. Do first relayout with the full available size.
    m_widget->set_raw_size(raw_size());
    m_widget->do_relayout();

    // 2. If the widget doesn't fit, relayout once more (because we will add
    // scrollbars, which will change available size).
    auto actually_needed_size = raw_size();
    bool needs_horizontal_scrollbar = false;
    bool needs_vertical_scrollbar = false;
    if (m_widget->total_size().x() > raw_size().x()) {
        actually_needed_size.set_y(actually_needed_size.x() - 5);
        needs_horizontal_scrollbar = true;
    }
    if (m_widget->total_size().y() > raw_size().y()) {
        actually_needed_size.set_x(actually_needed_size.x() - 5);
        needs_vertical_scrollbar = true;
    }
    fmt::print("{} != {} ?\n", actually_needed_size, raw_size());
    if (actually_needed_size != raw_size()) {
        auto size = m_widget->total_size();
        if (needs_horizontal_scrollbar) {
            size.set_y(size.y() - 5);
        }
        if (needs_vertical_scrollbar) {
            size.set_x(size.x() - 5);
        }
        m_widget->set_raw_size(size);
        m_widget->do_relayout();
    }
}

void ScrollableContainer::dump(unsigned depth) {
    Widget::dump(depth);
    for (unsigned i = 0; i < depth; i++)
        std::cout << "-   ";

    if (!is_visible())
        return;
    ++depth;
    m_widget->dump(depth);
}

EML::EMLErrorOr<void> ScrollableContainer::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));
    m_widget = TRY(object.require_and_construct_object<Widget>("widget", loader));
    m_widget->set_widget_tree_root(widget_tree_root());
    m_widget->init();
    return {};
}

EML_REGISTER_CLASS(ScrollableContainer)

}
