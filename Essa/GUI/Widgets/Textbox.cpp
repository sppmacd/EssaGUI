#include "Textbox.hpp"

#include <Essa/GUI/Application.hpp>
#include <Essa/GUI/EML/Loader.hpp>
#include <Essa/GUI/NotifyUser.hpp>
#include <Essa/GUI/Widgets/TextEditor.hpp>
#include <Essa/GUI/Widgets/Widget.hpp>
#include <EssaUtil/CharacterType.hpp>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
#include <string>

namespace GUI {

bool Textbox::find_decimal() const {
    for (const auto& c : content()) {
        if (c == '.')
            return true;
    }
    return false;
}

std::string Textbox::m_fix_content(std::string content) const {
    size_t i = content.size();
    while (i > 0 && content[i - 1] == '0')
        i--;

    if (content[i - 1] == '.')
        return content.substr(0, i) + '0';
    return content.substr(0, std::min(m_limit, i));
}

void Textbox::m_fit_in_range() {
    if (is_focused())
        return;

    auto maybe_value = content().parse<double>();
    if (maybe_value.is_error()) {
        return;
    }
    double val = maybe_value.release_value();
    std::ostringstream oss;
    oss << std::fixed;
    if (val < m_min)
        oss << m_min;
    else if (val > m_max)
        oss << m_max;
    else
        return;
    set_content(Util::UString { m_fix_content(oss.str()) }, NotifyUser::No);
}

bool Textbox::can_insert_codepoint(uint32_t ch) const {
    if (content().size() >= m_limit)
        return false;

    switch (m_type) {
    case TEXT:
        return isprint(ch);
    case NUMBER:
        return isdigit(ch) || (ch == '.' && !m_has_decimal);
    }
    return false;
}

void Textbox::on_content_change() {
    if (m_type == Type::NUMBER)
        m_fit_in_range();
}

EML::EMLErrorOr<void> Textbox::load_from_eml_object(EML::Object const& object, EML::Loader& loader) {
    TRY(TextEditor::load_from_eml_object(object, loader));
    m_limit = TRY(object.get_property("limit", static_cast<double>(m_limit)).to_double());
    m_min = TRY(object.get_property("min_value", m_min).to_double());
    m_max = TRY(object.get_property("max_value", m_min).to_double());
    return {};
}

EML_REGISTER_CLASS(Textbox);

}