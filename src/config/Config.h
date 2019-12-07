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

#ifndef CONFIG_H
#define CONFIG_H
#include "../DataContainer.h"
#include "../HashTable.h"
#include "../filesystem/FileSystem.h"
#include "defs.h"
#include <sstream>

extern bool RETURN [[noreturn]] (const string &message);

class Config
{
public:
    enum Setting : uint8_t {
        UNKNOWN                                     ,

        // bool settings
        GENERAL__OUTPUT_TO_STDO                     ,
        GENERAL__USE_UTF8_BOM                       ,
        GENERAL__CREATE_PHP_INCLUDE_FILE            ,
        CSS__INCLUDE_EXTERNAL_STYLESHEETS           ,
        CSS__REMOVE_COMMENTS                        ,
        CSS__MINIFY_NUMBERS                         ,
        CSS__MINIFY_COLORS                          ,
        CSS__REWRITE_ANGLES                         ,
        CSS__USE_RGBA_HEX_NOTATION                  ,
        CSS__MINIFY_IDS                             ,
        CSS__MINIFY_CLASS_NAMES                     ,
        CSS__MINIFY_CUSTOM_PROPERTIES               ,
        CSS__MINIFY_ANIMATION_NAMES                 ,
        CSS__REWRITE_FUNCTIONS                      ,
        CSS__REMOVE_EMPTY_RULES                     ,
        CSS__MERGE_MEDIA_RULES                      ,

        // string settings
        GENERAL__INPUT_WORKING_DIRECTORY            ,
        GENERAL__OUTPUT_WORKING_DIRECTORY           ,
        GENERAL__INPUT_PATH                         ,
        GENERAL__OUTPUT_PATH                        ,
        GENERAL__PHP_ID_ARRAY_NAME                  ,
        GENERAL__PHP_CLASS_ARRAY_NAME               ,
        GENERAL__PHP_CUSTOM_PROPERTY_ARRAY_NAME     ,
        GENERAL__PHP_ANIMATION_ARRAY_NAME           ,
        GENERAL__TAB_WIDTH                          ,

        // list settings
        GENERAL__CSS_FILE_EXTENSIONS                ,
        CSS__COMMENT_TERMS
    };

    Config();

    void
    readConf(),
    validatePaths(),
    printConfigInfo();

    inline void
    setConfigFilePath(const string &config_file_path),
    setInputPath(const string &path),
    setOutputPath(const string &path),
    setPhpIdArrayName(const string &name),
    setPhpClassArrayName(const string &name),
    setPhpCustomPropertyArrayName(const string &name),
    setPhpAnimationArrayName(const string &name),
    setTabWidth(const uint8_t tab_width),
    enable(const Setting setting),
    enable(const initializer_list<const Setting> settings),
    disable(const Setting setting),
    disable(const initializer_list<const Setting> settings);

    inline bool
    isEnabled(const uint32_t setting) const;

    inline uint32_t
    boolSettings() const;

    inline const DataContainer<string>
    cssFileExtensions() const,
    cssCommentTerms() const;

    inline const string
    &configFilePath() const,
    &inputWorkingDirectory() const,
    &outputWorkingDirectory() const,
    &inputPath() const,
    &outputPath() const,
    &phpIdArrayName() const,
    &phpClassArrayName() const,
    &phpCustomPropertyArrayName() const,
    &phpAnimationArrayName() const;

    inline uint8_t
    tabWidth() const;

    inline bool
    isRead() const;

private:
    inline void
    setListSetting(const Setting setting, const DataContainer<string> &value_list),
    setStringSetting(const Setting setting, const string &value),
    setNumericSetting(const Setting setting, const uint8_t value),
    setBoolSettings(const uint32_t settings),

    throwConfigFileError [[noreturn]] (const string &message, uint64_t line_number, uint64_t col_number);

    void
    readConfigFile(const string &path, string &content),
    parseConfigFileContent(const string &content);

    bool
    isListSetting(const string& str_setting, Setting &setting) const,
    isStringSetting(const string& str_setting, Setting &setting) const,
    isBoolSetting(const string& str_setting, Setting &setting) const,
    isNumericSetting(const string& str_setting, Setting &setting) const;

    const DataContainer<string>
    getListSettingValues(const Setting setting) const;

    string &
    getStringSettingValue(const Setting setting);

