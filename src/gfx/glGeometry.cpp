#include "gfx/glGeometry.h"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>


namespace Gfx {


// ========================================================================== //


namespace {


const void* toOffsetPointer(std::size_t offset) {
    return std::bit_cast<const void*>(static_cast<std::uintptr_t>(offset));
}

const std::array<GLfloat, 12>& unitQuadVertices() {
    static_assert(unitQuadVertexCount * 2 == 12, "unit quad vertex count must match the vertex array");

    static const std::array<GLfloat, 12> vertices = {
        -0.5f, -0.5f,
         0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f, -0.5f,
         0.5f,  0.5f,
        -0.5f,  0.5f,
    };

    return vertices;
}

} // namespace


void setVertexAttributes(QOpenGLFunctions_2_0& gl, std::span<const VertexAttribute> attributes, std::size_t stride) {
    for (const VertexAttribute& attribute : attributes) {
        gl.glEnableVertexAttribArray(attribute.index);
        gl.glVertexAttribPointer(
            attribute.index,
            attribute.components,
            GL_FLOAT,
            GL_FALSE,
            static_cast<GLsizei>(stride),
            toOffsetPointer(attribute.offset)
        );
    }
}

void createUnitQuad(QOpenGLFunctions_2_0& gl, QOpenGLBuffer& buffer, QOpenGLVertexArrayObject& vao) {
    constexpr std::array<VertexAttribute, 1> layout{{
        {0, 2, 0},
    }};

    const std::array<GLfloat, 12>& vertices = unitQuadVertices();

    vao.create();
    buffer.create();

    vao.bind();
    buffer.bind();
    buffer.allocate(vertices.data(), static_cast<int>(sizeof(vertices)));
    setVertexAttributes(gl, layout, 2 * sizeof(GLfloat));
    buffer.release();
    vao.release();
}


// ========================================================================== //


} // namespace Gfx
