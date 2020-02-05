#include "CssSelectorAttribute.h"
using namespace CSS::Parsing::Elements;

CssSelectorAttribute::CssSelectorAttribute() :
    CssSelector(ATTRIBUTE),
    m_value(make_shared<string>()) {}

CssSelectorAttribute::CssSelectorAttribute(
        string name, string value, const Operation operation,
        CssSelectorPtr parental_selector, const bool case_insensitive) :
    CssSelector(ATTRIBUTE, move(parental_selector), move(name)),
    m_value(make_shared<string>(move(value))),
    m_operation(operation),
    m_case_insensitive_operation(case_insensitive) {}
