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
    std::string defaultValue, implicitValue;

    std::function<autojson::JSON()> stringifyFunction;
    std::function<void(const autojson::JSON&)> deserializeFunction;
    std::function<void(CommandLineArgument&, cxxopts::Options&)> optionAdderGen;

    void* object;

    CommandLineArgument(std::function<autojson::JSON()> stringifyFunction, std::function<void(const autojson::JSON&)> deserializeFunction,
                        std::function<void(CommandLineArgument&, cxxopts::Options&)> optionAdderGen, void* object, std::string longName);

    CommandLineArgument& Name(const std::string& longName);

    CommandLineArgument& Short(const std::string& shortName);

    CommandLineArgument& Description(const std::string& description);

    CommandLineArgument& Group(const std::string& group);

    CommandLineArgument& ArgumentType(const std::string& argumentType);

    CommandLineArgument& DefaultValue(const std::string& defaultValue);

    CommandLineArgument& ImplicitValue(const std::string& implicitValue);

    operator CommandLineArgumentFinish();
};

extern std::vector<CommandLineArgument> allArguments;

void ParseEasyFlags(int argc, char** argv);

} // namespace easyflags

#define MACROPASTER(x, y) x##_##y
#define MACROEVALUATOR(x, y) MACROPASTER(x, y)

#define AddArgument(type, name)                                                                                                                            \
    type name;                                                                                                                                             \
    easyflags::CommandLineArgumentFinish MACROEVALUATOR(_num, __COUNTER__) =                                                                               \
        easyflags::CommandLineArgument([]() -> autojson::JSON { return autojson::JSON(name); }, [](const autojson::JSON& j) { name = j.operator type(); }, \
                                       [](easyflags::CommandLineArgument& cmdarg, cxxopts::Options& options) {                                             \
                                           auto value = cxxopts::value<type>(name);                                                                        \
                                           if (cmdarg.defaultValue != "") {                                                                                \
                                               value->default_value(cmdarg.defaultValue);                                                                  \
                                           }                                                                                                               \
                                           if (cmdarg.implicitValue != "") {                                                                               \
                                               value->implicit_value(cmdarg.implicitValue);                                                                \
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
