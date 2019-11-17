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

#include "CssModifier.h"
#include "../../filesystem/FileSystemWorker.h"
using namespace CSS::Minification;

CssModifier::CssModifier()
{
    s_output_to_stdo                = cfg.isEnabled(Config::GENERAL__OUTPUT_TO_STDO);
    s_use_utf8_bom                  = cfg.isEnabled(Config::GENERAL__USE_UTF8_BOM);
    s_create_php_include_file       = cfg.isEnabled(Config::GENERAL__CREATE_PHP_INCLUDE_FILE);
    s_include_external_stylesheets  = cfg.isEnabled(Config::CSS__INCLUDE_EXTERNAL_STYLESHEETS);
    s_remove_comments               = cfg.isEnabled(Config::CSS__REMOVE_COMMENTS);
    s_remove_empty_rules            = cfg.isEnabled(Config::CSS__REMOVE_EMPTY_RULES);
    s_minify_ids                    = cfg.isEnabled(Config::CSS__MINIFY_IDS);
    s_minify_class_names            = cfg.isEnabled(Config::CSS__MINIFY_CLASS_NAMES);
    s_minify_custom_properties      = cfg.isEnabled(Config::CSS__MINIFY_CUSTOM_PROPERTIES);
    s_minify_animation_names        = cfg.isEnabled(Config::CSS__MINIFY_ANIMATION_NAMES);
    s_minify_numbers                = cfg.isEnabled(Config::CSS__MINIFY_NUMBERS);
    s_minify_colors                 = cfg.isEnabled(Config::CSS__MINIFY_COLORS);
    s_rewrite_angles                = cfg.isEnabled(Config::CSS__REWRITE_ANGLES);
    s_use_rgba_hex_color_notation   = cfg.isEnabled(Config::CSS__USE_RGBA_HEX_NOTATION);
    s_rewrite_functions             = cfg.isEnabled(Config::CSS__REWRITE_FUNCTIONS);
}

void
CssModifier::
visit(const CssAtRulePtr &at_rule)
{
    if (m_vendor.maybePrefixedKeyword(at_rule->keyword(), "import")) {
        pushContext(AT_RULE_IMPORT);
        maybeImportStyleSheet(at_rule);
    }

    if (m_vendor.maybePrefixedKeyword(at_rule->keyword(), "charset")) {
        const auto &charset = static_pointer_cast<CssString>(at_rule->expressions().front().front());
        charset->setValue(String::toLower(charset->value()));

        if (s_use_utf8_bom) {
            if (charset->value() == "utf-8") {
                m_block_stack.top()->removeElement(at_rule);
                return;
            }

            const auto message =
                "The configuration file says, the UTF8 byte order mark should be written," NEWLINE
                "but the document specifies another encoding using a @charset rule: " + charset->value() + DBLNEWLINE
                "Please decide how to act: " NEWLINE
                "1. Write UTF8 BOM and remove the @charset rule" NEWLINE
                "2. Don't write UTF8 BOM to the current stylesheet and preserve the @charset rule" NEWLINE;

            const auto &choice = requestAction(message, 2);

            while (true) {
                switch (choice) {
                case 1:
                    m_block_stack.top()->removeElement(at_rule);
                    Console::writeLine("UTF8 BOM has been written." NEWLINE
                                       "@charset rule has been removed." NEWLINE);
                    return;
                case 2:
                    setUseUtf8BomFlag(false);
                    Console::writeLine("UTF8 BOM has not been written." NEWLINE
                                       "@charset rule has been preserved." NEWLINE);
                    break;
                default:
                    Console::writeLine("Invalid choice '" + to_string(choice) + "'" NEWLINE);
                    continue;
                }

                break;
            }
        }
    }

    if (at_rule->block()) {
        if (!at_rule->block()->elements().empty()) {
            if (m_vendor.maybePrefixedKeyword(at_rule->keyword(), "keyframes")) {
                // Minify animation names, if this is enabled in the config file or by default
                if (s_minify_animation_names) {
                    const auto &expressions = at_rule->expressions().front();
                    if (expressions.front() && expressions.front()->isIdentifier()) {
                        if (expressions.front()->isIdentifier()) {
                            const auto &animation_name = static_pointer_cast<CssIdentifier>(expressions.front());
                            if (!m_css_animation_name_definition_list->hasElement(animation_name->value()))
                                m_css_animation_name_definition_list->emplace_back(animation_name->value());

                            const auto &found = m_css_animation_name_replacement_list->find(animation_name->value());

                            if (found == m_css_animation_name_replacement_list->end()) {
                                const auto &ident_pair = make_shared<IdentPair>(make_shared<string>(animation_name->value()));
                                m_css_animation_name_replacement_list->emplace(animation_name->value(), ident_pair);
                                // Let current identifier element point to the new entry inside the animation replacement list
                                animation_name->setValue(ident_pair->replacement_id);
                            } else {
                                // Let current identifier element point to the existing entry inside the animation replacement list
                                animation_name->setValue(found->second->replacement_id);
                                // Increase appearance counter
                                ++found->second->count;
                            }
                        }
                    }
                }

                pushContext(KEYFRAMES_BLOCK);
            }

            at_rule->block()->accept(*this);

            if (m_vendor.maybePrefixedKeyword(at_rule->keyword(), "keyframes")) popContextIf(KEYFRAMES_BLOCK);
        } else {
            if (s_remove_empty_rules) {
                m_block_stack.top()->removeElement(at_rule);
                return;
            }
        }
    }

    for (const auto &list : at_rule->expressions())
        for (const auto &element : list)
            element->accept(*this);

    if (m_vendor.maybePrefixedKeyword(at_rule->keyword(), "import"))
        popContextIf(AT_RULE_IMPORT);
}

