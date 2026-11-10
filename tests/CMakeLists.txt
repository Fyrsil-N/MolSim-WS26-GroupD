include(FetchContent) # needed to download googletest

# googletest dependency
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/063de7e9578f82b369302001269680b4b1553359.zip # use v1.18.0 (newest release)
)

# disable downloading things like mock subprojects that are large and not needed
set(INSTALL_GTEST OFF)
FetchContent_MakeAvailable(googletest)

# collect all cpp files
file(GLOB_RECURSE TESTS_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        # header don't need to be included but this might be necessary for some IDEs
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# add the actual tests
add_executable(MolSimTests ${TESTS_SRC})
target_link_libraries(MolSimTests
        PRIVATE
        MolSimCore
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(MolSimTests)
