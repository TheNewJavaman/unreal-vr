cmake_minimum_required(VERSION 3.15)
project(UnrealVR CXX)
include(FetchContent)

FetchContent_Declare(minhook
        GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
        GIT_TAG 4a455528f61b5a375b1f9d44e7d296d47f18bb18)
FetchContent_MakeAvailable(minhook)
add_subdirectory(PatternStreams)
add_subdirectory(UnrealReflector)
add_subdirectory(UnrealCameraInjector)
add_subdirectory(UnrealVRCore)