void
CssModifier::
visit(const CssBlockPtr &block)
{
    if (block->elements().empty()) return;

    switch (block->blockType()) {
    case CssBlock::STYLESHEET:
        m_stylesheets.push(block);
        pushContext(STYLESHEET);
        break;
    case CssBlock::CURLY:
        pushContext(CURLY_BLOCK);
        break;
    case CssBlock::PAREN:
        pushContext(PAREN_BLOCK);
        break;
    case CssBlock::SQUARE:
        pushContext(SQUARE_BLOCK);
        break;
    case CssBlock::DEFAULT:
        pushContext(DEFAULT_BLOCK);
        break;
    }

    m_block_stack.emplace(block);

    block->elements().iterateAll([&](const CssBaseElementPtr &element) -> void {
        element->accept(*this);
    });

    m_block_stack.pop();

    if (block->blockType() == CssBlock::STYLESHEET) {
        if (useUtf8Bom()) {
            const auto &utf8_bom = make_shared<CssString>("\xef\xbb\xbf", true);
            block->prependElement(utf8_bom);
        }

        if (s_minify_ids)
            generateIds();
        if (s_minify_class_names)
            generateClassNames();
        if (s_minify_custom_properties)
            generateCustomPropertyNames();
        if (s_minify_animation_names)
            generateAnimationNames();

        if (s_minify_custom_properties) {
            for (auto &cprop : *m_css_custom_property_replacement_list) {
                const auto &found = m_css_custom_property_definition_list->find(cprop.first);

                if (found == m_css_custom_property_definition_list->end()) {
                    cout << "Undeclared custom property '"
                         << cprop.first
                         << "' renamed to '"
                         << *cprop.second->replacement_id
                         << "'"
                         << endl;
                }
            }
        }

        m_css_custom_property_definition_list->clear();

        if (s_minify_animation_names) {
            for (auto &anim : *m_css_animation_name_replacement_list) {
                const auto &found = m_css_animation_name_definition_list->find(anim.first);

                if (found == m_css_animation_name_definition_list->end()) {
                    cout << "Undeclared animation name '"
                         << anim.first
                         << "' renamed to '"
                         << *anim.second->replacement_id
                         << "'"
                         << endl;
                }
            }
        }

        m_css_animation_name_definition_list->clear();

        if (s_create_php_include_file &&
            // Make sure, the file is written only on the initial input file
            m_stylesheets.size() == 1 &&
            !cfg.isEnabled(Config::GENERAL__OUTPUT_TO_STDO) &&
            (s_minify_ids ||
             s_minify_class_names ||
             s_minify_custom_properties ||
             s_minify_animation_names) &&
            (!m_css_id_replacement_list->empty() ||
             !m_css_class_replacement_list->empty() ||
             !m_css_custom_property_replacement_list->empty() ||
             !m_css_animation_name_replacement_list->empty())) {

            if (!s_output_to_stdo)
                Console::write("Writing PHP include file.\r");

            writePhpIncludeFile(APP_NAME ".php");

            if (!s_output_to_stdo)
                Console::writeLine("[Done] Writing PHP include file.");
        }

        if (!s_output_to_stdo && s_minify_ids && s_minify_class_names &&
            s_minify_custom_properties && s_minify_animation_names &&
            // Output only on the initial input file
            m_stylesheets.size() == 1) {

            string
            id_count = to_string(m_css_id_replacement_list->size()),
            class_count = to_string(m_css_class_replacement_list->size()),
            cprop_count = to_string(m_css_custom_property_replacement_list->size()),
            anim_count = to_string(m_css_animation_name_replacement_list->size());

            DataContainer<string> counter_data = {
                id_count, class_count, cprop_count, anim_count
            };

            sort(counter_data.begin(), counter_data.end(),
            [](const string &a, const string &b) {
                return a.length() > b.length();
            });

            auto max_len = static_cast<uint8_t>(counter_data.front().length());

            cout << "\n" << String::repeatChar('-', max_len + 18U) << "\n"
                 << "Found:\n"
                 << String::repeatChar('-', max_len + 18U) << "\n"
                 << String::repeatChar(' ', max_len - id_count.length())
                 << id_count
                 << " ids\n"
                 << String::repeatChar(' ', max_len - class_count.length())
                 << class_count
                 << " classes\n"
                 << String::repeatChar(' ', max_len - cprop_count.length())
                 << cprop_count
                 << " custom properties\n"
                 << String::repeatChar(' ', max_len - anim_count.length())
                 << anim_count
                 << " animation names\n"
                 << String::repeatChar('-', max_len + 18U) << "\n"
                 << endl;
        }

        m_stylesheets.pop();
    }

    popContextIf({STYLESHEET, CURLY_BLOCK, PAREN_BLOCK, SQUARE_BLOCK, DEFAULT_BLOCK});
}

void
CssModifier::
visit(const CssDeclarationPtr &declaration)
{
    m_declaration = declaration;

    // If custom property name minification is enabled in the config file or by default...
    if (s_minify_custom_properties) {
        if(declaration->namePtr()->isOfType(CssBaseElement::CUSTOM_PROPERTY)) {
            if (!m_css_custom_property_definition_list->hasElement(declaration->name()))
                m_css_custom_property_definition_list->emplace_back(declaration->name());

            // Check if custom property name already exists inside the custom property replacement list
            const auto &found = m_css_custom_property_replacement_list->find(declaration->name());

            // If it already exists...
            if (found != m_css_custom_property_replacement_list->end()) {
                // Let identifier point to this entry
                declaration->setName(found->second->replacement_id);
                // Increment occurrence counter
                ++found->second->count;
            } else {
                // Append it to the custom property replacement list
                const auto &ident_pair = make_shared<IdentPair>(make_shared<string>(declaration->name()));
                m_css_custom_property_replacement_list->emplace(declaration->name(), ident_pair);
                // Let declaration name now point to the new entry of the custom property replacement list
                declaration->setName(ident_pair->replacement_id);
            }
        }
    }

    // If animation name minification is enabled in the config file or by default...
    if (s_minify_animation_names) {
        if (declaration->name() == "animation" || declaration->name() == "animation-name") {
            if (declaration->values().front()[0]->isIdentifier()) {
                const auto &identifier = static_pointer_cast<CssIdentifier>(declaration->values().front()[0]);
                const auto &found = m_css_animation_name_replacement_list->find(identifier->value());

                if (identifier->value() != "none") {
                    if (found != m_css_animation_name_replacement_list->end()) {
                        identifier->setValue(found->second->replacement_id);
                        ++found->second->count;
                    } else {
                        const auto &ident_pair = make_shared<IdentPair>(make_shared<string>(identifier->value()));
                        m_css_animation_name_replacement_list->emplace(identifier->value(), ident_pair);
                        identifier->setValue(ident_pair->replacement_id);
                    }
                }
            }
        }
    }

    pushContext(DECLARATION_PROPERTY_VALUE);

    for (const auto &list : declaration->values())
        for (const auto &value : list)
            value->accept(*this);

    popContextIf(DECLARATION_PROPERTY_VALUE);

    m_declaration = nullptr;
}

