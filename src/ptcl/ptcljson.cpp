#include "ptcl/ptcljson.h"

#include <QDataStream>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>

namespace Ptcl {


// ========================================================================== //


enum class JsonFileType {
    ProjectFile    = 0,
    TextureFile    = 1,
    EmitterSetFile = 2,
    EmitterFile    = 3,
};

QJsonObject createMetaInfo(JsonFileType type, s32 version) {
    QJsonObject metaInfo{};
    metaInfo["fileType"] = static_cast<s32>(type);
    metaInfo["version"]  = version;
    return metaInfo;
}

QJsonObject vec3fToJson(const Math::Vector3f& vector) {
    QJsonObject vectorJson{};
    vectorJson["x"] = vector.getX();
    vectorJson["y"] = vector.getY();
    vectorJson["z"] = vector.getZ();
    return vectorJson;
}

QJsonObject vec3iToJson(const Math::Vector3i& vector) {
    QJsonObject vectorJson{};
    vectorJson["x"] = vector.getX();
    vectorJson["y"] = vector.getY();
    vectorJson["z"] = vector.getZ();
    return vectorJson;
}

QJsonObject vec2fToJson(const Math::Vector2f& vector) {
    QJsonObject vectorJson{};
    vectorJson["x"] = vector.getX();
    vectorJson["y"] = vector.getY();
    return vectorJson;
}

QJsonObject colorToJson(const Gfx::Color& color) {
    QJsonObject colorJson{};
    colorJson["r"] = color.r();
    colorJson["g"] = color.g();
    colorJson["b"] = color.b();
    colorJson["a"] = color.a();
    return colorJson;
}

QJsonArray matrixToJson(const Math::Matrix34f& matrix) {
    QJsonArray matrixJson{};

    for (s32 row = 0; row < 3; ++row) {
        QJsonArray rowJson{};

        for (s32 col = 0; col < 4; ++col) {
            rowJson.append(matrix(row, col));
        }

        matrixJson.append(rowJson);
    }

    return matrixJson;
}

QJsonObject scaleAnimToJson(const Emitter::ScaleAnim& anim) {
    QJsonObject animJson{};
    animJson["initScale"]     = vec2fToJson(anim.initScale);
    animJson["diffScale21"]   = vec2fToJson(anim.diffScale21);
    animJson["diffScale32"]   = vec2fToJson(anim.diffScale32);
    animJson["scaleSection1"] = anim.scaleSection1;
    animJson["scaleSection2"] = anim.scaleSection2;
    animJson["isFlatStart"]   = anim.isFlatStart;
    return animJson;
}

QJsonObject alphaAnimToJson(const Emitter::AlphaAnim& anim) {
    QJsonObject animJson{};
    animJson["initAlpha"]     = anim.initAlpha;
    animJson["diffAlpha21"]   = anim.diffAlpha21;
    animJson["diffAlpha32"]   = anim.diffAlpha32;
    animJson["alphaSection1"] = anim.alphaSection1;
    animJson["alphaSection2"] = anim.alphaSection2;
    animJson["isFlatStart"]   = anim.isFlatStart;
    return animJson;
}

std::optional<QString> exportTexture(const Texture& texture, s32 idx, const QDir& dir) {
    QJsonObject textureJson{};
    textureJson["metaInfo"] = createMetaInfo(JsonFileType::TextureFile, 1);

    textureJson["format"] = static_cast<s32>(texture.textureFormat());
    textureJson["width"]  = texture.textureData().width();
    textureJson["height"] = texture.textureData().height();

    QByteArray bytes(texture.textureDataRaw());
    QString base64 = QString::fromLatin1(bytes.toBase64());
    textureJson["data"] = base64;

    auto textureName = QString("tex_%1.ptex").arg(idx);

    QFile textureFile{dir.filePath(textureName)};
    if (!textureFile.open(QIODevice::WriteOnly)) {
        return std::nullopt;
    }

    textureFile.write(QJsonDocument(textureJson).toJson());
    return textureName;
}

QJsonObject exportTextures(const TextureList& textures, const QDir& dir) {
    QJsonObject texturesListJson{};
    for (s32 idx = 0; idx < static_cast<s32>(textures.size()); ++idx) {
        const auto textureName = exportTexture(*textures.at(idx), idx, dir);

        if (textureName) {
            texturesListJson[QString::number(idx)] = dir.dirName() + "/" + *textureName;
        }
    }
    return texturesListJson;
}

std::optional<QString> exportEmitter(const Emitter& emitter, s32 idx, const QDir& dir) {
    QJsonObject emitterJson{};
    emitterJson["metaInfo"] = createMetaInfo(JsonFileType::EmitterFile, 1);

    emitterJson["flag"] = static_cast<s64>(emitter.flags().value());
    emitterJson["followType"] = static_cast<s64>(emitter.followType());
    emitterJson["name"] = emitter.name();
    emitterJson["randomSeed"] = static_cast<s64>(emitter.randomSeed().raw());
    emitterJson["billboardType"] = static_cast<s64>(emitter.billboardType());
    emitterJson["isPolygon"] = emitter.isPolygon();
    emitterJson["isVelLook"] = emitter.isVelLook();
    emitterJson["isEmitterBillboardMtx"] = emitter.isEmitterBillboardMtx();
    emitterJson["isFollow"] = emitter.isFollow();

    emitterJson["isDirectional"] = emitter.isDirectional();
    emitterJson["gravity"] = vec3fToJson(emitter.gravity());

    emitterJson["ptclLife"] = emitter.ptclLife();
    emitterJson["ptclLifeRandom"] = emitter.ptclLifeRandom();

    emitterJson["isStopEmitInFade"] = emitter.isStopEmitInFade();
    emitterJson["alphaAddInFade"] = emitter.alphaAddInFade();

    emitterJson["transformRT"] = matrixToJson(emitter.transformRT());
    emitterJson["transformSRT"] = matrixToJson(emitter.transformSRT());

    emitterJson["scaleAnim"] = scaleAnimToJson(emitter.scaleAnim());
    emitterJson["scaleRand"] = emitter.scaleRand();

    emitterJson["emitStartFrame"] = emitter.emitStartFrame();
    emitterJson["emitEndFrame"] = emitter.emitEndFrame();
    emitterJson["lifeStep"] = emitter.lifeStep();
    emitterJson["lifeStepRandom"] = emitter.lifeStepRandom();
    emitterJson["emitRate"] = emitter.emitRate();

    emitterJson["figureVelocity"] = emitter.figureVelocity();
    emitterJson["velocityDir"] = vec3fToJson(emitter.velocityDirection());
    emitterJson["initVelocity"] = emitter.initialVelocity();
    emitterJson["initVelocityRandom"] = emitter.initialVelocityRandom();
    emitterJson["spreadVec"] = vec3fToJson(emitter.spreadVector());
    emitterJson["airResist"] = emitter.airResistance();

    emitterJson["volumeTblIndex"] = emitter.volumeTblIndex();
    emitterJson["volumeType"] = static_cast<s64>(emitter.volumeType());
    emitterJson["volumeRadius"] = vec3fToJson(emitter.volumeRadius());
    emitterJson["volumeArcWidth"] = emitter.volumeArcWidth();
    emitterJson["volumeArcStart"] = emitter.volumeArcStart();

    emitterJson["rotType"] = static_cast<s64>(emitter.rotationType());
    emitterJson["initRot"] = vec3iToJson(emitter.initialRotation());
    emitterJson["initRotRandom"] = vec3iToJson(emitter.initialRotationRandom());
    emitterJson["rotVel"] = vec3iToJson(emitter.rotationVelocity());
    emitterJson["rotVelRandom"] = vec3iToJson(emitter.rotationVelocityRandom());
    emitterJson["rotBasis"] = vec2fToJson(emitter.rotationBasis());

    emitterJson["alphaAnim"] = alphaAnimToJson(emitter.alphaAnim());

    emitterJson["blendFunc"] = static_cast<s64>(emitter.blendFunction());
    emitterJson["depthFunc"] = static_cast<s64>(emitter.depthFunction());
    emitterJson["combinerFunc"] = static_cast<s64>(emitter.combinerFunction());

    {
        QJsonArray color0Json{};
        for (auto& color : emitter.color0()) {
            color0Json.append(colorToJson(color));
        }
        emitterJson["color0"] = color0Json;
    }

    emitterJson["colorSection1"] = emitter.colorSection1();
    emitterJson["colorSection2"] = emitter.colorSection2();
    emitterJson["colorSection3"] = emitter.colorSection3();
    emitterJson["colorNumRepeat"] = emitter.colorNumRepeat();
    emitterJson["colorCalcType"] = static_cast<s64>(emitter.colorCalcType());

    emitterJson["color1"] = colorToJson(emitter.primaryColor());

    emitterJson["textureWrapT"] = static_cast<s64>(emitter.textureWrapT());
    emitterJson["textureWrapS"] = static_cast<s64>(emitter.textureWrapS());
    emitterJson["textureLodLevel"] = emitter.textureLodLevel();
    emitterJson["textureFilter"] = static_cast<s64>(emitter.textureFilter());
    emitterJson["numTexturePattern"] = emitter.numTexturePattern();
    emitterJson["numTextureDivisionX"] = emitter.numTextureDivisionX();
    emitterJson["numTextureDivisionY"] = emitter.numTextureDivisionY();
    emitterJson["textureUVScale"] = vec2fToJson(emitter.textureUVScale());
    {
        QJsonArray patternTableJson{};
        for (auto& val : emitter.texturePatternTable()) {
            patternTableJson.append(val);
        }
        emitterJson["texturePatternTable"] = patternTableJson;
    }
    emitterJson["texturePatFreq"] = emitter.texturePatternFrequency();
    emitterJson["texturePatFrameCount"] = emitter.texturePatternFrameCount();
    // TODO: Handle embedding texture data if single export
    if (!emitter.textureHandle().isValid()) {
        emitterJson["texture"] = -1;
    } else {
        emitterJson["texture"] = static_cast<s64>(emitter.textureHandle()->Id());
    }

    // Complex properties
    {
        QJsonObject complexJson{};
        complexJson["fluctuationScale"] = emitter.fluctuationScale();
        complexJson["fluctuationFreq"] = emitter.fluctuationFrequency();
        complexJson["fluctuationPhaseRand"] = emitter.isFluctuationPhaseRandom();
        complexJson["fluctuationFlags"] = emitter.fluctuationFlags().value();

        complexJson["stripeType"] = static_cast<s64>(emitter.stripeType());
        complexJson["stripeNumHistory"] = emitter.stripeNumHistory();
        complexJson["stripeStartAlpha"] = emitter.stripeStartAlpha();
        complexJson["stripeEndAlpha"] = emitter.stripeEndAlpha();
        complexJson["stripeUVScrollSpeed"] = vec2fToJson(emitter.stripeUVScrollSpeed());
        complexJson["stripeHistoryStep"] = emitter.stripeHistoryStep();
        complexJson["stripeDirInterpolate"] = emitter.stripeDirInterpolate();
        complexJson["stripeFlags"] = emitter.stripeFlags().value();

        complexJson["fieldRandomBlank"] = emitter.fieldRandomBlank();
        complexJson["fieldRandomVelAdd"] = vec3fToJson(emitter.fieldRandomVelAdd());

        complexJson["fieldMagnetPower"] = emitter.fieldMagnetPower();
        complexJson["fieldMagnetPos"] = vec3fToJson(emitter.fieldMagnetPos());
        complexJson["fieldMagnetFlag"] = static_cast<s64>(emitter.fieldMagnetFlag().value());

        complexJson["fieldSpinRotate"] = emitter.fieldSpinRotate();
        complexJson["fieldSpinAxis"] = static_cast<s64>(emitter.fieldSpinAxis());

        complexJson["fieldCollisionType"] = static_cast<s64>(emitter.fieldCollisionType());
        complexJson["fieldCollisionIsWorld"] = emitter.fieldCollisionIsWorld();
        complexJson["fieldCollisionCoord"] = emitter.fieldCollisionCoord();
        complexJson["fieldCollisionCoef"] = emitter.fieldCollisionCoef();

        complexJson["fieldConvergenceType"] = static_cast<s64>(emitter.fieldConvergenceType());
        complexJson["fieldConvergencePos"] = vec3fToJson(emitter.fieldConvergencePos());

        complexJson["fieldPosAddPosition"] = vec3fToJson(emitter.fieldPosAddPosition());

        complexJson["fieldFlags"] = emitter.fieldFlags().value();

        // Child properties
        {
            QJsonObject childJson{};
            childJson["billboardType"] = static_cast<s64>(emitter.childBillboardType());

            childJson["emitRate"] = emitter.childEmitRate();
            childJson["emitTiming"] = emitter.childEmitTiming();
            childJson["life"] = emitter.childLife();
            childJson["emitStep"] = emitter.childEmitStep();

            childJson["randVelocity"] = vec3fToJson(emitter.childRandVelocity());
            childJson["gravity"] = vec3fToJson(emitter.childGravity());
            childJson["velocityInheritRate"] = emitter.childVelocityInheritRate();
            childJson["initialPositionRand"] = emitter.childInitalPositionRand();
            childJson["figureVelocity"] = emitter.childFigureVelocity();
            childJson["airResist"] = emitter.childAirResistance();

            childJson["rotationType"] = static_cast<s64>(emitter.childRotationType());
            childJson["initialRotation"] = vec3iToJson(emitter.childInitialRotation());
            childJson["initialRotationRandom"] = vec3iToJson(emitter.childInitialRotationRandom());
            childJson["rotationVelocity"] = vec3iToJson(emitter.childRotationVelocity());
            childJson["rotationVelocityRandom"] = vec3iToJson(emitter.childRotationVelocityRandom());
            childJson["rotationBasis"] = vec2fToJson(emitter.childRotationBasis());

            childJson["scale"] = vec2fToJson(emitter.childScale());
            childJson["scaleTarget"] = vec2fToJson(emitter.childScaleTarget());
            childJson["scaleInheritRate"] = emitter.childScaleInheritRate();
            childJson["scaleStartFrame"] = emitter.childScaleStartFrame();

            childJson["textureWrapT"] = static_cast<s64>(emitter.childTextureWrapT());
            childJson["textureWrapS"] = static_cast<s64>(emitter.childTextureWrapS());
            childJson["textureLodLevel"] = emitter.childTextureLodLevel();
            childJson["textureFilter"] = static_cast<s64>(emitter.childTextureFilter());
            childJson["textureUVScale"] = vec2fToJson(emitter.childTextureUVScale());
            // TODO: Handle embedding texture data if single export
            if (!emitter.childTextureHandle().isValid()) {
                childJson["texture"] = -1;
            } else {
                childJson["texture"] = static_cast<s64>(emitter.childTextureHandle()->Id());
            }

            childJson["color0"] = colorToJson(emitter.childSecondaryColor());
            childJson["color1"] = colorToJson(emitter.childPrimaryColor());

            childJson["alpha"] = emitter.childAlpha();
            childJson["alphaTarget"] = emitter.childAlphaTarget();
            childJson["alphaInit"] = emitter.childAlphaInit();
            childJson["alphaStartFrame"] = emitter.childAlphaStartFrame();
            childJson["alphaBaseFrame"] = emitter.childAlphaBaseFrame();

            childJson["blendFunc"] = static_cast<s64>(emitter.childBlendFunc());
            childJson["depthFunc"] = static_cast<s64>(emitter.childDepthFunc());
            childJson["combinerFunc"] = static_cast<s64>(emitter.childCombinerFunc());

            complexJson["child"] = childJson;
        }

        complexJson["childFlags"] = emitter.childFlags().value();

        emitterJson["complex"] = complexJson;
    }

    auto emitterName = QString("emitter_%1_%2.pemt").arg(idx).arg(emitter.name());

    QFile emitterFile{dir.filePath(emitterName)};
    if (!emitterFile.open(QIODevice::WriteOnly)) {
        return std::nullopt;
    }

    emitterFile.write(QJsonDocument(emitterJson).toJson());
    return emitterName;
}

QJsonObject exportEmitters(const EmitterList& emitters, const QDir& dir) {
    QJsonObject emitterSetListJson{};
    for (s32 idx = 0; idx < static_cast<s32>(emitters.size()); ++idx) {
        const auto emitterName = exportEmitter(*emitters.at(idx), idx, dir);

        if (emitterName) {
            emitterSetListJson[QString::number(idx)] = dir.dirName() + "/" + *emitterName;
        }
    }
    return emitterSetListJson;
}

std::optional<QString> exportEmitterSet(const EmitterSet& emitterSet, s32 idx, const QDir& dir) {
    const auto emitterSetName = QString("set_%1_%2").arg(idx).arg(emitterSet.name());

    QJsonObject emitterSetJson{};
    emitterSetJson["metaInfo"] = createMetaInfo(JsonFileType::EmitterSetFile, 1);

    emitterSetJson["name"]           = emitterSet.name();
    emitterSetJson["userData"]       = static_cast<s64>(emitterSet.userData());
    emitterSetJson["lastUpdateDate"] = static_cast<s64>(emitterSet.lastUpdateDate());

    dir.mkdir(emitterSetName);
    QDir emitterSetDir{dir.filePath(emitterSetName)};
    emitterSetJson["emitters"] = exportEmitters(emitterSet.emitters(), emitterSetDir);

    auto emitterSetFileName = QString("%1.pset").arg(emitterSetName);

    QFile emitterSetFile{dir.filePath(emitterSetFileName)};
    if (!emitterSetFile.open(QIODevice::WriteOnly)) {
        return std::nullopt;
    }

    emitterSetFile.write(QJsonDocument(emitterSetJson).toJson());
    return emitterSetFileName;
}

QJsonObject exportEmitterSets(const EmitterSetList& emitterSets, const QDir& dir) {
    QJsonObject emitterSetListJson{};
    for (s32 idx = 0; idx < static_cast<s32>(emitterSets.size()); ++idx) {
        const auto emitterSetName = exportEmitterSet(*emitterSets.at(idx), idx, dir);

        if (emitterSetName) {
            emitterSetListJson[QString::number(idx)] = dir.dirName() + "/" + *emitterSetName;
        }
    }
    return emitterSetListJson;
}

// ========================================================================== //


namespace PtclJson {


bool exportProject(const PtclRes& res, const QString& dirPath) {
    QString nativePath = QDir::toNativeSeparators(dirPath);

    QDir projectDir{nativePath};

    projectDir.mkdir("textures");
    projectDir.mkdir("emitterSets");

    QDir texturesDir{projectDir.filePath("textures")};
    QDir emitterSetsDir{projectDir.filePath("emitterSets")};

    QJsonObject projectJson{};
    projectJson["metaInfo"]    = createMetaInfo(JsonFileType::ProjectFile, 1);
    projectJson["name"]        = res.name();
    projectJson["textures"]    = exportTextures(res.textures(), texturesDir);
    projectJson["emitterSets"] = exportEmitterSets(res.getEmitterSets(), emitterSetsDir);

    auto projectName = QString("%1.ptclproj").arg(res.name());

    QFile projectFile{projectDir.filePath(projectName)};
    if (!projectFile.open(QIODevice::WriteOnly)) {
        return false;
    }

    projectFile.write(QJsonDocument(projectJson).toJson());
    return true;
}


} // namespace PtclJson


// ========================================================================== //


} // namespace Ptcl
