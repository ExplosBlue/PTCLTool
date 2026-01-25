#include "editor/combinerPropertiesWidget.h"

#include <QGridLayout>
#include <QFormLayout>
#include <QPainter>
#include <QPainterPath>


// ========================================================================== //


CombinerStageWidget::CombinerStageWidget(QWidget* parent) :
    QWidget{parent} {
}

void CombinerStageWidget::setStage(Combiner::Stage stage, s32 num) {
    mStage = stage;
    mStageNum = num;
    updateStage();
    updateGeometry();
}

void CombinerStageWidget::setCombinerSrc(const Ptcl::TextureHandle* texture, const Ptcl::binColor3f* constant, const Ptcl::binColor4f* primary, const QImage* previous) {
    mTextureSrc = texture;
    mConstantSrc = constant;
    mPrimarySrc = primary;
    mPrevSrc = previous;
    updateStageOutput();
}

void CombinerStageWidget::updateStage() {
    updateStageOutput();
    update();
}

const QImage& CombinerStageWidget::getStageOutput() const {
    return mOutputImage;
}

void CombinerStageWidget::drawBackground(QPainter& painter) {
    painter.save();
    QRectF drawRect = rect().adjusted(sMargin, sMargin, -sMargin, -sMargin);
    painter.setBrush(sBackgroundColor);
    painter.setPen(sBorderColor);
    painter.drawRoundedRect(drawRect, sBorderRadius, sBorderRadius);
    painter.restore();
}

QImage CombinerStageWidget::colorToImage(const QColor& color) const {
    QImage img(sThumbSize, sThumbSize, QImage::Format_RGBA8888);
    img.fill(color);
    return img;
}

QImage CombinerStageWidget::visualizeColorOrAlpha(const QImage& src) const {
    const QImage srcRGBA = src.convertToFormat(QImage::Format_RGBA8888);
    QImage out(src.size(), QImage::Format_RGBA8888);

    if (mStage.type == Combiner::StageType::Color) {
        for (s32 y = 0; y < srcRGBA.height(); ++y) {
            for (s32 x = 0; x < srcRGBA.width(); ++x) {
                QColor c = srcRGBA.pixelColor(x, y);
                c.setAlpha(255);
                out.setPixelColor(x, y, c);
            }
        }
        return out;
    }

    for (s32 y = 0; y < srcRGBA.height(); ++y) {
        for (s32 x = 0; x < srcRGBA.width(); ++x) {
            const s32 a = src.pixelColor(x, y).alpha();
            out.setPixelColor(x, y, QColor(a, a, a, 255));
        }
    }
    return out;
}

QImage CombinerStageWidget::applyOperandTransform(const QImage& src, Combiner::Operand op) const {
    if (op == Combiner::Operand::Src) {
        return src;
    }

    QImage out(src.size(), QImage::Format_RGBA8888);

    for (s32 y = 0; y < src.height(); ++y) {
        for (s32 x = 0; x < src.width(); ++x) {
            QColor c = src.pixelColor(x, y);

            if (op == Combiner::Operand::SrcR) {
                s32 r = c.red();
                out.setPixelColor(x, y, QColor(r, r, r, 255));
            } else if (op == Combiner::Operand::OneMinusSrc) {
                out.setPixelColor(x, y, QColor(255 - c.red(), 255 - c.green(), 255 - c.blue(), 255));
            }
        }
    }

    return out;
}

void CombinerStageWidget::drawInput(QPainter& painter, const QRect& drawRect, s32 inputIdx) {
    painter.save();
    const auto input = mStage.inputs[inputIdx];

    painter.setBrush(sThumbBackgroundColor);
    painter.setPen(sThumbBorderColor);
    painter.drawRoundedRect(drawRect, sThumbRadius, sThumbRadius);

    QString label;

    if (input == Combiner::Input::Texture) {
        label = "Tex";
    } else if (input == Combiner::Input::Constant) {
        label = "Const";
    } else if (input == Combiner::Input::Primary) {
        label = "Primary";
    } else if (input == Combiner::Input::Previous) {
        label = "Prev";
    }

    QRect contentRect = drawRect.adjusted(sThumbPadding, sThumbPadding, -sThumbPadding, -sThumbPadding);

    auto img = getTransformedInput(inputIdx);
    painter.drawImage(contentRect, img);

    painter.setPen(Qt::white);
    auto fm = painter.fontMetrics();
    const s32 textWidth = fm.horizontalAdvance(label);
    painter.drawText(QRect(drawRect.center().x() - textWidth / 2, drawRect.bottom() + sLabelGap, textWidth, sLabelHeight), Qt::AlignCenter, label);
    painter.restore();
    // TODO: Draw overlays on top of input thumbnails to indicate operand
}