void
CssModifier::
visit(const CssNumberPtr &number)
{
    // Try to minify numbers, if this is enabled in the config file or by default
    if (s_minify_numbers)
        number->setNumber(getShortNumber(number->value()));
}

void
CssModifier::
visit(const CssPercentagePtr &percentage)
{
    visit(static_pointer_cast<CssNumber>(percentage));
}

void
CssModifier::
visit(const CssDimensionPtr &dimension)
{
    if (dimension->replacementElement()) {
        dimension->replacementElement()->accept(*this);
        return;
    }

    visit(static_pointer_cast<CssNumber>(dimension));

    if (m_declaration) {

        // https://www.w3.org/TR/css-values-3/#lengths

        // Remove dimension unit for length dimensions, if dimension number value is 0
        if (dimension->value() == "0" && dimension->unit(
            {"px", "em", "rem", "pt", "vw", "vh", "ex", "ch",
             "vmin", "vmax", "cm", "mm", "Q", "in", "pc"}))
            dimension->setReplacementElement(make_shared<CssNumber>(dimension->value()));

        // Rewrite ms to s. Example: 100ms => .1s
        if (dimension->unit() == "ms") {
            const auto &number = dimension->value();

            if (number.length() == 3) {
                string new_number;

                if (number.back() == '0' && number.front() != '0') {
                    if (number.at(1) == '0') {
                        new_number = '.';
                        new_number += number.front();
                    } else {
                        new_number = '.';
                        new_number += number.front();
                        new_number += number.at(1);
                    }

                    dimension->setNumber(new_number);
                    dimension->setUnit("s");
                }
            }
        }
    }
}

void
CssModifier::
visit(const CssFunctionPtr &function)
{
    // If current function is a URL, push the corresponding context onto the context stack
    // This is important for unquoted URLs
    if (function->name("url")) pushContext(FUNCTION_URL);

    for (const auto &list : function->parameters())
        for (const auto &element : list)
            element->accept(*this);

    if (s_rewrite_functions) {
        // Rewrite hsl()/hsla() functions to rgb()/rgba() functions
        if (function->name({"hsl", "hsla"})) {
            maybeManipulateHslaFunction(function);
            if (function->replacementElement()) {
                function->replacementElement()->accept(*this);
                return;
            }
        }
        // Rewrite rgb()/rgba() functions to rgb/rgba hex color notation
        else if (function->name({"rgb", "rgba"})) {
            replaceRgbaFuncWithRgbaHexColor(function);
            if (function->replacementElement()) {
                function->replacementElement()->accept(*this);
                return;
            }
        }
        else if (function->name("linear-gradient")) {
            maybeRewriteLinearGradientFunction(function);
        }
    }

    // Pop the URL context from the top of the context stack again
    if (function->name("url")) popContextIf(FUNCTION_URL);
}

void
CssModifier::
visit(const CssIdentifierPtr &identifier)
{
    if (identifier->replacementElement()) {
        identifier->replacementElement()->accept(*this);
        return;
    }
}

void
CssModifier::
visit(const CssCustomPropertyPtr &custom_property)
{
    // If custom property minification is enabled in the config file or by default...
    if (s_minify_custom_properties) {
        // Check if custom property name already exists inside the custom property replacement list
        const auto &found = m_css_custom_property_replacement_list->find(custom_property->value());

        // If it already exists...
        if (found != m_css_custom_property_replacement_list->end()) {
            // Let identifier point to this entry
            custom_property->setValue(found->second->replacement_id);
            // Increment occurrence counter
            ++found->second->count;
        } else {
            // Append it to the custom property replacement list
            const auto &ident_pair = make_shared<IdentPair>(make_shared<string>(custom_property->value()));
            m_css_custom_property_replacement_list->emplace(custom_property->value(), ident_pair);
            // Let declaration name now point to the new entry of the custom property replacement list
            custom_property->setValue(ident_pair->replacement_id);
        }
    }
}

void
CssModifier::
visit(const CssColorPtr &color)
{
    // Try to minify colors, if this is enabled in the config file or by default
    if (s_minify_colors) {
        if (color->colorType() == CssColor::PREDEFINED_NAME) {
            if (m_declaration) {
                for (const auto &color_pair : s_css_color_table) {
                    if (color->value() == color_pair.first &&
                        color->value().length() > color_pair.second.length()+1) {
                        color->setColorType(CssColor::HEX_LITERAL);
                        color->setValue(getShortHexColorValue(color_pair.second));
                    }
                }
            }
        } else {
            if (m_declaration) {
                string value;

                if (color->value().length() == 6U || color->value().length() == 8U) {
                    value = getShortHexColorValue(color->value());
                    color->setValue(value);
                } else value = color->value();

                for (const auto &color_pair : s_css_color_table) {
                    if (value == color_pair.second) {
                        if (value.length()+1 > color_pair.first.length()) {
                            color->setColorType(CssColor::PREDEFINED_NAME);
                            color->setValue(color_pair.first);
                        } else {
                            color->setValue(value);
                        }

                        break;
                    }
                }
            }
        }
    }
}

void
CssModifier::
visit(const CssQualifiedRulePtr &qualified_rule)
{
    if (qualified_rule->block()) {
        if (!qualified_rule->block()->elements().empty())
            qualified_rule->block()->accept(*this);
        // Remove empty rules, if this is enabled in the config file or by default
        else if (s_remove_empty_rules)
            if (qualified_rule->block()->elements().empty()) {
                m_block_stack.top()->removeElement(qualified_rule);
                return;
            }
    }

    // Iterate through all selectors of the current rule
    for (const auto &selector : qualified_rule->selectors())
        selector->accept(*this);
}