    uint8_t
    getNumericSettingValue(const Setting setting) const;

    inline void
    setIsRead(const bool is_read = true);

    HashTable<const Setting, DataContainer<string> >
    m_list_settings;

    HashTable<const Setting, string>
    m_string_settings;

    HashTable<const Setting, uint8_t>
    m_numeric_settings;

    struct ConfigTerms
    {
        ConfigTerms() :
            headers({
                "general", "css"
            }),
            bool_settings({
                { "general_use_utf8_bom",                   Config::GENERAL__USE_UTF8_BOM },
                { "general_create_php_include_file",        Config::GENERAL__CREATE_PHP_INCLUDE_FILE },

                { "css_include_external_stylesheets",       Config::CSS__INCLUDE_EXTERNAL_STYLESHEETS },
                { "css_remove_comments",                    Config::CSS__REMOVE_COMMENTS },
                { "css_minify_numbers",                     Config::CSS__MINIFY_NUMBERS },
                { "css_minify_colors",                      Config::CSS__MINIFY_COLORS },
                { "css_rewrite_angles",                     Config::CSS__REWRITE_ANGLES },
                { "css_use_rgba_hex_notation",              Config::CSS__USE_RGBA_HEX_NOTATION },
                { "css_minify_ids",                         Config::CSS__MINIFY_IDS },
                { "css_minify_class_names",                 Config::CSS__MINIFY_CLASS_NAMES },
                { "css_minify_custom_properties",           Config::CSS__MINIFY_CUSTOM_PROPERTIES },
                { "css_minify_animation_names",             Config::CSS__MINIFY_ANIMATION_NAMES },
                { "css_rewrite_functions",                  Config::CSS__REWRITE_FUNCTIONS },
                { "css_remove_empty_rules",                 Config::CSS__REMOVE_EMPTY_RULES },
                { "css_merge_media_rules",                  Config::CSS__MERGE_MEDIA_RULES }
            }),
            string_settings({
                { "general_input_working_directory",        Config::GENERAL__INPUT_WORKING_DIRECTORY },
                { "general_output_working_directory",       Config::GENERAL__OUTPUT_WORKING_DIRECTORY },
                { "general_input_path",                     Config::GENERAL__INPUT_PATH },
                { "general_output_path",                    Config::GENERAL__OUTPUT_PATH },
                { "general_php_id_array_name",              Config::GENERAL__PHP_ID_ARRAY_NAME },
                { "general_php_class_array_name",           Config::GENERAL__PHP_CLASS_ARRAY_NAME },
                { "general_php_cprop_array_name",           Config::GENERAL__PHP_CUSTOM_PROPERTY_ARRAY_NAME },
                { "general_php_animation_array_name",       Config::GENERAL__PHP_ANIMATION_ARRAY_NAME }
            }),
            list_settings({
                { "general_css_file_extensions",            Config::GENERAL__CSS_FILE_EXTENSIONS },
                { "css_comment_terms",                      Config::CSS__COMMENT_TERMS }
            }),
            numeric_settings({
                { "general_tab_width",                      Config::GENERAL__TAB_WIDTH }
            }) {}

        const DataContainer<string>
        headers;

        const DataContainer<pair<const string, const Config::Setting> >
        bool_settings, string_settings, list_settings, numeric_settings;
    } const m_config_terms;

    uint32_t m_bool_settings;

    // Contains the path to config file
    string m_config_file {CONFIG_FILE_PATH};

    // If configuration file has already been read...
    bool m_config_is_read;
} extern cfg;

inline void
Config::
setConfigFilePath(const string &config_file_path)
{
    m_config_file = config_file_path;
}

inline void
Config::
setIsRead(const bool is_read)
{
    m_config_is_read = is_read;
}

inline bool
Config::
isRead() const
{
    return m_config_is_read;
}

inline const DataContainer<string>
Config::
cssFileExtensions() const
{
    return m_list_settings.find(GENERAL__CSS_FILE_EXTENSIONS)->second;
}

inline const DataContainer<string>
Config::
cssCommentTerms() const
{
    return m_list_settings.find(CSS__COMMENT_TERMS)->second;
}

inline const string &
Config::
configFilePath() const
{
    return m_config_file;
}

inline const string &
Config::
inputWorkingDirectory() const
{
    return m_string_settings.find(GENERAL__INPUT_WORKING_DIRECTORY)->second;
}

