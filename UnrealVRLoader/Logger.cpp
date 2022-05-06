#include "Logger.h"

namespace UnrealVr {
    Logger::Logger(std::string source) {
        if (source.length() > 24) {
            this->source = source.substr(0, 21) + "...";
        } else {
            this->source = std::format("{: >24}", source);
        }
    }
}
