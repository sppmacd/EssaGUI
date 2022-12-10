#pragma once

#include <Essa/GUI/Graphics/Window.hpp>
#include <Essa/GUI/Overlay.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>

namespace GUI {

class ToolWindow : public Overlay {
public:
    explicit ToolWindow(HostWindow&, std::string id = "ToolWindow");

    static constexpr auto TitleBarSize = 28;
    static constexpr auto MinSize = 50;
    static constexpr auto ResizeBorderWidth = 10;

    virtual Util::Vector2f position() const override { return m_position; }
    void set_position(Util::Vector2f position) { m_position = position; }
    void center_on_screen();

    virtual Util::Vector2f size() const override { return m_size; }
    void set_size(Util::Vector2f size) { m_size = size; }

    CREATE_VALUE(Util::UString, title, "")

    virtual Util::Rectf full_rect() const override {
        return Util::Rect { position() - Util::Vector2f(0, TitleBarSize), size() + Util::Vector2f(0, TitleBarSize) }
            .inflated(ResizeBorderWidth / 2);
    }
    Util::Rectf titlebar_rect() const { return { position() - Util::Vector2f(0, TitleBarSize), { size().x(), TitleBarSize } }; }

    enum class ResizeDirection {
        Top,
        Right,
        Bottom,
        Left
    };

    Util::Rectf resize_rect(ResizeDirection direction) const;

protected:
    virtual void handle_event(llgl::Event) override;
    virtual void draw(Gfx::Painter&) override;

private:
    virtual void update() override {
        WidgetTreeRoot::update();
        m_first_tick = false;
    }

    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const&, EML::Loader& loader) override;

    Util::Vector2f m_position;
    Util::Vector2f m_size;
    bool m_moving = false;
    std::array<std::optional<ResizeDirection>, 2> m_resize_directions;
    Util::Vector2f m_initial_dragging_position;
    Util::Vector2f m_initial_dragging_size;
    Util::Vector2i m_drag_position;
    bool m_first_tick = true;

    struct TitlebarButton {
        std::function<void()> on_click;
        bool hovered = false;
        bool mousedown = false;
    };
    std::vector<TitlebarButton> m_titlebar_buttons;
};

}