target_sources(
    dvd
    PRIVATE
        Application.cpp

        # CORE
        Core/Event.hpp
        Core/Event.cpp
        Core/Window.hpp
        Core/Window.cpp

        #STB_IMAGE
        stb/stb_image.h
)

target_include_directories(dvd PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_precompile_headers(dvd PRIVATE pch.hpp pch.cpp)

target_link_libraries(dvd PRIVATE glfw)
target_link_libraries(dvd PRIVATE glm)
target_link_libraries(dvd PRIVATE lwvl)
