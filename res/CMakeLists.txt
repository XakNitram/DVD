# We define this target over the individual resource targets so they can all be built together,
# rather than shaders then GLFW, then fonts, then Glad
add_custom_target(resources)
add_dependencies(dvd resources)

# Maybe this should be a macro.
function(export_resources)
    cmake_parse_arguments(PARSE_ARGV 0 "EXPORT" "" "DIRECTORY" "DEPENDS")
    if(NOT EXPORT_DIRECTORY)
        message(FATAL_ERROR "No directory specified (with DIRECTORY keyword) in call to function 'export_resources'")
        return()
    elseif(NOT EXPORT_DEPENDS)
        message(FATAL_ERROR "No dependencies specified (with DEPENDS keyword) in call to function 'export_resources'")
        return()
    endif()

    string(TOLOWER "${EXPORT_DIRECTORY}" TARGET)

    set(OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/Data/${EXPORT_DIRECTORY}/")
    list(TRANSFORM EXPORT_DEPENDS PREPEND ${OUTPUT_DIRECTORY} OUTPUT_VARIABLE RESOURCE_OUT)
    list(TRANSFORM EXPORT_DEPENDS PREPEND "${CMAKE_CURRENT_LIST_DIR}/${EXPORT_DIRECTORY}/" OUTPUT_VARIABLE RESOURCE_IN)

    add_custom_target(${TARGET} DEPENDS ${RESOURCE_OUT})
    add_dependencies(resources ${TARGET})

    add_custom_command(
        DEPENDS ${RESOURCE_IN}
        OUTPUT ${RESOURCE_OUT}

        COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E copy ${RESOURCE_IN} ${OUTPUT_DIRECTORY}

        COMMENT "Copied ${TARGET} to output"
    )
endfunction()

export_resources(
    DIRECTORY Shaders
    DEPENDS
    quad.vert
    quad.frag
    spacetime.frag
)

export_resources(
    DIRECTORY Textures
    DEPENDS
    dvd-logo.png
)
