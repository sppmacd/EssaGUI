#pragma once

#include "NotifyUser.hpp"
#include "Widget.hpp"
#include <functional>
#include <optional>

namespace GUI {

class Button : public Widget {
public:
    std::function<void()> on_click;

    virtual void handle_event(Event&) override;
    virtual void draw(GUI::Window& window) const override = 0;

    bool is_active() const { return m_active; }

    void set_active(bool active, NotifyUser notify_user = NotifyUser::Yes) {
        if (m_active != active) {
            m_active = active;
            if (notify_user == NotifyUser::Yes && on_change)
                on_change(active);
        }
    }

    void set_active_without_action(bool state) { m_active = state; }

    void set_toggleable(bool t) { m_toggleable = t; }
    bool is_toggleable() const { return m_toggleable; }

    std::function<void(bool)> on_change;

    // TODO: Replace this with full theme override system
    virtual Theme::ButtonColors& override_button_colors() {
        if (m_button_colors_override)
            return *m_button_colors_override;
        m_button_colors_override = default_button_colors();
        return *m_button_colors_override;
    }

protected:
    Theme::BgFgTextColors colors_for_state() const;

    virtual Theme::ButtonColors default_button_colors() const = 0;

    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader&) override;
    virtual bool accepts_focus() const override { return true; }

private:
    void click();

    std::optional<Theme::ButtonColors> m_button_colors_override;

    bool m_toggleable { false };
    bool m_active { false };
    bool m_pressed_on_button { false };
};

}
