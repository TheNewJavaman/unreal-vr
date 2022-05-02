#pragma once

#include "AService.h"

namespace UnrealVr {
    /**
     * Defines required functionality for virtual reality runtimes
     */
    class AXrService : public AService, public AInitable, public AStoppable {};
}
