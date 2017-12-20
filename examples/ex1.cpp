#include "EasyFlags.hpp"

using namespace std;

AddArgument(int, logInfo).Group("").DefaultValue(0).ImplicitValue(1);
AddArgument(bool, exitOnDie).Group("testGroup");
AddArgument(string, randomString).Group("jsonTest");

int main(int argc, char** argv) {
    easyflags::ParseEasyFlags(argc, argv);
    cerr << "log_info\t" << logInfo << '\n';
    cerr << "die\t" << exitOnDie << '\n';
    cerr << "json_value\t" << randomString << '\n';
}