void
CssModifier::
visit(const CssStringPtr &string_ptr)
{
    // If the string is a param of the url() function, unquote the string.
    if (context(FUNCTION_URL)) {
        string_ptr->setUnquotedFlag();

        for (const auto &c : string_ptr->value()) {
            switch (c) {
            case '"':
            case '\'':
            case '(':
            case ')':
            case '\\':
                string_ptr->setUnquotedFlag(false);
                break;
            default:
                if (bool(isspace(c))) { string_ptr->setUnquotedFlag(false); break; }
                continue;
            }

            break;
        }
    }
}

void
CssModifier::
visit(const CssSelectorCombinatorPtr &selector_combinator)
{
    if (selector_combinator->left())
        selector_combinator->left()->accept(*this);

    if (selector_combinator->right())
        selector_combinator->right()->accept(*this);
}

void
CssModifier::
visit(const CssSelectorPtr &selector)
{
    // If current selector is a keyframes selector
    if (hasContext(KEYFRAMES_BLOCK)) {
        // Replace "from" by "0%"
        if (selector->name() == "from")
            selector->setName("0%");
        // Replace "100%" by "to"
        else if (selector->name() == "100%")
            selector->setName("to");
    }

    if (selector->selectorType() == CssSelector::ID && s_minify_ids) {
        const auto &found = m_css_id_replacement_list->find(selector->name());

        if (found == m_css_id_replacement_list->end()) {
            // Create class list entry from selector's current value
            const auto &ident_pair = make_shared<IdentPair>(make_shared<string>(selector->name()));
            m_css_id_replacement_list->emplace(selector->name(), ident_pair);
            // Let selector point to the newly created class list entry value
            selector->setName(ident_pair->replacement_id);
        } else {
            // Let selector point to the already existent class list value
            selector->setName(found->second->replacement_id);
            // Increment occurrence counter
            ++found->second->count;
        }
    }
    else if (selector->selectorType() == CssSelector::CLASS && s_minify_class_names) {
        const auto &found = m_css_class_replacement_list->find(selector->name());

        if (found == m_css_class_replacement_list->end()) {
            // Create class list entry from selector's current value
            const auto ident_pair = make_shared<IdentPair>(make_shared<string>(selector->name()));
            m_css_class_replacement_list->emplace(selector->name(), ident_pair);
            // Let selector point to the replacement value
            selector->setName(ident_pair->replacement_id);
        } else {
            // Let selector point to the already existent class list value
            selector->setName(found->second->replacement_id);
            // Increment occurrence counter
            ++found->second->count;
        }
    }

    if (selector->selectorType() == CssSelector::AN_PLUS_B) {
        if (selector->name() == "even")
            selector->setName("2n");
        else if (selector->name() == "2n+1")
            selector->setName("odd");
    }

    if (selector->parentalSelector())
        selector->parentalSelector()->accept(*this);

    if (!selector->subSelectors().empty())
        for (const auto &selector : selector->subSelectors())
            selector->accept(*this);
}

void
CssModifier::
visit(const CssSelectorAttributePtr &attribute_selector)
{
    if (attribute_selector->parentalSelector())
        attribute_selector->parentalSelector()->accept(*this);
}

void
CssModifier::
visit(const CssDelimiterPtr &/*delimiter*/)
{

}

void
CssModifier::
visit(const CssUnicodeRangePtr &/*unicode_range*/)
{

}

void
CssModifier::
visit(const CssSupportsConditionPtr &/*supports_condition*/)
{

}

void
CssModifier::
visit(const CssCommentPtr &)
{

}

void
CssModifier::
writePhpIncludeFile(const string &file_name)
{
    string buffer  = "<?php ";

    const auto writeArray = [&](const shared_ptr<HashTable<string, IdentPairPtr> > &list) -> void {
            string php_array_name;

            if (&*list == &*m_css_id_replacement_list)
                php_array_name = cfg.phpIdArrayName();
            else if (&*list == &*m_css_class_replacement_list)
                php_array_name = cfg.phpClassArrayName();
            else if (&*list == &*m_css_custom_property_replacement_list)
                php_array_name = cfg.phpCustomPropertyArrayName();
            else if (&*list == &*m_css_animation_name_replacement_list)
                php_array_name = cfg.phpAnimationArrayName();

            buffer += "$";
            buffer += php_array_name;
            buffer += "=array(";

            for (auto itr = list->begin(); itr != list->end(); ++itr) {
                buffer += "'";
                buffer += itr->first;
                buffer += "'=>'";
                buffer += *itr->second->replacement_id;
                buffer += "'";

                if (next(itr) != list->end())
                    buffer += ",";
            }

            buffer += ");";
    };

    const auto lists = {
        m_css_id_replacement_list,
        m_css_class_replacement_list,
        m_css_custom_property_replacement_list,
        m_css_animation_name_replacement_list
    };

    for (const auto &list : lists)
        if (!list->empty())
            writeArray(list);

    buffer += "?>";

    FileSystemWorker::writeFile(cfg.outputPath() + DIR_SEP + file_name, buffer);
}

void
CssModifier::
generateNames(shared_ptr<HashTable<string, IdentPairPtr> > &list, string &name)
{
    // Sorting rule
    const auto sorting_condition =
    [](const IdentPairPtr &a, const IdentPairPtr &b)
    { return a->original_id->length() * a->count < b->original_id->length() * b->count; };

    // Create temporal container
    DataContainer<IdentPairPtr> vect;

    // Fill temporal container with pointers to entries of the replacement list
    for (const auto &key_value_pair : *list)
        vect.emplace_back(key_value_pair.second);

    // Sort only if container size is greater than 52
    if (vect.size() > 52)
        sort(vect.rbegin(), vect.rend(), sorting_condition);

    // Generate replacement IDs for all the empty entries in the container
    for (size_t i = 0; i < vect.size(); ++i)
        if (vect[i]->replacement_id->empty())
            *vect[i]->replacement_id = getShortId(name);
}

void
CssModifier::
generateIds()
{
    generateNames(m_css_id_replacement_list, m_id_replacement_name);
}

void
CssModifier::
generateClassNames()
{
    generateNames(m_css_class_replacement_list, m_class_replacement_name);
}

