#include "lwvl/lwvl.hpp"
#include <iostream>

void lwvl::WorldBlock::attach(const lwvl::ShaderProgram& program) {
    GLuint indices[2];
    const char* names[2] {
        "World.projection", "World.camera"
    };

    glGetUniformIndices(program.id(), 2, names, indices);
    auto [projectionIndex, cameraIndex] = indices;

    std::cout << glGetProgramResourceIndex(program.id(), GL_UNIFORM_BLOCK, "World") << std::endl << std::endl;
    //std::cout
    //<< glGetProgramResourceIndex(program.id(), GL_UNIFORM, "World.projection") << std::endl
    //<< glGetProgramResourceIndex(program.id(), GL_UNIFORM, "World.camera") << std::endl
    //<< std::endl;
}
