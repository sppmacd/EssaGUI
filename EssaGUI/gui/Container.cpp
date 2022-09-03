#include "Container.hpp"

#include "Application.hpp"
#include "EssaGUI/eml/Loader.hpp"

#include <EssaGUI/eml/Loader.hpp>
#include <EssaUtil/Config.hpp>
#include <cassert>
#include <iostream>
#include <vector>

namespace GUI {

void Layout::set_multipliers(std::initializer_list<float> list) {
    m_multipliers.clear();

    for (auto& l : list) {
        m_multipliers.push_back(l);
    }
}

EML::EMLErrorOr<void> Layout::load_from_eml_object(EML::Object const& object, EML::Loader&) {
    m_padding = TRY(object.get_property("padding", 0.0).to_double());
    return {};
}

void BoxLayout::run(Container& container) {
    // std::cout << "BOXLAYOUT " << m_container.size().x() << "," << m_container.size().y() << " spacing=" << m_spacing << std::endl;
    auto vec2f_main_coord_by_orientation = [this](auto vec) -> auto{
        if constexpr (requires() { vec.x(); }) {
            if (m_orientation == Orientation::Horizontal)
                return vec.x();
            return vec.y();
        }
        else {
            if (m_orientation == Orientation::Horizontal)
                return vec.x;
            return vec.y;
        }
    };
    auto vec2f_cross_coord_by_orientation = [this](auto vec) -> auto{
        if (m_orientation == Orientation::Vertical)
            return vec.x();
        return vec.y();
    };
    auto convert_vector_by_orientation = [this](Util::Vector2f vec) {
        if (m_orientation == Orientation::Vertical)
            return Util::Vector2f { vec.y(), vec.x() };
        return vec;
    };

    auto total_spacing_size = (m_spacing * (container.widgets().size() - 1));

    // 1. Compute widget size (in main axis) if it has fixed size
    for (auto& w : container.widgets()) {
        if (!w->is_visible())
            continue;

        float size = 0;
        switch (vec2f_main_coord_by_orientation(w->input_size()).unit()) {
        case Length::Px:
        case Length::PxOtherSide:
            // std::cout << "test" << std::endl;
            size = vec2f_main_coord_by_orientation(w->input_size()).value();
            break;
        case Length::Percent:
            // std::cout << size << std::endl;
            size = vec2f_main_coord_by_orientation(w->input_size()).value() * (vec2f_main_coord_by_orientation(container.size()) - total_spacing_size) / 100.0;
            break;
        case Length::Auto:
            size = 0;
            break;
        case Length::Initial:
            ESSA_UNREACHABLE;
        }
        w->set_raw_size(convert_vector_by_orientation({ size, vec2f_cross_coord_by_orientation(container.size()) - m_padding * 2 }));
    }

    // 2. Compute size available for auto-sized widgets
    float available_size_for_autosized_widgets = vec2f_main_coord_by_orientation(container.size()) - m_padding * 2;
    size_t autosized_widget_count = 0;
    for (auto& w : container.widgets()) {
        if (!w->is_visible())
            continue;
        if (vec2f_main_coord_by_orientation(w->input_size()).unit() == Length::Auto)
            autosized_widget_count++;
        else
            available_size_for_autosized_widgets -= vec2f_main_coord_by_orientation(w->size()) + m_spacing;
    }

    // 3. Set autosized widgets' size + arrange widgets
    float autosized_widget_size = (available_size_for_autosized_widgets - (m_spacing * (autosized_widget_count - 1))) / autosized_widget_count;
    if (autosized_widget_size < 0)
        autosized_widget_size = 0;
    size_t index = 0;
    switch (m_content_alignment) {
    case ContentAlignment::BoxStart: {
        float current_position = 0;
        for (auto& w : container.widgets()) {
            if (!w->is_visible())
                continue;
            if (vec2f_main_coord_by_orientation(w->input_size()).unit() == Length::Auto)
                w->set_raw_size(convert_vector_by_orientation({ autosized_widget_size, vec2f_cross_coord_by_orientation(container.size()) - m_padding * 2 }));
            w->set_raw_position(convert_vector_by_orientation({ vec2f_main_coord_by_orientation(container.position()) + current_position + m_padding,
                vec2f_cross_coord_by_orientation(container.position()) + m_padding }));
            current_position += vec2f_main_coord_by_orientation(w->size()) + m_spacing;
            index++;
        }
    } break;
    case ContentAlignment::BoxEnd: {
        float current_position = vec2f_main_coord_by_orientation(container.size());
        for (auto it = container.widgets().rbegin(); it != container.widgets().rend(); it++) {
            auto& w = *it;
            if (!w->is_visible())
                continue;
            if (vec2f_main_coord_by_orientation(w->input_size()).unit() == Length::Auto)
                w->set_raw_size(convert_vector_by_orientation({ autosized_widget_size, vec2f_cross_coord_by_orientation(container.size()) - m_padding * 2 }));
            current_position -= vec2f_main_coord_by_orientation(w->size()) + m_spacing;
            w->set_raw_position(convert_vector_by_orientation({ vec2f_main_coord_by_orientation(container.position()) + current_position + m_padding,
                vec2f_cross_coord_by_orientation(container.position()) + m_padding }));
            index++;
        }
    } break;
    }

    /*for(auto& w : widgets())
    {
        std::cout << w->input_size().x() << "," << w->input_size().y() << " @ " << w->input_position().x() << "," << w->input_position().y() << " ----> ";
        std::cout << w->size().x() << "," << w->size().y() << " @ " << w->position().x() << "," << w->position().y() << std::endl;
    }*/
}

EML::EMLErrorOr<void> BoxLayout::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Layout::load_from_eml_object(object, loader));

    m_spacing = TRY(object.get_property("spacing", 0.0).to_double());
    return {};
}