inline const string &
Config::
outputWorkingDirectory() const
{
    return m_string_settings.find(GENERAL__OUTPUT_WORKING_DIRECTORY)->second;
}

inline void
Config::
setInputPath(const string &path)
{
    setStringSetting(GENERAL__INPUT_PATH, path);
}

inline const string &
Config::
inputPath() const
{
    return m_string_settings.find(GENERAL__INPUT_PATH)->second;
}

inline void
Config::
setOutputPath(const string &path)
{
    setStringSetting(GENERAL__OUTPUT_PATH, path);
}

inline const string &
Config::
outputPath() const
{
    return m_string_settings.find(GENERAL__OUTPUT_PATH)->second;
}

inline void
Config::
setPhpIdArrayName(const string &name)
{
    setStringSetting(GENERAL__PHP_ID_ARRAY_NAME, name);
}

inline const string &
Config::
phpIdArrayName() const
{
    return m_string_settings.find(GENERAL__PHP_ID_ARRAY_NAME)->second;
}

inline void
Config::
setPhpClassArrayName(const string &name)
{
    setStringSetting(GENERAL__PHP_CLASS_ARRAY_NAME, name);
}

inline const string &
Config::
phpClassArrayName() const
{
    return m_string_settings.find(GENERAL__PHP_CLASS_ARRAY_NAME)->second;
}

inline void
Config::
setPhpCustomPropertyArrayName(const string &name)
{
    setStringSetting(GENERAL__PHP_CUSTOM_PROPERTY_ARRAY_NAME, name);
}

inline const string &
Config::
phpCustomPropertyArrayName() const
{
    return m_string_settings.find(GENERAL__PHP_CUSTOM_PROPERTY_ARRAY_NAME)->second;
}

inline void
Config::
setPhpAnimationArrayName(const string &name)
{
    setStringSetting(GENERAL__PHP_ANIMATION_ARRAY_NAME, name);
}

inline const string &
Config::
phpAnimationArrayName() const
{
    return m_string_settings.find(GENERAL__PHP_ANIMATION_ARRAY_NAME)->second;
}

inline void
Config::
setTabWidth(const uint8_t tab_width)
{
    setNumericSetting(GENERAL__TAB_WIDTH, tab_width);
}

inline uint8_t
Config::
tabWidth() const
{
    return m_numeric_settings.find(GENERAL__TAB_WIDTH)->second;
}

inline bool
Config::
isEnabled(const uint32_t setting) const
{
    const auto pos = static_cast<uint16_t>(1U << (setting - 1U));
    const bool isset = (m_bool_settings & pos) == pos;
    return isset;
}

inline uint32_t
Config::
boolSettings() const
{
    return m_bool_settings;
}

inline void
Config::
enable(const Setting setting)
{
    m_bool_settings |= (1U << (setting - 1U));
}

inline void
Config::
enable(const initializer_list<const Setting> settings)
{
    for (const auto &setting : settings)
        m_bool_settings |= (1U << (setting - 1U));
}

inline void
Config::
disable(const Setting setting)
{
    m_bool_settings &= ~(1U << (setting - 1U));
}

inline void
Config::
disable(const initializer_list<const Setting> settings)
{
    for (const auto &setting : settings)
        m_bool_settings &= ~(1U << (setting - 1U));
}

inline void
Config::
setListSetting(const Setting setting, const DataContainer<string> &value_list)
{
    const auto &item = m_list_settings.find(setting);

    if (item != m_list_settings.end())
        item->second = value_list;
}

inline void
Config::
setStringSetting(const Setting setting, const string &value)
{
    const auto &item = m_string_settings.find(setting);

    if (item != m_string_settings.end())
        item->second = value;
}

inline void
Config::
setNumericSetting(const Setting setting, const uint8_t value)
{
    const auto &item = m_numeric_settings.find(setting);

    if (item != m_numeric_settings.end())
        item->second = uint8_t(value);
}

inline void
Config::
setBoolSettings(const uint32_t settings)
{
    m_bool_settings = settings;
}

inline void
Config::
throwConfigFileError(const string &message, uint64_t line_number, uint64_t col_number)
{
	RETURN("Invalid configuration file."
	       "\nError on row " + to_string(line_number) + \
	       " col " + to_string(col_number+1) + ".\n" +
	       message);
}

#endif // CONFIG_H
