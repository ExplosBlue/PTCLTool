#pragma once

#include <cstddef>
#include <span>

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLVertexArrayObject>


namespace Gfx {


// ========================================================================== //


inline constexpr GLsizei unitQuadVertexCount = 6;

struct VertexAttribute {
    GLuint index;
    GLint components;
    std::size_t offset;
};

void setVertexAttributes(QOpenGLFunctions_2_0& gl, std::span<const VertexAttribute> attributes, std::size_t stride);

void createUnitQuad(QOpenGLFunctions_2_0& gl, QOpenGLBuffer& buffer, QOpenGLVertexArrayObject& vao);


// ========================================================================== //


} // namespace Gfx
