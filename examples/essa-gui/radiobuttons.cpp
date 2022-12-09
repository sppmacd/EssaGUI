#include <EssaGUI/Application.hpp>
#include <EssaGUI/Overlays/MessageBox.hpp>
#include <EssaGUI/Widgets/RadioButton.hpp>
#include <EssaGUI/Widgets/RadioGroup.hpp>
#include <EssaGUI/Widgets/TextEditor.hpp>
#include <EssaGUI/Widgets/Textbox.hpp>
#include <EssaGUI/Widgets/Widget.hpp>
#include <EssaUtil/Color.hpp>
#include <EssaUtil/UString.hpp>
#include <EssaUtil/Units.hpp>
#include <cstddef>
#include <string>

int main() {
    GUI::Application app;
    auto& host_window = app.create_host_window({ 500, 500 }, "Radio buttons");

    auto& container1 = host_window.set_main_widget<GUI::Container>();
    container1.set_layout<GUI::VerticalBoxLayout>();

    auto radiogrp = container1.add_widget<GUI::RadioGroup>();

    for (size_t i = 0; i < 5; i++) {
        radiogrp->add_radio("radio" + Util::to_ustring(i));
    }

    radiogrp->on_change = [](size_t index) {
        std::cout << "Selected index: " << index << "\n";
    };

    radiogrp->set_row_height(15.0_px);

    app.run();
    return 0;
}
