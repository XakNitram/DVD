#include "lwvl/lwvl.hpp"

void lwvl::clear() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void lwvl::viewport(lwvl::Viewport port) {
    glViewport(port.x, port.y, port.width, port.height);
}

lwvl::Viewport lwvl::viewport() {
    GLsizei prevViewport[4];
    glGetIntegerv(GL_VIEWPORT, prevViewport);
    const auto[prevX, prevY, prevWidth, prevHeight] = prevViewport;
    return {prevX, prevY, prevWidth, prevHeight};
}
