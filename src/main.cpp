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

#include "main.h"

Config cfg;

using namespace std::chrono;

static DataContainer<pair<const string, const string> > arg_pair_list;

int main(int argc, char **argv)
{
    ios_base::sync_with_stdio(false);

    DataContainer<string> args;

    for (int32_t i = 1; i < argc; ++i) args.emplace_back(argv[i]);

    if (parseArguments(args, arg_pair_list)) {
        for (const auto &pair : arg_pair_list)
            !isSupportedArgument(pair.first) &&
                RETURN(APP_HEADER "Unsupported argument: " + pair.first + DBLNEWLINE + INFO_ON_NO_ARGUMENTS);

        if (!isSet("--stdo")) {
            Console::write(APP_HEADER);

            if (isSet("--help"))
                Console::write(APP_LICENSE_HEADER);
        }

        if (isSet("--config-file")) {
            string config_file_path = attrVal("--config-file");

            if (config_file_path.empty() || !FileSystem::isAbsolutePath(config_file_path))
                RETURN("Expected absolute file path after argument '--config-file'");

            if (!FileSystem::exists(config_file_path))
                RETURN("Passed config file does not exist.");

            if (!FileSystem::isReadable(config_file_path))
                RETURN("Passed config file is not readable. Check permissions.");

            cfg.setConfigFilePath(config_file_path);
        }

        if (argc == 1) {
            if (!cfg.isRead()) cfg.readConf();

            if (cfg.inputWorkingDirectory().empty() && cfg.inputPath().empty() &&
                cfg.outputWorkingDirectory().empty() && cfg.outputPath().empty()) {
                Console::write(APP_LICENSE_HEADER INFO_ON_NO_ARGUMENTS DBLNEWLINE);
                return 0;
            }
        }

        if (FileSystem::exists(cfg.configFilePath()) && !isSet("--create-config-file"))
            if (!cfg.isRead()) cfg.readConf();

        prepare(arg_pair_list);

        if (!cfg.isEnabled(Config::GENERAL__OUTPUT_TO_STDO)) {
            Console::writeLine("Input  file: " + cfg.inputPath());
            Console::writeLine("Output path: " + cfg.outputPath() + NEWLINE);
        }

        high_resolution_clock::time_point t1, t2;

        if (!cfg.isEnabled(Config::GENERAL__OUTPUT_TO_STDO))
            t1 = high_resolution_clock::now();

        FileSystemWorker::process();

        if (!cfg.isEnabled(Config::GENERAL__OUTPUT_TO_STDO)) {
            t2 = high_resolution_clock::now();

            auto duration = duration_cast<microseconds>(t2 - t1).count();
            string time_unit = TIME_UNIT;

            if (duration > 1e3 && duration < 1e6) {
                duration /= int64_t(1e3);
                time_unit = "ms";
            }
            else if (duration > 1e6 && duration < 60e6) {
                duration /= int64_t(1e6);
                time_unit = "s";
            }

            Console::writeLine("Operation took: " + to_string(duration) + time_unit + NEWLINE);
        }
    }

    return 0;
}

/// Parse command line arguments
bool parseArguments(DataContainer<string> &args, DataContainer<pair<const string, const string> > &arg_map)
{
    if (!args.empty()) {
        for (auto itr = args.begin(); itr != args.end(); ++itr) {
            if ((itr->front() == '-' && itr->length() == 2) ||
                (itr->length() > 2 && itr->substr(0, 2) == "--")) {
                if (itr+1 != args.end()) {
                    if ((itr+1)->front() == '-') {
                        arg_map.emplace_back(*itr, "");
                    } else {
                        arg_map.emplace_back(*itr, *(itr+1));
                        ++itr;
                    }
                } else {
                    arg_map.emplace_back(*itr, "");
                }
            } else {
                !(itr->front() == '-' && itr->length() > 2) &&
                    RETURN("Unknown argument: " + *itr);

                arg_map.emplace_back(string(itr->begin(), itr->begin()+2),
                                     itr->substr(2, itr->length() - 2));
            }
        }
    }

    return true;
}

/// Check, if passed command line argument is supported
bool isSupportedArgument(const string &arg)
{
    const initializer_list<const string> supported_arg_list
            = {"-i", "-o", "--help", "--create-config-file", "--config-info", "--config-file", "--stdo"};

    return find(supported_arg_list.begin(),
           supported_arg_list.end(),
           arg) != supported_arg_list.end();
}

/// Check, if a specific command line argument is in use
inline bool isSet(const string &arg) {
    for (const auto &pair : arg_pair_list)
        if (pair.first == arg)
            return true;

    return false;
}

/// Get the value passed with a specific command line argument
inline string attrVal(const string &arg) {
    for (const auto &pair : arg_pair_list)
        if (pair.first == arg)
            return pair.second;

    return string();
}

