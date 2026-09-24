#include "editor/viewport/viewportGrid.h"

#include <QDebug>

#include <array>
#include <cstddef>
#include <memory>
#include <vector>


namespace PtclTool {


// ========================================================================== //


static constexpr const char* cGridVertShader = R"(#version 150 core
in vec3 aPos;
uniform mat4 uViewProj;
void main() {
    gl_Position = uViewProj * vec4(aPos, 1.0);
}
)";

static constexpr const char* cGridFragShader = R"(#version 150 core
uniform vec4 uColor;
out vec4 fragColor;
void main() {
    fragColor = uColor;
}
)";

static constexpr s32 cGridCellSize = 10;
static constexpr s32 cGridExtent = 250;

static constexpr std::array<GLfloat, 12> cAxes = {
    -static_cast<GLfloat>(cGridExtent), 0.0f, 0.0f,
     static_cast<GLfloat>(cGridExtent), 0.0f, 0.0f,
    0.0f, 0.0f, -static_cast<GLfloat>(cGridExtent),
    0.0f, 0.0f,  static_cast<GLfloat>(cGridExtent),
};

static constexpr std::array<GLfloat, 4> cGridColor = {
    0.25f, 0.29f, 0.33f, 1.0f,
};

static constexpr std::array<GLfloat, 4> cAxisColor = {
    0.62f, 0.64f, 0.67f, 1.0f,
};


static void createLineBuffer(QOpenGLFunctions_2_0& gl,
    QOpenGLBuffer& buffer, QOpenGLVertexArrayObject& vertexArray,
    const GLfloat* vertices, std::size_t vertexCount) {
    vertexArray.create();
    buffer.create();

    vertexArray.bind();
    buffer.bind();
    buffer.allocate(vertices, static_cast<s32>(vertexCount * sizeof(GLfloat)));
    gl.glEnableVertexAttribArray(0);
    gl.glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);

    buffer.release();
    vertexArray.release();
}


void ViewportGrid::initialize(QOpenGLFunctions_2_0& gl) {
    if (mInitialized) {
        return;
    }

    auto program = std::make_unique<QOpenGLShaderProgram>();
    if (!program->addShaderFromSourceCode(QOpenGLShader::Vertex, cGridVertShader)) {
        qWarning() << "[viewport] Failed to compile grid vertex shader:" << program->log();
        return;
    }
    if (!program->addShaderFromSourceCode(QOpenGLShader::Fragment, cGridFragShader)) {
        qWarning() << "[viewport] Failed to compile grid fragment shader:" << program->log();
        return;
    }
    if (!program->link()) {
        qWarning() << "[viewport] Failed to link grid shader:" << program->log();
        return;
    }

    const auto viewProjectionLocation = program->uniformLocation("uViewProj");
    const auto colorLocation = program->uniformLocation("uColor");
    if (viewProjectionLocation < 0 || colorLocation < 0) {
        qWarning() << "[viewport] Grid shader uniforms were not found.";
        return;
    }

    std::vector<GLfloat> grid;
    const auto lineCount = static_cast<std::size_t>(cGridExtent) * 2
        / static_cast<std::size_t>(cGridCellSize) + 1;
    grid.reserve(lineCount * 2 * 2 * 3);

    const auto extent = static_cast<GLfloat>(cGridExtent);
    for (s32 i = -cGridExtent; i <= cGridExtent; i += cGridCellSize) {
        const auto coordinate = static_cast<GLfloat>(i);

        // Line parallel to the Z axis.
        grid.insert(grid.end(), {
            coordinate, 0.0f, -extent,
            coordinate, 0.0f,  extent,
        });

        // Line parallel to the X axis.
        grid.insert(grid.end(), {
            -extent, 0.0f, coordinate,
             extent, 0.0f, coordinate,
        });
    }
    mGridVertexCount = static_cast<GLsizei>(grid.size() / 3);
    createLineBuffer(gl, mGridVbo, mGridVao, grid.data(), grid.size());

    mAxisVertexCount = static_cast<GLsizei>(cAxes.size() / 3);
    createLineBuffer(gl, mAxisVbo, mAxisVao, cAxes.data(), cAxes.size());

    mViewProjectionLocation = viewProjectionLocation;
    mColorLocation = colorLocation;
    mProgram = std::move(program);
    mInitialized = true;
}

void ViewportGrid::draw(QOpenGLFunctions_2_0& gl,
    const Math::Matrix44f& viewProjection) {
    if (!mInitialized || mProgram == nullptr) {
        return;
    }

    GLint previousProgram = 0;
    GLint previousDepthFunction = GL_LESS;
    const GLboolean depthTestEnabled = gl.glIsEnabled(GL_DEPTH_TEST);
    gl.glGetIntegerv(GL_CURRENT_PROGRAM, &previousProgram);
    if (depthTestEnabled) {
        gl.glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunction);
    }

    mProgram->bind();
    gl.glUniformMatrix4fv(mViewProjectionLocation, 1, GL_TRUE, viewProjection.data());

    gl.glEnable(GL_DEPTH_TEST);
    gl.glDepthFunc(GL_LEQUAL);

    gl.glUniform4fv(mColorLocation, 1, cGridColor.data());
    mGridVao.bind();
    gl.glDrawArrays(GL_LINES, 0, mGridVertexCount);
    mGridVao.release();

    gl.glUniform4fv(mColorLocation, 1, cAxisColor.data());
    mAxisVao.bind();
    gl.glDrawArrays(GL_LINES, 0, mAxisVertexCount);
    mAxisVao.release();

    mProgram->release();
    gl.glUseProgram(static_cast<GLuint>(previousProgram));
    if (depthTestEnabled) {
        gl.glEnable(GL_DEPTH_TEST);
        gl.glDepthFunc(static_cast<GLenum>(previousDepthFunction));
    } else {
        gl.glDisable(GL_DEPTH_TEST);
    }
}

void ViewportGrid::cleanup() {
    mProgram.reset();

    mGridVao.destroy();
    mGridVbo.destroy();
    mAxisVao.destroy();
    mAxisVbo.destroy();

    mGridVertexCount = 0;
    mAxisVertexCount = 0;
    mViewProjectionLocation = -1;
    mColorLocation = -1;
    mInitialized = false;
}


// ========================================================================== //


} // namespace PtclTool