void CombinerStageWidget::drawEquation(QPainter& painter, const QRect& drawRect) {
    painter.save();
    switch (mStage.func) {
    case Combiner::Func::Replace:
        drawReplace(painter, drawRect);
        break;
    case Combiner::Func::Modulate:
        drawBinaryOp(painter, drawRect, "×");
        break;
    case Combiner::Func::Subtract:
        drawBinaryOp(painter, drawRect, "-");
        break;
    case Combiner::Func::MultAdd:
        drawMultAdd(painter, drawRect);
        break;
    }
    painter.restore();
}

void CombinerStageWidget::drawReplace(QPainter& painter, const QRect& drawRect) {
    QRect a(drawRect.center().x() - sThumbSize / 2, drawRect.center().y() - sThumbSize / 2, sThumbSize, sThumbSize);
    drawInput(painter, a, 0);
}

void CombinerStageWidget::drawParen(QPainter& painter, const QRect& drawRect, bool isLeft) {
    painter.save();
    QPainterPath path;

    if (isLeft) {
        path.moveTo(drawRect.right(), drawRect.top());
        path.quadTo(drawRect.left(), drawRect.top(), drawRect.left(), drawRect.center().y());
        path.quadTo(drawRect.left(), drawRect.bottom(), drawRect.right(), drawRect.bottom());
    } else {
        path.moveTo(drawRect.left(), drawRect.top());
        path.quadTo(drawRect.right(), drawRect.top(), drawRect.right(), drawRect.center().y());
        path.quadTo(drawRect.right(), drawRect.bottom(), drawRect.left(), drawRect.bottom());
    }

    QPen pen(sEquationColor);
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);
    painter.restore();
}

void CombinerStageWidget::drawMultAdd(QPainter& painter, const QRect& drawRect) {
    painter.save();
    const s32 totalWidth = calcEquationWidth();

    // Start offset
    s32 x = drawRect.center().x() - totalWidth / 2;
    s32 y = drawRect.center().y() - sThumbSize / 2;

    // Left Paren
    QRect parenL(x, y - sParenPad, sParenW, sParenH);
    x += parenL.width() + sOperatorGap;

    // Input A
    QRect a(x, y, sThumbSize, sThumbSize);
    x += sThumbSize + sOperatorGap;

    // Operator ×
    QRect opMul(x, y, sOperatorWidth, sThumbSize);
    x += opMul.width() + sOperatorGap;

    // Input B
    QRect b(x, y, sThumbSize, sThumbSize);
    x += sThumbSize + sOperatorGap;

    // Right Paren
    QRect parenR(x, y - sParenPad, sParenW, sParenH);
    x += parenR.width() + sOperatorGap;

    // Operator +
    QRect opAdd(x, y, sOperatorWidth, sThumbSize);
    x += opAdd.width() + sOperatorGap;

    // Input C
    QRect c(x, y, sThumbSize, sThumbSize);

    drawParen(painter, parenL, true);
    drawParen(painter, parenR, false);

    drawInput(painter, a, 0);
    drawInput(painter, b, 1);
    drawInput(painter, c, 2);

    painter.setPen(sEquationColor);
    painter.setFont(equationFont());
    painter.drawText(opMul, Qt::AlignCenter, "×");
    painter.drawText(opAdd, Qt::AlignCenter, "+");
    painter.restore();
}

