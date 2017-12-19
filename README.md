
RegisterFlag(string, logFile).Group("Logging").Name("log_file").Short("l").ArgumentType("FILE").DefaultValue("", "No logs").ImplicitValue("").Description(
        "Redirect logs to this file");

extern vector<int> acceptableLogLevels;
RegisterFlag(int, logLevel).Group("Logging").Name("v,verbose_level").DefaultValue(0).ImplicitValue(-1, "all logging").ArgumentType("LEVEL").Description(
        "Print any logs with level <= verbose_level").InitAfter("acceptableLogLevels").Init([]() {
    if (logLevel == -1) {
        allLogging = true;
        return;
    }

    for (int i = 1; i <= logLevel; i += 1) {
        acceptableLogLevels.push_back(i);
    }
});

RegisterFlag(bool, logInfo).Group("Logging").Name("info"),DefaultValue(0).ImplicitValue(1).Description("Log info as well?");

RegisterFlag(bool, dieOnErrors).Group("Logging").Name("no_die_on_error").DefaultValue(1).ImplicitValue(0).Description("Continue program if an error rises");

RegisterMultipleValueFlag(int, acceptableLogLevels).Name("log_only").DefaultValue({}).Split(":")
.ParseArgument([](string arg) {
    auto arguments = RunBashCommand("for i in {" + arg + "}; do echo $i; done").Split("\n");
    if (CanParseInt(arg, nullptr)) {
        arguments = {arg};
    }

    for (auto itr : arguments) {
        int v;
        if (CanParseInt(arg, v)) {
            acceptableLogLevels.push_back(v);
        } else {
            ArgError("Can't parse argument: " + itr);
        }
    }
});

/*
    --log_file          (logFile=cool_binary.log)
                        (logFile=logger.log)
    --log_file=l.log    (logFile=l.log)
    --log_file q.log    (logFile=q.log)
    -l a.log            (logFile=a.log)


    --verbose_level     (logLevel=-1) // all logging
    -v                  (logLevel=-1) // all logging
    --verbose_level=5   (logLevel=5)

    --info              (logInfo=1)
                        (logInfo=0)

    --no_die_on_error   (dieOnErrors=0)
                        (dieOnErrors=1)

    --log_only          // error (no implicit value)
    --log_only 1 --log_only 2   (acceptableLogLevels = {1, 2})
    --log_only=1:2:3:10..15     (acceptableLogLevels = {1, 2, 3, 10, 11, 12, 13, 14, 15})


    --verbose_level 3 --log_only=10..15     (acceptableLogLevels = {1, 2, 3, 10, 11, 12, 13, 14, 15})

    --help

Logging
    -l, --log_file  [=FILE(="logger.txt")]
            Redirect logs to this file (default: "logger.log")

    -v, --verbose_level [=LEVEL(=all logging)]
            Print any logs with level <= verbose_level (default: 0)

        --info
            Log info as well?

        --no_die_on_error
            Contienu program if an error rises

        --acceptable_log_levels
*/
