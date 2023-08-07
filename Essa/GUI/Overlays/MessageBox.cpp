#include "MessageBox.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/Graphics/Text.hpp>
#include <Essa/GUI/Overlays/ToolWindow.hpp>
#include <Essa/GUI/Widgets/Container.hpp>
#include <Essa/GUI/Widgets/TextButton.hpp>
#include <Essa/GUI/Widgets/Textfield.hpp>

namespace GUI {

MessageBox::MessageBox(WidgetTreeRoot& wnd, Util::UString message, Util::UString title, Buttons buttons)
    : WindowRoot(wnd) {

    auto& prompt_container = set_main_widget<GUI::Container>();

    auto& prompt_container_layout = prompt_container.set_layout<GUI::VerticalBoxLayout>();
    prompt_container_layout.set_spacing(20);
    prompt_container_layout.set_padding(Boxi::all_equal(20));

    auto* prompt_text = prompt_container.add_widget<GUI::Textfield>();
    prompt_text->set_content(Gfx::RichText::parse(message));
    prompt_text->set_size({ Util::Length::Auto, Util::Length::Auto });

    {
        auto text_rect = prompt_text->needed_size_for_text();
        Util::Size2u total_size { 40 + text_rect.x(), 80 + text_rect.y() };
        window().setup(std::move(title), total_size, {});
        window().center_on_screen();
    }

    auto* button_container = prompt_container.add_widget<GUI::Container>();
    button_container->set_layout<GUI::HorizontalBoxLayout>().set_spacing(20);
    button_container->set_size({ Util::Length::Auto, 30.0_px });

    auto add_button = [this, &button_container](ButtonRole button_role, Util::UString label, Util::Color bg_color) {
        auto* button = button_container->add_widget<GUI::TextButton>();
        button->set_alignment(GUI::Align::Center);
        button->set_content(std::move(label));

        button->override_button_colors().normal.unhovered.background = bg_color;
        // TODO: Take this from theme somehow?
        button->override_button_colors().normal.hovered.background = bg_color + Util::Color { 20, 20, 20 };

        button->on_click = [this, button_role]() {
            m_clicked_button = button_role;
            close();
        };
        return button;
    };

    auto const& theme = GUI::Application::the().theme();

    if (buttons == Buttons::YesNo) {
        m_default_button = add_button(ButtonRole::Yes, "Yes", theme.positive);
        add_button(ButtonRole::No, "No", theme.negative);
    }
    else if (buttons == Buttons::Ok) {
        m_default_button = add_button(ButtonRole::Ok, "Ok", theme.neutral);
    }
}

Widget::EventHandlerResult MessageBox::handle_event(Event const& event) {
    if (auto const* keypress = event.get<Event::KeyPress>(); keypress && keypress->code() == llgl::KeyCode::Enter && m_default_button) {
        m_default_button->on_click();
        return Widget::EventHandlerResult::Accepted;
    }
    return Widget::EventHandlerResult::NotAccepted;
}

MessageBox::ButtonRole message_box(Util::UString message, Util::UString title, MessageBox::Buttons buttons) {
    auto msgbox = GUI::Application::the().open_host_window<GUI::MessageBox>(std::move(message), std::move(title), buttons);
    msgbox.window.show_modal();
    return msgbox.root.clicked_button();
}

MessageBox::ButtonRole message_box(HostWindow&, Util::UString message, Util::UString title, MessageBox::Buttons buttons) {
    return message_box(std::move(message), std::move(title), buttons);
}

}
