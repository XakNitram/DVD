#include "lwvl/lwvl.hpp"

lwvl::VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_id);
}

lwvl::VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_id);
}

void lwvl::VertexArray::bind() {
    glBindVertexArray(m_id);
}

void lwvl::VertexArray::clear() {
    glBindVertexArray(0);
}

uint32_t lwvl::VertexArray::instances() const { return m_instances; }

void lwvl::VertexArray::instances(uint32_t count) { m_instances = count; }

void lwvl::VertexArray::attribute(uint8_t dimensions, GLenum type, int64_t stride, int64_t offset, uint32_t divisor) {
    glEnableVertexAttribArray(m_attributes);
    glVertexAttribPointer(m_attributes, dimensions, type, GL_FALSE, stride, reinterpret_cast<void *>(offset));
    glVertexAttribDivisor(m_attributes, divisor);
    m_attributes++;
}

void lwvl::VertexArray::drawArrays(PrimitiveMode mode, int count) const {
    glDrawArraysInstanced(static_cast<GLenum>(mode), 0, count, m_instances);
}

void lwvl::VertexArray::drawElements(PrimitiveMode mode, int count, ByteFormat type) const {
    glDrawElementsInstanced(static_cast<GLenum>(mode), count, static_cast<GLenum>(type), nullptr, m_instances);
}

void
lwvl::VertexArray::multiDrawArrays(PrimitiveMode mode, const GLint *firsts, const GLsizei *counts, GLsizei drawCount) {
    glMultiDrawArrays(static_cast<GLenum>(mode), firsts, counts, drawCount);
}

void lwvl::VertexArray::multiDrawElements(
    PrimitiveMode mode, const GLsizei *counts, ByteFormat type, const void *const *indices, GLsizei drawCount
) {
    glMultiDrawElements(static_cast<GLenum>(mode), counts, static_cast<GLenum>(type), indices, drawCount);
}

GLuint lwvl::VertexArray::id() const {
    return m_id;
}

