# OpenMP for the parallel force calculation otherwise the program runs serially.
option(ENABLE_OPENMP "Use OpenMP to parallelize the force calculation" ON)

if (ENABLE_OPENMP)
    message(STATUS "OpenMP enabled")

    find_package(OpenMP COMPONENTS CXX)

    if (OpenMP_CXX_FOUND)
        message(STATUS "Found OpenMP Version: ${OpenMP_CXX_VERSION}")
    else ()
        message(FATAL_ERROR "OpenMP not found")
    endif ()

    target_link_libraries(MolSimCore
            PUBLIC
            OpenMP::OpenMP_CXX
    )

    target_compile_definitions(MolSimCore
            PUBLIC
            ENABLE_OPENMP
    )
endif ()
