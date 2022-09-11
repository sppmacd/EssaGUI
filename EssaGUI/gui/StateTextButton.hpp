#pragma once
#include "Application.hpp"
#include "TextAlign.hpp"
#include "Widget.hpp"
#include <functional>
#include <vector>

namespace GUI {

template<typename T>
class StateTextButton : public Widget {
    struct State {
        Util::Color bg_color, fg_color, text_color;
        T state;
    };

public:
    void add_state(Util::UString content, T state, Util::Color bg_color, Util::Color fg_color = Util::Colors::Blue, Util::Color text_color = Util::Colors::White);

    CREATE_VALUE(size_t, font_size, 20)
    CREATE_VALUE(Align, alignment, Align::CenterLeft);

    Align get_alignment() const { return m_alignment; };
    void set_alignment(Align alignment) { m_alignment = alignment; };

    std::pair<Util::UString, State> get_element_from_index(unsigned index) const;

    T state() const;

    std::function<void(T)> on_change;

    void set_index(unsigned index);

private:
    virtual void draw(GUI::Window& window) const override;
    virtual void handle_event(Event& event) override;

    std::vector<std::pair<Util::UString, State>> m_states;

    size_t m_index = 0;
};

}

// These ifndefs are just so that clangd doesn't complain that much
#ifndef STATE_TEXT_BUTTON_INL
#    define STATE_TEXT_BUTTON_HPP
#    include "StateTextButton.inl"
#endif
