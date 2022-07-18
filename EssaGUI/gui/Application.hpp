#pragma once

#include "Container.hpp"
#include "ContextMenu.hpp"
#include "Theme.hpp"
#include "ToolWindow.hpp"
#include "Tooltip.hpp"
#include "WidgetTreeRoot.hpp"

#include <LLGL/Resources/TTFFont.hpp>
#include <iostream>
#include <list>

namespace GUI {

class Application : public WidgetTreeRoot {
public:
    explicit Application(GUI::Window&);

    static Application& the();

    // TODO: Find a way for this to be private
    virtual void draw() override;

    virtual void handle_event(llgl::Event) override;

    // TODO: Move it to some specialized class
    llgl::TTFFont font;
    llgl::TTFFont bold_font;
    llgl::TTFFont fixed_width_font;

    enum class NotificationLevel {
        Error
    };
    void spawn_notification(Util::UString message, NotificationLevel);

    template<class T = Overlay, class... Args>
    requires(std::is_base_of_v<Overlay, T>)
        T& open_overlay(Args&&... args) {
        return static_cast<T&>(open_overlay_impl(std::make_unique<T>(window(), std::forward<Args>(args)...)));
    }

    struct OpenOrFocusResult {
        ToolWindow* window {};
        bool opened {};
    };
    // FIXME: Generalize it like normal open_overlay
    OpenOrFocusResult open_or_focus_tool_window(Util::UString title, std::string id);

    // NOTE: The opened context menu is modal, meaning that this
    //       function won't return until user chooses an action
    //       or dismisses the menu.
    void open_context_menu(ContextMenu);

    Overlay* focused_overlay() const { return m_focused_overlay; }

    template<class Callback>
    void for_each_overlay(Callback&& callback) {
        for (auto& wnd : m_overlays)
            callback(*wnd);
    }

    TooltipOverlay& add_tooltip(Tooltip t);
    void remove_closed_overlays();

    virtual Util::Vector2f position() const override { return {}; }
    virtual Util::Vector2f size() const override { return Util::Vector2f { window().size() }; }

    void set_theme(Theme const& theme) { m_theme = &theme; }
    Theme const& theme() const { return *m_theme; }

    void focus_overlay(Overlay&);

private:
    virtual void handle_events() override;
    virtual void update() override;

    struct Notification {
        int remaining_ticks = 120;
        Util::UString message;
        NotificationLevel level {};
    };

    void draw_notification(Notification const&, float y);
    llgl::Event transform_event(Util::Vector2f offset, llgl::Event event) const;
    Overlay& open_overlay_impl(std::unique_ptr<Overlay>);

    using OverlayList = std::list<std::unique_ptr<Overlay>>;

    void focus_window(OverlayList::iterator);

    OverlayList m_overlays;
    Util::Vector2f m_next_overlay_position { 10, 10 + ToolWindow::TitleBarSize };
    Overlay* m_focused_overlay = nullptr;
    std::vector<Notification> m_notifications;
    Theme const* m_theme = &Theme::default_theme();
};

}
