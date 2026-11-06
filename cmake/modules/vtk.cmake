option(ENABLE_VTK_OUTPUT "Enable VTK output" ON)

if (ENABLE_VTK_OUTPUT)
    message(STATUS "VTK output enabled")

    # 1. Force the VTK cache configuration to enable the OpenMP backend
    set(VTK_SMP_ENABLE_OPENMP ON CACHE BOOL "Enable OpenMP backend for VTK" FORCE)

    # 2. (Optional) Force OpenMP to be the default backend at runtime
    set(VTK_SMP_IMPLEMENTATION_TYPE "OpenMP" CACHE STRING "Default VTK SMP implementation" FORCE)

    # VTK Library, only add required components for less unnecessary includes
    find_package(VTK REQUIRED COMPONENTS
            CommonCore
            CommonDataModel
            IOXML
    )

    if (VTK_FOUND)
        message(STATUS "Found VTK Version: ${VTK_VERSION}")
    else ()
        message(FATAL_ERROR "VTK not found")
    endif ()

    if (VTK_VERSION VERSION_GREATER_EQUAL 8.9)
        include_directories(${VTK_INCLUDE_DIRS})
    else ()
        include(${VTK_USE_FILE})
    endif ()

    target_link_libraries(MolSimCore
            PUBLIC
            ${VTK_LIBRARIES}
    )

    target_compile_definitions(MolSimCore
            PUBLIC
            ENABLE_VTK_OUTPUT
    )
endif ()
