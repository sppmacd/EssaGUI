#pragma once

#include "EssaGUI/gui/Container.hpp"

namespace GUI{

class Border : public Container{
public:
    Border() {
        set_layout<HorizontalBoxLayout>();
    }

    template<class T, class... Args>
    requires(std::is_base_of_v<Widget, T>&& requires(Args&&... args) { T(std::forward<Args>(args)...); })
    T* add_child(Args&&... args){
        
        m_widgets.clear();
        
        auto child = add_widget<T>(std::forward<Args>(args)...);
        child->set_size({Length::Auto, Length::Auto});

        return child;
    } 
};

}
