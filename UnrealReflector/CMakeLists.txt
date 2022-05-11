set(CMAKE_CXX_STANDARD 20)
add_library(UnrealReflector
        src/ACacheable.cpp
        src/UnrealCore.cpp
        src/UnrealMath.cpp)
target_include_directories(UnrealReflector PUBLIC include)
target_link_libraries(UnrealReflector PUBLIC PatternStreams)