/// Check configuration, before start of processing
void prepare(DataContainer<pair<const string, const string> > &args)
{
    if (isSet("--stdo")) {
        isSet("-o") &&
            RETURN("Use either '-o' or '--stdo'. This arguments cannot be combined.");

        cfg.enable(Config::GENERAL__OUTPUT_TO_STDO);
    }

    if (!cfg.isEnabled(Config::GENERAL__OUTPUT_TO_STDO) &&
        cfg.isEnabled(Config::GENERAL__CREATE_PHP_INCLUDE_FILE)) {
        const auto checkPhpVariableName = [&](const string &var) -> void {
            if (var.empty()) {
                string setting_name;

                if (&var == &cfg.phpIdArrayName())
                    setting_name = "php_id_array_name";
                else if (&var == &cfg.phpClassArrayName())
                    setting_name = "php_class_array_name";
                else if (&var == &cfg.phpCustomPropertyArrayName())
                    setting_name = "php_cprop_array_name";
                else if (&var == &cfg.phpAnimationArrayName())
                    setting_name = "php_animation_array_name";

                RETURN("'create_php_include_file' is enabled, but '" + setting_name +
                       "' value is empty." DBLNEWLINE "Check configuration file.");
            }
        };

        checkPhpVariableName(cfg.phpIdArrayName());
        checkPhpVariableName(cfg.phpClassArrayName());
        checkPhpVariableName(cfg.phpCustomPropertyArrayName());
        checkPhpVariableName(cfg.phpAnimationArrayName());
    }

    if (isSet("--help")) {
        !(args.size() == 1 && attrVal("--help").empty()) &&
            RETURN("Use '--help' as the only argument.");

        Console::writeLine(HELP_PAGE_CONTENT);
        exit(0);
    }

    if (isSet("--create-config-file")) {
        !(args.size() == 1 && attrVal("--create-config-file").empty()) &&
            RETURN("Use '--create-config-file' as the only argument.");

        if (!FileSystem::exists(CONFIG_FILE_PATH)) {
            writeConfigFile();
            Console::writeLine("Configuration file '" CONFIG_FILE_PATH "' has been created." NEWLINE);
        } else {
            String input;

            while (true) {
                Console::write("Configuration file already exists. Overwrite with defaults? (y/N): ");
                getline(cin, input);

                if (!input.empty() && input.toLower() != "y" && input.toLower() != "n") {
                    Console::writeLine("Invalid input: " + input);
                    continue;
                }

                break;
            }

            if (input.empty() || input.toLower() == "n") {
                Console::writeLine(NEWLINE "Kept existing configuration file." NEWLINE);
            }
            else if (input.toLower() == "y") {
                writeConfigFile();
                Console::writeLine(NEWLINE "Configuration file has been reset to defaults." NEWLINE);
            }
        }

        exit(0);
    }

    if (isSet("--config-info")) {
        !(arg_pair_list.size() == 1 || (arg_pair_list.size() == 2 && isSet("--config-file"))) &&
            RETURN("Use '--config-info' as the only argument or with '--config-file'.");

        cfg.printConfigInfo();
        exit(0);
    }

    if (isSet("-i") && !attrVal("-i").empty()) {
        string input_path = FileSystem::getCleanPath(attrVal("-i"));

        if (cfg.inputWorkingDirectory().empty()) {
            !FileSystem::isAbsolutePath(input_path) &&
                RETURN("Expected absolute input path, when input working directory is not set.");
        } else {
            FileSystem::isAbsolutePath(input_path) &&
                RETURN("Expected relative input path, when input working directory is set.");

            input_path = FileSystem::getCleanPath(cfg.inputWorkingDirectory() + DIR_SEP + input_path);
        }

        cfg.setInputPath(input_path);
    } else {
        string input_path;

        if (cfg.inputWorkingDirectory().empty()) {
            if (!cfg.inputPath().empty()) {
                !FileSystem::isAbsolutePath(cfg.inputPath()) &&
                    RETURN("Expected absolute input path, when input working directory is not set.");

                input_path = cfg.inputPath();
            }
        } else {
            if (!cfg.inputPath().empty()) {
                FileSystem::isAbsolutePath(cfg.inputPath()) &&
                    RETURN("Expected relative input path, when input working directory is set.");

                input_path = FileSystem::getCleanPath(cfg.inputWorkingDirectory() + DIR_SEP + cfg.inputPath());
            }
        }

        cfg.setInputPath(input_path);

        cfg.inputPath().empty() &&
            RETURN("The input path is not specified.");

        !FileSystem::exists(cfg.inputPath()) &&
            RETURN("The input file, specified in the configuration file, does not exist.");
    }

    if ((isSet("-o") && !attrVal("-o").empty())) {
        string output_path = FileSystem::getCleanPath(attrVal("-o"));

        if (cfg.outputWorkingDirectory().empty()) {
            !FileSystem::isAbsolutePath(output_path) &&
                RETURN("Expected absolute output path, when output working directory is not set.");
        } else {
            FileSystem::isAbsolutePath(output_path) &&
                RETURN("Expected relative output path, when output working directory is set.");

            output_path = FileSystem::getCleanPath(cfg.outputWorkingDirectory() + DIR_SEP + output_path);
        }

        cfg.setOutputPath(output_path);
    } else {
        if (!isSet("--stdo")) {
            string output_path;

            if (!cfg.outputWorkingDirectory().empty()) {
                !FileSystem::isAbsolutePath(cfg.outputWorkingDirectory()) &&
                    RETURN("Expected absolute output working directory path.");

                if (!cfg.outputPath().empty()) {
                    FileSystem::isAbsolutePath(cfg.outputPath()) &&
                        RETURN("Expected relative output path, when output working directory is set.");

                    output_path = FileSystem::getCleanPath(cfg.outputWorkingDirectory() + DIR_SEP + cfg.outputPath());
                } else {
                    output_path = FileSystem::getCleanPath(cfg.outputWorkingDirectory());
                }
            } else {
                !cfg.outputPath().empty() &&
                !FileSystem::isAbsolutePath(cfg.outputPath()) &&
                    RETURN("Expected absolute output path, when output working directory is not set.");

                output_path = cfg.outputPath();
            }

            output_path.empty() &&
                RETURN("The output path is not specified.");

            cfg.setOutputPath(output_path);
        } else {
            // Include imported stylesheets by default, when standard output is used.
            cfg.enable(Config::CSS__INCLUDE_EXTERNAL_STYLESHEETS);
        }
    }

    args.clear();
}

bool RETURN(const string &message)
{
    Console::writeLine(message + DBLNEWLINE "Abort." NEWLINE);
    exit(1);
}
