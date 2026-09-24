#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <memory>

#include "math/matrix.h"


namespace PtclTool {


// ========================================================================== //


class ViewportGrid {
public:
    void initialize(QOpenGLFunctions_2_0& gl);
    void draw(QOpenGLFunctions_2_0& gl, const Math::Matrix44f& viewProjection);
    void cleanup();

private:
    std::unique_ptr<QOpenGLShaderProgram> mProgram;

    QOpenGLBuffer mGridVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mGridVao{};
    GLsizei mGridVertexCount{0};

    QOpenGLBuffer mAxisVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mAxisVao{};
    GLsizei mAxisVertexCount{0};

    GLint mViewProjectionLocation{-1};
    GLint mColorLocation{-1};
    bool mInitialized{false};
};


// ========================================================================== //


} // namespace PtclTool