void
CssModifier::
generateCustomPropertyNames()
{
    generateNames(m_css_custom_property_replacement_list, m_cprop_replacement_name);
}

void
CssModifier::
generateAnimationNames()
{
    generateNames(m_css_animation_name_replacement_list, m_animation_replacement_name);
}

const string
CssModifier::
getShortHexColorValue(const string &hex_value)
{
    // If hex value has a length of 6 digits (RGB)...
    if (hex_value.length() == 6U) {
        // If the hex digits are pairwise the same, replace them with one digit.
        // Example: 0055aa => 05a
        if (hex_value[0] == hex_value[1] &&
            hex_value[2] == hex_value[3] &&
            hex_value[4] == hex_value[5]) {

            string new_hex_value;
            new_hex_value  = hex_value[0];
            new_hex_value += hex_value[2];
            new_hex_value += hex_value[4];

            return new_hex_value;
        }
    // If hex value has a length of 8 digits (RGBA)...
    } else if (hex_value.length() == 8U) {
        // If the hex digits are pairwise the same, replace them with one digit.
        // Example: 0055aaff => 05af
        if (hex_value[0] == hex_value[1] &&
            hex_value[2] == hex_value[3] &&
            hex_value[4] == hex_value[5] &&
            hex_value[6] == hex_value[7]) {

            string new_hex_value;
            new_hex_value  = hex_value[0];
            new_hex_value += hex_value[2];
            new_hex_value += hex_value[4];
            new_hex_value += hex_value[6];

            return new_hex_value;
        }
    }

    return hex_value;
}

const string
CssModifier::
getShortNumber(const string &number_value)
{
    string value = number_value;

    // Remove leading zeros
    while (value.front() == '0' && value.length() > 1)
        value.erase(value.begin());

    // If number contains a dot...
    if (String::contains(value, '.')) {
        // Remove trailing zeros
        while (value.back() == '0') value.pop_back();
        // Remove trailing dot
        if (value.back() == '.') value.pop_back();
        // If value is an empty string now, set it to zero.
        if (value.empty()) value = "0";
    }
    // If number does not contain a dot...
    else {
        for (auto itr = value.rbegin(); itr != value.rend(); ++itr) {
            // If number ends with some zeros, iterate through value
            // until a non-zero digit is found
            if (*itr == '0') continue;

            // Get count of zeroes
            const auto diff = distance(value.rbegin(), itr);

            // If zero count is greater than 2...
            if (diff > 2) {
                // Replace number with the equivalent scientific notation
                // Example: 10000 => 1e4
                value = string(value.begin(), itr.base()) + 'e' + to_string(diff);
            }

            break;
        }
    }

    return value;
}

/*static*/ void
CssModifier::
replaceRgbaFuncWithRgbaHexColor(const CssFunctionPtr &function)
{
    // https://www.w3.org/TR/css-color-4/#rgb-functions
    // https://www.w3.org/TR/css-color-4/#hex-notation

    const auto &params = function->parameters();
    CssBaseElementPtr r_base_elem, g_base_elem, b_base_elem, a_base_elem;

    switch (params.size()) {
    case 3U:
        r_base_elem = params[0][0];
        g_base_elem = params[1][0];
        b_base_elem = params[2][0];
        break;
    case 4U:
        r_base_elem = params[0][0];
        g_base_elem = params[1][0];
        b_base_elem = params[2][0];
        a_base_elem = params[3][0];
        break;
    case 1U:
        if (params[0].size() == 3U) {
            r_base_elem = params[0][0];
            g_base_elem = params[0][1];
            b_base_elem = params[0][2];
        } else if (params[0].size() == 5U && params[0][3]->isDelimiter()) {
            if (static_pointer_cast<CssDelimiter>(params[0][3])->value() == "/") {
                r_base_elem = params[0][0];
                g_base_elem = params[0][1];
                b_base_elem = params[0][2];
                a_base_elem = params[0][4];
            } else return;
        } else return;

        break;
    default: return;
    }

    if (r_base_elem && g_base_elem && b_base_elem) {
        uint8_t uint8_r = 0, uint8_g = 0, uint8_b = 0, uint8_a = UCHAR_MAX;
        string str_a;

        if (r_base_elem->isNumber() && g_base_elem->isNumber() && b_base_elem->isNumber()) {
            uint8_r = uint8_t(stoi(static_pointer_cast<CssNumber>(r_base_elem)->value()));
            uint8_g = uint8_t(stoi(static_pointer_cast<CssNumber>(g_base_elem)->value()));
            uint8_b = uint8_t(stoi(static_pointer_cast<CssNumber>(b_base_elem)->value()));
        }
        else if (r_base_elem->isPercentage() && g_base_elem->isPercentage() && b_base_elem->isPercentage()) {
            const auto
            &r_elem = static_pointer_cast<CssPercentage>(r_base_elem),
            &g_elem = static_pointer_cast<CssPercentage>(g_base_elem),
            &b_elem = static_pointer_cast<CssPercentage>(b_base_elem);

            uint8_r = uint8_t(round(2.55F * stof(r_elem->value())));
            uint8_g = uint8_t(round(2.55F * stof(g_elem->value())));
            uint8_b = uint8_t(round(2.55F * stof(b_elem->value())));
        } else return;

        if (a_base_elem) {
            if (s_use_rgba_hex_color_notation) {
                if (a_base_elem->isNumber())
                    uint8_a = uint8_t(round(255.0F * stof(static_pointer_cast<CssNumber>(a_base_elem)->value())));
                else if (a_base_elem->isPercentage())
                    uint8_a = uint8_t(round(2.55F * stof(static_pointer_cast<CssPercentage>(a_base_elem)->value())));

                if (uint8_a == 0U) {
                    const auto color = make_shared<CssColor>(CssColor::PREDEFINED_NAME, "transparent");
                    function->setReplacementElement(color);
                    return;
                }
            } else {
                if (a_base_elem->isNumber())
                    str_a = static_pointer_cast<CssNumber>(a_base_elem)->value();
                else if (a_base_elem->isPercentage())
                    str_a = percentageToNumber(static_pointer_cast<CssPercentage>(a_base_elem)->value());

                if (str_a == "0") {
                    const auto color = make_shared<CssColor>(CssColor::PREDEFINED_NAME, "transparent");
                    function->setReplacementElement(color);
                    return;
                }
            }
        }

        if (s_use_rgba_hex_color_notation || ((str_a.empty() || str_a == "1") && uint8_a == UCHAR_MAX)) {
            const auto &hex_color = getHexColorFromRGBA(uint8_r, uint8_g, uint8_b, uint8_a);
            const auto &hex_color_element = make_shared<CssColor>(CssColor::HEX_LITERAL, hex_color);
            function->setReplacementElement(hex_color_element);
            return;
        }

        r_base_elem->setReplacementElement(make_shared<CssNumber>(to_string(uint8_r)));
        g_base_elem->setReplacementElement(make_shared<CssNumber>(to_string(uint8_g)));
        b_base_elem->setReplacementElement(make_shared<CssNumber>(to_string(uint8_b)));

        // Replace percentage only if number is shorter
        if (a_base_elem && a_base_elem->isPercentage() &&
            static_pointer_cast<CssPercentage>(a_base_elem)->value().length() + 1 > str_a.length())
            a_base_elem->setReplacementElement(make_shared<CssNumber>(str_a));
    }
}

