#include "ptcl/ptclChildData.h"


namespace Ptcl {


// ========================================================================== //

std::unique_ptr<ChildData> ChildData::clone() const {
    auto newData = std::make_unique<ChildData>();

    newData->mBasicProperties = mBasicProperties;
    newData->mEmissionProperties = mEmissionProperties;
    newData->mVelocityProperties = mVelocityProperties;
    newData->mTextureProperties = mTextureProperties;
    newData->mColorProperties = mColorProperties;
    newData->mAlphaProperties = mAlphaProperties;
    newData->mCombinerProperties = mCombinerProperties;
    newData->mRotationProperties = mRotationProperties;
    newData->mScaleProperties = mScaleProperties;
    newData->mTextureHandle = mTextureHandle.clone();
    return newData;
}

TextureHandle& ChildData::textureHandle() {
    return mTextureHandle;
}

const TextureHandle& ChildData::textureHandle() const {
    return mTextureHandle;
}

void ChildData::setTexture(const std::shared_ptr<Texture>& texture) {
    mTextureHandle.set(texture);
}

const ChildData::BasicProperties& ChildData::basicProperties() const {
    return mBasicProperties;
}

void ChildData::setBasicProperties(const BasicProperties& basicProperties) {
    mBasicProperties = basicProperties;
}

const ChildData::EmissionProperties& ChildData::emissionProperties() const {
    return mEmissionProperties;
}

void ChildData::setEmissionProperties(const EmissionProperties& emissionProperties) {
    mEmissionProperties = emissionProperties;
}

const ChildData::VelocityProperties& ChildData::velocityProperties() const {
    return mVelocityProperties;
}

void ChildData::setVelocityProperties(const VelocityProperties& velocityProperties) {
    mVelocityProperties = velocityProperties;
}

const ChildData::TextureProperties& ChildData::textureProperties() const {
    return mTextureProperties;
}

void ChildData::setTextureProperties(const TextureProperties& textureProperties) {
    mTextureProperties = textureProperties;
}

const ChildData::ColorProperties& ChildData::colorProperties() const {
    return mColorProperties;
}

void ChildData::setColorProperties(const ColorProperties& colorProperties) {
    mColorProperties = colorProperties;
}

const ChildData::AlphaProperties& ChildData::alphaProperties() const {
    return mAlphaProperties;
}

void ChildData::setAlphaProperties(const AlphaProperties& alphaProperties) {
    mAlphaProperties = alphaProperties;
}

const ChildData::CombinerProperties& ChildData::combinerProperties() const {
    return mCombinerProperties;
}

void ChildData::setCombinerProperties(const CombinerProperties& combinerProperties) {
    mCombinerProperties = combinerProperties;
}

const ChildData::RotationProperties& ChildData::rotationProperties() const {
    return mRotationProperties;
}

void ChildData::setRotationProperties(const RotationProperties& rotationProperties) {
    mRotationProperties = rotationProperties;
}

const ChildData::ScaleProperties& ChildData::scaleProperties() const {
    return mScaleProperties;
}

void ChildData::setScaleProperties(const ScaleProperties& scaleProperties) {
    mScaleProperties = scaleProperties;
}

void ChildData::initFromBinary(const BinChildData& childData) {

    mBasicProperties = {
        .billboardType = childData.childBillboardType
    };

    mEmissionProperties = {
        .emitRate = childData.childEmitRate,
        .emitTiming = childData.childEmitTiming,
        .life = childData.childLife,
        .emitStep = childData.childEmitStep
    };

    mVelocityProperties = {
        .randVel = { childData.childRandVel.x, childData.childRandVel.y, childData.childRandVel.z },
        .gravity = { childData.childGravity.x, childData.childGravity.y, childData.childGravity.z },
        .velInheritRate = childData.childVelInheritRate,
        .initPosRand = childData.childInitPosRand,
        .figurVel = childData.childFigurVel,
        .airResist = childData.childAirResist
    };

    mRotationProperties = {
        .rotType = childData.childRotType,
        .initRot = { childData.childInitRot.x, childData.childInitRot.y, childData.childInitRot.z },
        .initRotRand = { childData.childInitRotRand.x, childData.childInitRotRand.y, childData.childInitRotRand.z },
        .rotVel = { childData.childRotVel.x, childData.childRotVel.y, childData.childRotVel.z },
        .rotVelRand = { childData.childRotVelRand.x, childData.childRotVelRand.y, childData.childRotVelRand.z },
        .rotBasis = { childData.childRotBasis.x, childData.childRotBasis.y }
    };

    mScaleProperties = {
        .scale = { childData.childScale.x, childData.childScale.y },
        .scaleTarget = { childData.childScaleTarget.x, childData.childScaleTarget.y },
        .scaleInheritRate = childData.childScaleInheritRate,
        .scaleStartFrame = childData.childScaleStartFrame
    };

    mTextureProperties = {
        .textureWrapT = childData.childTextureRes.wrapT,
        .textureWrapS = childData.childTextureRes.wrapS,
        .textureMagFilter = childData.childTextureRes.magFilter,
        .textureMinFilter = static_cast<TextureFilter>(childData.childTextureRes.minMipFilter & 0x1),
        .textureMipFilter = static_cast<TextureMipFilter>((childData.childTextureRes.minMipFilter >> 1) & 0x3),
        .texUVScale = { childData.childTexUScale, childData.childTexVScale }
    };

    mColorProperties = {
        .color0 = childData.childColor0,
        .color1 = childData.childColor1
    };

    mAlphaProperties = {
        .alpha = childData.childAlpha,
        .alphaTarget = childData.childAlphaTarget,
        .alphaInit = childData.childAlphaInit,
        .alphaStartFrame = childData.childAlphaStartFrame,
        .alphaBaseFrame = childData.childAlphaBaseFrame
    };

    mCombinerProperties = {
        .blendFunc = childData.childBlendType,
        .depthFunc = childData.childDepthType,
        .combinerFunc = childData.childCombinerType
    };
}


// ========================================================================== //


} // namespace ptcl
