``` c++
AddArgument(string, logFile).Group("Logging").Name("log_file").Short("l").ArgumentType("FILE").DefaultValue("", "No logs").ImplicitValue("").Description(
        "Redirect logs to this file");

AddArgument(int, logLevel).Group("Logging").Name("v,verbose_level").DefaultValue(0).ImplicitValue(-1, "all logging").ArgumentType("LEVEL").Description(
        "Print any logs with level <= verbose_level")

AddArgument(bool, logInfo).Group("Logging").Name("info"),DefaultValue(0).ImplicitValue(1).Description("Log info as well?");

AddArgument(bool, dieOnErrors).Group("Logging").Name("no_die_on_error").DefaultValue(1).ImplicitValue(0).Description("Continue program if an error rises");
```
## How to use the binary
```
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
```