EML_REGISTER_CLASS(HorizontalBoxLayout);
EML_REGISTER_CLASS(VerticalBoxLayout);

void BasicLayout::run(Container& container) {
    auto resolve_position = [&](double container_size, double widget_size, Length const& input_position) -> float {
        switch (input_position.unit()) {
        case Length::Px:
            return input_position.value();
        case Length::PxOtherSide:
            return container_size - widget_size - input_position.value();
        case Length::Percent:
            return widget_size * container_size / 100.0;
        default:
            return 0;
        }
    };

    auto resolve_size = [&](double container_size, Length const& input_size) -> float {
        switch (input_size.unit()) {
        case Length::Px:
        case Length::PxOtherSide:
            return input_size.value();
        case Length::Percent:
            return input_size.value() * container_size / 100.0;
        default:
            return 0;
        }
    };

    for (auto& w : container.widgets()) {
        auto input_position = w->input_position();
        w->set_raw_size({ resolve_size(container.size().x(), w->input_size().x), resolve_size(container.size().y(), w->input_size().y) });
        auto x = resolve_position(container.size().x(), w->size().x(), input_position.x);
        auto y = resolve_position(container.size().y(), w->size().y(), input_position.y);
        w->set_raw_position({ x + container.position().x(), y + container.position().y() });
    }
}

void Container::do_relayout() {
    Widget::do_relayout();
    for (auto const& w : m_widgets) {
        if (w->is_visible())
            w->do_relayout();
    }
}

void Container::do_draw(GUI::Window& window) const {
    Widget::do_draw(window);
    for (auto const& w : m_widgets) {
        if (w->is_visible())
            w->do_draw(window);
    }
}

void Container::do_handle_event(Event& event) {
    for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); it++) {
        auto& widget = *it;
        if (!widget->is_visible() || !widget->is_enabled())
            continue;

        widget->do_handle_event(event);
        if (event.is_handled())
            break;
    }
    // FIXME: Proper stacking contexts
    if (!event.is_handled())
        Widget::do_handle_event(event);
}

void Container::handle_event(Event& event) {
    if (event.type() == llgl::Event::Type::KeyPress && event.event().key.keycode == llgl::KeyCode::Tab) {
        if (focus_next_widget(false))
            event.set_handled();
    }
}

std::optional<size_t> Container::focused_widget_index(bool recursive) const {
    for (size_t c = 0; auto& w : m_widgets) {
        if (widget_tree_root().focused_widget() == w.get())
            return c;
        if (recursive) {
            auto container = dynamic_cast<Container*>(w.get());
            if (container) {
                auto index = container->focused_widget_index(true);
                if (index.has_value())
                    return c;
            }
        }
        c++;
    }
    return {};
}