/*static*/ void
CssModifier::
maybeManipulateHslaFunction(const CssFunctionPtr &function)
{
    // https://www.w3.org/TR/css-color-4/#the-hsl-notation

    const auto &params = function->parameters();
    CssBaseElementPtr h_base_elem, s_base_elem, l_base_elem, a_base_elem;

    switch (params.size()) {
    case 3:
        h_base_elem = params[0][0];
        s_base_elem = params[1][0];
        l_base_elem = params[2][0];
        break;
    case 4:
        h_base_elem = params[0][0];
        s_base_elem = params[1][0];
        l_base_elem = params[2][0];
        a_base_elem = params[3][0];
        break;
    case 1:
        if (params[0].size() == 3) {
            h_base_elem = params[0][0];
            s_base_elem = params[0][1];
            l_base_elem = params[0][2];
        } else if (params[0].size() == 5U && params[0][3]->isDelimiter()) {
            if (static_pointer_cast<CssDelimiter>(params[0][3])->value() == "/") {
                h_base_elem = params[0][0];
                s_base_elem = params[0][1];
                l_base_elem = params[0][2];
                a_base_elem = params[0][4];
            } else return;
        } else return;

        break;
    default: return;
    }

    if (h_base_elem && s_base_elem && l_base_elem) {
        if (!s_base_elem->isPercentage() && !l_base_elem->isPercentage()) return;

        const auto
        s_elem = static_pointer_cast<CssPercentage>(s_base_elem),
        l_elem = static_pointer_cast<CssPercentage>(l_base_elem);

        const auto
        h_elem = static_pointer_cast<CssNumber>(h_base_elem),
        a_elem = static_pointer_cast<CssNumber>(a_base_elem);

        auto a = float(UCHAR_MAX);

        if (a_elem) {
            if (a_elem->isPercentage())
                a *= stof(percentageToNumber(a_elem->value()));
            else
                a *= stof(a_elem->value());

            if (a == 0.0F) {
                const auto identifier = make_shared<CssIdentifier>("transparent");
                function->setReplacementElement(identifier);
                return;
            }
        }

        // Recalculate hsl()/hsla() function to rgb()/rgba() function
        const auto calculateRgbValues = [](const string &hue, const string &sat, const string &light) -> const DataContainer<uint8_t> {
            float
            h = stof(hue),
            s = stof(sat) / 100.F,
            l = stof(light) / 100.F;

            if (h == 360.F) h = 0.F;

            float
            C = (1 - fabs(2.F * l - 1)) * s,
            X = C * (1.F - fabs(h / 60.F - 1.F)),
            m = l - C / 2.F;

            float r = m, g = m, b = m;

            if (h >= 0.F && h < 60.F) {
                r += C; g += X;
            }
            else if (h >= 60.F && h < 120.F) {
                r += X; g += C;
            }
            else if (h >= 120.F && h < 180.F) {
                g += C; b += X;
            }
            else if (h >= 180.F && h < 240.F) {
                g += X; b += C;
            }
            else if (h >= 240.F && h < 300.F) {
                r += X; b += C;
            }
            else if (h >= 300.F && h < 360.F) {
                r += C; b += X;
            }

            return {
                uint8_t(round(r * 255.0F)),
                uint8_t(round(g * 255.0F)),
                uint8_t(round(b * 255.0F))
            };
        };

        DataContainer<uint8_t> rgb_colors = calculateRgbValues(h_elem->value(), s_elem->value(), l_elem->value());

        const string
        str_r = to_string(rgb_colors.at(0)),
        str_g = to_string(rgb_colors.at(1)),
        str_b = to_string(rgb_colors.at(2));

        string str_a;

        const auto rgb_function = make_shared<CssFunction>(function->name().back() == 'a' ? "rgba" : "rgb");

        rgb_function->parameters() = {
            {make_shared<CssNumber>(str_r)},
            {make_shared<CssNumber>(str_g)},
            {make_shared<CssNumber>(str_b)}
        };

        if (a < 255.0F) {
            a /= 255.0F;

            // ### If the rgb()/rgba() function gets longer than hsl()/hsla() function, don't rewrite

            str_a = getShortNumber(to_string(a));

            const uint8_t
            str_h_length = uint8_t(h_elem->value().length() + (h_elem->isDimension() ? static_pointer_cast<CssDimension>(h_elem)->unit().length() : 0)),
            str_s_length = uint8_t(s_elem->value().length() + 1),
            str_l_length = uint8_t(l_elem->value().length() + 1),
            str_a_length = uint8_t(str_a.length());

            const uint64_t
            hsla_param_length = str_h_length + str_s_length + str_l_length + str_a_length,
            rgba_param_length = str_r.length() + str_g.length() + str_b.length() + str_a_length;

            if (hsla_param_length <= rgba_param_length) {
                const auto &number = make_shared<CssNumber>(percentageToNumber(a_elem->value()));
                a_elem->setReplacementElement(number);
                return;
            }

            // ###

            rgb_function->parameters().push_back({make_shared<CssNumber>(str_a)});
        }

        function->setReplacementElement(rgb_function);
    }
}

