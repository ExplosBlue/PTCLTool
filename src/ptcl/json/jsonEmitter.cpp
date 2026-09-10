#include "ptcl/json/jsonEmitter.h"

#include "ptcl/json/jsonCommon.h"
#include "ptcl/json/jsonTexture.h"
#include "util/fileUtil.h"

#include <algorithm>


namespace Ptcl::Json {


// ========================================================================== //


QJsonValue textureRefToJson(const TextureHandle& handle, bool embedTextures, const TextureIndexMap* textureMap) {
    if (!handle.isValid()) {
        return -1;
    }
    if (embedTextures) {
        return textureToJson(*handle.get());
    }
    return static_cast<s64>(textureMap->at(handle.get()));
}

QJsonObject childToJson(const Emitter& emitter, bool embedTextures, const TextureIndexMap* textureMap) {
    QJsonObject json{};
    json.insert("billboardType", static_cast<s64>(emitter.childBillboardType()));

    json.insert("emitRate",   emitter.childEmitRate());
    json.insert("emitTiming", emitter.childEmitTiming());
    json.insert("life",       emitter.childLife());
    json.insert("emitStep",   emitter.childEmitStep());

    json.insert("randVelocity",        vec3fToJson(emitter.childRandVelocity()));
    json.insert("gravity",             vec3fToJson(emitter.childGravity()));
    json.insert("velocityInheritRate", floatToJson(emitter.childVelocityInheritRate()));
    json.insert("initialPositionRand", floatToJson(emitter.childInitalPositionRand()));
    json.insert("figureVelocity",      floatToJson(emitter.childFigureVelocity()));
    json.insert("airResist",           floatToJson(emitter.childAirResistance()));

    json.insert("rotationType",           static_cast<s64>(emitter.childRotationType()));
    json.insert("initialRotation",        vec3iToJson(emitter.childInitialRotation()));
    json.insert("initialRotationRandom",  vec3iToJson(emitter.childInitialRotationRandom()));
    json.insert("rotationVelocity",       vec3iToJson(emitter.childRotationVelocity()));
    json.insert("rotationVelocityRandom", vec3iToJson(emitter.childRotationVelocityRandom()));
    json.insert("rotationBasis",          vec2fToJson(emitter.childRotationBasis()));

    json.insert("scale",            vec2fToJson(emitter.childScale()));
    json.insert("scaleTarget",      vec2fToJson(emitter.childScaleTarget()));
    json.insert("scaleInheritRate", floatToJson(emitter.childScaleInheritRate()));
    json.insert("scaleStartFrame",  emitter.childScaleStartFrame());

    json.insert("textureWrapT",    static_cast<s64>(emitter.childTextureWrapT()));
    json.insert("textureWrapS",    static_cast<s64>(emitter.childTextureWrapS()));
    json.insert("textureLodLevel", emitter.childTextureLodLevel());
    json.insert("textureFilter",   static_cast<s64>(emitter.childTextureFilter()));
    json.insert("textureUVScale",  vec2fToJson(emitter.childTextureUVScale()));
    json.insert("texture",         textureRefToJson(emitter.childTextureHandle(), embedTextures, textureMap));

    json.insert("color0", colorToJson(emitter.childSecondaryColor()));
    json.insert("color1", colorToJson(emitter.childPrimaryColor()));

    json.insert("alpha",           floatToJson(emitter.childAlpha()));
    json.insert("alphaTarget",     floatToJson(emitter.childAlphaTarget()));
    json.insert("alphaInit",       floatToJson(emitter.childAlphaInit()));
    json.insert("alphaStartFrame", emitter.childAlphaStartFrame());
    json.insert("alphaBaseFrame",  emitter.childAlphaBaseFrame());

    json.insert("blendFunc",    static_cast<s64>(emitter.childBlendFunc()));
    json.insert("depthFunc",    static_cast<s64>(emitter.childDepthFunc()));
    json.insert("combinerFunc", static_cast<s64>(emitter.childCombinerFunc()));

    return json;
}

QJsonObject complexToJson(const Emitter& emitter, bool embedTextures, const TextureIndexMap* textureMap) {
    QJsonObject json{};
    json.insert("fluctuationScale",     floatToJson(emitter.fluctuationScale()));
    json.insert("fluctuationFreq",      emitter.fluctuationFrequency());
    json.insert("fluctuationPhaseRand", emitter.isFluctuationPhaseRandom());
    json.insert("fluctuationFlags",     emitter.fluctuationFlags().value());

    json.insert("stripeType",           static_cast<s64>(emitter.stripeType()));
    json.insert("stripeNumHistory",     emitter.stripeNumHistory());
    json.insert("stripeStartAlpha",     floatToJson(emitter.stripeStartAlpha()));
    json.insert("stripeEndAlpha",       floatToJson(emitter.stripeEndAlpha()));
    json.insert("stripeUVScrollSpeed",  vec2fToJson(emitter.stripeUVScrollSpeed()));
    json.insert("stripeHistoryStep",    emitter.stripeHistoryStep());
    json.insert("stripeDirInterpolate", floatToJson(emitter.stripeDirInterpolate()));
    json.insert("stripeFlags",          emitter.stripeFlags().value());

    json.insert("fieldRandomBlank",  emitter.fieldRandomBlank());
    json.insert("fieldRandomVelAdd", vec3fToJson(emitter.fieldRandomVelAdd()));

    json.insert("fieldMagnetPower", floatToJson(emitter.fieldMagnetPower()));
    json.insert("fieldMagnetPos",   vec3fToJson(emitter.fieldMagnetPos()));
    json.insert("fieldMagnetFlag",  static_cast<s64>(emitter.fieldMagnetFlag().value()));

    json.insert("fieldSpinRotate", emitter.fieldSpinRotate());
    json.insert("fieldSpinAxis",   static_cast<s64>(emitter.fieldSpinAxis()));

    json.insert("fieldCollisionType",    static_cast<s64>(emitter.fieldCollisionType()));
    json.insert("fieldCollisionIsWorld", emitter.fieldCollisionIsWorld());
    json.insert("fieldCollisionCoord",   floatToJson(emitter.fieldCollisionCoord()));
    json.insert("fieldCollisionCoef",    floatToJson(emitter.fieldCollisionCoef()));

    json.insert("fieldConvergencePos",  vec3fToJson(emitter.fieldConvergencePos()));
    json.insert("fieldConvergenceRate", floatToJson(emitter.fieldConvergenceRate()));

    json.insert("fieldPosAddPosition", vec3fToJson(emitter.fieldPosAddPosition()));

    json.insert("fieldFlags", emitter.fieldFlags().value());

    json.insert("child", childToJson(emitter, embedTextures, textureMap));

    json.insert("childFlags", emitter.childFlags().value());

    return json;
}

QJsonObject emitterToJson(const Emitter& emitter, bool embedTextures, const TextureIndexMap* textureMap) {
    QJsonObject json{};
    json.insert("metaInfo", createMetaInfo(FileKind::Emitter, 1));

    json.insert("type",                  static_cast<s64>(emitter.type()));
    json.insert("flag",                  static_cast<s64>(emitter.flags().value()));
    json.insert("followType",            static_cast<s64>(emitter.followType()));
    json.insert("name",                  emitter.name());
    json.insert("randomSeed",            static_cast<s64>(emitter.randomSeed().raw()));
    json.insert("billboardType",         static_cast<s64>(emitter.billboardType()));
    json.insert("isPolygon",             emitter.isPolygon());
    json.insert("isVelLook",             emitter.isVelLook());
    json.insert("isEmitterBillboardMtx", emitter.isEmitterBillboardMtx());
    json.insert("isFollow",              emitter.isFollow());

    json.insert("isDirectional", emitter.isDirectional());
    json.insert("gravity",       vec3fToJson(emitter.gravity()));

    json.insert("ptclLife",       emitter.ptclLife());
    json.insert("ptclLifeRandom", emitter.ptclLifeRandom());

    json.insert("isStopEmitInFade", emitter.isStopEmitInFade());
    json.insert("alphaAddInFade",   floatToJson(emitter.alphaAddInFade()));

    json.insert("transformRT",  matrixToJson(emitter.transformRT()));
    json.insert("transformSRT", matrixToJson(emitter.transformSRT()));

    json.insert("scaleAnim", scaleAnimToJson(emitter.scaleAnim()));
    json.insert("scaleRand", floatToJson(emitter.scaleRand()));

    json.insert("emitStartFrame", emitter.emitStartFrame());
    json.insert("emitEndFrame",   emitter.emitEndFrame());
    json.insert("lifeStep",       emitter.lifeStep());
    json.insert("lifeStepRandom", emitter.lifeStepRandom());
    json.insert("emitRate",       emitter.emitRate());

    json.insert("figureVelocity",     floatToJson(emitter.figureVelocity()));
    json.insert("velocityDir",        vec3fToJson(emitter.velocityDirection()));
    json.insert("initVelocity",       floatToJson(emitter.initialVelocity()));
    json.insert("initVelocityRandom", floatToJson(emitter.initialVelocityRandom()));
    json.insert("spreadVec",          vec3fToJson(emitter.spreadVector()));
    json.insert("airResist",          floatToJson(emitter.airResistance()));

    json.insert("volumeTblIndex", emitter.volumeTblIndex());
    json.insert("volumeType",     static_cast<s64>(emitter.volumeType()));
    json.insert("volumeRadius",   vec3fToJson(emitter.volumeRadius()));
    json.insert("volumeArcWidth", emitter.volumeArcWidth());
    json.insert("volumeArcStart", emitter.volumeArcStart());

    json.insert("rotType",       static_cast<s64>(emitter.rotationType()));
    json.insert("initRot",       vec3iToJson(emitter.initialRotation()));
    json.insert("initRotRandom", vec3iToJson(emitter.initialRotationRandom()));
    json.insert("rotVel",        vec3iToJson(emitter.rotationVelocity()));
    json.insert("rotVelRandom",  vec3iToJson(emitter.rotationVelocityRandom()));
    json.insert("rotBasis",      vec2fToJson(emitter.rotationBasis()));

    json.insert("alphaAnim", alphaAnimToJson(emitter.alphaAnim()));

    json.insert("blendFunc",    static_cast<s64>(emitter.blendFunction()));
    json.insert("depthFunc",    static_cast<s64>(emitter.depthFunction()));
    json.insert("combinerFunc", static_cast<s64>(emitter.combinerFunction()));

    {
        QJsonArray color0Json{};
        for (auto& color : emitter.color0()) {
            color0Json.append(colorToJson(color));
        }
        json.insert("color0", color0Json);
    }

    json.insert("colorSection1",  emitter.colorSection1());
    json.insert("colorSection2",  emitter.colorSection2());
    json.insert("colorSection3",  emitter.colorSection3());
    json.insert("colorNumRepeat", emitter.colorNumRepeat());
    json.insert("colorCalcType",  static_cast<s64>(emitter.colorCalcType()));

    json.insert("color1", colorToJson(emitter.primaryColor()));

    json.insert("textureWrapT",        static_cast<s64>(emitter.textureWrapT()));
    json.insert("textureWrapS",        static_cast<s64>(emitter.textureWrapS()));
    json.insert("textureLodLevel",     emitter.textureLodLevel());
    json.insert("textureFilter",       static_cast<s64>(emitter.textureFilter()));
    json.insert("numTexturePattern",   emitter.numTexturePattern());
    json.insert("numTextureDivisionX", emitter.numTextureDivisionX());
    json.insert("numTextureDivisionY", emitter.numTextureDivisionY());
    json.insert("textureUVScale",      vec2fToJson(emitter.textureUVScale()));
    {
        QJsonArray patternTableJson{};
        for (auto& val : emitter.texturePatternTable()) {
            patternTableJson.append(val);
        }
        json.insert("texturePatternTable", patternTableJson);
    }
    json.insert("texturePatFreq",       emitter.texturePatternFrequency());
    json.insert("texturePatFrameCount", emitter.texturePatternFrameCount());
    json.insert("isTexPatAnim",         emitter.isTexturePatternAnim());
    json.insert("texture",              textureRefToJson(emitter.textureHandle(), embedTextures, textureMap));

    json.insert("complex", complexToJson(emitter, embedTextures, textureMap));

    return json;
}

Texture* textureRefFromJson(const QJsonValue& textureValue, const TextureList& textures) {
    if (textureValue.isObject() || !textureValue.isDouble()) {
        return nullptr;
    }

    const s32 texIdx = textureValue.toInt();
    if (texIdx < 0 || texIdx >= static_cast<s32>(textures.size()) || !textures[texIdx]) {
        return nullptr;
    }

    return textures[texIdx].get();
}

bool childFromJson(Emitter& emitter, const QJsonObject& json, const TextureList& textures) {
    const auto billboardType = json["billboardType"];

    const auto emitRate   = json["emitRate"];
    const auto emitTiming = json["emitTiming"];
    const auto life       = json["life"];
    const auto emitStep   = json["emitStep"];

    const auto randVelocity        = json["randVelocity"];
    const auto gravity             = json["gravity"];
    const auto velocityInheritRate = json["velocityInheritRate"];
    const auto initialPositionRand = json["initialPositionRand"];
    const auto figureVelocity      = json["figureVelocity"];
    const auto airResist           = json["airResist"];

    const auto rotationType           = json["rotationType"];
    const auto initialRotation        = json["initialRotation"];
    const auto initialRotationRandom  = json["initialRotationRandom"];
    const auto rotationVelocity       = json["rotationVelocity"];
    const auto rotationVelocityRandom = json["rotationVelocityRandom"];
    const auto rotationBasis          = json["rotationBasis"];

    const auto scale            = json["scale"];
    const auto scaleTarget      = json["scaleTarget"];
    const auto scaleInheritRate = json["scaleInheritRate"];
    const auto scaleStartFrame  = json["scaleStartFrame"];

    const auto textureWrapT    = json["textureWrapT"];
    const auto textureWrapS    = json["textureWrapS"];
    const auto textureLodLevel = json["textureLodLevel"];
    const auto textureFilter   = json["textureFilter"];
    const auto textureUVScale  = json["textureUVScale"];
    const auto texture         = json["texture"];

    const auto color0 = json["color0"];
    const auto color1 = json["color1"];

    const auto alpha           = json["alpha"];
    const auto alphaTarget     = json["alphaTarget"];
    const auto alphaInit       = json["alphaInit"];
    const auto alphaStartFrame = json["alphaStartFrame"];
    const auto alphaBaseFrame  = json["alphaBaseFrame"];

    const auto blendFunc    = json["blendFunc"];
    const auto depthFunc    = json["depthFunc"];
    const auto combinerFunc = json["combinerFunc"];

    // Validate
    if (!billboardType.isDouble()) {
        return false;
    }
    if (!emitRate.isDouble() || !emitTiming.isDouble() || !life.isDouble() || !emitStep.isDouble()) {
        return false;
    }
    if (!randVelocity.isObject() || !gravity.isObject()) {
        return false;
    }
    if (!isNumberValue(velocityInheritRate) || !isNumberValue(initialPositionRand) || !isNumberValue(figureVelocity) || !isNumberValue(airResist)) {
        return false;
    }
    if (!rotationType.isDouble()) {
        return false;
    }
    if (!initialRotation.isObject() || !initialRotationRandom.isObject() || !rotationVelocity.isObject() || !rotationVelocityRandom.isObject() || !rotationBasis.isObject()) {
        return false;
    }
    if (!scale.isObject() || !scaleTarget.isObject()) {
        return false;
    }
    if (!isNumberValue(scaleInheritRate)) {
        return false;
    }
    if (!scaleStartFrame.isDouble()) {
        return false;
    }
    if (!textureWrapT.isDouble() || !textureWrapS.isDouble()) {
        return false;
    }
    if (!textureLodLevel.isDouble() || !textureFilter.isDouble()) {
        return false;
    }
    if (!textureUVScale.isObject()) {
        return false;
    }
    if (!texture.isDouble() && !texture.isObject()) {
        return false;
    }
    if (!color0.isObject() || !color1.isObject()) {
        return false;
    }
    if (!isNumberValue(alpha) || !isNumberValue(alphaTarget) || !isNumberValue(alphaInit)) {
        return false;
    }
    if (!alphaStartFrame.isDouble() || !alphaBaseFrame.isDouble()) {
        return false;
    }
    if (!blendFunc.isDouble() || !depthFunc.isDouble() || !combinerFunc.isDouble()) {
        return false;
    }

    // Apply
    emitter.setChildBillboardType(static_cast<BillboardType>(billboardType.toInteger()));

    emitter.setChildEmitRate(emitRate.toInt());
    emitter.setChildEmitTiming(emitTiming.toInt());
    emitter.setChildLife(life.toInt());
    emitter.setChildEmitStep(emitStep.toInt());

    emitter.setChildRandVelocity(jsonToVec3f(randVelocity.toObject()));
    emitter.setChildGravity(jsonToVec3f(gravity.toObject()));
    emitter.setChildVelocityInheritRate(jsonToFloat(velocityInheritRate));
    emitter.setChildInitialPositionRand(jsonToFloat(initialPositionRand));
    emitter.setChildFigureVelocity(jsonToFloat(figureVelocity));
    emitter.setChildAirResistance(jsonToFloat(airResist));

    emitter.setChildRotationType(static_cast<RotType>(rotationType.toInteger()));
    emitter.setChildInitialRotation(jsonToVec3i(initialRotation.toObject()));
    emitter.setChildInitialRotationRandom(jsonToVec3i(initialRotationRandom.toObject()));
    emitter.setChildRotationVelocity(jsonToVec3i(rotationVelocity.toObject()));
    emitter.setChildRotationVelocityRandom(jsonToVec3i(rotationVelocityRandom.toObject()));
    emitter.setChildRotationBasis(jsonToVec2f(rotationBasis.toObject()));

    emitter.setChildScale(jsonToVec2f(scale.toObject()));
    emitter.setChildScaleTarget(jsonToVec2f(scaleTarget.toObject()));
    emitter.setChildScaleInheritRate(jsonToFloat(scaleInheritRate));
    emitter.setChildScaleStartFrame(scaleStartFrame.toInt());

    emitter.setChildTextureWrapT(static_cast<TextureWrap>(textureWrapT.toInteger()));
    emitter.setChildTextureWrapS(static_cast<TextureWrap>(textureWrapS.toInteger()));
    emitter.setChildTextureLodLevel(static_cast<u8>(textureLodLevel.toInt()));
    emitter.setChildTextureFilter(static_cast<TextureFilter>(textureFilter.toInteger()));
    emitter.setChildTextureUVScale(jsonToVec2f(textureUVScale.toObject()));

    if (auto* tex = textureRefFromJson(texture, textures)) {
        emitter.setChildTexture(tex);
    }

    emitter.setChildSecondaryColor(jsonToColor(color0.toObject()));
    emitter.setChildPrimaryColor(jsonToColor(color1.toObject()));

    emitter.setChildAlpha(jsonToFloat(alpha));
    emitter.setChildAlphaTarget(jsonToFloat(alphaTarget));
    emitter.setChildAlphaInit(jsonToFloat(alphaInit));
    emitter.setChildAlphaStartFrame(alphaStartFrame.toInt());
    emitter.setChildAlphaBaseFrame(alphaBaseFrame.toInt());

    emitter.setChildBlendFunc(static_cast<BlendFuncType>(blendFunc.toInteger()));
    emitter.setChildDepthFunc(static_cast<DepthFuncType>(depthFunc.toInteger()));
    emitter.setChildCombinerFunc(static_cast<ColorCombinerFuncType>(combinerFunc.toInteger()));

    return true;
}

bool complexFromJson(Emitter& emitter, const QJsonObject& json, const TextureList& textures) {
    const auto fluctuationScale     = json["fluctuationScale"];
    const auto fluctuationFreq      = json["fluctuationFreq"];
    const auto fluctuationPhaseRand = json["fluctuationPhaseRand"];
    const auto fluctuationFlags     = json["fluctuationFlags"];

    const auto stripeType           = json["stripeType"];
    const auto stripeNumHistory     = json["stripeNumHistory"];
    const auto stripeStartAlpha     = json["stripeStartAlpha"];
    const auto stripeEndAlpha       = json["stripeEndAlpha"];
    const auto stripeUVScrollSpeed  = json["stripeUVScrollSpeed"];
    const auto stripeHistoryStep    = json["stripeHistoryStep"];
    const auto stripeDirInterpolate = json["stripeDirInterpolate"];
    const auto stripeFlags          = json["stripeFlags"];

    const auto fieldRandomBlank  = json["fieldRandomBlank"];
    const auto fieldRandomVelAdd = json["fieldRandomVelAdd"];

    const auto fieldMagnetPower = json["fieldMagnetPower"];
    const auto fieldMagnetPos   = json["fieldMagnetPos"];
    const auto fieldMagnetFlag  = json["fieldMagnetFlag"];

    const auto fieldSpinRotate = json["fieldSpinRotate"];
    const auto fieldSpinAxis   = json["fieldSpinAxis"];

    const auto fieldCollisionType    = json["fieldCollisionType"];
    const auto fieldCollisionIsWorld = json["fieldCollisionIsWorld"];
    const auto fieldCollisionCoord   = json["fieldCollisionCoord"];
    const auto fieldCollisionCoef    = json["fieldCollisionCoef"];

    const auto fieldConvergencePos  = json["fieldConvergencePos"];
    const auto fieldConvergenceRate = json["fieldConvergenceRate"];

    const auto fieldPosAddPosition = json["fieldPosAddPosition"];

    const auto fieldFlags = json["fieldFlags"];

    const auto child      = json["child"];
    const auto childFlags = json["childFlags"];

    // Validate
    if (!isNumberValue(fluctuationScale) || !isNumberValue(fluctuationFreq)) {
        return false;
    }
    if (!fluctuationPhaseRand.isBool()) {
        return false;
    }
    if (!fluctuationFlags.isDouble()) {
        return false;
    }
    if (!stripeType.isDouble()) {
        return false;
    }
    if (!stripeNumHistory.isDouble()) {
        return false;
    }
    if (!isNumberValue(stripeStartAlpha) || !isNumberValue(stripeEndAlpha)) {
        return false;
    }
    if (!stripeUVScrollSpeed.isObject()) {
        return false;
    }
    if (!stripeHistoryStep.isDouble()) {
        return false;
    }
    if (!isNumberValue(stripeDirInterpolate)) {
        return false;
    }
    if (!stripeFlags.isDouble()) {
        return false;
    }
    if (!fieldRandomBlank.isDouble()) {
        return false;
    }
    if (!fieldRandomVelAdd.isObject()) {
        return false;
    }
    if (!isNumberValue(fieldMagnetPower)) {
        return false;
    }
    if (!fieldMagnetPos.isObject()) {
        return false;
    }
    if (!fieldMagnetFlag.isDouble()) {
        return false;
    }
    if (!fieldSpinRotate.isDouble() || !fieldSpinAxis.isDouble()) {
        return false;
    }
    if (!fieldCollisionType.isDouble()) {
        return false;
    }
    if (!fieldCollisionIsWorld.isBool()) {
        return false;
    }
    if (!isNumberValue(fieldCollisionCoord) || !isNumberValue(fieldCollisionCoef)) {
        return false;
    }
    if (!fieldConvergencePos.isObject()) {
        return false;
    }
    if (!fieldPosAddPosition.isObject()) {
        return false;
    }
    if (!fieldFlags.isDouble()) {
        return false;
    }
    if (!child.isObject()) {
        return false;
    }
    if (!childFlags.isDouble()) {
        return false;
    }

    // Apply
    emitter.setFluctuationScale(jsonToFloat(fluctuationScale));
    emitter.setFluctuationFrequency(jsonToFloat(fluctuationFreq));
    emitter.setFluctuationPhaseRandom(fluctuationPhaseRand.toBool());
    emitter.setFluctuationFlags(BitFlag<FluctuationFlag>(static_cast<u16>(fluctuationFlags.toInt())));

    emitter.setStripeType(static_cast<StripeType>(stripeType.toInteger()));
    emitter.setStripeNumHistory(stripeNumHistory.toInt());
    emitter.setStripeStartAlpha(jsonToFloat(stripeStartAlpha));
    emitter.setStripeEndAlpha(jsonToFloat(stripeEndAlpha));
    emitter.setStripeUVScrollSpeed(jsonToVec2f(stripeUVScrollSpeed.toObject()));
    emitter.setStripeHistoryStep(stripeHistoryStep.toInt());
    emitter.setStripeDirInterpolate(jsonToFloat(stripeDirInterpolate));
    emitter.setStripeFlags(BitFlag<StripeFlag>(static_cast<u16>(stripeFlags.toInt())));

    emitter.setFieldRandomBlank(fieldRandomBlank.toInt());
    emitter.setFieldRandomVelAdd(jsonToVec3f(fieldRandomVelAdd.toObject()));

    emitter.setFieldMagnetPower(jsonToFloat(fieldMagnetPower));
    emitter.setFieldMagnetPos(jsonToVec3f(fieldMagnetPos.toObject()));
    {
        const BitFlag<FieldMagnetFlag> magnetFlags{static_cast<u32>(fieldMagnetFlag.toInteger())};
        emitter.setFieldMagnetAxisTargetX(magnetFlags.isSet(FieldMagnetFlag::AxisTargetX));
        emitter.setFieldMagnetAxisTargetY(magnetFlags.isSet(FieldMagnetFlag::AxisTargetY));
        emitter.setFieldMagnetAxisTargetZ(magnetFlags.isSet(FieldMagnetFlag::AxisTargetZ));
    }

    emitter.setFieldSpinRotate(fieldSpinRotate.toInt());
    emitter.setFieldSpinAxis(static_cast<FieldSpinAxis>(fieldSpinAxis.toInteger()));

    emitter.setFieldCollisionType(static_cast<FieldCollisionType>(fieldCollisionType.toInteger()));
    emitter.setFieldCollisionIsWorld(fieldCollisionIsWorld.toBool());
    emitter.setFieldCollisionCoord(jsonToFloat(fieldCollisionCoord));
    emitter.setFieldCollisionCoef(jsonToFloat(fieldCollisionCoef));

    emitter.setFieldConvergencePos(jsonToVec3f(fieldConvergencePos.toObject()));
    emitter.setFieldConvergenceRate(jsonToFloat(fieldConvergenceRate));

    emitter.setFieldPosAddPosition(jsonToVec3f(fieldPosAddPosition.toObject()));

    emitter.setFieldFlags(BitFlag<FieldFlag>(static_cast<u16>(fieldFlags.toInt())));

    if (!childFromJson(emitter, child.toObject(), textures)) {
        return false;
    }

    emitter.setChildFlags(BitFlag<ChildFlag>(static_cast<u16>(childFlags.toInt())));

    return true;
}

std::optional<Emitter> emitterFromJson(const QJsonObject& json, const TextureList& textures) {
    if (!validateMetaInfo(json["metaInfo"].toObject(), FileKind::Emitter, 1)) {
        return std::nullopt;
    }

    const auto type          = json["type"];
    const auto flag          = json["flag"];
    const auto followType    = json["followType"];
    const auto name          = json["name"];
    const auto randomSeed    = json["randomSeed"];
    const auto billboardType = json["billboardType"];

    const auto isDirectional = json["isDirectional"];
    const auto gravity       = json["gravity"];

    const auto ptclLife       = json["ptclLife"];
    const auto ptclLifeRandom = json["ptclLifeRandom"];

    const auto isStopEmitInFade = json["isStopEmitInFade"];
    const auto alphaAddInFade   = json["alphaAddInFade"];

    const auto transformRT  = json["transformRT"];
    const auto transformSRT = json["transformSRT"];

    const auto scaleAnim = json["scaleAnim"];
    const auto scaleRand = json["scaleRand"];

    const auto emitStartFrame = json["emitStartFrame"];
    const auto emitEndFrame   = json["emitEndFrame"];
    const auto lifeStep       = json["lifeStep"];
    const auto lifeStepRandom = json["lifeStepRandom"];
    const auto emitRate       = json["emitRate"];

    const auto figureVelocity     = json["figureVelocity"];
    const auto velocityDir        = json["velocityDir"];
    const auto initVelocity       = json["initVelocity"];
    const auto initVelocityRandom = json["initVelocityRandom"];
    const auto spreadVec          = json["spreadVec"];
    const auto airResist          = json["airResist"];

    const auto volumeTblIndex = json["volumeTblIndex"];
    const auto volumeType     = json["volumeType"];
    const auto volumeRadius   = json["volumeRadius"];
    const auto volumeArcWidth = json["volumeArcWidth"];
    const auto volumeArcStart = json["volumeArcStart"];

    const auto rotType       = json["rotType"];
    const auto initRot       = json["initRot"];
    const auto initRotRandom = json["initRotRandom"];
    const auto rotVel        = json["rotVel"];
    const auto rotVelRandom  = json["rotVelRandom"];
    const auto rotBasis      = json["rotBasis"];

    const auto alphaAnim = json["alphaAnim"];

    const auto blendFunc    = json["blendFunc"];
    const auto depthFunc    = json["depthFunc"];
    const auto combinerFunc = json["combinerFunc"];

    const auto color0         = json["color0"];
    const auto colorSection1  = json["colorSection1"];
    const auto colorSection2  = json["colorSection2"];
    const auto colorSection3  = json["colorSection3"];
    const auto colorNumRepeat = json["colorNumRepeat"];
    const auto colorCalcType  = json["colorCalcType"];
    const auto color1         = json["color1"];

    const auto textureWrapT         = json["textureWrapT"];
    const auto textureWrapS         = json["textureWrapS"];
    const auto textureLodLevel      = json["textureLodLevel"];
    const auto textureFilter        = json["textureFilter"];
    const auto numTexturePattern    = json["numTexturePattern"];
    const auto numTextureDivisionX  = json["numTextureDivisionX"];
    const auto numTextureDivisionY  = json["numTextureDivisionY"];
    const auto textureUVScale       = json["textureUVScale"];
    const auto texturePatternTable  = json["texturePatternTable"];
    const auto texturePatFreq       = json["texturePatFreq"];
    const auto texturePatFrameCount = json["texturePatFrameCount"];
    const auto isTexPatAnim         = json["isTexPatAnim"];
    const auto texture              = json["texture"];

    const auto complex = json["complex"];

    // Validate
    if (!type.isDouble() || !flag.isDouble() || !followType.isDouble()) {
        return std::nullopt;
    }
    if (!name.isString()) {
        return std::nullopt;
    }
    if (!randomSeed.isDouble() || !billboardType.isDouble()) {
        return std::nullopt;
    }
    if (!isDirectional.isBool()) {
        return std::nullopt;
    }
    if (!gravity.isObject()) {
        return std::nullopt;
    }
    if (!ptclLife.isDouble() || !ptclLifeRandom.isDouble()) {
        return std::nullopt;
    }
    if (!isStopEmitInFade.isBool()) {
        return std::nullopt;
    }
    if (!isNumberValue(alphaAddInFade)) {
        return std::nullopt;
    }
    if (!transformRT.isArray() || !transformSRT.isArray()) {
        return std::nullopt;
    }
    if (!scaleAnim.isObject()) {
        return std::nullopt;
    }
    if (!isNumberValue(scaleRand)) {
        return std::nullopt;
    }
    if (!emitStartFrame.isDouble() || !emitEndFrame.isDouble() || !lifeStep.isDouble() || !lifeStepRandom.isDouble() || !emitRate.isDouble()) {
        return std::nullopt;
    }
    if (!isNumberValue(figureVelocity) || !isNumberValue(initVelocity) || !isNumberValue(initVelocityRandom) || !isNumberValue(airResist)) {
        return std::nullopt;
    }
    if (!velocityDir.isObject() || !spreadVec.isObject()) {
        return std::nullopt;
    }
    if (!volumeTblIndex.isDouble() || !volumeType.isDouble() || !volumeArcWidth.isDouble() || !volumeArcStart.isDouble()) {
        return std::nullopt;
    }
    if (!volumeRadius.isObject()) {
        return std::nullopt;
    }
    if (!rotType.isDouble()) {
        return std::nullopt;
    }
    if (!initRot.isObject() || !initRotRandom.isObject() || !rotVel.isObject() || !rotVelRandom.isObject() || !rotBasis.isObject()) {
        return std::nullopt;
    }
    if (!alphaAnim.isObject()) {
        return std::nullopt;
    }
    if (!blendFunc.isDouble() || !depthFunc.isDouble() || !combinerFunc.isDouble()) {
        return std::nullopt;
    }
    if (!color0.isArray()) {
        return std::nullopt;
    }
    if (!colorSection1.isDouble() || !colorSection2.isDouble() || !colorSection3.isDouble() || !colorNumRepeat.isDouble() || !colorCalcType.isDouble()) {
        return std::nullopt;
    }
    if (!color1.isObject()) {
        return std::nullopt;
    }
    if (!textureWrapT.isDouble() || !textureWrapS.isDouble() || !textureLodLevel.isDouble() || !textureFilter.isDouble()) {
        return std::nullopt;
    }
    if (!numTexturePattern.isDouble() || !numTextureDivisionX.isDouble() || !numTextureDivisionY.isDouble()) {
        return std::nullopt;
    }
    if (!textureUVScale.isObject()) {
        return std::nullopt;
    }
    if (!texturePatternTable.isArray()) {
        return std::nullopt;
    }
    if (!texturePatFreq.isDouble() || !texturePatFrameCount.isDouble() || !isTexPatAnim.isBool()) {
        return std::nullopt;
    }
    if (!texture.isDouble() && !texture.isObject()) {
        return std::nullopt;
    }
    if (!complex.isObject()) {
        return std::nullopt;
    }

    // Apply
    Emitter emitter{};

    // Basic Properties
    emitter.setType(static_cast<EmitterType>(type.toInteger()));
    emitter.flags() = BitFlag<EmitterFlag>(static_cast<u32>(flag.toInteger()));
    emitter.setFollowType(static_cast<FollowType>(followType.toInteger()));
    emitter.setName(name.toString());
    emitter.setRandomSeed(PtclSeed{static_cast<u32>(randomSeed.toInteger())});
    emitter.setBillboardType(static_cast<BillboardType>(billboardType.toInteger()));

    emitter.setDirectional(isDirectional.toBool());
    emitter.setGravity(jsonToVec3f(gravity.toObject()));

    emitter.setPtclLife(ptclLife.toInt());
    emitter.setPtclLifeRandom(ptclLifeRandom.toInt());

    emitter.setIsStopEmitInFade(isStopEmitInFade.toBool());
    emitter.setAlphaAddInFade(jsonToFloat(alphaAddInFade));

    {
        const auto rt = jsonToMatrix(transformRT.toArray());
        const auto srt = jsonToMatrix(transformSRT.toArray());
        emitter.setTransformFromMatrices(rt, srt);
    }

    emitter.setScaleAnim(scaleAnimFromJson(scaleAnim.toObject()));
    emitter.setScaleRand(jsonToFloat(scaleRand));

    emitter.setEmitStartFrame(emitStartFrame.toInt());
    emitter.setEmitEndFrame(emitEndFrame.toInt());
    emitter.setLifeStep(lifeStep.toInt());
    emitter.setLifeStepRandom(lifeStepRandom.toInt());
    emitter.setEmitRate(emitRate.toInt());

    emitter.setFigureVelocity(jsonToFloat(figureVelocity));
    emitter.setVelocityDirection(jsonToVec3f(velocityDir.toObject()));
    emitter.setInitialVelocity(jsonToFloat(initVelocity));
    emitter.setInitialVelocityRandom(jsonToFloat(initVelocityRandom));
    emitter.setSpreadVector(jsonToVec3f(spreadVec.toObject()));
    emitter.setAirResistance(jsonToFloat(airResist));

    emitter.setVolumeTblIndex(static_cast<u8>(volumeTblIndex.toInt()));
    emitter.setVolumeType(static_cast<VolumeType>(volumeType.toInteger()));
    emitter.setVolumeRadius(jsonToVec3f(volumeRadius.toObject()));
    emitter.setVolumeArcWidth(volumeArcWidth.toInt());
    emitter.setVolumeArcStart(volumeArcStart.toInt());

    emitter.setRotationType(static_cast<RotType>(rotType.toInteger()));
    emitter.setInitialRotation(jsonToVec3i(initRot.toObject()));
    emitter.setInitialRotationRandom(jsonToVec3i(initRotRandom.toObject()));
    emitter.setRotationVelocity(jsonToVec3i(rotVel.toObject()));
    emitter.setRotationVelocityRandom(jsonToVec3i(rotVelRandom.toObject()));
    emitter.setRotationBasis(jsonToVec2f(rotBasis.toObject()));

    emitter.setAlphaAnim(alphaAnimFromJson(alphaAnim.toObject()));

    emitter.setBlendFunction(static_cast<BlendFuncType>(blendFunc.toInteger()));
    emitter.setDepthFunction(static_cast<DepthFuncType>(depthFunc.toInteger()));
    emitter.setCombinerFunction(static_cast<ColorCombinerFuncType>(combinerFunc.toInteger()));

    {
        const QJsonArray color0Json = color0.toArray();
        for (s32 i = 0; i < 3 && i < color0Json.size(); ++i) {
            auto color = jsonToColor(color0Json[i].toObject());
            switch (i) {
            case 0: emitter.setStartColor(color); break;
            case 1: emitter.setMidColor(color); break;
            case 2: emitter.setEndColor(color); break;
            }
        }
    }

    emitter.setColorSection1(colorSection1.toInt());
    emitter.setColorSection2(colorSection2.toInt());
    emitter.setColorSection3(colorSection3.toInt());
    emitter.setColorNumRepeat(colorNumRepeat.toInt());
    emitter.setColorCalcType(static_cast<ColorCalcType>(colorCalcType.toInteger()));
    emitter.setPrimaryColor(jsonToColor(color1.toObject()));

    emitter.setTextureWrapT(static_cast<TextureWrap>(textureWrapT.toInteger()));
    emitter.setTextureWrapS(static_cast<TextureWrap>(textureWrapS.toInteger()));
    emitter.setTextureLodLevel(static_cast<u8>(textureLodLevel.toInt()));
    emitter.setTextureFilter(static_cast<TextureFilter>(textureFilter.toInteger()));
    emitter.setNumTexturePattern(static_cast<u16>(numTexturePattern.toInt()));
    emitter.setNumTextureDivisionX(static_cast<u8>(std::max(1, numTextureDivisionX.toInt())));
    emitter.setNumTextureDivisionY(static_cast<u8>(std::max(1, numTextureDivisionY.toInt())));
    emitter.setTextureUVScale(jsonToVec2f(textureUVScale.toObject()));
    {
        const QJsonArray patternTableJson = texturePatternTable.toArray();
        std::array<u8, 16> patternTable{};
        for (s32 i = 0; i < 16 && i < patternTableJson.size(); ++i) {
            patternTable[i] = static_cast<u8>(patternTableJson[i].toInt());
        }
        emitter.setTexturePatternTable(patternTable);
    }
    emitter.setTexturePatternFrequency(static_cast<u16>(texturePatFreq.toInt()));
    emitter.setTexturePatternFrameCount(static_cast<u16>(texturePatFrameCount.toInt()));
    emitter.setIsTexturePatternAnim(isTexPatAnim.toBool());

    if (auto* tex = textureRefFromJson(texture, textures)) {
        emitter.setTexture(tex);
    }

    if (!complexFromJson(emitter, complex.toObject(), textures)) {
        return std::nullopt;
    }

    return emitter;
}

bool exportEmitter(const Emitter& emitter, const QString& filePath) {
    return writeJsonFile(emitterToJson(emitter, true, nullptr), filePath);
}

static std::optional<Emitter> importEmitterFile(const QString& filePath, const TextureList& textures) {
    const auto readResult = readJsonFile(filePath);
    if (!readResult) {
        return std::nullopt;
    }
    return emitterFromJson(*readResult, textures);
}

static void validateImportedEmitter(Emitter& emitter, TextureList& textures, PtclSanitizeReport& report) {
    const QString emitterContext = emitter.name().isEmpty() ? QStringLiteral("Emitter") : QStringLiteral("Emitter '%1'").arg(emitter.name());
    report.setContext(emitterContext);

    emitter.validate(report);

    for (s32 i = 0; i < static_cast<s32>(textures.size()); ++i) {
        if (!textures[i]) {
            continue;
        }
        report.setContext(QStringLiteral("%1 / Texture %2").arg(emitterContext).arg(i));
        textures[i]->validate(report);
    }

    report.setContext({});
}

static std::optional<ImportEmitterResult> importStandaloneEmitter(const QJsonObject& json, const QJsonValue& texVal, const QJsonValue& childTexVal) {
    TextureList textures{};

    if (texVal.isObject()) {
        auto tex = textureFromJson(texVal.toObject());
        if (tex) {
            textures.push_back(std::make_unique<Texture>(std::move(*tex)));
        }
    }
    if (childTexVal.isObject()) {
        auto tex = textureFromJson(childTexVal.toObject());
        if (tex) {
            textures.push_back(std::make_unique<Texture>(std::move(*tex)));
        }
    }

    auto emitter = emitterFromJson(json, textures);
    if (!emitter) {
        return std::nullopt;
    }

    ImportEmitterResult result{std::make_unique<Emitter>(std::move(*emitter)), std::move(textures), {}};
    validateImportedEmitter(*result.emitter, result.textures, result.report);
    return result;
}

static std::optional<ImportEmitterResult> importLinkedEmitter(const QJsonObject& json, const QString& filePath, const QString& projectDir) {
    const QDir sourceProjectDir = sourceProjectDirFor(filePath, projectDir, 2);

    auto sourceTextures = importProjectTextures(sourceProjectDir);
    if (!sourceTextures) {
        return std::nullopt;
    }

    auto emitter = emitterFromJson(json, *sourceTextures);
    if (!emitter) {
        return std::nullopt;
    }

    TextureList resultTextures{};
    TextureRemap remap{};
    reIdEmitterTextures(*emitter, resultTextures, remap);

    ImportEmitterResult result{std::make_unique<Emitter>(std::move(*emitter)), std::move(resultTextures), {}};
    validateImportedEmitter(*result.emitter, result.textures, result.report);
    return result;
}

std::optional<ImportEmitterResult> importEmitter(const QString& filePath, const QString& projectDir) {
    const auto readResult = readJsonFile(filePath);
    if (!readResult) {
        return std::nullopt;
    }
    const auto& emitterJson = *readResult;

    if (!validateMetaInfo(emitterJson["metaInfo"].toObject(), FileKind::Emitter, 1)) {
        return std::nullopt;
    }

    const QJsonValue texVal = emitterJson["texture"];
    const QJsonObject complexJson = emitterJson["complex"].toObject();
    const QJsonObject childJson = complexJson["child"].toObject();
    const QJsonValue childTexVal = childJson["texture"];

    if (texVal.isObject() || childTexVal.isObject()) {
        return importStandaloneEmitter(emitterJson, texVal, childTexVal);
    }

    return importLinkedEmitter(emitterJson, filePath, projectDir);
}

static Texture* getOrCreateReIded(Texture* sourceTex, TextureList& resultTextures, TextureRemap& remap) {
    if (!sourceTex || sourceTex->isPlaceholder()) {
        return nullptr;
    }

    auto it = remap.find(sourceTex);
    if (it != remap.end()) {
        return it->second;
    }

    auto newTex = cloneTexture(*sourceTex);
    auto* ptr = newTex.get();
    remap[sourceTex] = ptr;
    resultTextures.push_back(std::move(newTex));
    return ptr;
}

void reIdEmitterTextures(Emitter& emitter, TextureList& resultTextures, TextureRemap& remap) {
    if (emitter.textureHandle().isValid()) {
        if (auto* newTex = getOrCreateReIded(emitter.texture(), resultTextures, remap)) {
            emitter.setTexture(newTex);
        }
    }
    if (emitter.childTextureHandle().isValid()) {
        if (auto* newTex = getOrCreateReIded(emitter.childTexture(), resultTextures, remap)) {
            emitter.setChildTexture(newTex);
        }
    }
}

std::optional<QString> exportEmitter(const Emitter& emitter, s32 idx, const QDir& dir, const TextureIndexMap& textureMap) {
    auto emitterJson = emitterToJson(emitter, false, &textureMap);

    auto emitterName = QString("emitter_%1_%2").arg(idx).arg(emitter.name());
    emitterName = FileUtil::ensureExtention(emitterName, FileKind::Emitter);

    if (!writeJsonFile(emitterJson, dir.filePath(emitterName))) {
        return std::nullopt;
    }

    return emitterName;
}

QJsonObject exportEmitters(const EmitterList& emitters, const QDir& dir, const TextureIndexMap& textureMap) {
    QJsonObject emitterSetListJson{};
    for (s32 idx = 0; idx < static_cast<s32>(emitters.size()); ++idx) {
        const auto emitterName = exportEmitter(*emitters.at(idx), idx, dir, textureMap);

        if (emitterName) {
            emitterSetListJson[QString::number(idx)] = dir.dirName() + "/" + *emitterName;
        }
    }
    return emitterSetListJson;
}

std::optional<EmitterList> importEmitters(const QJsonObject& emittersJson, const QDir& dir, const TextureList& textures) {
    EmitterList emitters{};
    emitters.resize(emittersJson.size());

    for (auto it = emittersJson.constBegin(); it != emittersJson.constEnd(); ++it) {
        bool ok{false};
        const size_t idx = it.key().toInt(&ok);
        if (!ok || idx >= emitters.size()) {
            return std::nullopt;
        }

        const QString emitterPath = dir.filePath(it.value().toString());
        auto emitter = importEmitterFile(emitterPath, textures);
        if (!emitter) {
            return std::nullopt;
        }

        emitters[idx] = std::make_unique<Emitter>(std::move(*emitter));
    }

    for (const auto& emitter : emitters) {
        if (!emitter) {
            return std::nullopt;
        }
    }

    return emitters;
}


// ========================================================================== //


} // namespace Ptcl::Json
