#include "EasyFlags.hpp"

#include <algorithm>
#include <functional>
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "cxxopts.hpp"

namespace easyflags {

std::vector<CommandLineArgument> allArguments;

AddArgument(std::string, generateJSONConfig)
    .Description("Generate a JSON file that will contain all the current arguments. Write that JSON to FILE")
    .Group("Usage")
    .ImplicitValue("newConfig.json")
    .ArgumentType("FILE");

CommandLineArgument::CommandLineArgument(                                                                            //
    std::function<autojson::JSON()> stringifyFunction,                                                               //
    std::function<void(const autojson::JSON&)> deserializeFunction,                                                  //
    std::function<void(CommandLineArgument&, cxxopts::Options&)> optionAdderGen, void* object, std::string longName) //
    :                                                                                                                //
      longName(longName),
      stringifyFunction(stringifyFunction),
      deserializeFunction(deserializeFunction),
      optionAdderGen(optionAdderGen),
      object(object) {
}

CommandLineArgument& CommandLineArgument::Name(const std::string& longName) {
    this->longName = longName;
    return *this;
}

CommandLineArgument& CommandLineArgument::Short(const std::string& shortName) {
    this->shortName = shortName;
    return *this;
}

CommandLineArgument& CommandLineArgument::Description(const std::string& description) {
    this->description = description;
    return *this;
}

CommandLineArgument& CommandLineArgument::Group(const std::string& group) {
    this->group = group;
    return *this;
}

CommandLineArgument& CommandLineArgument::ArgumentType(const std::string& argumentType) {
    this->argumentType = argumentType;
    return *this;
}

CommandLineArgument& CommandLineArgument::DefaultValue(const std::string& defaultValue) {
    this->defaultValue = defaultValue;
    return *this;
}

CommandLineArgument& CommandLineArgument::ImplicitValue(const std::string& implicitValue) {
    this->implicitValue = implicitValue;
    return *this;
}

CommandLineArgument::operator CommandLineArgumentFinish() {
    allArguments.push_back(*this);
    return CommandLineArgumentFinish();
}

int LevenshteinDistance(const std::string& a, const std::string& b) {
    std::vector<std::vector<int>> d(a.size() + 1, std::vector<int>(b.size() + 1, 0));
    for (int i = 1; i <= (int)a.size(); i += 1) {
        d[i][0] = i;
    }

    for (int i = 1; i <= (int)b.size(); i += 1) {
        d[0][i] = i;
    }

    for (int i = 1; i <= (int)a.size(); i += 1) {
        for (int j = 1; j <= (int)b.size(); j += 1) {
            d[i][j] = std::min({d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (a[i - 1] == b[j - 1] ? 0 : 1)});
        }
    }

    return d[a.size()][b.size()];
}

std::string ClosestOption(const std::string& wrong_option, const std::vector<std::string>& all_options) {
    int mn_distance = 1e9;
    std::string result = "";

    for (const std::string& option : all_options) {
        std::vector<std::string> small_words;
        std::string current_word = "";
        for (char itr : option) {
            if (itr == '-') {
                small_words.push_back(current_word);
                current_word = "";
            } else {
                current_word += itr;
            }
        }

        small_words.push_back(current_word);
        std::vector<int> perm;
        for (int i = 0; i < (int)small_words.size(); i += 1) {
            perm.push_back(i);
        }

        do {
            std::string new_option = "";
            for (auto itr : perm) {
                new_option += small_words[itr] + "-";
            }
            new_option.pop_back();
            auto current_distance = LevenshteinDistance(wrong_option, new_option);
            if (current_distance < mn_distance) {
                mn_distance = current_distance;
                result = option;
            }
        } while (next_permutation(perm.begin(), perm.end()));
    }

    return result;
}

void ApplyJSONFile(const std::string& filePath, std::map<std::string, CommandLineArgument*> argumentHash) {
    auto JSON = autojson::JSON::readFromFile(filePath);
    for (std::pair<std::string, autojson::JSON> itr : JSON.operator std::map<std::string, autojson::JSON>()) {
        auto& key = itr.first;
        auto& value = itr.second;
        if (argumentHash.count(key)) {
            argumentHash[key]->deserializeFunction(value);
        } else {
            std::cerr << "[WARNING] "
                      << "Unknown command line option '" << key << "' from JSON " << filePath << '\n';
        }
    }
}

void InitCommandLineArguments(int argc, char** argv) {
    cxxopts::Options options("");

    std::vector<std::string> JSONImports;
    options.add_options("Usage")("i,importConfig", "Import arguments from json file. Imports are first and are applied in order.",
                                 cxxopts::value<std::vector<std::string>>(JSONImports)->implicit_value("config.json"), "FILE");

    std::vector<std::string> allOptions = {"help"};
    std::vector<std::string> allGroups = {"Usage", ""};
    std::map<std::string, CommandLineArgument*> argumentHash;
    for (auto& cmdarg : allArguments) {
        argumentHash[cmdarg.longName] = &cmdarg;
        allOptions.push_back(cmdarg.longName);
        allGroups.push_back(cmdarg.group);
        cmdarg.optionAdderGen(cmdarg, options);
    }

    std::sort(allGroups.begin(), allGroups.end());
    allGroups.resize(unique(allGroups.begin(), allGroups.end()) - allGroups.begin());

    std::vector<std::string> positional;
    options.add_options()("positional", "", cxxopts::value<std::vector<std::string>>(positional));

    options.add_options()("h,help", "");
    options.parse_positional("positional");

    int argc_copy = argc;
    char** argv_copy = (char**)malloc(argc * sizeof(char*));
    for (int i = 0; i < argc_copy; i += 1) {
        argv_copy[i] = (char*)malloc(strlen(argv[i]) * sizeof(char));
        strcpy(argv_copy[i], argv[i]);
    }

    try {
        auto result = options.parse(argc_copy, argv_copy);

        if (result.count("help")) {
            std::cout << options.help(allGroups) << std::endl;
            exit(0);
        }

        if (JSONImports.size()) {
            for (auto itr : JSONImports) {
                ApplyJSONFile(itr, argumentHash);
            }
            options.parse(argc, argv);
        }

        if (generateJSONConfig != "") {
            // generate JSON config and write it to file
            auto path = generateJSONConfig;
            argumentHash.erase("generateJSONConfig");
            autojson::JSON j;
            for (auto itr : argumentHash) {
                j[itr.first] = autojson::JSON(itr.second->stringifyFunction());
            }

            std::ofstream fout(path.c_str());
            if (!(fout << j.stringify(true) << '\n')) {
                std::cerr << "[ERROR] can't write JSON to file " << path << '\n';
                exit(1);
            } else {
                fout.close();
                exit(0);
            }
        }
    } catch (const cxxopts::option_not_exists_exception& e) {
        auto GetOption = [](const std::string& exception_message) {
            std::string result = "";
            bool quoted = false;
            for (char32_t c : exception_message) {
                if (c == 4294967266) {
                    quoted = false;
                }

                if (quoted) {
                    result += c;
                }

                if (c == 4294967192) {
                    quoted = true;
                }
            }

            return result;
        };

        std::cout << e.what() << u8". Did you mean ‘" << ClosestOption(GetOption(e.what()), allOptions) << u8"’?\n";
        exit(1);
    } catch (const cxxopts::OptionException& e) {
        std::cout << "error parsing options: " << e.what() << "\n";
        exit(1);
    }
}

} // namespace easyflags
