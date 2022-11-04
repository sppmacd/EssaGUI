#include <EssaGUI/gui/Application.hpp>
#include <EssaGUI/gui/MessageBox.hpp>
#include <EssaGUI/gui/TextEditor.hpp>
#include <EssaGUI/gui/Textbox.hpp>
#include <EssaGUI/gui/textediting/SyntaxHighlighter.hpp>

class TestSyntaxHighlighter : public GUI::SyntaxHighlighter {
    virtual std::vector<GUI::TextStyle> styles() const override {
        return {
            GUI::TextStyle { .color = Util::Colors::IndianRed },
            GUI::TextStyle { .color = Util::Colors::Lime },
        };
    }

    virtual std::vector<GUI::StyledTextSpan> spans(Util::UString const& input) const override {
        std::vector<GUI::StyledTextSpan> result;
        for (size_t s = 0; s < input.size(); s++) {
            result.push_back(GUI::StyledTextSpan { .span_start = s, .span_size = 1, .style_index = s % 2 });
        }
        return result;
    }
};

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "Text Editor");

    auto& container1 = host_window.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::VerticalBoxLayout>();
    auto text_editor = container1.add_widget<GUI::TextEditor>();
    text_editor->set_placeholder("Test placeholder");
    text_editor->set_syntax_highlighter(std::make_unique<TestSyntaxHighlighter>());

    auto number_text_box = container1.add_widget<GUI::Textbox>();
    number_text_box->set_placeholder("Enter number");
    number_text_box->on_enter = [&host_window](Util::UString const& content) {
        GUI::message_box(host_window, "You entered number: " + content, "Message", GUI::MessageBox::Buttons::Ok);
    };

    auto text_box = container1.add_widget<GUI::Textbox>();
    text_box->set_placeholder("Enter text");
    text_box->set_type(GUI::Textbox::Type::TEXT);
    text_box->on_enter = [&host_window](Util::UString const& content) {
        GUI::message_box(host_window, "You entered text: " + content, "Message", GUI::MessageBox::Buttons::Ok);
    };

    app.run();
    return 0;
}
