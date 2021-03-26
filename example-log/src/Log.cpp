#include "Log.h"
#include <stdio.h>
#include <string>
#include "nuttx/config.h"
#include "json.hpp"

using json = nlohmann::json;

void Log::print(std::string message) {

    FILE *fp = fopen("/dev/ttyS0", "w");
    if (fp == NULL) {
        printf("Error opening serial port!\n");
        return;
    }

    json jMsg;
    jMsg["teste"] = message;

    std::string msg = jMsg.dump() + "\n";

    /* Try to force input data on stdin */
    fwrite(msg.c_str(), sizeof(char), msg.length(), fp);

    fclose(fp);
}
