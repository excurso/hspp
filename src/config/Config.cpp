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

#include "Config.h"

Config::Config() : m_bool_settings(0)
{
    //####################### Default settings #######################

    /// Default file extensions which will be used,
    /// if no configuration file is available.
    const DataContainer<string> CSS_FILE_EXTENSIONS {
        ".css"
    };

    const uint8_t TAB_WIDTH = 4;

    /// Default comment terms. Comments containing this terms are kept.
    const DataContainer<string> CSS_COMMENT_TERMS {
        "copyright", "license"
    };

    /// Default minification settings
    constexpr uint32_t DEFAULT_BOOL_SETTINGS =
        (1U << (Config::CSS__REMOVE_COMMENTS - 1U)) |
        (1U << (Config::CSS__MINIFY_NUMBERS - 1U)) |
        (1U << (Config::CSS__MINIFY_COLORS - 1U)) |
        (1U << (Config::CSS__REMOVE_EMPTY_RULES - 1U));

    //##################### Default settings end #####################

    for (const auto &setting : m_config_terms.list_settings)
        m_list_settings.emplace(setting.second, DataContainer<string>());

    for (const auto &setting : m_config_terms.string_settings)
        m_string_settings.emplace(setting.second, string());

    for (const auto &setting : m_config_terms.numeric_settings)
        m_numeric_settings.emplace(setting.second, uint8_t());

    setListSetting(GENERAL__CSS_FILE_EXTENSIONS, CSS_FILE_EXTENSIONS);
    setNumericSetting(GENERAL__TAB_WIDTH, TAB_WIDTH);

    setListSetting(CSS__COMMENT_TERMS, CSS_COMMENT_TERMS);
    setBoolSettings(DEFAULT_BOOL_SETTINGS);
}

void
Config::
readConf()
{
    string config_file_content;

    readConfigFile(m_config_file, config_file_content);
    parseConfigFileContent(config_file_content);
}

void
Config::
readConfigFile(const string &path, string &content)
{
    if (FileSystem::exists(path) && !FileSystem::readFile(path, content)) {
        cerr << "Could not read config file." << endl;
    }
}

void
Config::
parseConfigFileContent(const string &content)
{
    String line, header, param, value;
    uint64_t line_number = 0;
    String::iterator itr;
    Setting setting;
    istringstream content_stream(content);

    while (getline(content_stream, line)) {
        ++line_number;

        if (line.empty()) continue;

        param.clear(); value.clear();
        itr = line.begin();

        line.skipSpace(itr);

        if (*itr == '#') continue;
        if (*itr == '[')
        {
            const auto begin = itr+1,
            end = find(begin, line.end(), ']');

            if (end != line.end()) {
                header = String(begin, end);
                header.trim();

                if (header.empty()) {
                    throwConfigFileError("Empty header not allowed.", line_number, static_cast<uint64_t>(distance(line.begin(), itr)));
                }
            }
            else {
                itr = begin;

                throwConfigFileError("Unclosed header identificator.", line_number, static_cast<uint64_t>(distance(line.begin(), itr)));
            }

            continue;
        }

        if (!line.isCharOfRange(itr, 'a', 'z')) {
            throwConfigFileError("Not allowed character '" + string(1, *itr) + "'.", line_number, static_cast<uint64_t>(distance(line.begin(), itr)));
        }

        param = line.readCharSequence(itr, SPACE_CHARS "=#");

        if ((itr+1) != line.end()) {
            line.skipSpace(itr);

            if (itr != line.end()) {
                if (*itr == '=') {
                    if ((itr+1) != line.end()) {
                        ++itr;
                        line.skipSpace(itr);

                        value = line.readCharSequence(itr, "#");
                        value.trim();
                    }
                }
                else {
                    throwConfigFileError("Expected '=', found '" + string(1, *itr) + "'.", line_number, uint64_t(distance(line.begin(), itr)));
                }
            }
            else {
                throwConfigFileError("Expected '='.", line_number, uint64_t(distance(line.begin(), itr)));
            }

        }

        setting = UNKNOWN;

        if (isStringSetting(header + "_" + param, setting)) {
            if (getStringSettingValue(setting).empty())
                setStringSetting(setting, value);
        }
        else if (isBoolSetting(header + "_" + param, setting)) {
            if (value != CONFIG_BOOL_VALUE_TRUE && value != CONFIG_BOOL_VALUE_FALSE) {
                throwConfigFileError("Parameter '" + param + "' in the [" + header + "] section" NEWLINE +
                                     "must be set either to '" CONFIG_BOOL_VALUE_TRUE "' or '" CONFIG_BOOL_VALUE_FALSE "'." NEWLINE +
                                    (value.empty() ? "No value" : "'" + value + "'") + " found.", line_number,
                                     uint64_t(distance(line.begin(), itr-=distance(value.begin(), value.end()))));
            }

            param = String(header + "_" + param);

            if (value == CONFIG_BOOL_VALUE_TRUE)
                enable(setting);
            else if (value == CONFIG_BOOL_VALUE_FALSE)
                disable(setting);
        }
        else if (isListSetting(header + "_" + param, setting))
            setListSetting(setting, value.split(" "));
        else if (isNumericSetting(header + "_" + param, setting)) {
            if (!all_of(value.begin(), value.end(), ::isdigit))
                throwConfigFileError("Invalid value '" + value + "'", line_number, 0);

            setNumericSetting(setting, uint8_t(stoi(value)));
        }

        if (setting == UNKNOWN)
            throwConfigFileError("Unknown setting '" + param + "'.", line_number, 0);
    }

    validatePaths();

    setIsRead();
}

