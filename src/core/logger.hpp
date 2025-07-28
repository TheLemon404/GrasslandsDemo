#pragma once
#include <string>

class Logger {
public:
    void Log(std::string message);
    void Log(float message);
    void ThrowRuntimeError(std::string message);
};