bool Container::focus_next_widget(bool called_from_child) {
    auto index_opt = focused_widget_index(called_from_child);
    if (!index_opt.has_value())
        return false;
    // if (!called_from_child)
    //     std::cout << "======================RUNNING TAB FOCUS======================" << std::endl;
    auto index = index_opt.value();

    if (m_widgets[index]->steals_focus())
        return false;

    do {
        index++;
        // dump(0);
        // std::cout << "focus_next_widget " << called_from_child << ": " << index << " " << m_widgets.size() << std::endl;
        if (index == m_widgets.size()) {
            if (m_parent && !steals_focus())
                m_parent->focus_next_widget(true);
            else
                focus_first_child_or_self();
            break;
        }
        else if (m_widgets[index]->is_visible() && m_widgets[index]->accepts_focus() && !m_widgets[index]->steals_focus()) {
            // std::cout << "Focusing first_child_or_self: " << index << std::endl;
            m_widgets[index]->focus_first_child_or_self();
            break;
        }
    } while (true);
    return true;
}

void Container::focus_first_child_or_self() {
    if (m_widgets.size() == 0) {
        std::cout << "No widgets to focus @" << this << std::endl;
        return;
    }

    for (auto& widget : m_widgets) {
        if (widget->is_visible() && widget->accepts_focus() && !widget->steals_focus()) {
            widget->focus_first_child_or_self();
            break;
        }
    }
}

bool Container::accepts_focus() const {
    if (steals_focus())
        return false;
    for (auto& widget : m_widgets) {
        if (widget->is_visible() && widget->accepts_focus())
            return true;
    }
    return false;
}

void Container::do_update() {
    Widget::do_update();
    for (auto const& w : m_widgets)
        w->do_update();
}

void Container::relayout() {
    if (m_widgets.empty())
        return;
    for (auto& w : m_widgets) {
        auto initial_size = w->initial_size();
        if (w->m_input_size.x == Length::Initial)
            w->m_input_size.x = initial_size.x;
        if (w->m_input_size.y == Length::Initial)
            w->m_input_size.y = initial_size.y;
    }
    if (!m_layout) {
        std::cout << "Not trying to relayout widget without layout!" << std::endl;
        dump(0);
        assert(false);
    }
    if (m_layout->padding() == 0)
        m_layout->set_padding(intrinsic_padding());
    m_layout->run(*this);
}

void Container::dump(unsigned depth) {
    Widget::dump(depth);
    ++depth;
    for (unsigned i = 0; i < depth; i++)
        std::cout << "-   ";
    if (m_layout) {
        std::cout << "layout: " << typeid(*m_layout).name() << std::endl;
    }
    else {
        std::cout << "layout: NONE!" << std::endl;
    }
    if (!is_visible())
        return;
    for (auto& w : m_widgets) {
        w->dump(depth);
    }
}

Widget* Container::find_widget_by_id(std::string_view id) const {
    // FIXME: Make it not O(n)
    for (auto& w : m_widgets) {
        if (w->id() == id)
            return w.get();
    }
    return nullptr;
}

std::vector<Widget*> Container::find_widgets_by_class_name(std::string_view class_name) const {
    std::vector<Widget*> result;
    for (auto& w : m_widgets) {
        if (w->class_name() == class_name)
            result.push_back(w.get());
    }
    return result;
}

void Container::m_find_widgets_by_class_name_recursively_helper(std::string_view class_name, std::vector<Widget*>& vec) const {
    auto widget_vec = find_widgets_by_class_name(class_name);
    for (auto& w : widget_vec)
        vec.push_back(w);

    for (auto& w : m_widgets) {
        auto container = dynamic_cast<Container*>(w.get());
        if (container) {
            container->m_find_widgets_by_class_name_recursively_helper(class_name, vec);
        }
    }
}

std::vector<Widget*> Container::find_widgets_by_class_name_recursively(std::string_view class_name) const {
    std::vector<Widget*> result;
    m_find_widgets_by_class_name_recursively_helper(class_name, result);

    return result;
}

Widget* Container::find_widget_by_id_recursively(std::string_view id) const {
    auto widget = find_widget_by_id(id);
    if (widget)
        return widget;
    for (auto& w : m_widgets) {
        auto container = dynamic_cast<Container*>(w.get());
        if (container) {
            widget = container->find_widget_by_id_recursively(id);
            if (widget)
                return widget;
        }
    }
    return nullptr;
}

EML::EMLErrorOr<void> Container::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(Widget::load_from_eml_object(object, loader));

    m_layout = TRY(object.require_and_construct_object<Layout>("layout", loader));

    for (auto const& child : object.objects) {
        std::shared_ptr<Widget> widget = TRY(child.construct<Widget>(loader, widget_tree_root()));
        assert(widget);
        add_created_widget(std::move(widget));
    }

    return {};
}

EML_REGISTER_CLASS(Container);

}
