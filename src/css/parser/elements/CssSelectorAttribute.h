#ifndef CSSSELECTORATTRIBUTE_H
#define CSSSELECTORATTRIBUTE_H
#include "CssSelector.h"

namespace CSS {
namespace Parsing {
namespace Elements {

class CssSelectorAttribute final : public CssSelector
{
public:
    using CssSelectorAttributePtr = shared_ptr<CssSelectorAttribute>;

    enum Operation : uint8_t {
        NONE, EQUAL, ROOF_EQUAL, DOLLAR_EQUAL,
        PIPE_EQUAL, TILDE_EQUAL, ASTERISK_EQUAL
    };

    explicit
    CssSelectorAttribute(),
    CssSelectorAttribute(
        string name, string value, const Operation operation,
        CssSelectorPtr parental_selector, const bool case_insensitive = false);

    static inline CssSelectorAttributePtr
    /// Cast CssSelectorPtr to CssSelectorAttributePtr
    fromBase(const CssSelectorPtr &element);

    inline void
    accept(CssVisitorInterface &visitor) override,

    setAttributeName(const string &name),
    setAttributeValue(const string &value),
    setAttributeValue(const shared_ptr<string> &value),
    setOperation(const Operation operation),
    setCaseInsensitiveFlag(const bool case_insensitive = true);

    inline const string
    &attributeName() const,
    &attributeValue() const;

    inline Operation
    operation() const;

    inline bool
    attributeName(const string &name) const,
    attributeName(const initializer_list<string> candidates) const,
    caseInsensitive() const;

private:
    shared_ptr<string> m_value;
    Operation m_operation {NONE};
    bool m_case_insensitive_operation {false};
};

using CssSelectorAttributePtr = CssSelectorAttribute::CssSelectorAttributePtr;

inline void
CssSelectorAttribute::
accept(CssVisitorInterface &visitor)
{
    visitor.visit(static_pointer_cast<CssSelectorAttribute>(shared_from_this()));
}

/*static*/ inline CssSelectorAttributePtr
CssSelectorAttribute::
fromBase(const CssSelectorPtr &element)
{
    return static_pointer_cast<CssSelectorAttribute>(element);
}

inline void
CssSelectorAttribute::
setAttributeName(const string &name)
{
    m_name->reserve(name.length());
    *m_name = name;
}

inline const string &
CssSelectorAttribute::
attributeName() const
{
    return *m_name;
}

inline bool
CssSelectorAttribute::
attributeName(const string &name) const
{
    return *m_name == name;
}

inline bool
CssSelectorAttribute::
attributeName(const initializer_list<string> candidates) const
{
    for (const auto &candidate : candidates)
        if (*m_name == candidate) return true;

    return false;
}

inline void
CssSelectorAttribute::
setAttributeValue(const string &value)
{
    m_value->reserve(value.length());
    *m_value = value;
}

inline void
CssSelectorAttribute::
setAttributeValue(const shared_ptr<string> &value)
{
    m_value = value;
}

inline const string &
CssSelectorAttribute::
attributeValue() const
{
    return *m_value;
}

inline void
CssSelectorAttribute::
setOperation(const Operation operation)
{
    m_operation = operation;
}

inline auto
CssSelectorAttribute::
operation() const -> Operation
{
    return m_operation;
}

inline void
CssSelectorAttribute::
setCaseInsensitiveFlag(const bool case_insensitive)
{
    m_case_insensitive_operation = case_insensitive;
}

inline bool
CssSelectorAttribute::
caseInsensitive() const
{
    return m_case_insensitive_operation;
}

} // namespace Elements
} // namespace Parsing
} // namespace CSS

#endif // CSSSELECTORATTRIBUTE_H
