#include "ptcl/ptcljson.h"

#include "ptcl/ptclValidator.h"

#include <QDataStream>

#include <cmath>
#include <cstring>
#include <QFile>
#include <QFileInfo>
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

QJsonValue floatToJson(f32 value) {
    if (value == 0.0f && std::signbit(value)) {
        return {QString("-0")};
    }
    return {static_cast<f64>(value)};
}

QJsonObject vec3fToJson(const Math::Vector3f& vector) {
    QJsonObject vectorJson{};
    vectorJson["x"] = floatToJson(vector.getX());
    vectorJson["y"] = floatToJson(vector.getY());
    vectorJson["z"] = floatToJson(vector.getZ());
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
    vectorJson["x"] = floatToJson(vector.getX());
    vectorJson["y"] = floatToJson(vector.getY());
    return vectorJson;
}

QJsonObject colorToJson(const Gfx::Color& color) {
    QJsonObject colorJson{};
    colorJson["r"] = floatToJson(color.r());
    colorJson["g"] = floatToJson(color.g());
    colorJson["b"] = floatToJson(color.b());
    colorJson["a"] = floatToJson(color.a());
    return colorJson;
}

QJsonArray matrixToJson(const Math::Matrix34f& matrix) {
    QJsonArray matrixJson{};

    for (s32 row = 0; row < 3; ++row) {
        QJsonArray rowJson{};

        for (s32 col = 0; col < 4; ++col) {
            rowJson.append(floatToJson(matrix(row, col)));
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
    animJson["initAlpha"]     = floatToJson(anim.initAlpha);
    animJson["diffAlpha21"]   = floatToJson(anim.diffAlpha21);
    animJson["diffAlpha32"]   = floatToJson(anim.diffAlpha32);
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

    emitterJson["type"] = static_cast<s64>(emitter.type());
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
    emitterJson["alphaAddInFade"] = floatToJson(emitter.alphaAddInFade());

    emitterJson["transformRT"] = matrixToJson(emitter.transformRT());
    emitterJson["transformSRT"] = matrixToJson(emitter.transformSRT());

    emitterJson["scaleAnim"] = scaleAnimToJson(emitter.scaleAnim());
    emitterJson["scaleRand"] = floatToJson(emitter.scaleRand());

    emitterJson["emitStartFrame"] = emitter.emitStartFrame();
    emitterJson["emitEndFrame"] = emitter.emitEndFrame();
    emitterJson["lifeStep"] = emitter.lifeStep();
    emitterJson["lifeStepRandom"] = emitter.lifeStepRandom();
    emitterJson["emitRate"] = emitter.emitRate();

    emitterJson["figureVelocity"] = floatToJson(emitter.figureVelocity());
    emitterJson["velocityDir"] = vec3fToJson(emitter.velocityDirection());
    emitterJson["initVelocity"] = floatToJson(emitter.initialVelocity());
    emitterJson["initVelocityRandom"] = floatToJson(emitter.initialVelocityRandom());
    emitterJson["spreadVec"] = vec3fToJson(emitter.spreadVector());
    emitterJson["airResist"] = floatToJson(emitter.airResistance());

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
    emitterJson["isTexPatAnim"] = emitter.isTexturePatternAnim();
    // TODO: Handle embedding texture data if single export
    if (!emitter.textureHandle().isValid()) {
        emitterJson["texture"] = -1;
    } else {
        emitterJson["texture"] = static_cast<s64>(emitter.textureHandle()->Id());
    }

    // Complex properties
    {
        QJsonObject complexJson{};
        complexJson["fluctuationScale"] = floatToJson(emitter.fluctuationScale());
        complexJson["fluctuationFreq"] = emitter.fluctuationFrequency();
        complexJson["fluctuationPhaseRand"] = emitter.isFluctuationPhaseRandom();
        complexJson["fluctuationFlags"] = emitter.fluctuationFlags().value();

        complexJson["stripeType"] = static_cast<s64>(emitter.stripeType());
        complexJson["stripeNumHistory"] = emitter.stripeNumHistory();
        complexJson["stripeStartAlpha"] = floatToJson(emitter.stripeStartAlpha());
        complexJson["stripeEndAlpha"] = floatToJson(emitter.stripeEndAlpha());
        complexJson["stripeUVScrollSpeed"] = vec2fToJson(emitter.stripeUVScrollSpeed());
        complexJson["stripeHistoryStep"] = emitter.stripeHistoryStep();
        complexJson["stripeDirInterpolate"] = floatToJson(emitter.stripeDirInterpolate());
        complexJson["stripeFlags"] = emitter.stripeFlags().value();

        complexJson["fieldRandomBlank"] = emitter.fieldRandomBlank();
        complexJson["fieldRandomVelAdd"] = vec3fToJson(emitter.fieldRandomVelAdd());

        complexJson["fieldMagnetPower"] = floatToJson(emitter.fieldMagnetPower());
        complexJson["fieldMagnetPos"] = vec3fToJson(emitter.fieldMagnetPos());
        complexJson["fieldMagnetFlag"] = static_cast<s64>(emitter.fieldMagnetFlag().value());

        complexJson["fieldSpinRotate"] = emitter.fieldSpinRotate();
        complexJson["fieldSpinAxis"] = static_cast<s64>(emitter.fieldSpinAxis());

        complexJson["fieldCollisionType"] = static_cast<s64>(emitter.fieldCollisionType());
        complexJson["fieldCollisionIsWorld"] = emitter.fieldCollisionIsWorld();
        complexJson["fieldCollisionCoord"] = floatToJson(emitter.fieldCollisionCoord());
        complexJson["fieldCollisionCoef"] = floatToJson(emitter.fieldCollisionCoef());

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
            childJson["velocityInheritRate"] = floatToJson(emitter.childVelocityInheritRate());
            childJson["initialPositionRand"] = floatToJson(emitter.childInitalPositionRand());
            childJson["figureVelocity"] = floatToJson(emitter.childFigureVelocity());
            childJson["airResist"] = floatToJson(emitter.childAirResistance());

            childJson["rotationType"] = static_cast<s64>(emitter.childRotationType());
            childJson["initialRotation"] = vec3iToJson(emitter.childInitialRotation());
            childJson["initialRotationRandom"] = vec3iToJson(emitter.childInitialRotationRandom());
            childJson["rotationVelocity"] = vec3iToJson(emitter.childRotationVelocity());
            childJson["rotationVelocityRandom"] = vec3iToJson(emitter.childRotationVelocityRandom());
            childJson["rotationBasis"] = vec2fToJson(emitter.childRotationBasis());

            childJson["scale"] = vec2fToJson(emitter.childScale());
            childJson["scaleTarget"] = vec2fToJson(emitter.childScaleTarget());
            childJson["scaleInheritRate"] = floatToJson(emitter.childScaleInheritRate());
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

            childJson["alpha"] = floatToJson(emitter.childAlpha());
            childJson["alphaTarget"] = floatToJson(emitter.childAlphaTarget());
            childJson["alphaInit"] = floatToJson(emitter.childAlphaInit());
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


bool validateMetaInfo(const QJsonObject& metaInfo, JsonFileType type, s32 version) {
    return (metaInfo["fileType"].toInt() != static_cast<s32>(type) || metaInfo["version"].toInt() != version);
}

f32 jsonToFloat(const QJsonValue& json) {
    if (json.isString() && json.toString() == "-0") {
        return -0.0f;
    }
    return static_cast<f32>(json.toDouble());
}

Math::Vector3f jsonToVec3f(const QJsonObject& json) {
    return Math::Vector3f{
        jsonToFloat(json["x"]),
        jsonToFloat(json["y"]),
        jsonToFloat(json["z"])
    };
}

Math::Vector3i jsonToVec3i(const QJsonObject& json) {
    return Math::Vector3i{
        json["x"].toInt(),
        json["y"].toInt(),
        json["z"].toInt()
    };
}

Math::Vector2f jsonToVec2f(const QJsonObject& json) {
    return Math::Vector2f{
        jsonToFloat(json["x"]),
        jsonToFloat(json["y"])
    };
}

Gfx::Color jsonToColor(const QJsonObject& json) {
    return Gfx::Color{
        jsonToFloat(json["r"]),
        jsonToFloat(json["g"]),
        jsonToFloat(json["b"]),
        jsonToFloat(json["a"])
    };
}

Math::Matrix34f jsonToMatrix(const QJsonArray& json) {
    Math::Matrix34f matrix{};
    for (s32 row = 0; row < 3; ++row) {
        const QJsonArray rowJson = json[row].toArray();
        for (s32 col = 0; col < 4; ++col) {
            matrix(row, col) = jsonToFloat(rowJson[col]);
        }
    }
    return matrix;
}

Emitter::ScaleAnim scaleAnimFromJson(const QJsonObject& json) {
    return Emitter::ScaleAnim{
        .initScale = jsonToVec2f(json["initScale"].toObject()),
        .diffScale21 = jsonToVec2f(json["diffScale21"].toObject()),
        .diffScale32 = jsonToVec2f(json["diffScale32"].toObject()),
        .scaleSection1 = json["scaleSection1"].toInt(),
        .scaleSection2 = json["scaleSection2"].toInt(),
        .isFlatStart = json["isFlatStart"].toBool()
    };
}

Emitter::AlphaAnim alphaAnimFromJson(const QJsonObject& json) {
    return Emitter::AlphaAnim{
        .initAlpha = jsonToFloat(json["initAlpha"]),
        .diffAlpha21 = jsonToFloat(json["diffAlpha21"]),
        .diffAlpha32 = jsonToFloat(json["diffAlpha32"]),
        .alphaSection1 = json["alphaSection1"].toInt(),
        .alphaSection2 = json["alphaSection2"].toInt(),
        .isFlatStart = json["isFlatStart"].toBool()
    };
}

std::optional<Texture> importTexture(const QString& filePath, std::optional<u32> id = std::nullopt) {
    QFile file{filePath};

    if (!file.open(QIODevice::ReadOnly)) {
        return std::nullopt;
    }

    QJsonParseError parseError{};
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
        return std::nullopt;
    }

    const QJsonObject textureJson = jsonDoc.object();

    if (validateMetaInfo(textureJson["metaInfo"].toObject(), JsonFileType::TextureFile, 1)) {
        return std::nullopt;
    }

    const auto format = static_cast<TextureFormat>(textureJson["format"].toInt());
    const s32 width = textureJson["width"].toInt();
    const s32 height = textureJson["height"].toInt();

    const auto data = QByteArray::fromBase64(textureJson["data"].toString().toLatin1());
    std::vector<u8> dataVec(data.begin(), data.end());

    return Texture{
        &dataVec,
        width,
        height,
        format,
        id
    };
}

std::optional<TextureList> importTextures(const QJsonObject& texturesJson, const QDir& projectDir) {
    TextureList textures{};
    textures.resize(texturesJson.size());

    for (auto it = texturesJson.constBegin(); it != texturesJson.constEnd(); ++it) {
        bool ok{false};

        const size_t idx = it.key().toInt(&ok);

        if (!ok || idx < 0 || idx >= textures.size()) {
            return std::nullopt;
        }

        const QString texturePath = projectDir.filePath(it.value().toString());
        auto texture = importTexture(texturePath, static_cast<u32>(idx));

        if (!texture) {
            return std::nullopt;
        }

        textures[idx] = (std::make_unique<Texture>(std::move(*texture)));
    }
    return textures;
}

std::unique_ptr<Emitter> importEmitter(const QString& filePath, const TextureList& textures) {
    QFile file{filePath};

    if (!file.open(QIODevice::ReadOnly)) {
        return nullptr;
    }

    QJsonParseError parseError{};
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
        return nullptr;
    }

    const QJsonObject emitterJson = jsonDoc.object();

    if (validateMetaInfo(emitterJson["metaInfo"].toObject(), JsonFileType::EmitterFile, 1)) {
        return nullptr;
    }

    auto emitter = std::make_unique<Emitter>();

    // Basic Properties
    emitter->setType(static_cast<EmitterType>(emitterJson["type"].toInteger()));
    emitter->flags() = BitFlag<EmitterFlag>(static_cast<u32>(emitterJson["flag"].toInteger()));
    emitter->setFollowType(static_cast<FollowType>(emitterJson["followType"].toInteger()));
    emitter->setName(emitterJson["name"].toString());
    emitter->setRandomSeed(PtclSeed{static_cast<u32>(emitterJson["randomSeed"].toInteger())});
    emitter->setBillboardType(static_cast<BillboardType>(emitterJson["billboardType"].toInteger()));

    emitter->setDirectional(emitterJson["isDirectional"].toBool());
    emitter->setGravity(jsonToVec3f(emitterJson["gravity"].toObject()));

    emitter->setPtclLife(emitterJson["ptclLife"].toInt());
    emitter->setPtclLifeRandom(emitterJson["ptclLifeRandom"].toInt());

    emitter->setIsStopEmitInFade(emitterJson["isStopEmitInFade"].toBool());
    emitter->setAlphaAddInFade(jsonToFloat(emitterJson["alphaAddInFade"]));

    {
        const auto rt = jsonToMatrix(emitterJson["transformRT"].toArray());
        const auto srt = jsonToMatrix(emitterJson["transformSRT"].toArray());
        emitter->setTransformFromMatrices(rt, srt);
    }

    emitter->setScaleAnim(scaleAnimFromJson(emitterJson["scaleAnim"].toObject()));
    emitter->setScaleRand(jsonToFloat(emitterJson["scaleRand"]));

    emitter->setEmitStartFrame(emitterJson["emitStartFrame"].toInt());
    emitter->setEmitEndFrame(emitterJson["emitEndFrame"].toInt());
    emitter->setLifeStep(emitterJson["lifeStep"].toInt());
    emitter->setLifeStepRandom(emitterJson["lifeStepRandom"].toInt());
    emitter->setEmitRate(emitterJson["emitRate"].toInt());

    emitter->setFigureVelocity(jsonToFloat(emitterJson["figureVelocity"]));
    emitter->setVelocityDirection(jsonToVec3f(emitterJson["velocityDir"].toObject()));
    emitter->setInitialVelocity(jsonToFloat(emitterJson["initVelocity"]));
    emitter->setInitialVelocityRandom(jsonToFloat(emitterJson["initVelocityRandom"]));
    emitter->setSpreadVector(jsonToVec3f(emitterJson["spreadVec"].toObject()));
    emitter->setAirResistance(jsonToFloat(emitterJson["airResist"]));

    emitter->setVolumeTblIndex(static_cast<u8>(emitterJson["volumeTblIndex"].toInt()));
    emitter->setVolumeType(static_cast<VolumeType>(emitterJson["volumeType"].toInteger()));
    emitter->setVolumeRadius(jsonToVec3f(emitterJson["volumeRadius"].toObject()));
    emitter->setVolumeArcWidth(emitterJson["volumeArcWidth"].toInt());
    emitter->setVolumeArcStart(emitterJson["volumeArcStart"].toInt());

    emitter->setRotationType(static_cast<RotType>(emitterJson["rotType"].toInteger()));
    emitter->setInitialRotation(jsonToVec3i(emitterJson["initRot"].toObject()));
    emitter->setInitialRotationRandom(jsonToVec3i(emitterJson["initRotRandom"].toObject()));
    emitter->setRotationVelocity(jsonToVec3i(emitterJson["rotVel"].toObject()));
    emitter->setRotationVelocityRandom(jsonToVec3i(emitterJson["rotVelRandom"].toObject()));
    emitter->setRotationBasis(jsonToVec2f(emitterJson["rotBasis"].toObject()));

    emitter->setAlphaAnim(alphaAnimFromJson(emitterJson["alphaAnim"].toObject()));

    emitter->setBlendFunction(static_cast<BlendFuncType>(emitterJson["blendFunc"].toInteger()));
    emitter->setDepthFunction(static_cast<DepthFuncType>(emitterJson["depthFunc"].toInteger()));
    emitter->setCombinerFunction(static_cast<ColorCombinerFuncType>(emitterJson["combinerFunc"].toInteger()));

    {
        const QJsonArray color0Json = emitterJson["color0"].toArray();
        for (s32 i = 0; i < 3 && i < color0Json.size(); ++i) {
            auto color = jsonToColor(color0Json[i].toObject());
            switch (i) {
            case 0: emitter->setStartColor(color); break;
            case 1: emitter->setMidColor(color); break;
            case 2: emitter->setEndColor(color); break;
            }
        }
    }
    emitter->setColorSection1(emitterJson["colorSection1"].toInt());
    emitter->setColorSection2(emitterJson["colorSection2"].toInt());
    emitter->setColorSection3(emitterJson["colorSection3"].toInt());
    emitter->setColorNumRepeat(emitterJson["colorNumRepeat"].toInt());
    emitter->setColorCalcType(static_cast<ColorCalcType>(emitterJson["colorCalcType"].toInteger()));
    emitter->setPrimaryColor(jsonToColor(emitterJson["color1"].toObject()));

    emitter->setTextureWrapT(static_cast<TextureWrap>(emitterJson["textureWrapT"].toInteger()));
    emitter->setTextureWrapS(static_cast<TextureWrap>(emitterJson["textureWrapS"].toInteger()));
    emitter->setTextureLodLevel(static_cast<u8>(emitterJson["textureLodLevel"].toInt()));
    emitter->setTextureFilter(static_cast<TextureFilter>(emitterJson["textureFilter"].toInteger()));
    emitter->setNumTexturePattern(static_cast<u16>(emitterJson["numTexturePattern"].toInt()));
    emitter->setNumTextureDivisionX(static_cast<u8>(emitterJson["numTextureDivisionX"].toInt()));
    emitter->setNumTextureDivisionY(static_cast<u8>(emitterJson["numTextureDivisionY"].toInt()));
    emitter->setTextureUVScale(jsonToVec2f(emitterJson["textureUVScale"].toObject()));
    {
        const QJsonArray patternTableJson = emitterJson["texturePatternTable"].toArray();
        std::array<u8, 16> patternTable{};
        for (s32 i = 0; i < 16 && i < patternTableJson.size(); ++i) {
            patternTable[i] = static_cast<u8>(patternTableJson[i].toInt());
        }
        emitter->setTexturePatternTable(patternTable);
    }
    emitter->setTexturePatternFrequency(static_cast<u16>(emitterJson["texturePatFreq"].toInt()));
    emitter->setTexturePatternFrameCount(static_cast<u16>(emitterJson["texturePatFrameCount"].toInt()));
    emitter->setIsTexturePatternAnim(emitterJson["isTexPatAnim"].toBool());

    // Resolve texture handle
    {
        const s32 texIdx = emitterJson["texture"].toInt();
        if (texIdx >= 0 && texIdx < static_cast<s32>(textures.size())) {
            emitter->setTexture(textures[texIdx].get());
        }
    }

    // Complex Properties
    {
        const QJsonObject complexJson = emitterJson["complex"].toObject();

        emitter->setFluctuationScale(jsonToFloat(complexJson["fluctuationScale"]));
        emitter->setFluctuationFrequency(jsonToFloat(complexJson["fluctuationFreq"]));
        emitter->setFluctuationPhaseRandom(complexJson["fluctuationPhaseRand"].toBool());
        emitter->setFluctuationFlags(BitFlag<FluctuationFlag>(static_cast<u16>(complexJson["fluctuationFlags"].toInt())));

        emitter->setStripeType(static_cast<StripeType>(complexJson["stripeType"].toInteger()));
        emitter->setStripeNumHistory(complexJson["stripeNumHistory"].toInt());
        emitter->setStripeStartAlpha(jsonToFloat(complexJson["stripeStartAlpha"]));
        emitter->setStripeEndAlpha(jsonToFloat(complexJson["stripeEndAlpha"]));
        emitter->setStripeUVScrollSpeed(jsonToVec2f(complexJson["stripeUVScrollSpeed"].toObject()));
        emitter->setStripeHistoryStep(complexJson["stripeHistoryStep"].toInt());
        emitter->setStripeDirInterpolate(jsonToFloat(complexJson["stripeDirInterpolate"]));
        emitter->setStripeFlags(BitFlag<StripeFlag>(static_cast<u16>(complexJson["stripeFlags"].toInt())));

        emitter->setFieldRandomBlank(complexJson["fieldRandomBlank"].toInt());
        emitter->setFieldRandomVelAdd(jsonToVec3f(complexJson["fieldRandomVelAdd"].toObject()));

        emitter->setFieldMagnetPower(jsonToFloat(complexJson["fieldMagnetPower"]));
        emitter->setFieldMagnetPos(jsonToVec3f(complexJson["fieldMagnetPos"].toObject()));
        {
            const BitFlag<FieldMagnetFlag> magnetFlags{static_cast<u32>(complexJson["fieldMagnetFlag"].toInteger())};
            emitter->setFieldMagnetAxisTargetX(magnetFlags.isSet(FieldMagnetFlag::AxisTargetX));
            emitter->setFieldMagnetAxisTargetY(magnetFlags.isSet(FieldMagnetFlag::AxisTargetY));
            emitter->setFieldMagnetAxisTargetZ(magnetFlags.isSet(FieldMagnetFlag::AxisTargetZ));
        }

        emitter->setFieldSpinRotate(complexJson["fieldSpinRotate"].toInt());
        emitter->setFieldSpinAxis(static_cast<FieldSpinAxis>(complexJson["fieldSpinAxis"].toInteger()));

        emitter->setFieldCollisionType(static_cast<FieldCollisionType>(complexJson["fieldCollisionType"].toInteger()));
        emitter->setFieldCollisionIsWorld(complexJson["fieldCollisionIsWorld"].toBool());
        emitter->setFieldCollisionCoord(jsonToFloat(complexJson["fieldCollisionCoord"]));
        emitter->setFieldCollisionCoef(jsonToFloat(complexJson["fieldCollisionCoef"]));

        emitter->setFieldConvergenceType(static_cast<FieldConvergenceType>(complexJson["fieldConvergenceType"].toInteger()));
        emitter->setFieldConvergencePos(jsonToVec3f(complexJson["fieldConvergencePos"].toObject()));

        emitter->setFieldPosAddPosition(jsonToVec3f(complexJson["fieldPosAddPosition"].toObject()));

        emitter->setFieldFlags(BitFlag<FieldFlag>(static_cast<u16>(complexJson["fieldFlags"].toInt())));

        // Child Properties
        {
            const QJsonObject childJson = complexJson["child"].toObject();

            emitter->setChildBillboardType(static_cast<BillboardType>(childJson["billboardType"].toInteger()));

            emitter->setChildEmitRate(childJson["emitRate"].toInt());
            emitter->setChildEmitTiming(childJson["emitTiming"].toInt());
            emitter->setChildLife(childJson["life"].toInt());
            emitter->setChildEmitStep(childJson["emitStep"].toInt());

            emitter->setChildRandVelocity(jsonToVec3f(childJson["randVelocity"].toObject()));
            emitter->setChildGravity(jsonToVec3f(childJson["gravity"].toObject()));
            emitter->setChildVelocityInheritRate(jsonToFloat(childJson["velocityInheritRate"]));
            emitter->setChildInitialPositionRand(jsonToFloat(childJson["initialPositionRand"]));
            emitter->setChildFigureVelocity(jsonToFloat(childJson["figureVelocity"]));
            emitter->setChildAirResistance(jsonToFloat(childJson["airResist"]));

            emitter->setChildRotationType(static_cast<RotType>(childJson["rotationType"].toInteger()));
            emitter->setChildInitialRotation(jsonToVec3i(childJson["initialRotation"].toObject()));
            emitter->setChildInitialRotationRandom(jsonToVec3i(childJson["initialRotationRandom"].toObject()));
            emitter->setChildRotationVelocity(jsonToVec3i(childJson["rotationVelocity"].toObject()));
            emitter->setChildRotationVelocityRandom(jsonToVec3i(childJson["rotationVelocityRandom"].toObject()));
            emitter->setChildRotationBasis(jsonToVec2f(childJson["rotationBasis"].toObject()));

            emitter->setChildScale(jsonToVec2f(childJson["scale"].toObject()));
            emitter->setChildScaleTarget(jsonToVec2f(childJson["scaleTarget"].toObject()));
            emitter->setChildScaleInheritRate(jsonToFloat(childJson["scaleInheritRate"]));
            emitter->setChildScaleStartFrame(childJson["scaleStartFrame"].toInt());

            emitter->setChildTextureWrapT(static_cast<TextureWrap>(childJson["textureWrapT"].toInteger()));
            emitter->setChildTextureWrapS(static_cast<TextureWrap>(childJson["textureWrapS"].toInteger()));
            emitter->setChildTextureLodLevel(static_cast<u8>(childJson["textureLodLevel"].toInt()));
            emitter->setChildTextureFilter(static_cast<TextureFilter>(childJson["textureFilter"].toInteger()));
            emitter->setChildTextureUVScale(jsonToVec2f(childJson["textureUVScale"].toObject()));

            // Resolve child texture handle
            {
                const s32 childTexIdx = childJson["texture"].toInt();
                if (childTexIdx >= 0 && childTexIdx < static_cast<s32>(textures.size())) {
                    emitter->setChildTexture(textures[childTexIdx].get());
                }
            }

            emitter->setChildSecondaryColor(jsonToColor(childJson["color0"].toObject()));
            emitter->setChildPrimaryColor(jsonToColor(childJson["color1"].toObject()));

            emitter->setChildAlpha(jsonToFloat(childJson["alpha"]));
            emitter->setChildAlphaTarget(jsonToFloat(childJson["alphaTarget"]));
            emitter->setChildAlphaInit(jsonToFloat(childJson["alphaInit"]));
            emitter->setChildAlphaStartFrame(childJson["alphaStartFrame"].toInt());
            emitter->setChildAlphaBaseFrame(childJson["alphaBaseFrame"].toInt());

            emitter->setChildBlendFunc(static_cast<BlendFuncType>(childJson["blendFunc"].toInteger()));
            emitter->setChildDepthFunc(static_cast<DepthFuncType>(childJson["depthFunc"].toInteger()));
            emitter->setChildCombinerFunc(static_cast<ColorCombinerFuncType>(childJson["combinerFunc"].toInteger()));
        }

        emitter->setChildFlags(BitFlag<ChildFlag>(static_cast<u16>(complexJson["childFlags"].toInt())));
    }

    return emitter;
}

std::optional<EmitterSet> importEmitterSet(const QString& filePath, const TextureList& textures) {
    QFile file{filePath};

    if (!file.open(QIODevice::ReadOnly)) {
        return std::nullopt;
    }

    QJsonParseError parseError{};
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(file.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
        return std::nullopt;
    }

    const QJsonObject emitterSetJson = jsonDoc.object();

    if (validateMetaInfo(emitterSetJson["metaInfo"].toObject(), JsonFileType::EmitterSetFile, 1)) {
        return std::nullopt;
    }

    const auto name = emitterSetJson["name"].toString();
    const auto userData = static_cast<u32>(emitterSetJson["userData"].toInteger());
    const auto lastUpdateDate = static_cast<u32>(emitterSetJson["lastUpdateDate"].toInteger());

    EmitterSet emitterSet{};

    emitterSet.setName(name);
    emitterSet.setUserData(userData);
    emitterSet.setLastUpdateDate(lastUpdateDate);

    const QDir emitterSetDir{QFileInfo(filePath).absolutePath()};

    const QJsonObject emittersJson = emitterSetJson["emitters"].toObject();
    for (auto it = emittersJson.constBegin(); it != emittersJson.constEnd(); ++it) {
        bool ok{false};
        const s32 idx = it.key().toInt(&ok);
        if (!ok) {
            return std::nullopt;
        }

        const QString emitterPath = emitterSetDir.filePath(it.value().toString());
        auto emitter = importEmitter(emitterPath, textures);
        if (!emitter) {
            return std::nullopt;
        }

        emitterSet.insertEmitter(idx, std::move(emitter));
    }

    return emitterSet;
}

std::optional<EmitterSetList> importEmitterSets(const QJsonObject& emitterSetsJson, const QDir& projectDir, const TextureList& textures) {
    EmitterSetList emitterSets{};
    emitterSets.resize(emitterSetsJson.size());

    for (auto it = emitterSetsJson.constBegin(); it != emitterSetsJson.constEnd(); ++it) {
        bool ok{false};

        const size_t idx = it.key().toInt(&ok);

        if (!ok || idx < 0 || idx >= emitterSets.size()) {
            return std::nullopt;
        }

        const QString emitterSetPath = projectDir.filePath(it.value().toString());
        auto emitterSet = importEmitterSet(emitterSetPath, textures);

        if (!emitterSet) {
            return std::nullopt;
        }

        emitterSets[idx] = (std::make_unique<EmitterSet>(std::move(*emitterSet)));
    }
    return emitterSets;
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

bool importProject(const QString& projPath, PtclRes& res, [[maybe_unused]] PtclSanitizeReport& report) {
    QFile projectFile{projPath};
    if (!projectFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonParseError parseError{};
    const QJsonDocument jsonDoc = QJsonDocument::fromJson(projectFile.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError || !jsonDoc.isObject()) {
        return false;
    }

    const QJsonObject projectJson = jsonDoc.object();

    if (validateMetaInfo(projectJson["metaInfo"].toObject(), JsonFileType::ProjectFile, 1)) {
        return false;
    }

    res.setName(projectJson["name"].toString());

    const QDir projectDir{QFileInfo(projPath).absolutePath()};

    auto textures = importTextures(projectJson["textures"].toObject(), projectDir);
    if (!textures) {
        return false;
    }

    res.textures() = std::move(*textures);

    auto emitterSets = importEmitterSets(projectJson["emitterSets"].toObject(), projectDir, res.textures());
    if (!emitterSets) {
        return false;
    }

    res.getEmitterSets() = std::move(*emitterSets);

    // TODO: Validate stuff

    return true;
}

} // namespace PtclJson


// ========================================================================== //


} // namespace Ptcl
