#pragma once

#include "Container.hpp"
#include "Slider.hpp"
#include "Textbox.hpp"
#include "Textfield.hpp"

namespace GUI {

class ValueSlider : public Container {
public:
    virtual void on_init() override;

    double min() const { return m_slider->min(); }
    void set_min(double min) { m_slider->set_min(min); }
    double max() const { return m_slider->max(); }
    void set_max(double max) { m_slider->set_max(max); }
    double step() const { return m_slider->step(); }
    void set_step(double step) { m_slider->set_step(step); }

    void set_name_textfield_size(Length l) { m_name_textfield->set_size({ l, Length::Auto }); }
    void set_unit_textfield_size(Length l) { m_unit_textfield->set_size({ l, Length::Auto }); }

    void set_name(Util::UString name) { m_name_textfield->set_content(std::move(name)); }
    void set_unit(Util::UString unit) { m_unit_textfield->set_content(std::move(unit)); }

    double value() const;
    void set_value(double value, NotifyUser = NotifyUser::Yes);

    Slider& slider() { return *m_slider; }

    std::function<void(double)> on_change;

private:
    virtual LengthVector initial_size() const override { return { Length::Auto, { static_cast<float>(theme().line_height), Length::Unit::Px } }; }

    virtual EML::EMLErrorOr<void> load_from_eml_object(EML::Object const& object, EML::Loader& loader) override;

    Textfield* m_name_textfield = nullptr;
    Slider* m_slider = nullptr;
    Textbox* m_textbox = nullptr;
    Textfield* m_unit_textfield = nullptr;
};

}
