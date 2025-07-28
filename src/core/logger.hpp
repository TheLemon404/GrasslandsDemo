#pragma once
#include <string>

struct Logger {
    void Log(std::string message);
    void Log(float message);
    void ThrowRuntimeError(std::string message);
};
