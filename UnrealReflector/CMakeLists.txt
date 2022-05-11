add_library(UnrealReflector
        src/UnrealReflection.cpp)
target_include_directories(UnrealReflector PUBLIC include)
target_link_libraries(UnrealReflector PUBLIC PatternStreams)