bool
Config::
isBoolSetting(const string &str_setting, Setting &setting) const
{
    for (const auto &pair : m_config_terms.bool_settings)
        if (pair.first == str_setting) {
            setting = pair.second;
            return true;
        }

    return false;
}

bool
Config::
isNumericSetting(const string &str_setting, Setting &setting) const
{
    for (const auto &pair : m_config_terms.numeric_settings)
        if (pair.first == str_setting) {
            setting = pair.second;
            return true;
        }

    return false;
}

const DataContainer<string>
Config::
getListSettingValues(const Setting setting) const
{
    return m_list_settings.find(setting)->second;
}

string &
Config::
getStringSettingValue(const Setting setting)
{
    return m_string_settings.find(setting)->second;
}

uint8_t
Config::
getNumericSettingValue(const Setting setting) const
{
    return m_numeric_settings.find(setting)->second;
}

bool
Config::
isStringSetting(const string& str_setting, Setting &setting) const
{
    for (const auto &pair : m_config_terms.string_settings)
        if (pair.first == str_setting) {
            setting = pair.second;
            return true;
        }

    return false;
}

bool
Config::
isListSetting(const string &str_setting, Setting &setting) const
{
    for (const auto &pair : m_config_terms.list_settings)
        if (pair.first == str_setting) {
            setting = pair.second;
            return true;
        }

    return false;
}

void
Config::
validatePaths()
{
    const auto validate = [](string &working_directory) {
        !FileSystem::isAbsolutePath(working_directory) &&
            RETURN("Working directories have to be absolute paths. See configuration file.");

        // Remove directory separators at the end
        while (&working_directory.front() != &working_directory.back() &&
               working_directory.back() == DIR_SEP[0])
            working_directory.pop_back();
    };

    auto &input_working_directory = getStringSettingValue(GENERAL__INPUT_WORKING_DIRECTORY);

    if (!input_working_directory.empty()) {
        // Validate input working directory path
        validate(input_working_directory);
    }

    auto &output_working_directory = getStringSettingValue(GENERAL__OUTPUT_WORKING_DIRECTORY);

    if (!output_working_directory.empty()) {
        // Validate output working directory path
        validate(output_working_directory);
    }
}

void
Config::
printConfigInfo()
{
    const auto settingValue = [&](const Config::Setting setting) -> const string {
        return isEnabled(setting) ? CONFIG_BOOL_VALUE_TRUE : CONFIG_BOOL_VALUE_FALSE;
    };

    cout << "Current configuration: " << DBLNEWLINE;

    for (const auto &header : m_config_terms.headers) {
        cout << "[" << header << "]" << NEWLINE;

        for (const auto &setting : m_config_terms.list_settings)
            if (String::startsWith(setting.first, {header + "_"})) {
                cout << setting.first.substr(header.length()+1) << " = ";

                const auto &list = m_list_settings.find(setting.second)->second;

                if (!list.empty())
                    for (const auto &item : list)
                        cout << item << (&item != &list.back() ? " " : NEWLINE);
                else cout << "[[empty]]" NEWLINE;
            }

        for (const auto &setting : m_config_terms.string_settings)
            if (String::startsWith(setting.first, {header + "_"})) {
                const auto &str = m_string_settings.find(setting.second)->second;
                cout << setting.first.substr(header.length()+1) << " = " << (str.empty() ? "[[empty]]" : str) << NEWLINE;
            }

        for (const auto &setting : m_config_terms.numeric_settings)
            if (String::startsWith(setting.first, {header + "_"})) {
                const auto &num = m_numeric_settings.find(setting.second)->second;
                cout << setting.first.substr(header.length()+1) << " = " << (num == 0 ? "0" : to_string(num)) << NEWLINE;
            }

        for (const auto &setting : m_config_terms.bool_settings)
            if (String::startsWith(setting.first, {header + "_"}))
                cout << setting.first.substr(header.length()+1) << " = " << settingValue(setting.second) << NEWLINE;

        if (&header != &m_config_terms.headers.back())
            cout << NEWLINE;
    }

    cout << endl;
}
