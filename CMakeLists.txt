cmake_minimum_required(VERSION 3.24)

# Enable C++20. You may raise the standard if you want
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# define project name, version
project(MolSim_GroupD VERSION 0.2.0 LANGUAGES CXX)

# let ccmake and cmake-gui offer the default build type options
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel")

# set Release as the default build type if it is not yet set.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)

# collect all cpp files except the MolSim.cpp main
file(GLOB_RECURSE LIB_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        # header don't need to be included but this might be necessary for some IDEs
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
)
list(FILTER LIB_SRC EXCLUDE REGEX MolSim\.cpp)

# activate all compiler warnings. Clean up your code :P
# depending on the compiler different flags are used
set(COMPILE_FLAGS
        "$<$<CXX_COMPILER_ID:GNU>:-Wall;-Wextra;-Wpedantic>"
        "$<$<CXX_COMPILER_ID:Clang>:-Wall;-Wextra;-Wpedantic>"
        "$<$<CXX_COMPILER_ID:AppleClang>:-Wall;-Wextra;-Wpedantic>"
        # disable some spam warnings for icpc...
        $<$<CXX_COMPILER_ID:Intel>:-w3 -wd383,981,1418,1572,2259>
)

include(FetchContent) # needed to download spdlog

# spdlog dependency
FetchContent_Declare(
        spdlog
        URL https://github.com/gabime/spdlog/archive/79524ddd08a4ec981b7fea76afd08ee05f83755d.zip # use v1.17.0 (newest release)
)
FetchContent_MakeAvailable(spdlog)

# build the core libary
add_library(MolSimCore ${LIB_SRC})
target_include_directories(MolSimCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(MolSimCore PUBLIC spdlog::spdlog)
target_compile_options(MolSimCore PRIVATE ${COMPILE_FLAGS})


# build the actual executable
add_executable(MolSim ${CMAKE_CURRENT_SOURCE_DIR}/src/MolSim.cpp)
target_link_libraries(MolSim PRIVATE MolSimCore)
target_compile_options(MolSim PRIVATE ${COMPILE_FLAGS})

# make downloading and using googletest optional as it slows down compilation
option(USE_GOOGLETEST "Download googletest and build the tests" ON)
# build the google tests
if (USE_GOOGLETEST)
    enable_testing()
    add_subdirectory(tests)
endif ()

# add additional modules
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules)

include(vtk)
include(clang-format)
include(doxygen)