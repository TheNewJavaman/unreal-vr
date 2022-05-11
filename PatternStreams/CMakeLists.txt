set(CMAKE_CXX_STANDARD 20)
add_library(PatternStreams
        src/PatternStreams.cpp
        src/PatternStreamsPrivate.h src/PatternStreamsPrivate.cpp)
target_include_directories(PatternStreams PUBLIC include)
