#pragma once

#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>

#include <array>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "gfx/camera.h"
#include "gfx/color.h"
#include "math/matrix.h"
#include "math/vector.h"
#include "ptcl/renderer/drawFrame.h"
#include "ptcl/renderer/textureCache.h"
#include "ptcl/ptclEnum.h"
#include "typedefs.h"


class QOpenGLFunctions_2_0;


namespace Ptcl::Render {


// ========================================================================== //


struct AttributeBinding {
    std::string_view name;
    GLuint index;
};

class BatchProgram {
public:
    bool create(std::string_view name, const std::string& vertexSource,
        const std::string& fragmentSource,
        std::span<const AttributeBinding> attributeBindings = {});

    void destroy();

    bool isValid() const { return mProgram != nullptr; }

    void bind(QOpenGLFunctions_2_0& gl, const Math::Matrix44f& viewProjection) const;

    void release() const;

    void setCombinerConfig(ColorCombinerFuncType combiner) const;
    void setTextureSample(const TextureBinding& texture) const;
    void setConstantColor(const Gfx::Color& color) const;
    void setAlphaRef(GLfloat value) const;

    void setInt(GLint location, GLint value) const;
    void setFloat(GLint location, GLfloat value) const;
    void setVec2(GLint location, GLfloat x, GLfloat y) const;
    void setVec3(GLint location, GLfloat x, GLfloat y, GLfloat z) const;
    void setVec4(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;

    GLint uniformLocation(std::string_view name) const;

private:
    std::unique_ptr<QOpenGLShaderProgram> mProgram;

    GLint mViewProjection{-1};
    GLint mTextureSampler{-1};
    GLint mCombinerConfig{-1};
    GLint mHasTexture{-1};
    GLint mTexel{-1};
    GLint mConstantColor{-1};
    GLint mAlphaRef{-1};
};


// ========================================================================== //


class Renderer {
public:
    void initialize(QOpenGLFunctions_2_0& gl);
    void render(QOpenGLFunctions_2_0& gl, const DrawFrame& frame, const Gfx::Camera& camera);
    void cleanup(QOpenGLFunctions_2_0& gl);

private:
    struct ParticleUniforms {
        GLint center{-1};
        GLint axisX{-1};
        GLint axisY{-1};
        GLint primaryColor{-1};
        GLint uvOffset{-1};
        GLint uvScale{-1};
    };

    struct CompactUniforms {
        GLint pointScale{-1};
        GLint wrapS{-1};
        GLint wrapT{-1};
    };

    struct StripeUniforms {
        GLint primaryColor{-1};
    };

    struct StripeVertex {
        f32 x;
        f32 y;
        f32 z;
        f32 u;
        f32 v;
        f32 alpha;
    };

    struct CompactPointVertex {
        // xyz = world position, w = rotation in degrees
        std::array<f32, 4> centerRotation;
        // xy = particle scale, zw = unused
        std::array<f32, 4> scale;
        // rgb = particle colour, a = particle alpha
        std::array<f32, 4> color;
        // xy = uv offset, zw = uv size
        std::array<f32, 4> uv;
    };

    void renderBatches(QOpenGLFunctions_2_0& gl, const Gfx::Camera& camera, const std::vector<ParticleDrawBatch>& batches);
    void renderCompactBatches(QOpenGLFunctions_2_0& gl, const Gfx::Camera& camera, const std::vector<ParticleDrawBatch>& batches);
    void renderStripes(QOpenGLFunctions_2_0& gl, const Gfx::Camera& camera, const std::vector<DrawStripe>& stripes);

    void drawParticle(QOpenGLFunctions_2_0& gl, const EmitterState& state, const ParticleState& particle, const Gfx::Camera& camera);
    void appendStripeVertices(std::vector<StripeVertex>& vertices, const StripePoint& point, const Gfx::Camera& camera) const;

    static CompactPointVertex toCompactVertex(const ParticleState& particle);

    void applyBatchState(QOpenGLFunctions_2_0& gl, const BatchProgram& program, const EmitterState& state, const Gfx::Color& constantSource);

    void createStripeBuffer(QOpenGLFunctions_2_0& gl);
    void createCompactBuffer(QOpenGLFunctions_2_0& gl);

    BatchProgram mParticleProgram;
    BatchProgram mCompactProgram;
    BatchProgram mStripeProgram;

    ParticleUniforms mParticleUniforms{};
    CompactUniforms mCompactUniforms{};
    StripeUniforms mStripeUniforms{};

    QOpenGLBuffer mParticleVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mParticleVao{};
    QOpenGLBuffer mStripeVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mStripeVao{};
    QOpenGLBuffer mCompactVbo{QOpenGLBuffer::VertexBuffer};
    QOpenGLVertexArrayObject mCompactVao{};

    TextureCache mTextures;
};


// ========================================================================== //


} // namespace Ptcl::Render
