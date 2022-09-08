#pragma once

#include "Container.hpp"

namespace GUI {

class Frame : public Container {
public:
    static constexpr float BorderRadius = 20;

private:
    virtual void draw(GUI::Window&) const override;
    virtual Boxf intrinsic_padding() const override { return Boxf::all_equal(BorderRadius); }
    virtual bool steals_focus() const override { return true; }
};

}
