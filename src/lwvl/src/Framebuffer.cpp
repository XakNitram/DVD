#include "lwvl/lwvl.hpp"

lwvl::Framebuffer::Framebuffer() {
    glGenFramebuffers(1, &m_id);
}

lwvl::Framebuffer::~Framebuffer() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDeleteFramebuffers(1, &m_id);
    m_id = 0;
}

void lwvl::Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void lwvl::Framebuffer::attach(lwvl::Attachment point, lwvl::Texture &texture, int level) {
    glFramebufferTexture(GL_FRAMEBUFFER, static_cast<GLenum>(point), texture.m_id, level);
}

void lwvl::Framebuffer::attach1D(lwvl::Attachment point, lwvl::Texture &texture, int level) {
    glFramebufferTexture1D(GL_FRAMEBUFFER, static_cast<GLenum>(point), texture.target(), texture.m_id, level);
}

void lwvl::Framebuffer::attach2D(lwvl::Attachment point, lwvl::Texture &texture, int level) {
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, static_cast<GLenum>(point),
        texture.target(), texture.m_id, level
    );
}

void lwvl::Framebuffer::attach3D(lwvl::Attachment point, lwvl::Texture &texture, int layer, int level) {
    glFramebufferTexture3D(GL_FRAMEBUFFER, static_cast<GLenum>(point), texture.target(), texture.m_id, level, layer);
}

void lwvl::Framebuffer::clear() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint lwvl::Framebuffer::id() const {
    return m_id;
}
