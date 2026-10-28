# make doc_doxygen optional if someone does not have / like doxygen
option(BUILD_DOC_DOXYGEN "Build Doxygen documentation" ON)


if(BUILD_DOC_DOXYGEN)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        # adds the optional target doc_doxygen to the builder
        add_custom_target(doc_doxygen
                COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating API documentation with Doxygen"
                VERBATIM
        )
    else()
        message(WARNING "Doxygen is required to build the documentation, but it was not found.")
    endif()
endif()