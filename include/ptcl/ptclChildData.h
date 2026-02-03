#pragma once

#include "math/vector.h"

#include "ptcl/ptclBinary.h"
#include "ptcl/ptclEnum.h"
#include "ptcl/ptclTexture.h"

#include "typedefs.h"


namespace Ptcl {


// ========================================================================== //


class ChildData {

public:
    struct BasicProperties {
        BillboardType billboardType{BillboardType::Billboard};
    };

    struct EmissionProperties {
        s32 emitRate{1};
        s32 emitTiming{0};
        s32 life{10};
        s32 emitStep{1};
    };

    struct VelocityProperties {
        Math::Vector3f randVel{0.0f, 0.0f, 0.0f};
        Math::Vector3f gravity{0.0f, -1.0f, 0.0f};
        f32 velInheritRate{1.0f};
        f32 initPosRand{0.0f};
        f32 figurVel{0.1f};
        f32 airResist{1.0f};
    };

    struct RotationProperties {
        RotType rotType{RotType::None};
        Math::Vector3i initRot{0, 0, 0};
        Math::Vector3i initRotRand{0, 0, 0};
        Math::Vector3i rotVel{0, 0, 0};
        Math::Vector3i rotVelRand{0, 0, 0};
        Math::Vector2f rotBasis{0.0f, 0.0f};
    };

    struct ScaleProperties {
        Math::Vector2f scale{1.0f, 1.0f};
        Math::Vector2f scaleTarget{1.0f, 1.0f};
        f32 scaleInheritRate{1.0f};
        s32 scaleStartFrame{0};
    };

    struct TextureProperties {
        TextureWrap textureWrapT{TextureWrap::ClampToEdge};
        TextureWrap textureWrapS{TextureWrap::ClampToEdge};
        TextureFilter textureMagFilter{TextureFilter::Nearest};
        TextureFilter textureMinFilter{TextureFilter::Nearest};
        TextureMipFilter textureMipFilter{TextureMipFilter::None};
        Math::Vector2f texUVScale{1.0f, 1.0f};
    };

    struct ColorProperties {
        binColor4f color0{1.0f, 1.0f, 1.0f, 1.0f};
        binColor3f color1{255.0f, 255.0f, 255.0f};
    };

    struct AlphaProperties {
        f32 alpha{1.0f};
        f32 alphaTarget{1.0f};
        f32 alphaInit{1.0f};
        s32 alphaStartFrame{1};
        s32 alphaBaseFrame{1};
    };

    struct CombinerProperties {
        BlendFuncType blendFunc{BlendFuncType::Translucent};
        DepthFuncType depthFunc{DepthFuncType::Unk0};
        ColorCombinerFuncType combinerFunc{ColorCombinerFuncType::CombinerConfig0};
    };

public:
    ChildData() = default;

    ChildData(const ChildData&) = delete;
    ChildData& operator=(const ChildData&) = delete;

    TextureHandle& textureHandle();
    const TextureHandle& textureHandle() const;
    void setTexture(const std::shared_ptr<Texture>& texture);

    const BasicProperties &basicProperties() const;
    void setBasicProperties(const BasicProperties &basicProperties);

    const EmissionProperties& emissionProperties() const;
    void setEmissionProperties(const EmissionProperties& emissionProperties);

    const VelocityProperties& velocityProperties() const;
    void setVelocityProperties(const VelocityProperties& velocityProperties);

    const TextureProperties& textureProperties() const;
    void setTextureProperties(const TextureProperties& textureProperties);

    const ColorProperties& colorProperties() const;
    void setColorProperties(const ColorProperties& colorProperties);

    const AlphaProperties& alphaProperties() const;
    void setAlphaProperties(const AlphaProperties& alphaProperties);

    const CombinerProperties& combinerProperties() const;
    void setCombinerProperties(const CombinerProperties& combinerProperties);

    const RotationProperties& rotationProperties() const;
    void setRotationProperties(const RotationProperties& rotationProperties);

    const ScaleProperties& scaleProperties() const;
    void setScaleProperties(const ScaleProperties& scaleProperties);

    void initFromBinary(const BinChildData& childData);

private:
    BasicProperties mBasicProperties{};
    EmissionProperties mEmissionProperties{};
    VelocityProperties mVelocityProperties{};
    TextureProperties mTextureProperties{};
    ColorProperties mColorProperties{};
    AlphaProperties mAlphaProperties{};
    CombinerProperties mCombinerProperties{};
    RotationProperties mRotationProperties{};
    ScaleProperties mScaleProperties{};

    TextureHandle mTextureHandle{};
};


// ========================================================================== //


} // namespace Ptcl
