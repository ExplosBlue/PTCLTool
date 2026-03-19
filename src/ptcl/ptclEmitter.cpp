#include "ptcl/ptclEmitter.h"


namespace Ptcl {


// ========================================================================== //

std::unique_ptr<Emitter> Emitter::clone() const {
    auto newEmitter = std::make_unique<Emitter>();

    newEmitter->mFlag = mFlag;

    // Basic Properties
    newEmitter->mType = mType;
    newEmitter->mFollowType = mFollowType;
    newEmitter->mName = mName;
    newEmitter->mRandomSeed = mRandomSeed;
    newEmitter->mBillboardType = mBillboardType;
    newEmitter->mIsPolygon = mIsPolygon;
    newEmitter->mIsVelLook = mIsVelLook;
    newEmitter->mIsEmitterBillboardMtx = mIsEmitterBillboardMtx;
    newEmitter->mIsFollow = mIsFollow;

    // Gravity Properties
    newEmitter->mIsDirectional = mIsDirectional;
    newEmitter->mGravity = mGravity;

    // Lifespan Properties
    newEmitter->mPtclLife = mPtclLife;
    newEmitter->mPtclLifeRnd = mPtclLifeRnd;

    // Transform Properties
    newEmitter->mTransformSRT = mTransformSRT;
    newEmitter->mTransformRT = mTransformRT;

    // Termination Properties
    newEmitter->mIsStopEmitInFade = mIsStopEmitInFade;
    newEmitter->mAlphaAddInFade = mAlphaAddInFade;

    // Emission Properties
    newEmitter->mEmitStartFrame = mEmitStartFrame;
    newEmitter->mEmitEndFrame = mEmitEndFrame;
    newEmitter->mLifeStep = mLifeStep;
    newEmitter->mLifeStepRnd = mLifeStepRnd;
    newEmitter->mEmitRate = mEmitRate;

    // Velocity Properties
    newEmitter->mFigureVelocity = mFigureVelocity;
    newEmitter->mVelocityDir = mVelocityDir;
    newEmitter->mInitVelocity = mInitVelocity;
    newEmitter->mInitVelocityRnd = mInitVelocityRnd;
    newEmitter->mSpreadVec = mSpreadVec;
    newEmitter->mAirResistance = mAirResistance;

    // Volume Properties
    newEmitter->mVolumeTblIndex = mVolumeTblIndex;
    newEmitter->mVolumeType = mVolumeType;
    newEmitter->mVolumeRadius = mVolumeRadius;
    newEmitter->mVolumeSweepStart = mVolumeSweepStart;
    newEmitter->mVolumeSweepParam = mVolumeSweepParam;

    // Color Properties
    newEmitter->mColor0 = mColor0;
    newEmitter->mColorSection1 = mColorSection1;
    newEmitter->mColorSection2 = mColorSection2;
    newEmitter->mColorSection3 = mColorSection3;
    newEmitter->mColorNumRepeat = mColorNumRepeat;
    newEmitter->mColorCalcType = mColorCalcType;
    newEmitter->mColor1 = mColor1;

    // Alpha Properties
    newEmitter->mAlphaAnim = mAlphaAnim;

    // Scale Properties
    newEmitter->mScaleAnim = mScaleAnim;
    newEmitter->mScaleRand = mScaleRand;

    // Rotation Properties
    newEmitter->mRotType = mRotType;
    newEmitter->mInitRot = mInitRot;
    newEmitter->mInitRotRand = mInitRotRand;
    newEmitter->mRotVel = mRotVel;
    newEmitter->mRotVelRand = mRotVelRand;
    newEmitter->mRotBasis = mRotBasis;

    // Combiner Properties
    newEmitter->mBlendFunc = mBlendFunc;
    newEmitter->mDepthFunc = mDepthFunc;
    newEmitter->mCombinerFunc = mCombinerFunc;

    // Texture Properties
    newEmitter->mTextureWrapT = mTextureWrapT;
    newEmitter->mTextureWrapS = mTextureWrapS;
    newEmitter->mTextureMagFilter = mTextureMagFilter;
    newEmitter->mTextureMinFilter = mTextureMinFilter;
    newEmitter->mTextureMipFilter = mTextureMipFilter;
    newEmitter->mNumTexturePattern = mNumTexturePattern;
    newEmitter->mNumTextureDivisionX = mNumTextureDivisionX;
    newEmitter->mNumTextureDivisionY = mNumTextureDivisionY;
    newEmitter->mTextureUVScale = mTextureUVScale;
    newEmitter->mTexturePatternTbl = mTexturePatternTbl;
    newEmitter->mTexturePatternFrequency = mTexturePatternFrequency;
    newEmitter->mTexturePatternTblUse = mTexturePatternTblUse;
    newEmitter->mIsTexturePatternAnim = mIsTexturePatternAnim;
    newEmitter->mTextureHandle = mTextureHandle;

    newEmitter->mChildFlags = mChildFlags;
    newEmitter->mChild = mChild;

    // Fluctuation Properties
    newEmitter->mFluctuationFlags = mFluctuationFlags;
    newEmitter->mFluctuationScale = mFluctuationScale;
    newEmitter->mFluctuationFreq = mFluctuationFreq;
    newEmitter->mFluctuationPhaseRnd = mFluctuationPhaseRnd;

    // Stripe Properties
    newEmitter->mStripeFlags = mStripeFlags;
    newEmitter->mStripeType = mStripeType;
    newEmitter->mStripeNumHistory = mStripeNumHistory;
    newEmitter->mStripeStartAlpha = mStripeStartAlpha;
    newEmitter->mStripeEndAlpha = mStripeEndAlpha;
    newEmitter->mStripeUVScrollSpeed = mStripeUVScrollSpeed;
    newEmitter->mStripeHistoryStep = mStripeHistoryStep;
    newEmitter->mStripeDirInterpolate = mStripeDirInterpolate;

    // Field Properties
    newEmitter->mFieldFlags = mFieldFlags;
    newEmitter->mFieldRandom = mFieldRandom;
    newEmitter->mFieldMagnet = mFieldMagnet;
    newEmitter->mFieldSpin = mFieldSpin;
    newEmitter->mFieldCollision = mFieldCollision;
    newEmitter->mFieldConvergence = mFieldConvergence;
    newEmitter->mFieldPosAdd = mFieldPosAdd;

    return newEmitter;
}

BitFlag<EmitterFlag>& Emitter::flags() {
    return mFlag;
}

const BitFlag<EmitterFlag>& Emitter::flags() const {
    return mFlag;
}

void Emitter::initFieldRandom(const BinFieldRandomData& randomData) {
    mFieldRandom = {
        .randomBlank = randomData.fieldRandomBlank,
        .randomVelAdd = Math::Vector3f(randomData.fieldRandomVelAdd.x, randomData.fieldRandomVelAdd.y, randomData.fieldRandomVelAdd.z)
    };
}

void Emitter::initFieldMagnet(const BinFieldMagnetData& magnetData) {
    mFieldMagnet = {
        .magnetPower = magnetData.fieldMagnetPower,
        .magnetPos = Math::Vector3f(magnetData.fieldMagnetPos.x, magnetData.fieldMagnetPos.y, magnetData.fieldMagnetPos.z),
        .magnetFlag = magnetData.fieldMagnetFlag,
    };
}

void Emitter::initFieldSpin(const BinFieldSpinData& spinData) {
    mFieldSpin = {
        .spinRotate = spinData.fieldSpinRotate,
        .spinAxis = spinData.fieldSpinAxis
    };
}

void Emitter::initFieldCollision(const BinFieldCollisionData& collisionData) {
    mFieldCollision = {
        .collisionType = collisionData.fieldCollisionType,
        .collisionIsWorld = collisionData.fieldCollisionIsWorld > 0,
        .collisionCoord = collisionData.fieldCollisionCoord,
        .collisionCoef = collisionData.fieldCollisionCoef
    };
}

void Emitter::initFieldConvergence(const BinFieldConvergenceData& convergenceData) {
    mFieldConvergence = {
        .convergenceType = convergenceData.fieldConvergenceType,
        .convergencePos = Math::Vector3f(convergenceData.fieldConvergencePos.x, convergenceData.fieldConvergencePos.y, convergenceData.fieldConvergencePos.z)
    };
}

void Emitter::initFieldPosAdd(const BinFieldPosAddData& posAddData) {
    mFieldPosAdd = {
        .posAdd = Math::Vector3f(posAddData.fieldPosAdd.x, posAddData.fieldPosAdd.y, posAddData.fieldPosAdd.z)
    };
}

void Emitter::initFluctuationData(const BinFluctuationData& fluctuationData) {
    mFluctuationScale = fluctuationData.fluctuationScale;
    mFluctuationFreq = fluctuationData.fluctuationFreq;
    mFluctuationPhaseRnd = static_cast<bool>(fluctuationData.fluctuationPhaseRnd);
}

void Emitter::initStripeData(const BinStripeData& stripeData) {
    mStripeType = stripeData.stripeType;
    mStripeNumHistory = stripeData.stripeNumHistory;
    mStripeStartAlpha = stripeData.stripeStartAlpha;
    mStripeEndAlpha = stripeData.stripeEndAlpha;
    mStripeUVScrollSpeed = {stripeData.uvScrollSpeed.x, stripeData.uvScrollSpeed.y};
    mStripeHistoryStep = stripeData.stripeHistoryStep;
    mStripeDirInterpolate = stripeData.stripeDirInterpolate;
}

void Emitter::initChild(const BinChildData& childData) {
    mChild = {
        .billboardType = childData.childBillboardType,

        .emitRate = childData.childEmitRate,
        .emitTiming = childData.childEmitTiming,
        .life = childData.childLife,
        .emitStep = childData.childEmitStep,

        .randVel = { childData.childRandVel.x, childData.childRandVel.y, childData.childRandVel.z },
        .gravity = { childData.childGravity.x, childData.childGravity.y, childData.childGravity.z },
        .velInheritRate = childData.childVelInheritRate,
        .initPosRand = childData.childInitPosRand,
        .figurVel = childData.childFigurVel,
        .airResist = childData.childAirResist,

        .rotType = childData.childRotType,
        .initRot = { childData.childInitRot.x, childData.childInitRot.y, childData.childInitRot.z },
        .initRotRand = { childData.childInitRotRand.x, childData.childInitRotRand.y, childData.childInitRotRand.z },
        .rotVel = { childData.childRotVel.x, childData.childRotVel.y, childData.childRotVel.z },
        .rotVelRand = { childData.childRotVelRand.x, childData.childRotVelRand.y, childData.childRotVelRand.z },
        .rotBasis = { childData.childRotBasis.x, childData.childRotBasis.y },

        .scale = { childData.childScale.x, childData.childScale.y },
        .scaleTarget = { childData.childScaleTarget.x, childData.childScaleTarget.y },
        .scaleInheritRate = childData.childScaleInheritRate,
        .scaleStartFrame = childData.childScaleStartFrame,

        .textureWrapT = childData.childTextureRes.wrapT,
        .textureWrapS = childData.childTextureRes.wrapS,
        .textureMagFilter = childData.childTextureRes.magFilter,
        .textureMinFilter = static_cast<TextureFilter>(childData.childTextureRes.minMipFilter & 0x1),
        .textureMipFilter = static_cast<TextureMipFilter>((childData.childTextureRes.minMipFilter >> 1) & 0x3),
        .texUVScale = { childData.childTexUScale, childData.childTexVScale },

        .color0 = childData.childColor0,
        .color1 = childData.childColor1,

        .alpha = childData.childAlpha,
        .alphaTarget = childData.childAlphaTarget,
        .alphaInit = childData.childAlphaInit,
        .alphaStartFrame = childData.childAlphaStartFrame,
        .alphaBaseFrame = childData.childAlphaBaseFrame,

        .blendFunc = childData.childBlendType,
        .depthFunc = childData.childDepthType,
        .combinerFunc = childData.childCombinerType
    };
}

bool Emitter::hasStripeData() const {
    const auto bType = mBillboardType;
    const bool isBillboardStripe = bType == BillboardType::Stripe || bType == BillboardType::ComplexStripe;
    return mType != EmitterType::Simple && isBillboardStripe;
}

void Emitter::initFromBinary(const BinCommonEmitterData& emitterData) {
    mFlag = emitterData.flag;

    // Texture Properties
    mTextureWrapT = emitterData.textureRes.wrapT;
    mTextureWrapS = emitterData.textureRes.wrapS;
    mTextureMagFilter = emitterData.textureRes.magFilter;
    mTextureMinFilter = static_cast<TextureFilter>(emitterData.textureRes.minMipFilter & 0x1);
    mTextureMipFilter = static_cast<TextureMipFilter>((emitterData.textureRes.minMipFilter >> 1) & 0x3);
    mNumTexturePattern = emitterData.numTexPat;
    mNumTextureDivisionX = emitterData.numTexDivX;
    mNumTextureDivisionY = emitterData.numTexDivY;
    mTextureUVScale = { emitterData.texUVScale.x, emitterData.texUVScale.y };
    mTexturePatternTbl = emitterData.texPatTbl;
    mTexturePatternFrequency = emitterData.texPatFreq;
    mTexturePatternTblUse = emitterData.texPatTblUse;
    mIsTexturePatternAnim = emitterData.isTexPatAnim;

    // Basic Properties
    mType = emitterData.type;
    mFollowType = emitterData.followType;
    // name - must be set after initialization
    mRandomSeed = PtclSeed{emitterData.randomSeed};
    mBillboardType = emitterData.billboardType;
    mIsPolygon = emitterData.isPolygon;
    mIsVelLook = emitterData.isVelLook;
    mIsEmitterBillboardMtx = emitterData.isEmitterBillboardMtx;
    mIsFollow = emitterData.isFollow;

    // Gravity Properties
    mIsDirectional = emitterData.isDirectional;
    mGravity = Math::Vector3f(emitterData.gravity.x, emitterData.gravity.y, emitterData.gravity.z);

    // Lifespan Properties
    mPtclLife = emitterData.ptclLife;
    mPtclLifeRnd = emitterData.ptclLifeRnd;

    // Transform Properties
    mTransformSRT = emitterData.transformSRT.toMatrix34f();
    mTransformRT = emitterData.transformRT.toMatrix34f();

    // Termination Properties
    mIsStopEmitInFade = emitterData.isStopEmitInFade;
    mAlphaAddInFade = emitterData.alphaAddInFade;

    // Emission Properties
    mEmitStartFrame = emitterData.startFrame;
    mEmitEndFrame = emitterData.endFrame;
    mLifeStep = emitterData.lifeStep;
    mLifeStepRnd = emitterData.lifeStepRnd;
    mEmitRate = emitterData.emitRate;

    // Velocity Properties
    mFigureVelocity = emitterData.figureVel;
    mVelocityDir = Math::Vector3f(emitterData.emitterVelDir.x, emitterData.emitterVelDir.y, emitterData.emitterVelDir.z);
    mInitVelocity = emitterData.initVel;
    mInitVelocityRnd = emitterData.initVelRnd;
    mSpreadVec = Math::Vector3f(emitterData.spreadVec.x, emitterData.spreadVec.y, emitterData.spreadVec.z);
    mAirResistance = emitterData.airResistance;

    // Volume Properties
    mVolumeTblIndex = emitterData.volumeTblIndex;
    mVolumeType = emitterData.volumeType;
    mVolumeRadius = Math::Vector3f(emitterData.volumeRadius.x, emitterData.volumeRadius.y, emitterData.volumeRadius.z);
    mVolumeSweepStart = emitterData.volumeSweepStart;
    mVolumeSweepParam = emitterData.volumeSweepParam;

    // Color Properties
    mColor0 = emitterData.color0;
    mColorSection1 = emitterData.colorSection1;
    mColorSection2 = emitterData.colorSection2;
    mColorSection3 = emitterData.colorSection3;
    mColorNumRepeat = emitterData.colorNumRepeat;
    mColorCalcType = static_cast<ColorCalcType>(emitterData.rotCalcType / 5);
    mColor1 = emitterData.color1;

    // Alpha Properties
    mAlphaAnim = {
        .initAlpha = emitterData.initAlpha,
        .diffAlpha21 = emitterData.diffAlpha21,
        .diffAlpha32 = emitterData.diffAlpha32,
        .alphaSection1 = emitterData.alphaSection1,
        .alphaSection2 = emitterData.alphaSection2,
    };

    // Scale Properties
    mScaleAnim = {
        .initScale = Math::Vector2f(emitterData.initScale.x, emitterData.initScale.y),
        .diffScale21 = Math::Vector2f(emitterData.diffScale21.x, emitterData.diffScale21.y),
        .diffScale32 = Math::Vector2f(emitterData.diffScale32.x, emitterData.diffScale32.y),
        .scaleSection1 = emitterData.scaleSection1,
        .scaleSection2 = emitterData.scaleSection2,
    };

    mScaleRand = emitterData.scaleRand;

    // Rotation Properties
    mRotType = static_cast<RotType>(emitterData.rotCalcType % 5);
    mInitRot = Math::Vector3i(emitterData.initRot.x, emitterData.initRot.y, emitterData.initRot.z);
    mInitRotRand = Math::Vector3i(emitterData.initRotRand.x, emitterData.initRotRand.y, emitterData.initRotRand.z);
    mRotVel = Math::Vector3i(emitterData.rotVel.x, emitterData.rotVel.y, emitterData.rotVel.z);
    mRotVelRand = Math::Vector3i(emitterData.rotVelRand.x, emitterData.rotVelRand.y, emitterData.rotVelRand.z);
    mRotBasis = Math::Vector2f(emitterData.rotBasis.x, emitterData.rotBasis.y);

    // Combiner Properties
    mBlendFunc = emitterData.blendFunc;
    mDepthFunc = emitterData.depthFunc;
    mCombinerFunc = emitterData.colorCombinerFunc;
}

void Emitter::initComplexFromBinary(const BinComplexEmitterData& emitterData) {
    mChildFlags = emitterData.childFlag,
    mFieldFlags = emitterData.fieldFlag,
    mFluctuationFlags = emitterData.fluctuationFlag;
    mStripeFlags = emitterData.stripeFlag;
}


// ========================================================================== //


} // namespace Ptcl
