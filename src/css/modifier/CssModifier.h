/******************************************************************************
This source file is part of the project
HyperSheetsPreprocessor (HSPP) - Optimizer and minifier for CSS
Copyright (C) 2019 Waldemar Zimpel <hspp@utilizer.de>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <https://www.gnu.org/licenses/>.
******************************************************************************/

#ifndef CSSMODIFIER_H
#define CSSMODIFIER_H
#include "../../general/visitor/VisitorInterface.h"
#include "../../Console.h"
#include "../../DataContainer.h"
#include "../../HashTable.h"
#include "../../String.h"
#include "../../filesystem/FileSystem.h"
#include "../../general/modifier/GeneralModifier.h"
#include "../CssVendorPrefixes.h"
#include "../parser/includes.h"
#include "IdentInfo.h"
#include "CssColorTable.h"
#include "restructuring/CssRestructuring.h"
#include <climits>
#include <stack>

namespace CSS {
namespace Minification {
using namespace General::Visitor;
using namespace CSS::Parsing::Elements;
using namespace General::Minification;

using CssVisitorInterface =
    VisitorInterface<CssAtRulePtr, CssBlockPtr, CssDeclarationPtr, CssPercentagePtr,
                     CssDimensionPtr, CssFunctionPtr, CssIdentifierPtr, CssCustomPropertyPtr,
                     CssNumberPtr, CssColorPtr, CssQualifiedRulePtr, CssStringPtr,
                     CssSelectorPtr, CssSelectorAttributePtr, CssSelectorCombinatorPtr,
                     CssDelimiterPtr, CssUnicodeRangePtr, CssSupportsConditionPtr, CssCommentPtr>;

class CssModifier final : public CssVisitorInterface, private GeneralModifier
{
public:
    explicit
    CssModifier();

    void
    visit(const CssAtRulePtr &)               override,
    visit(const CssBlockPtr &)                override,
    visit(const CssDeclarationPtr &)          override,
    visit(const CssPercentagePtr &)           override,
    visit(const CssDimensionPtr &)            override,
    visit(const CssFunctionPtr &)             override,
    visit(const CssIdentifierPtr &)           override,
    visit(const CssCustomPropertyPtr &)       override,
    visit(const CssNumberPtr &)               override,
    visit(const CssColorPtr &)                override,
    visit(const CssQualifiedRulePtr &)        override,
    visit(const CssStringPtr &)               override,
    visit(const CssSelectorPtr &)             override,
    visit(const CssSelectorAttributePtr &)    override,
    visit(const CssSelectorCombinatorPtr &)   override,
    visit(const CssDelimiterPtr &)            override,
    visit(const CssUnicodeRangePtr &)         override,
    visit(const CssSupportsConditionPtr &)    override,
    visit(const CssCommentPtr &)              override;

private:
    enum Context : uint8_t {
        STYLESHEET, FUNCTION_URL, KEYFRAMES_BLOCK, AT_RULE_IMPORT,
        DEFAULT_BLOCK, CURLY_BLOCK, PAREN_BLOCK, SQUARE_BLOCK
    };

    void
    // Generate replacement identifiers
    generateIds(),
    generateClassNames(),
    generateCustomPropertyNames(),
    generateAnimationNames();

    inline void
    pushContext(const Context),
    popContextIf(const Context),
    popContextIf(const initializer_list<const Context>);

    inline bool
    hasContext(const Context context) const,
    context(const Context context) const,
    context(const initializer_list<const Context> candidates) const;

    void
    writeJsonFile(const string &file_name);

    static string
    /// Returns a possibly minified version of a hex color
    getShortHexColorValue(const string &hex_value),

    /// Returns a possibly minified version of a number
    getShortNumber(string number_value),
    /// Returns an RGB/RGBA hex color value from decimal values
    getHexColorFromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = UCHAR_MAX),

    cutStringNumberToPrecision(string number, const uint8_t precision);

    static void
    replaceRgbaFuncWithRgbaHexColor(const CssFunctionPtr &function);

    static void
    maybeManipulateHslaFunction(const CssFunctionPtr &function);

    static bool
    /// If possible, replaces number with the scientific notation
    replaceNumberWithScientificNotation(const CssNumberPtr &number_element);

    static inline void
    maybeRewriteLinearGradientFunction(const CssFunctionPtr &function);

    void
    maybeImportStyleSheet(const CssAtRulePtr &import_rule);

    static uint8_t
    getStringNumberPrecision(const string &number);

    static const string
    percentageToNumber(string percentage);

    static bool
    minifyAngle(const CssDimensionPtr &dimension);

    string
    m_id_replacement_name,
    m_class_replacement_name,
    m_cprop_replacement_name,
    m_animation_replacement_name;

