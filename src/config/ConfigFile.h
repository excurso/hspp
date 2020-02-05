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
*******************************************************************************/

#ifndef CONFIGFILE_H
#define CONFIGFILE_H
#include "defs.h"
#include "../filesystem/FileSystemWorker.h"

inline void writeConfigFile()
{
    const auto boolSettingValue = [](const Config::Setting setting) -> const string {
        return cfg.isEnabled(setting) ? CONFIG_BOOL_VALUE_TRUE : CONFIG_BOOL_VALUE_FALSE;
    };

    const auto writeListValues = [](const DataContainer<string> &value_list) -> const string {
        string values;
        for (const auto &value : value_list) {
            values += value;

            if (&value != &value_list.back())
                values += " ";
        }

        return values;
    };

    static const string config_file_content =
        "##############################################\n"
        "# HyperSheetsPreprocessor configuration file #\n"
        "##############################################\n\n"

        "# For setting descriptions look:\n"
        "# https://utilizer.de/hspp/documentation#config-file\n\n"

        "[general]\n"
        "input_working_directory        = \n"
        "output_working_directory       = \n\n"

        "css_file_extensions            = " + writeListValues(cfg.cssFileExtensions()) + "\n\n"

        "input_path                     = \n"
        "output_path                    = \n\n"

        "beautify_output                = " + boolSettingValue(Config::GENERAL__BEAUTIFY_OUTPUT) + "\n\n"

        "create_json_file               = " + boolSettingValue(Config::GENERAL__CREATE_JSON_FILE) + "\n\n"

        "json_id_object_name            = ids\n"
        "json_class_object_name         = classes\n"
        "json_cprop_object_name         = cprops\n"
        "json_animation_object_name     = anims\n\n"

        "use_utf8_bom                   = " + boolSettingValue(Config::GENERAL__USE_UTF8_BOM) + "\n"
        "tab_width                      = " + to_string(cfg.tabWidth()) + "\n\n"

        "[css]\n"
        "include_external_stylesheets   = " + boolSettingValue(Config::CSS__INCLUDE_EXTERNAL_STYLESHEETS) + "\n\n"

        "remove_comments                = " + boolSettingValue(Config::CSS__REMOVE_COMMENTS) + "\n"
        "comment_terms                  = " + writeListValues(cfg.cssCommentTerms()) + "\n\n"

        "minify_numbers                 = " + boolSettingValue(Config::CSS__MINIFY_NUMBERS) + "\n"
        "minify_colors                  = " + boolSettingValue(Config::CSS__MINIFY_COLORS) + "\n"
        "rewrite_angles                 = " + boolSettingValue(Config::CSS__REWRITE_ANGLES) + "\n"
        "use_rgba_hex_notation          = " + boolSettingValue(Config::CSS__USE_RGBA_HEX_NOTATION) + "\n"
        "minify_ids                     = " + boolSettingValue(Config::CSS__MINIFY_IDS) + "\n"
        "minify_class_names             = " + boolSettingValue(Config::CSS__MINIFY_CLASS_NAMES) + "\n"
        "minify_custom_properties       = " + boolSettingValue(Config::CSS__MINIFY_CUSTOM_PROPERTIES) + "\n"
        "minify_animation_names         = " + boolSettingValue(Config::CSS__MINIFY_ANIMATION_NAMES) + "\n"
        "rewrite_functions              = " + boolSettingValue(Config::CSS__REWRITE_FUNCTIONS) + "\n"
        "remove_empty_rules             = " + boolSettingValue(Config::CSS__REMOVE_EMPTY_RULES) + "\n"
        "merge_media_rules              = " + boolSettingValue(Config::CSS__MERGE_MEDIA_RULES);

    FileSystemWorker::writeFile(CONFIG_FILE_PATH, config_file_content);
}

#endif // CONFIGFILE_H