/*static*/ const string
CssModifier::
getHexColorFromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    const bool use_alpha = a != UCHAR_MAX;
    constexpr array<char, 17> DIGITS = {"0123456789abcdef"};
    basic_string<uint8_t> uint_colors = { r, g, b, a };
    DataContainer<string> hex_colors = { string(), string(), string(), string() };
    constexpr uint8_t HEX_BASE = 16;

    for (uint8_t i = 0; i != (use_alpha ? 4 : 3); ++i) {
        do hex_colors[i] += DIGITS.at(uint_colors[i] % HEX_BASE); while ((uint_colors[i] /= HEX_BASE) != 0U);
        if (hex_colors[i].length() == 1) hex_colors[i] += DIGITS[0];
    }

    reverse(hex_colors[0].begin(), hex_colors[0].end());
    reverse(hex_colors[1].begin(), hex_colors[1].end());
    reverse(hex_colors[2].begin(), hex_colors[2].end());

    hex_colors[0] += hex_colors[1];
    hex_colors[0] += hex_colors[2];

    if (use_alpha) {
        reverse(hex_colors[3].begin(), hex_colors[3].end());
        hex_colors[0] += hex_colors[3];
    }

    return getShortHexColorValue(hex_colors[0]);
}

void
CssModifier::
maybeImportStyleSheet(const CssAtRulePtr &at_rule_import)
{
    string absolute_input_path, initial_import_path_value;

    ++s_import_depth;

    if (!at_rule_import->expressions().empty() && !at_rule_import->expressions()[0].empty()) {
        if (at_rule_import->expressions()[0][0]->isString()) {
            const auto &string_element = static_pointer_cast<CssString>(at_rule_import->expressions()[0][0]);
            initial_import_path_value = string_element->value();
        } else if (at_rule_import->expressions()[0][0]->isFunction()) {
            const auto &function_element = static_pointer_cast<CssFunction>(at_rule_import->expressions()[0][0]);

            if (function_element->name("url")) {
                const auto &string_element = static_pointer_cast<CssString>(function_element->parameters()[0][0]);
                initial_import_path_value = string_element->value();
            } else return;
        }

        initial_import_path_value.front() == '/' && RETURN("Absolute @import path '" + initial_import_path_value + "'. Consider using relative path.");

        absolute_input_path = FileSystem::getParentPath(cfg.inputPath()) + DIR_SEP + initial_import_path_value;

#ifdef WIN
        for (auto &chr : absolute_input_path)
            if (chr == '/') chr = '\\';
#endif

        absolute_input_path = FileSystem::getCleanPath(absolute_input_path);

        const auto base_name = FileSystem::getBaseName(absolute_input_path);
        const string indentation = String::repeat("> ", s_import_depth);

        if (!s_output_to_stdo)
            Console::writeLine("Processing import file '" + base_name + "'", indentation);

        string file_content;
        FileSystemWorker::readFile(absolute_input_path, file_content);
        FileSystemWorker::addInputFile(absolute_input_path);

        const auto &ast = CssParser::parse(file_content);

        if ((!at_rule_import->expressions().empty() && at_rule_import->expressions()[0].size() > 1) ||
             at_rule_import->expressions().size() > 1) {

            const auto at_rule_media = make_shared<CssAtRule>("media");
            at_rule_media->setBlock(make_shared<CssBlock>(CssBlock::CURLY));

            for (const auto &list : at_rule_import->expressions()) {
                for (const auto &expr : list)
                    if (&list != &at_rule_import->expressions().front() ||
                       (&list == &at_rule_import->expressions().front() &&
                        &expr != &list.front()))
                        at_rule_media->appendExpression(expr);

                if (&list != &at_rule_import->expressions().back())
                    at_rule_media->createList();
            }

            if (s_include_external_stylesheets) {
                ast->setBlockType(CssBlock::CURLY);
                at_rule_media->setBlock(ast);
                at_rule_import->setReplacementElement(at_rule_media);
                at_rule_import->replacementElement()->accept(*this);
            }
        } else {
            if (s_include_external_stylesheets) {
                ast->setBlockType(CssBlock::DEFAULT);
                at_rule_import->setReplacementElement(ast);
                at_rule_import->replacementElement()->accept(*this);
            }
        }

        if (s_include_external_stylesheets) {
            if (!s_output_to_stdo)
                Console::writeLine("[Done] Processing import file '" + base_name + "'", indentation);

            return;
        }

        ast->accept(*this);

        file_content.clear();
        CssGenerator css_generator(file_content);
        ast->accept(css_generator);

        string absolute_output_path, relative_path;

        if (!cfg.inputWorkingDirectory().empty()) {
            relative_path = FileSystem::getRelativePath(absolute_input_path, cfg.inputWorkingDirectory());

            while (relative_path.front() == DIR_SEP[0])
                relative_path.erase(relative_path.begin());

            absolute_output_path = FileSystem::getCleanPath(cfg.outputPath() + DIR_SEP + relative_path);
        }
        else {
            relative_path = FileSystem::getRelativePath2(cfg.inputPath(), absolute_input_path);
            absolute_output_path = FileSystem::getCleanPath(cfg.outputPath() + DIR_SEP + relative_path);
        }

        if (absolute_output_path.length() >= cfg.outputPath().length() &&
            absolute_output_path.substr(0, cfg.outputPath().length()) != cfg.outputPath())
            RETURN(NEWLINE "The output path '" + FileSystem::getParentPath(absolute_output_path) + "' leaves the output directory." NEWLINE
                   "Consider to use input/output working directory to build the correct directory structure." NEWLINE
                   "See configuration file." NEWLINE);

        FileSystemWorker::addOutputFile(absolute_output_path);

        FileSystemWorker::createPath(FileSystem::getParentPath(absolute_output_path));

        FileSystemWorker::writeFile(absolute_output_path, file_content);

        Console::writeLine("[Done] Processing import file '" + base_name + "'", indentation);
        Console::writeFileSizeDifference(FileSystem::getFileSize(absolute_input_path),
                                         FileSystem::getFileSize(absolute_output_path),
                                         indentation);
    }

    --s_import_depth;
}