void CombinerStageWidget::drawBinaryOp(QPainter& painter, const QRect& drawRect, const QString& operand) {
    painter.save();
    const s32 totalWidth = (sThumbSize * 2) + sOperatorWidth + (sOperatorGap * 2);

    // Start offset
    s32 x = drawRect.center().x() - totalWidth / 2;
    s32 y = drawRect.center().y() - sThumbSize / 2;

    // Input A
    QRect a(x, y, sThumbSize, sThumbSize);
    x += sThumbSize + sOperatorGap;

    // Operator
    QRect op(x, y, sOperatorWidth, sThumbSize);
    x += op.width() + sOperatorGap;

    // Input B
    QRect b(x, y, sThumbSize, sThumbSize);

    drawInput(painter, a, 0);
    drawInput(painter, b, 1);

    painter.setPen(sEquationColor);
    painter.setFont(equationFont());
    painter.drawText(op, Qt::AlignCenter, operand);
    painter.restore();
}

QImage CombinerStageWidget::scaleForThumbnail(const QImage& src) const {
    if (src.isNull()) {
        return {sThumbSize, sThumbSize, QImage::Format_RGBA8888};
    }

    auto srcSize = src.size();
    double scale = std::min(double(sThumbSize) / srcSize.width(), double(sThumbSize) / srcSize.height());

    s32 newW = s32(srcSize.width() * scale);
    s32 newH = s32(srcSize.height() * scale);

    QImage scaled = src.scaled(newW, newH, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QImage out(sThumbSize, sThumbSize, QImage::Format_RGBA8888);
    out.fill(Qt::transparent);

    QPainter painter(&out);
    s32 x = (sThumbSize - newW) / 2;
    s32 y = (sThumbSize - newH) / 2;
    painter.drawImage(x, y, scaled);
    return out;
}

QImage CombinerStageWidget::getTransformedInput(s32 inputIdx) const {
    const auto input = mStage.inputs[inputIdx];
    const auto operand = mStage.operands[inputIdx];

    QImage src;

    switch (input) {
    case Combiner::Input::Texture:
        if (mTextureSrc && mTextureSrc->isValid()) {
            src = scaleForThumbnail(mTextureSrc->get()->textureData());
        }
        break;
    case Combiner::Input::Constant:
        if (mConstantSrc) {
            QColor c;
            c.setRgbF(
                mConstantSrc->r / 255.0f,
                mConstantSrc->g / 255.0f,
                mConstantSrc->b / 255.0f);
            src = colorToImage(c);
        }
        break;
    case Combiner::Input::Primary:
        if (mPrimarySrc) {
            QColor c;
            c.setRgbF(mPrimarySrc->r, mPrimarySrc->g, mPrimarySrc->b);
            src = colorToImage(c);
        }
        break;
    case Combiner::Input::Previous:
        if (mPrevSrc) {
            src = *mPrevSrc;
        }
        break;
    }

    if (src.isNull()) {
        src = QImage(sThumbSize, sThumbSize, QImage::Format_RGBA8888);
        src.fill(Qt::transparent);
    }

    QImage vis = visualizeColorOrAlpha(src);
    vis = applyOperandTransform(vis, operand);
    return vis;
}

void CombinerStageWidget::updateStageOutput() {
    const s32 w = sThumbSize;
    const s32 h = sThumbSize;

    QImage out(w, h, QImage::Format_RGBA8888);
    out.fill(Qt::transparent);

    QImage a = getTransformedInput(0);
    QImage b = getTransformedInput(1);
    QImage c = getTransformedInput(2);

    for (s32 y = 0; y < h; ++y) {
        for (s32 x = 0; x < w; ++x) {
            QColor ca = a.pixelColor(x, y);
            QColor cb = b.pixelColor(x, y);
            QColor cc = c.pixelColor(x, y);
            QColor result;

            switch (mStage.func) {
            case Combiner::Func::Replace:
                result = ca;
                break;
            case Combiner::Func::Modulate:
                result.setRed(std::clamp(ca.red() * cb.red() / 255, 0, 255));
                result.setGreen(std::clamp(ca.green() * cb.green() / 255, 0, 255));
                result.setBlue(std::clamp(ca.blue() * cb.blue() / 255, 0, 255));
                result.setAlpha(std::clamp(ca.alpha() * cb.alpha() / 255, 0, 255));
                break;
            case Combiner::Func::Subtract:
                result.setRed(std::clamp(ca.red() - cb.red(), 0, 255));
                result.setGreen(std::clamp(ca.green() - cb.green(), 0, 255));
                result.setBlue(std::clamp(ca.blue() - cb.blue(), 0, 255));
                result.setAlpha(std::clamp(ca.alpha() - cb.alpha(), 0, 255));
                break;
            case Combiner::Func::MultAdd:
                result.setRed(std::clamp((ca.red() * cb.red() / 255) + cc.red(), 0, 255));
                result.setGreen(std::clamp((ca.green() * cb.green() / 255) + cc.green(), 0, 255));
                result.setBlue(std::clamp((ca.blue() * cb.blue() / 255) + cc.blue(), 0, 255));
                result.setAlpha(std::clamp((ca.alpha() * cb.alpha() / 255) + cc.alpha(), 0, 255));
                break;
            }

            if (mStage.scale == Combiner::Scale::Two) {
                result.setRed(std::min(result.red() * 2, 255));
                result.setGreen(std::min(result.green() * 2, 255));
                result.setBlue(std::min(result.blue() * 2, 255));
                result.setAlpha(std::min(result.alpha() * 2, 255));
            }

            out.setPixelColor(x, y, result);
        }
    }
    mOutputImage = out;
}

QFont CombinerStageWidget::equationFont() const {
    QFont f = font();
    f.setPixelSize(sThumbSize / 2);
    f.setWeight(QFont::Medium);
    return f;
}

QFont CombinerStageWidget::scaleFont() const {
    QFont f = font();
    f.setPixelSize(sThumbSize);
    f.setWeight(QFont::Medium);
    return f;
}

s32 CombinerStageWidget::calcEquationWidth() const {
    s32 width = 0;

    switch (mStage.func) {
    case Combiner::Func::Replace:
        width = sThumbSize;
        break;
    case Combiner::Func::Modulate:
    case Combiner::Func::Subtract:
        width = (sThumbSize * 2) + sOperatorWidth + (sOperatorGap * 2);
        break;
    case::Combiner::Func::MultAdd:
        width = (sThumbSize * 2) + (sOperatorWidth * 2) + (sParenW * 2) + (sOperatorGap * 6);
        break;
    }

    if (mStage.scale == Combiner::Scale::Two) {
        QFontMetrics fm(scaleFont());
        width += fm.horizontalAdvance("×2");
    }

    return width;
}

constexpr s32 CombinerStageWidget::calcEquationHeight() const {
    return sParenH + sLabelHeight + sLabelGap;
}

void CombinerStageWidget::drawScaleLabel(QPainter& painter, const QRect& drawRect) {
    if (mStage.scale != Combiner::Scale::Two) {
        return;
    }

    painter.save();
    const QString text = "×2";
    painter.setPen(sScaleLabelColor);
    painter.setFont(scaleFont());

    QFontMetrics fm(painter.fontMetrics());

    const s32 textW = fm.horizontalAdvance(text);
    const s32 textH = fm.height();

    QRect labelRect(
        drawRect.right(),
        drawRect.center().y() - textH / 2,
        textW,
        textH
    );

    painter.drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter, text);
    painter.restore();
}

