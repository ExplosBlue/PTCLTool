#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLFunctions_2_0>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <memory>


namespace PtclTool {


class ViewportCamera;


// ========================================================================== //


class ViewportGrid {
public:
    void initialize(QOpenGLFunctions_2_0& gl);
    void draw(QOpenGLFunctions_2_0& gl, const ViewportCamera& camera, float pixelRatio, float gridCellSize);
    void cleanup();

private:
    std::unique_ptr<QOpenGLShaderProgram> mProgram;

    QOpenGLBuffer mQuadVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mQuadVao{};

    GLint mInverseViewProjectionLocation{-1};
    GLint mCameraPositionLocation{-1};
    GLint mViewForwardLocation{-1};
    GLint mPlaneNormalLocation{-1};
    GLint mPlanePointLocation{-1};
    GLint mPlaneULocation{-1};
    GLint mPlaneVLocation{-1};
    GLint mAxisColorULocation{-1};
    GLint mAxisColorVLocation{-1};
    GLint mPerspectiveLocation{-1};
    GLint mFadeStartLocation{-1};
    GLint mFadeEndLocation{-1};
    GLint mPixelRatioLocation{-1};
    GLint mGridCellSizeLocation{-1};

    bool mInitialized{false};
};


// ========================================================================== //


} // namespace PtclTool
