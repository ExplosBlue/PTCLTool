#include "ptcl/ptclValidator.h"
#include "util/imageUtil.h"

#include <memory>
#include <stdexcept>
#include <utility>


namespace Ptcl {


// ========================================================================== //


PtclValidationResult PtclValidator::validate(PtclReadResult&& read) {
    PtclValidationResult result;
    mTextureCache.assign(read.textures.size(), nullptr);

    result.emitterSets.reserve(read.emitterSets.size());

    for (s32 setIndex = 0; setIndex < static_cast<s32>(read.emitterSets.size()); ++setIndex) {
        RawEmitterSetRecord& rawSet = read.emitterSets[setIndex];

        auto set = std::make_unique<EmitterSet>();
        set->setName(rawSet.name);
        set->setUserData(rawSet.data.userData);
        set->setLastUpdateDate(rawSet.data.lastUpdateDate);

        for (s32 emitterIndex = 0; emitterIndex < static_cast<s32>(rawSet.emitters.size()); ++emitterIndex) {
            RawEmitterRecord& rawEmitter = rawSet.emitters[emitterIndex];

            if (rawEmitter.isNull) {
                set->emitters().push_back(std::make_unique<Emitter>());
                continue;
            }

            const QString context = QStringLiteral("EmitterSet %1 / Emitter %2").arg(setIndex).arg(emitterIndex);
            result.report.setContext(context);

            const BinCommonEmitterData common = normalizeCommon(rawEmitter.common, result.report);

            auto emitter = std::make_unique<Emitter>();
            emitter->initFromBinary(common);
            emitter->setName(rawEmitter.name);
            emitter->setTexture(buildTexture(read, rawEmitter.textureIndex, result));

            if (rawEmitter.complex.has_value()) {
                emitter->initComplexFromBinary(*rawEmitter.complex);

                if (rawEmitter.child.has_value()) {
                    result.report.setContext(context + QStringLiteral(" [child]"));

                    const BinChildData child = normalizeChild(*rawEmitter.child, result.report);
                    emitter->initChild(child);
                    emitter->setChildTexture(buildTexture(read, rawEmitter.childTextureIndex, result));

                    result.report.setContext(context);
                }

                if (rawEmitter.fieldSpin.has_value()) {
                    emitter->initFieldSpin(normalizeFieldSpin(*rawEmitter.fieldSpin, result.report));
                }
                if (rawEmitter.fieldCollision.has_value()) {
                    emitter->initFieldCollision(normalizeFieldCollision(*rawEmitter.fieldCollision, result.report));
                }
                if (rawEmitter.fieldConvergence.has_value()) {
                    emitter->initFieldConvergence(normalizeFieldConvergence(*rawEmitter.fieldConvergence, result.report));
                }
                if (rawEmitter.fieldRandom.has_value()) {
                    emitter->initFieldRandom(*rawEmitter.fieldRandom);
                }
                if (rawEmitter.fieldMagnet.has_value()) {
                    emitter->initFieldMagnet(*rawEmitter.fieldMagnet);
                }
                if (rawEmitter.fieldPosAdd.has_value()) {
                    emitter->initFieldPosAdd(*rawEmitter.fieldPosAdd);
                }
                if (rawEmitter.fluctuation.has_value()) {
                    emitter->initFluctuationData(*rawEmitter.fluctuation);
                }
                if (rawEmitter.stripe.has_value()) {
                    emitter->initStripeData(normalizeStripe(*rawEmitter.stripe, result.report));
                }
            }

            set->emitters().push_back(std::move(emitter));
        }

        result.emitterSets.push_back(std::move(set));
    }

    return result;
}

BinCommonEmitterData PtclValidator::normalizeCommon(const BinCommonEmitterData& data, PtclSanitizeReport& report) {
    BinCommonEmitterData result = data;

    const auto wrapT = report.sanitize<TextureWrap>(data.textureRes.wrapModes & 0xF, TextureWrap::ClampToEdge, "textureWrapT");
    const auto wrapS = report.sanitize<TextureWrap>((data.textureRes.wrapModes >> 4) & 0xF, TextureWrap::ClampToEdge, "textureWrapS");
    result.textureRes.wrapModes = static_cast<u8>(std::to_underlying(wrapT)) | (static_cast<u8>(std::to_underlying(wrapS)) << 4);

    result.type = report.sanitize<EmitterType>(data.type, EmitterType::Compact, "type");
    result.followType = report.sanitize<FollowType>(data.followType, FollowType::PosOnly, "followType");
    result.billboardType = report.sanitize<BillboardType>(data.billboardType, BillboardType::BillboardY, "billboardType");
    result.volumeType = report.sanitize<VolumeType>(data.volumeType, VolumeType::Rectangle, "volumeType");

    const auto colorCalcType = report.sanitize<ColorCalcType>(data.rotCalcType / 5, ColorCalcType::Interpolate, "colorCalcType");
    const auto rotType = report.sanitize<RotType>(data.rotCalcType % 5, RotType::RotXYZ, "rotType");
    result.rotCalcType = static_cast<u32>(std::to_underlying(rotType)) + static_cast<u32>(std::to_underlying(colorCalcType)) * 5;

    result.blendFunc = report.sanitize<BlendFuncType>(data.blendFunc, BlendFuncType::Transparence, "blendFunc");
    result.depthFunc = report.sanitize<DepthFuncType>(data.depthFunc, DepthFuncType::Unk2, "depthFunc");
    result.colorCombinerFunc = report.sanitize<ColorCombinerFuncType>(data.colorCombinerFunc, ColorCombinerFuncType::CombinerConfig27, "combinerFunc");

    return result;
}

BinChildData PtclValidator::normalizeChild(const BinChildData& data, PtclSanitizeReport& report) {
    BinChildData result = data;

    result.childBillboardType = report.sanitize<BillboardType>(data.childBillboardType, BillboardType::BillboardY, "childBillboardType");
    result.childRotType = report.sanitize<RotType>(data.childRotType, RotType::RotXYZ, "childRotType");

    const auto wrapT = report.sanitize<TextureWrap>(data.childTextureRes.wrapModes & 0xF, TextureWrap::ClampToEdge, "childTextureWrapT");
    const auto wrapS = report.sanitize<TextureWrap>((data.childTextureRes.wrapModes >> 4) & 0xF, TextureWrap::ClampToEdge, "childTextureWrapS");
    result.childTextureRes.wrapModes = static_cast<u8>(std::to_underlying(wrapT)) | (static_cast<u8>(std::to_underlying(wrapS)) << 4);

    result.childBlendType = report.sanitize<BlendFuncType>(data.childBlendType, BlendFuncType::Transparence, "childBlendFunc");
    result.childDepthType = report.sanitize<DepthFuncType>(data.childDepthType, DepthFuncType::Unk2, "childDepthFunc");
    result.childCombinerType = report.sanitize<ColorCombinerFuncType>(data.childCombinerType, ColorCombinerFuncType::CombinerConfig27, "childCombinerFunc");

    return result;
}

BinFieldSpinData PtclValidator::normalizeFieldSpin(const BinFieldSpinData& data, PtclSanitizeReport& report) {
    BinFieldSpinData result = data;
    result.fieldSpinAxis = report.sanitize<FieldSpinAxis>(data.fieldSpinAxis, FieldSpinAxis::AxisZ, "fieldSpinAxis");
    return result;
}

BinFieldCollisionData PtclValidator::normalizeFieldCollision(const BinFieldCollisionData& data, PtclSanitizeReport& report) {
    BinFieldCollisionData result = data;
    result.fieldCollisionType = report.sanitize<FieldCollisionType>(data.fieldCollisionType, FieldCollisionType::Bounce, "fieldCollisionType");
    return result;
}

BinFieldConvergenceData PtclValidator::normalizeFieldConvergence(const BinFieldConvergenceData& data, PtclSanitizeReport& report) {
    BinFieldConvergenceData result = data;
    result.fieldConvergenceType = report.sanitize<FieldConvergenceType>(data.fieldConvergenceType, FieldConvergenceType::EmitterPos, "fieldConvergenceType");
    return result;
}

BinStripeData PtclValidator::normalizeStripe(const BinStripeData& data, PtclSanitizeReport& report) {
    BinStripeData result = data;
    result.stripeType = report.sanitize<StripeType>(data.stripeType, StripeType::EmitterUpDown, "stripeType");
    return result;
}

Texture* PtclValidator::buildTexture(PtclReadResult& read, s32 textureIndex, PtclValidationResult& result) {
    if (textureIndex < 0 || textureIndex >= static_cast<s32>(read.textures.size())) {
        return nullptr;
    }

    Texture*& cached = mTextureCache[textureIndex];
    if (cached != nullptr) {
        return cached;
    }

    RawTextureData& raw = read.textures[textureIndex];

    if (raw.width == 0 || raw.height == 0 || raw.width > ImageUtil::maxTextureDimension() || raw.height > ImageUtil::maxTextureDimension()) {
        throw std::runtime_error("PtclValidator - Invalid texture dimensions.");
    }

    if (static_cast<u64>(raw.width) * raw.height * 4 > ImageUtil::maxTextureBytes()) {
        throw std::runtime_error("PtclValidator - Texture raster too large.");
    }

    const auto format = result.report.sanitize<TextureFormat>(raw.format, TextureFormat::ETC1_A4, TextureFormat::RGBA8888, "textureFormat");

    if (raw.size < ImageUtil::textureDataMinBytes(static_cast<s32>(raw.width), static_cast<s32>(raw.height), format)) {
        throw std::runtime_error("PtclValidator - Texture data size too small for its format.");
    }

    auto texture = std::make_unique<Texture>(&raw.bytes, raw.width, raw.height, format);

    result.textures.push_back(std::move(texture));

    cached = result.textures.back().get();
    return cached;
}



} // namespace Ptcl