void CombinerStageWidget::drawStageLabel(QPainter& painter, const QRect& drawRect) {
    painter.save();

    const QString stageType = (mStage.type == Combiner::StageType::Color)
        ? QStringLiteral("Color")
        : QStringLiteral("Alpha");

    QString text = QStringLiteral("Stage %1 - %2").arg(mStageNum).arg(stageType);

    painter.setPen(sStageLabelColor);

    painter.drawText(
        drawRect.left() + (sPadding / 2),
        drawRect.top() + (sPadding / 2) + painter.fontMetrics().ascent(),
        text
    );
    painter.restore();
}

void CombinerStageWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);

    QRect body = contentsRect();

    const s32 eqWidth = calcEquationWidth();
    const s32 eqHeight = calcEquationHeight();
    QRect eqRect(body.center().x() - eqWidth / 2, body.center().y() - eqHeight / 2, eqWidth, eqHeight);

    drawStageLabel(painter, body);
    drawEquation(painter, eqRect);
    drawScaleLabel(painter, eqRect);
}

QSize CombinerStageWidget::sizeHint() const {
    return { calcEquationWidth() + (sPadding * 2), calcEquationHeight() + (sPadding * 2) };
}

// ========================================================================== //


CombinerPreviewWidget::CombinerPreviewWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QGridLayout(this);

    mainLayout->addWidget(&mColorStage0, 0, 0);
    mainLayout->addWidget(&mColorStage1, 0, 1);
    mainLayout->addWidget(&mAlphaStage0, 1, 0);
    mainLayout->addWidget(&mAlphaStage1, 1, 1);
}

