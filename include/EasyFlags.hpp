#ifndef EASYFLAGS_EASYFLAGS_HPP
#define EASYFLAGS_EASYFLAGS_HPP

#include <functional>
#include <string>
#include <vector>
#include "cxxopts.hpp"
#include "JSON.hpp"

namespace easyflags {

struct CommandLineArgumentFinish {
};

struct CommandLineArgument {
    std::string shortName, longName;
    std::string group;
    std::string description;

    std::string argumentType;
    autojson::JSON defaultValue, implicitValue;

    std::function<autojson::JSON()> stringifyFunction;
    std::function<void(const autojson::JSON&)> deserializeFunction;
    std::function<void(CommandLineArgument&, cxxopts::Options&)> optionAdderGen;

    void* object;

    CommandLineArgument() { }

    CommandLineArgument(std::function<autojson::JSON()> stringifyFunction, std::function<void(const autojson::JSON&)> deserializeFunction,
                        std::function<void(CommandLineArgument&, cxxopts::Options&)> optionAdderGen, void* object, std::string longName);

    CommandLineArgument& Name(const std::string& longName);

    CommandLineArgument& Short(const std::string& shortName);

    CommandLineArgument& Description(const std::string& description);

    CommandLineArgument& Group(const std::string& group);

    CommandLineArgument& ArgumentType(const std::string& argumentType);

    CommandLineArgument& DefaultValue(const autojson::JSON& defaultValue);

    CommandLineArgument& ImplicitValue(const autojson::JSON& implicitValue);

    operator CommandLineArgumentFinish();
};


void ParseEasyFlags(int argc, char** argv);

} // namespace easyflags

#define MACROPASTER(x, y) x##_##y
#define MACROEVALUATOR(x, y) MACROPASTER(x, y)

#define AddArgument(TYPE, name)                                                                                                                            \
    TYPE name;                                                                                                                                             \
    static easyflags::CommandLineArgumentFinish MACROEVALUATOR(_num, __COUNTER__) =                                                                        \
        easyflags::CommandLineArgument([]() -> autojson::JSON { return autojson::JSON(name); }, [](const autojson::JSON& j) { name = j.operator TYPE(); }, \
                                       [](easyflags::CommandLineArgument& cmdarg, cxxopts::Options& options) {                                             \
                                           auto value = cxxopts::value<TYPE>(name);                                                                        \
                                           if (cmdarg.defaultValue.type != autojson::JSONType::INVALID) {                                                  \
                                                auto strValue = cmdarg.defaultValue.stringify(true);                                                       \
                                                if (cmdarg.defaultValue.type == autojson::JSONType::STRING) {                                              \
                                                    strValue = strValue.substr(1, strValue.size() - 2);                                                    \
                                                }                                                                                                          \
                                                value->default_value(strValue);                                                                            \
                                           }                                                                                                               \
                                           if (cmdarg.implicitValue.type != autojson::JSONType::INVALID) {                                                 \
                                                auto strValue = cmdarg.implicitValue.stringify(true);                                                      \
                                                if (cmdarg.implicitValue.type == autojson::JSONType::STRING) {                                             \
                                                    strValue = strValue.substr(1, strValue.size() - 2);                                                    \
                                                }                                                                                                          \
                                                value->implicit_value(strValue);                                                                           \
                                           }                                                                                                               \
                                           std::string name = cmdarg.shortName;                                                                            \
                                           if (cmdarg.longName != "") {                                                                                    \
                                               if (name != "") {                                                                                           \
                                                   name += ",";                                                                                            \
                                               }                                                                                                           \
                                               name += cmdarg.longName;                                                                                    \
                                           }                                                                                                               \
                                           options.add_options(cmdarg.group)(name, cmdarg.description, value, cmdarg.argumentType);                        \
                                       },                                                                                                                  \
                                       &name, #name)

#ifndef easyflagsuselib
#include "easyflags_src/EasyFlags.cpp"
#endif

#endif // EASYFLAGS_EASYFLAGS_HPP