/*static*/ uint8_t
CssModifier::
getStringNumberPrecision(const string &number)
{
    for (auto itr = number.begin(); itr != number.end(); ++itr)
        if (*itr == '.')
            return static_cast<uint8_t>(distance(itr+1, number.end()));

    return 0;
}

/*static*/ const string
CssModifier::
cutStringNumberToPrecision(string number, const uint8_t precision)
{
    const auto dot_pos = find(number.rbegin(), number.rend(), '.');

    if (dot_pos != number.rend()) {
        const auto end = dot_pos.base() + precision;

        if (end < number.end())
            number = string(number.begin(), end);
    }

    return number;
};

/*static*/ const string
CssModifier::
percentageToNumber(string percentage)
{
    // Converts percentage to decimal number
    // Example: 70% => .7

    const auto dot = find(percentage.begin(), percentage.end(), '.');
    const auto count_before_dot = static_cast<uint8_t>(distance(percentage.begin(), dot));

    if (dot != percentage.end())
        percentage.erase(dot);

    if (count_before_dot == 1)
        percentage.insert(percentage.begin(), {'.', '0'});
    else
        percentage.insert(dot-2, '.');

    return getShortNumber(percentage);
}

/*static*/ bool
CssModifier::
minifyAngle(const CssDimensionPtr &dimension)
{
    // Return false, if no changes have been made

    // Do not handle angles, where scientific postfix is set
    if (!dimension->scientificPostfix().empty()) return false;

    if (dimension->value("0")) {
        dimension->setNegativeFlag(false);
        dimension->setUnit("deg");
        return true;
    }

    constexpr double
    DEG_ANGLE_BASE = 360.,
    GRAD_ANGLE_BASE = 400.,
    TURN_ANGLE_BASE = 1.,
    RAD_ANGLE_BASE = M_PI;

    double angle = stod(dimension->value());
    bool negative_angle = dimension->isNegative();
    const auto length_before = (negative_angle ? 1 : 0) + dimension->value().length() + dimension->unit().length();
    string str_unit = "deg";

    if (dimension->unit("grad")) {
        // Recalculate grad angle to deg angle
        angle /= GRAD_ANGLE_BASE;
        angle *= DEG_ANGLE_BASE;

        // Normalize angle to range 0...<360° if it is >= 360°
        while (angle >= DEG_ANGLE_BASE)
            angle -= DEG_ANGLE_BASE;

        if (angle == 0.) {
            dimension->setNegativeFlag(false);
            dimension->setNumber("0");
            dimension->setUnit("deg");
            return true;
        }
    }
    else if (dimension->unit("turn")) {
        // Normalize angle to range 0...<1turn if it is >= 1turn
        while (angle >= TURN_ANGLE_BASE)
            angle -= TURN_ANGLE_BASE;

        if (angle == 0.) {
            dimension->setNegativeFlag(false);
            dimension->setNumber("0");
            dimension->setUnit("deg");
            return true;
        }

        // Recalculate turn angle to deg angle
        angle /= TURN_ANGLE_BASE;
        angle *= DEG_ANGLE_BASE;
    }
    else if (dimension->unit("rad")) {
        // The result will be cut to initial angle precision
        const auto initial_angle_precision = getStringNumberPrecision(dimension->value());

        // Normalize angle to range 0...<PIrad if it is >= PIrad
        while (angle >= RAD_ANGLE_BASE)
            angle -= RAD_ANGLE_BASE;

        if (angle == 0.) {
            dimension->setNegativeFlag(false);
            dimension->setNumber("0");
            dimension->setUnit("deg");
            return true;
        }

        // Handle negative angles only, if angle >= 1 and the angle precition is at least 2
        if (negative_angle && angle >= 1. && initial_angle_precision > 1) {
            // Recalculate negative angle to positive angle
            angle = RAD_ANGLE_BASE - angle;
            // Get rid of minus sign
            negative_angle = false;
        }

        dimension->setNegativeFlag(negative_angle);
        dimension->setNumber(getShortNumber(cutStringNumberToPrecision(getShortNumber(to_string(angle)), initial_angle_precision)));

        return true;
    }

    // 350...360deg ==> 0...>-10deg
    if (angle > DEG_ANGLE_BASE - 10.) {
        angle = DEG_ANGLE_BASE - angle;
        negative_angle = !negative_angle;
    }
    // If negative angle
    else if (dimension->isNegative()) {
        // -100...>=-360deg ==> 0...<=260deg
        if (angle >= 100.) {
            // Recalculate negative angle to positive angle
            angle = DEG_ANGLE_BASE - angle;
            // Get rid of minus sign
            negative_angle = false;
        }
        else if (angle == 0.) {
            // Get rid of minus sign
            negative_angle = false;
        }
    }

    string str_angle = getShortNumber(to_string(angle));

    auto length_after = (negative_angle ? 1 : 0) + str_angle.length() + str_unit.length();

    // Make sure dimension won't have the same string length or get longer
    if (length_before >= length_after) {
        dimension->setNegativeFlag(negative_angle);
        dimension->setNumber(str_angle);
        dimension->setUnit(str_unit);
        return true;
    }

    // If dimension results in the same length or a longer string with deg unit,
    // try to minify the grad angle, if initially dimension has a grad angle
    if (dimension->unit("grad")) {
        angle = stod(dimension->value());
        str_unit = "grad";

        // 390...400deg ==> 0...>-10grad
        if (angle > GRAD_ANGLE_BASE - 10.) {
            angle = GRAD_ANGLE_BASE - angle;
            str_angle = getShortNumber(to_string(angle));
            negative_angle = true;

            length_after = str_angle.length() + str_unit.length() + 1;

            // Make sure dimension won't have the same string length or get longer
            if (length_before > length_after) {
                dimension->setNegativeFlag(negative_angle);
                dimension->setNumber(str_angle);
                dimension->setUnit(str_unit);
                return true;
            }
        }
    }

    // If no case applies, signal that no changes have been made by returning false
    return false;
}