void CombinerPreviewWidget::setConfig(s32 configIdx) {
    mConfigIdx = configIdx;

    mColorStage0.setStage(Combiner::configurations[mConfigIdx].colorStage[0], 0);
    mColorStage1.setStage(Combiner::configurations[mConfigIdx].colorStage[1], 1);

    mAlphaStage0.setStage(Combiner::configurations[mConfigIdx].alphaStage[0], 0);
    mAlphaStage1.setStage(Combiner::configurations[mConfigIdx].alphaStage[1], 1);
}

void CombinerPreviewWidget::setCombinerSrc(const Ptcl::TextureHandle* texture, const Ptcl::binColor3f* constant, const Ptcl::binColor4f* primary) {
    mColorStage0.setCombinerSrc(texture, constant, primary, nullptr);
    mColorStage1.setCombinerSrc(texture, constant, primary, &mColorStage0.getStageOutput());
    mAlphaStage0.setCombinerSrc(texture, constant, primary, nullptr);
    mAlphaStage1.setCombinerSrc(texture, constant, primary, &mAlphaStage0.getStageOutput());
}

void CombinerPreviewWidget::updateStages() {
    mColorStage0.updateStage();
    mColorStage1.updateStage();
    mAlphaStage0.updateStage();
    mAlphaStage1.updateStage();
}


// ========================================================================== //


CombinerPropertiesWidget::CombinerPropertiesWidget(QWidget* parent) :
    QWidget{parent} {

    auto* mainLayout = new QFormLayout(this);

    mFogCheckBox.setText("Enable Fog");

    mainLayout->addRow("Fog:", &mFogCheckBox);
    mainLayout->addRow("Blend Function:", &mBlendFuncComboBox);
    mainLayout->addRow("Depth Function:", &mDepthFuncComboBox);
    mainLayout->addRow("Combiner Function:", &mCombinerFuncComboBox);
    mainLayout->addWidget(&mCombinerPreview);

    connect(&mFogCheckBox, &QCheckBox::clicked, this, [this](bool checked) {
        mProps.isFogEnabled = checked;
        emit propertiesUpdated(mProps);
    });

    connect(&mBlendFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.blendFunc = mBlendFuncComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    connect(&mDepthFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        mProps.depthFunc = mDepthFuncComboBox.currentEnum();
        emit propertiesUpdated(mProps);
    });

    connect(&mCombinerFuncComboBox, &QComboBox::currentIndexChanged, this, [this]() {
        auto value = mCombinerFuncComboBox.currentEnum();
        mProps.combinerFunc = value;
        mCombinerPreview.setConfig(static_cast<s32>(value));
        emit propertiesUpdated(mProps);
    });
}

void CombinerPropertiesWidget::setProperties(const Ptcl::Emitter::CombinerProperties& properties) {
    QSignalBlocker b1(mBlendFuncComboBox);
    QSignalBlocker b2(mDepthFuncComboBox);
    QSignalBlocker b3(mCombinerFuncComboBox);
    QSignalBlocker b4(mFogCheckBox);

    mProps = properties;

    mFogCheckBox.setChecked(mProps.isFogEnabled);
    mBlendFuncComboBox.setCurrentEnum(mProps.blendFunc);
    mDepthFuncComboBox.setCurrentEnum(mProps.depthFunc);
    mCombinerFuncComboBox.setCurrentEnum(mProps.combinerFunc);
    mCombinerPreview.setConfig(static_cast<s32>(mProps.combinerFunc));
}

void CombinerPropertiesWidget::setCombinerSrc(const Ptcl::TextureHandle* texture, const Ptcl::binColor3f* constant, const Ptcl::binColor4f* primary) {
    mCombinerPreview.setCombinerSrc(texture, constant, primary);
}

void CombinerPropertiesWidget::updateCombinerPreview() {
    mCombinerPreview.updateStages();
}


// ========================================================================== //
