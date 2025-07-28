#include "logger.hpp"

#include <iostream>
#include <ostream>

void Logger::Log(std::string message) {
    std::cout << message.c_str() << std::endl;
}

void Logger::Log(float message) {
    std::cout << std::to_string(message) << std::endl;
}

void Logger::ThrowRuntimeError(std::string message) {
    std::cout << "RUNTIME ERROR: ";
    throw std::runtime_error(message.c_str());
}