    CssDimensionPtr m_dimension;
    CssDeclarationPtr m_declaration;
    stack<CssBlockPtr> m_stylesheets;

    DataContainer<Context> m_context_stack;
    stack<CssBlockPtr> m_block_stack;

    const Vendor m_vendor;

    CssRestructuring m_restructuring;
};

static bool
s_output_to_stdo = false,
s_use_utf8_bom = false,
s_create_json_file = false,
s_include_external_stylesheets = false,
s_remove_comments = false,
s_remove_empty_rules = false,
s_minify_ids = false,
s_minify_class_names = false,
s_minify_custom_properties = false,
s_minify_animation_names = false,
s_minify_numbers = false,
s_minify_colors = false,
s_rewrite_angles = false,
s_use_rgba_hex_color_notation = false,
s_rewrite_functions = false;

extern shared_ptr<HashTable<string, IdentInfo<shared_ptr<string> > > >
g_id_replacement_list,
g_class_replacement_list;

extern shared_ptr<HashTable<string, IdentInfo<CssIdentifierPtr> > >
g_cprop_replacement_list,
g_anim_replacement_list;

static uint8_t s_import_depth = 0;

inline void
CssModifier::
pushContext(const Context context)
{
    m_context_stack.emplace_back(context);
}

inline void
CssModifier::
popContextIf(const Context expected_context)
{
    if (m_context_stack.empty()) return;

    if (m_context_stack.back() == expected_context)
        m_context_stack.pop_back();
}

inline void
CssModifier::
popContextIf(const initializer_list<const Context> candidates)
{
    if (m_context_stack.empty()) return;

    for (const auto &context : candidates)
        if (m_context_stack.back() == context) {
            m_context_stack.pop_back();
            return;
        }
}

inline bool
CssModifier::
context(const Context context) const
{
    return !m_context_stack.empty() && m_context_stack.back() == context;
}

inline bool
CssModifier::
context(const initializer_list<const Context> candidates) const
{
    for (const auto &candidate : candidates)
        if (m_context_stack.back() == candidate)
            return true;

    return false;
}

inline bool
CssModifier::
hasContext(const Context context) const
{
    for (const auto &_context : m_context_stack)
        if (_context == context)
            return true;

    return false;
}

// This function is inlined, because it is called only from one position
/*static*/ inline void
CssModifier::
maybeRewriteLinearGradientFunction(const CssFunctionPtr &function)
{
    // The linear-gradient() function should be preminified.

    auto &params = function->parameters();

    if (!params.empty()) {
        // Check, if element is a system color
        const auto isSystemColor = [](const CssBaseElementPtr &element) {
            // https://drafts.csswg.org/css-color/#css-system-colors

            if (element->isIdentifier()) {
                const auto &identifier = static_pointer_cast<CssIdentifier>(element);

                if (identifier->value({"canvas", "text", "linktext", "visitedtext", "activetext",
                                       "buttonface", "buttontext", "field", "fieldtext", "highlight",
                                       "highlighttext", "graytext"})) {
                    return true;
                }
            }

            return false;
        };

        // Check, if element is a color element
        const auto isColor = [&isSystemColor](const CssBaseElementPtr &element) {
            // https://drafts.csswg.org/css-color/#color-type

            if (element->isColor())
                return true;

            if (element->isFunction()) {
                const auto &func = static_pointer_cast<CssFunction>(element);

                if (!func->name({"var", "rgb", "rgba", "hsl", "hsla", "hwb", "lab", "lch", "gray", "color", "device-cmyk"}))
                    return false;
            }
            else if (!isSystemColor(element))
                return false;

            return true;
        };

        const auto checkPercentage = [&isColor](DataContainer<DataContainer<CssBaseElementPtr> >::iterator begin, DataContainer<DataContainer<CssBaseElementPtr> >::iterator end) -> bool {
            CssBaseElementPtr percentage_base = nullptr;

            // Check, if all of the color stops are either a color, a percentage or a color with percentage.
            // Colors with dimensions are not handled.
            for (auto itr = begin; itr != end; ++itr) {
                switch (itr->size()) {
                case 1:
                    if (isColor(itr->front()))
                        continue;

                    if (itr->front()->isPercentage())
                        percentage_base = itr->front();

                    break;
                case 2:
                    if (isColor(itr->front()) && itr->back()->isPercentage())
                        percentage_base = itr->back();

                    break;
                default:
                    return false;
                }

                if (percentage_base) {
                    const auto &percentage = static_pointer_cast<CssPercentage>(percentage_base);
                    const auto number = stold(percentage->value());

                    // Check if number is not within range 0...100
                    if (number < 0.0L || number > 100.0L)
                        return false;

                    if ((itr != begin && percentage->value() == "0") ||
                        (itr != end-1 && percentage->value() == "100"))
                        return false;

                    percentage_base = nullptr;

                    continue;
                }

                return false;
            }

            return true;
        };

        /// Removes 0% position of the first color and 100% position of the last color
        /// in color stop list because these are defaults anyway.
        /// Handles percentages only.
        const auto removeUnnecessaryPercentages = [&checkPercentage](DataContainer<DataContainer<CssBaseElementPtr> >::iterator begin, DataContainer<DataContainer<CssBaseElementPtr> >::iterator end) -> bool {
            if (checkPercentage(begin, end)) {
                for (auto itr = begin; itr != end; ++itr) {
                    if (itr->size() == 2 && itr->front()->isColor() && itr->back()->isPercentage()) {
                        const auto &percentage = static_pointer_cast<CssPercentage>(itr->back());

                        if ((itr == begin && percentage->value() == "0") ||
                            (itr == end-1 && percentage->value() == "100"))
                            itr->erase(itr->end()-1);
                        else
                            continue;
                    }
                }

                return true;
            }

            return false;
        };

        /// Reverses color stops and recalculates percentages, if drawing direction has changed.
        /// Handles percentages only.
        const auto reverseColorStops = [](DataContainer<DataContainer<CssBaseElementPtr> >::iterator begin, DataContainer<DataContainer<CssBaseElementPtr> >::iterator end) -> bool {
                CssBaseElementPtr percentage_base = nullptr;

                for (auto itr = begin; itr != end; ++itr) {
                    switch (itr->size()) {
                    case 1:
                        if (itr->front()->isPercentage())
                            percentage_base = itr->front();

                        break;
                    case 2:
                        if (itr->back()->isPercentage())
                            percentage_base = itr->back();

                        break;
                    }

                    if (percentage_base) {
                        const auto &percentage = static_pointer_cast<CssPercentage>(percentage_base);

                        if (percentage->value() != "50") {
                            auto number = stod(percentage->value());

                            number = 100. - number;
                            percentage->setNumber(getShortNumber(to_string(number)));
                        }

                        percentage_base = nullptr;
                    }
                }

                reverse(begin, end);
                return true;
        };


        auto &param1 = params.front();

        if (param1.size() == 2 && param1.front()->isIdentifier() && param1.back()->isIdentifier()) {
            const auto &identifier1 = static_pointer_cast<CssIdentifier>(param1.front());
            const auto &identifier2 = static_pointer_cast<CssIdentifier>(param1.back());

            const auto dimension = make_shared<CssDimension>("", "");

            if (identifier1->value() == "to") {
                if (identifier2->value() == "bottom") {
                    dimension->setNumber("180");
                    dimension->setUnit("deg");
                }
                else if (identifier2->value() == "top") {
                    dimension->setNumber("0");
                    dimension->setUnit("deg");
                }
                else if (identifier2->value() == "left") {
                    dimension->setNumber("270");
                    dimension->setUnit("deg");
                }
                else if (identifier2->value() == "right") {
                    dimension->setNumber("90");
                    dimension->setUnit("deg");
                }

                if (!dimension->value().empty()) {
                    param1.clear();
                    param1.emplace_back(dimension);
                }
            }
        }

        if (param1.size() == 1) {
            if (param1.front()->isDimension()) {
                const auto &dimension = static_pointer_cast<CssDimension>(param1.front());

                minifyAngle(dimension);

                if ((dimension->value("0") && removeUnnecessaryPercentages(params.begin()+1, params.end()) &&
                     reverseColorStops(params.begin()+1, params.end())) ||
                    (dimension->value("180") && dimension->unit("deg") &&
                     removeUnnecessaryPercentages(params.begin()+1, params.end()))) {
                    params.erase(params.begin());
                }
                else if (dimension->unit("deg")) {
                    double angle = stod(dimension->value());

                    if (angle > 260.) {
                        angle = 360. - angle;

                        if (removeUnnecessaryPercentages(params.begin()+1, params.end()) &&
                            reverseColorStops(params.begin()+1, params.end())) {
                            dimension->setNumber(getShortNumber(to_string(angle)));
                            dimension->setUnit("deg");
                        }
                    } else {
                        removeUnnecessaryPercentages(params.begin()+1, params.end());
                    }
                }
            } else if (isColor(param1.front())) {
                removeUnnecessaryPercentages(params.begin(), params.end());
            }
        } else {
            removeUnnecessaryPercentages(params.begin(), params.end());
        }
    }
}

} // namespace Minification
} // namespace CSS

#endif // CSSMODIFIER_H
