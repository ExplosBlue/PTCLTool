#include "editor/inspector/textureInspector.h"

#include "editor/textureSelectDialog.h"

#include "util/paintUtil.h"

#include <QApplication>
#include <QGuiApplication>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QToolButton>
#include <QVBoxLayout>
#include <QGroupBox>


namespace PtclEditor {


// ========================================================================== //


TextureRepetitionSelector::TextureRepetitionSelector(QWidget* parent) :
    QWidget{parent} {

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setMinimumSize(128, 128);
    setMaximumSize(1024, 1024);
}

QSize TextureRepetitionSelector::sizeHint() const {
    return {512, 512};
}

void TextureRepetitionSelector::setSource(const QImage& image, s32 divX, s32 divY) {
    mTexture = image;
    mDivX = std::max(1, divX);
    mDivY = std::max(1, divY);
    updateLayoutCache();
    update();
}

void TextureRepetitionSelector::setRepetitions(s32 repX, s32 repY) {
    mRepX = std::max(1, repX);
    mRepY = std::max(1, repY);
    updateLayoutCache();
    update();
}

void TextureRepetitionSelector::updateLayoutCache() {
    if (mTexture.isNull()) {
        return;
    }

    const QSize widgetSize = size();

    QSizeF virtualSize{
        static_cast<f32>(mTexture.width()) * UVScale,
        static_cast<f32>(mTexture.height()) * UVScale
    };

    mScale = std::min(
        static_cast<f32>(widgetSize.width()) / static_cast<f32>(virtualSize.width()),
        static_cast<f32>(widgetSize.height()) / static_cast<f32>(virtualSize.height())
    );

    mScaledSize = QSize{
        static_cast<s32>(std::floor(virtualSize.width() * mScale)),
        static_cast<s32>(std::floor(virtualSize.height() * mScale))
    };

    const QPoint topLeft{
        (width() - mScaledSize.width()) / 2,
        (height() - mScaledSize.height()) / 2
    };

    mTexRect = QRect(topLeft, mScaledSize);
}

void TextureRepetitionSelector::drawTexture(QPainter& painter) {
    const s32 texW = mTexture.width();
    const s32 texH = mTexture.height();

    const s32 w = mTexRect.width();
    const s32 h = mTexRect.height();

    QImage output(w, h, QImage::Format::Format_ARGB32);

    for (s32 y = 0; y < h; ++y){
        for (s32 x = 0; x < w; ++x) {
            const f32 u = (static_cast<f32>(x) / static_cast<f32>(w)) * UVScale;
            const f32 v = (static_cast<f32>(y) / static_cast<f32>(h)) * UVScale;

            const f32 mu = PaintUtil::mirrorTextureCoord(u);
            const f32 mv = PaintUtil::mirrorTextureCoord(v);

            const s32 tx = std::clamp(static_cast<s32>(mu * static_cast<f32>(texW)), 0, texW - 1);
            const s32 ty = std::clamp(static_cast<s32>(mv * static_cast<f32>(texH)), 0, texH - 1);

            QColor color = mTexture.pixelColor(tx, ty);

            output.setPixelColor(x, y, color);
        }
    }

    painter.save();
    painter.drawImage(mTexRect.topLeft(), output);
    painter.restore();
}

void TextureRepetitionSelector::drawGrid(QPainter& painter) {
    const bool isDragDivision = mDragging && mDragMode == DragMode::Division;

    const s32 divX = isDragDivision ? mPreviewDivX : mDivX;
    const s32 divY = isDragDivision ? mPreviewDivY : mDivY;

    const f32 divCellW = static_cast<f32>(mTexRect.width()) / (static_cast<f32>(divX) * UVScale);
    const f32 divCellH = static_cast<f32>(mTexRect.height()) / (static_cast<f32>(divY) * UVScale);

    const QRect texRect = mTexRect;

    // --- total div is double the divisions (because UVScale = 2) ---
    const s32 totalDivX = divX * 2;
    const s32 totalDivY = divY * 2;

    auto drawLines = [&painter, divCellW, divCellH, totalDivX, totalDivY, texRect]() {
        for (s32 x = 1; x < totalDivX; ++x) {
            const s32 px = texRect.left() + static_cast<s32>(static_cast<f32>(x) * divCellW);
            painter.drawLine(px, texRect.top(), px, texRect.bottom());
        }

        for (s32 y = 1; y < totalDivY; ++y) {
            const s32 py = texRect.top() + static_cast<s32>(static_cast<f32>(y) * divCellH);
            painter.drawLine(texRect.left(), py, texRect.right(), py);
        }
    };

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(QPen(Qt::black, 3, Qt::DashLine));
    drawLines();
    painter.setPen(QPen(Qt::white, 1, Qt::DashLine));
    drawLines();

    painter.restore();
}

void TextureRepetitionSelector::updateFrameRect() {
    const bool isDragRepetition = mDragging && mDragMode == DragMode::Repetition;
    const bool isDragDivision = mDragging && mDragMode == DragMode::Division;

    const s32 repX = isDragRepetition ? mPreviewRepX : mRepX;
    const s32 repY = isDragRepetition ? mPreviewRepY : mRepY;

    const s32 divX = isDragDivision ? mPreviewDivX : mDivX;
    const s32 divY = isDragDivision ? mPreviewDivY : mDivY;

    const f32 tilesPerFrameX = static_cast<f32>(repX) / static_cast<f32>(divX);
    const f32 tilesPerFrameY = static_cast<f32>(repY) / static_cast<f32>(divY);

    const f32 tileW = static_cast<f32>(mTexRect.width()) / UVScale;
    const f32 tileH = static_cast<f32>(mTexRect.height()) / UVScale;

    const s32 frameW = static_cast<s32>(tilesPerFrameX * tileW);
    const s32 frameH = static_cast<s32>(tilesPerFrameY * tileH);

    mFrameRect = QRect(
        mTexRect.left(),
        mTexRect.top(),
        frameW,
        frameH
    );
}

void TextureRepetitionSelector::drawSelection(QPainter& painter) {
    painter.save();
    painter.setPen(QPen(Qt::yellow, 2));
    painter.drawRect(mFrameRect.adjusted(+3, +3, -3, -3));
    painter.restore();
}

void TextureRepetitionSelector::drawDimOverlay(QPainter& painter) {
    const s32 tilesX = static_cast<s32>(UVScale);
    const s32 tilesY = static_cast<s32>(UVScale);

    const s32 tileW = mTexRect.width()  / tilesX;
    const s32 tileH = mTexRect.height() / tilesY;

    painter.save();
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setRenderHint(QPainter::Antialiasing, false);

    for (s32 ty = 0; ty < tilesY; ++ty) {
        for (s32 tx = 0; tx < tilesX; ++tx) {

            if (tx == 0 && ty == 0) {
                continue;
            }

            QRect tileRect(
                mTexRect.left() + (tx * tileW),
                mTexRect.top()  + (ty * tileH),
                static_cast<s32>(tileW),
                static_cast<s32>(tileH)
            );

            if (tileRect.intersects(mFrameRect)) {
                continue;
            }

            painter.fillRect(tileRect, QColor(0, 0, 0, 160));
        }
    }
    painter.restore();
}

void TextureRepetitionSelector::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.fillRect(rect(), palette().color(QPalette::Base));

    if (mTexture.isNull()) {
        return;
    }

    updateFrameRect();

    painter.save();
    painter.setClipRect(mTexRect);
    PaintUtil::drawCheckerboard(painter, mTexRect);
    painter.restore();

    drawTexture(painter);
    drawGrid(painter);
    drawDimOverlay(painter);
    drawSelection(painter);
}

void TextureRepetitionSelector::mousePressEvent(QMouseEvent* event) {
    if (!mTexRect.contains(event->pos())) {
        return;
    }

    if (event->button() == Qt::RightButton) {
        mDragMode = DragMode::Division;
        mPreviewDivX = mDivX;
        mPreviewDivY = mDivY;
    } else if (event->button() == Qt::LeftButton) {
        mDragMode = DragMode::Repetition;
        mPreviewRepX = mRepX;
        mPreviewRepY = mRepY;
    } else {
        return;
    }

    mDragging = true;
    mDragStart = event->pos();
    mAccumulatedX = 0;
    mAccumulatedY = 0;

    setCursor(Qt::ClosedHandCursor);
    update();
}

void TextureRepetitionSelector::mouseMoveEvent(QMouseEvent* event) {
    if (!mDragging) {
        if (mTexRect.contains(event->pos())) {
            setCursor(Qt::OpenHandCursor);
        } else {
            unsetCursor();
        }
        return;
    }

    QPoint delta = event->pos() - mDragStart;
    mAccumulatedX += delta.x();
    mAccumulatedY += delta.y();
    mDragStart = event->pos();

    const f32 nx = (static_cast<f32>(event->pos().x() - mTexRect.left()) / static_cast<f32>(mTexRect.width()));
    const f32 ny = (static_cast<f32>(event->pos().y() - mTexRect.top()) / static_cast<f32>(mTexRect.height()));
    const f32 clampedX = std::clamp(nx, 0.0f, 1.0f);
    const f32 clampedY = std::clamp(ny, 0.0f, 1.0f);

    if (mDragMode == DragMode::Division) {
        const f32 pixelPerStep = 20;

        s32 stepX = static_cast<s32>(static_cast<f32>(mAccumulatedX) / pixelPerStep);
        s32 stepY = static_cast<s32>(static_cast<f32>(mAccumulatedY) / pixelPerStep);

        mAccumulatedX -= static_cast<s32>(static_cast<f32>(stepX) * pixelPerStep);
        mAccumulatedY -= static_cast<s32>(static_cast<f32>(stepY) * pixelPerStep);

        const s32 maxCells = 16;

        mPreviewDivX = std::clamp(mPreviewDivX + stepX, 1, maxCells);
        mPreviewDivY = std::clamp(mPreviewDivY + stepY, 1, maxCells);

        while (mPreviewDivX * mPreviewDivY > maxCells) {
            if (mPreviewDivX > mPreviewDivY) {
                --mPreviewDivX;
            } else {
                --mPreviewDivY;
            }
        }
    }
    else if (mDragMode == DragMode::Repetition) {
        const s32 maxRepX = mDivX * 2;
        const s32 maxRepY = mDivY * 2;

        mPreviewRepX = std::clamp(static_cast<s32>(clampedX * static_cast<f32>(maxRepX)) + 1, 1, maxRepX);
        mPreviewRepY = std::clamp(static_cast<s32>(clampedY * static_cast<f32>(maxRepY)) + 1, 1, maxRepY);
    }

    update();
}

void TextureRepetitionSelector::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);

    if (!mDragging) {
        return;
    }

    mDragging = false;

    if (mDragMode == DragMode::Division) {
        if (mPreviewDivX != mDivX || mPreviewDivY != mDivY) {
            mDivX = mPreviewDivX;
            mDivY = mPreviewDivY;
            emit divisionsChanged(mDivX, mDivY);
        }
    }
    else if (mDragMode == DragMode::Repetition) {
        if (mPreviewRepX != mRepX || mPreviewRepY != mRepY) {
            mRepX = mPreviewRepX;
            mRepY = mPreviewRepY;
            emit repetitionsChanged(mRepX, mRepY);
        }
    }

    mDragMode = DragMode::None;

    setCursor(mTexRect.contains(mapFromGlobal(QCursor::pos())) ? Qt::OpenHandCursor : Qt::ArrowCursor);
    update();
}

void TextureRepetitionSelector::resizeEvent(QResizeEvent* event) {
    const s32 side = std::min(width(), height());
    resize(side, side);

    updateLayoutCache();
    QWidget::resizeEvent(event);
}


// ========================================================================== //


TextureInspector::TextureInspector(QWidget* parent) :
    InspectorWidgetBase{parent} {

    // Texture Preview
    // mTexturePreview.setThumbnailSize(QSize(64, 64));
    mTexPatFreq.setMinimum(1);
    mAnimFrameCountSpinBox.setRange(2, 16);

    mChangeTextureButton.setText("Change Texture");

    // Frame Timeline
    mFrameTimeline.setFrames(std::vector<s32>(16, 0), 2);

    // Texture Settings Layout
    auto settingsLayout = new QGridLayout;
    settingsLayout->addWidget(new QLabel("Wrap U:"), 0, 0);
    settingsLayout->addWidget(&mWrapTComboBox, 0, 1);
    settingsLayout->addWidget(new QLabel("Wrap V:"), 1, 0);
    settingsLayout->addWidget(&mWrapSComboBox, 1, 1);
    settingsLayout->addWidget(new QLabel("Magnification Filter:"), 0, 2);
    settingsLayout->addWidget(&mMagFilterComboBox, 0, 3);
    settingsLayout->addWidget(new QLabel("Minification Filter:"), 1, 2);
    settingsLayout->addWidget(&mMinFilterComboBox, 1, 3);
    settingsLayout->addWidget(new QLabel("Mipmap Filter:"), 2, 2);
    settingsLayout->addWidget(&mMipFilterComboBox, 2, 3);
    settingsLayout->setColumnStretch(1, 1);
    settingsLayout->setColumnStretch(3, 1);

    auto textureConfigLayout = new QGridLayout;
    // textureConfigLayout->addWidget(new QLabel("Texture Divisions:"), 0, 0);
    // textureConfigLayout->addWidget(&mDivisionSelector, 1, 0);
    textureConfigLayout->addWidget(&mChangeTextureButton, 2, 0);
    textureConfigLayout->addWidget(new QLabel("Texture Sampling:"), 0, 0);
    textureConfigLayout->addWidget(&mRepetitionSelector, 1, 0);

    // Anim Mode
    mAnimModeComboBox.addItems({"Fixed Rate", "Fit to Life"});
    mAnimModeComboBox.setItemData(0, "Frames advance at a fixed interval, independent of particle lifetime", Qt::ToolTipRole);
    mAnimModeComboBox.setItemData(1, "Frames are distributed evenly across the particle's lifetime", Qt::ToolTipRole);

    // Texture Pattern Anim
    mTexPatGroupBox.setTitle("Texture Pattern Animation");

    auto texPatSettingsLayout = new QVBoxLayout;
    texPatSettingsLayout->setSpacing(4);

    auto controlsRow = new QHBoxLayout;
    controlsRow->addWidget(new QLabel("Animation Mode:"));
    controlsRow->addWidget(&mAnimModeComboBox);
    controlsRow->addSpacing(8);
    controlsRow->addWidget(new QLabel("Interval:"));
    controlsRow->addWidget(&mTexPatFreq);
    mTexPatFreq.setToolTip("Number of ticks between each animation frame (Fixed Rate mode only)");
    controlsRow->addSpacing(8);
    controlsRow->addWidget(new QLabel("Animation Frames:"));
    controlsRow->addWidget(&mAnimFrameCountSpinBox);
    mAnimFrameCountSpinBox.setToolTip("Number of frames in the animation sequence");

    mFramesWarningLabel = new QLabel(this);
    mFramesWarningLabel->setText("Frame count exceeds lifespan");
    mFramesWarningLabel->setStyleSheet("QLabel { color: #cc8800; font-weight: bold; }");
    mFramesWarningLabel->setVisible(false);
    controlsRow->addWidget(mFramesWarningLabel);
    controlsRow->addStretch();

    texPatSettingsLayout->addLayout(controlsRow);
    texPatSettingsLayout->addWidget(&mFrameTimeline);

    mTexPatGroupBox.setLayout(texPatSettingsLayout);

    mStartFrameList.setToolTip("Possible spawning frames.\nOne is randomly selected\nwhen a particle spawns.");

    // Main Layout
    auto mainLayout = new QGridLayout(this);
    mainLayout->addLayout(settingsLayout, 0, 0);
    mainLayout->addLayout(textureConfigLayout, 1, 0);
    mainLayout->addWidget(&mStartFrameList, 2, 0);

    auto* animContainer = new QVBoxLayout;
    mTexPatAnimCheckBox.setText("Enable Animation");
    animContainer->addWidget(&mTexPatAnimCheckBox);
    animContainer->addWidget(&mTexPatGroupBox);
    mainLayout->addLayout(animContainer, 3, 0);
    mainLayout->setColumnStretch(0, 1);

    setLayout(mainLayout);
    setupConnections();
}

void TextureInspector::setupConnections() {
    connect(&mChangeTextureButton, &QPushButton::clicked, this, &TextureInspector::changeTexture);

    // Wrap T
    connect(&mWrapTComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto wrap = mWrapTComboBox.currentEnum();
        setEmitterProperty(
            "Set Texture Wrap U",
            "SetTexWrapU",
            &Ptcl::Emitter::textureWrapT,
            &Ptcl::Emitter::setTextureWrapT,
            wrap
        );
    });

    // Wrap S
    connect(&mWrapSComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto wrap = mWrapSComboBox.currentEnum();
        setEmitterProperty(
            "Set Texture Wrap V",
            "SetTexWrapV",
            &Ptcl::Emitter::textureWrapS,
            &Ptcl::Emitter::setTextureWrapS,
            wrap
        );
    });

    // Mag Filter
    connect(&mMagFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto filter = mMagFilterComboBox.currentEnum();
        setEmitterProperty(
            "Set Texture Mag Filter",
            "SetTexMagFilter",
            &Ptcl::Emitter::textureMagFilter,
            &Ptcl::Emitter::setTextureMagFilter,
            filter
        );
    });

    // Min Filter
    connect(&mMinFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto filter = mMinFilterComboBox.currentEnum();
        setEmitterProperty(
            "Set Texture Min Filter",
            "SetTexMinFilter",
            &Ptcl::Emitter::textureMinFilter,
            &Ptcl::Emitter::setTextureMinFilter,
            filter
        );
    });

    // Mipmap Filter
    connect(&mMipFilterComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        Q_UNUSED(index);
        const auto filter = mMipFilterComboBox.currentEnum();
        setEmitterProperty(
            "Set Texture Mipmap Filter",
            "SetTexMipFilter",
            &Ptcl::Emitter::textureMipFilter,
            &Ptcl::Emitter::setTextureMipFilter,
            filter
        );
    });

    // Texture pattern count
    connect(&mStartFrameList, &StartFrameList::slotCountChanged, this, [this](u16 value) {
        setEmitterProperty(
            "Changed Start Frame Count",
            "SetNumTexPat",
            &Ptcl::Emitter::numTexturePattern,
            &Ptcl::Emitter::setNumTexturePattern,
            value
        );
    });

    connect(&mRepetitionSelector, &TextureRepetitionSelector::divisionsChanged, this, [this](s32 x, s32 y) {
        mDocument->undoStack()->beginMacro(formatHistoryLabel("Set Texture Divisions"));
        setEmitterProperty(
            "Set Texture Divisions X",
            "SetTexDivX",
            &Ptcl::Emitter::numTextureDivisionX,
            &Ptcl::Emitter::setNumTextureDivisionX,
            static_cast<u8>(x)
            );

        setEmitterProperty(
            "Set Texture Divisions Y",
            "SetTexDivY",
            &Ptcl::Emitter::numTextureDivisionY,
            &Ptcl::Emitter::setNumTextureDivisionY,
            static_cast<u8>(y)
            );
        mDocument->undoStack()->endMacro();

        // Clamp start frame variants to the new max
        const s32 totalCells = static_cast<s32>(x) * static_cast<s32>(y);
        mStartFrameList.setSlotMax(totalCells);
    });

    connect(&mTexPatFreq, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        mFrameTimeline.setFrameStep(static_cast<s32>(value));
        setEmitterProperty(
            "Set Texture Anim Frame Step",
            "SetTexPatFreq",
            &Ptcl::Emitter::texturePatternFrequency,
            &Ptcl::Emitter::setTexturePatternFrequency,
            static_cast<u16>(value)
        );
    });

    connect(&mAnimFrameCountSpinBox, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture Anim Frame Count",
            "SetTexPatFrameCount",
            &Ptcl::Emitter::texturePatternFrameCount,
            &Ptcl::Emitter::setTexturePatternFrameCount,
            static_cast<u16>(value)
        );
    });

    connect(&mFrameTimeline, &FrameTimeline::frameEdited, this, [this](s32 index, s32 value) {
        auto table = mEmitter->texturePatternTable();
        table[index] = static_cast<u8>(value);

        setEmitterProperty(
            "Set Texture Anim Frame Order",
            QString("SetTexPatTbl_%1").arg(index),
            &Ptcl::Emitter::texturePatternTable,
            &Ptcl::Emitter::setTexturePatternTable,
            table
        );
    });


    connect(&mRepetitionSelector, &TextureRepetitionSelector::repetitionsChanged, this, [this](s32 x, s32 y) {
        setEmitterProperty(
            "Set Texture Repetition",
            "SetTexRep",
            &Ptcl::Emitter::textureRepetitions,
            &Ptcl::Emitter::setTextureRepetitions,
            Math::Vector2i{x, y}
        );
    });

    connect(&mAnimModeComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        auto mode = static_cast<FrameAnimMode>(index);
        mFrameTimeline.setAnimMode(mode);

        u16 freq;
        if (mode == FrameAnimMode::FitToLife) {
            freq = 0;
        } else {
            freq = 1;
        }

        setEmitterProperty(
            "Set Texture Anim Mode",
            "SetTexAnimMode",
            &Ptcl::Emitter::texturePatternFrequency,
            &Ptcl::Emitter::setTexturePatternFrequency,
            freq
        );
    });

    connect(&mTexPatAnimCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
        mTexPatGroupBox.setVisible(checked);
        setEmitterProperty(
            "Toggle Texture Anim",
            "ToggleTexAnim",
            &Ptcl::Emitter::isTexturePatternAnim,
            &Ptcl::Emitter::setIsTexturePatternAnim,
            checked
        );
    });
}

void TextureInspector::populateProperties() {
    QSignalBlocker b1(mWrapTComboBox);
    QSignalBlocker b2(mWrapSComboBox);
    QSignalBlocker b3(mMagFilterComboBox);
    QSignalBlocker b4(mMinFilterComboBox);
    QSignalBlocker b5(mMipFilterComboBox);
    // QSignalBlocker b6(mNumTexPat);
    QSignalBlocker b7(mTexPatFreq);
    QSignalBlocker b8(mAnimFrameCountSpinBox);
    QSignalBlocker b9(mTexPatAnimCheckBox);
    QSignalBlocker b10(mAnimModeComboBox);

    const auto& texData = mEmitter->textureHandle()->textureData();
    mRepetitionSelector.setSource(texData, mEmitter->numTextureDivisionX(), mEmitter->numTextureDivisionY());
    mRepetitionSelector.setRepetitions(mEmitter->numTextureRepetitionsX(), mEmitter->numTextureRepetitionsY());

    const TextureFrameInfo frameInfo{
        .texture = &texData,
        .divX = mEmitter->numTextureDivisionX(),
        .divY = mEmitter->numTextureDivisionY(),
        .repX = mEmitter->numTextureRepetitionsX(),
        .repY = mEmitter->numTextureRepetitionsY(),
    };
    mFrameTimeline.setSource(frameInfo);

    mWrapTComboBox.setCurrentEnum(mEmitter->textureWrapT());
    mWrapSComboBox.setCurrentEnum(mEmitter->textureWrapS());
    mMagFilterComboBox.setCurrentEnum(mEmitter->textureMagFilter());
    mMinFilterComboBox.setCurrentEnum(mEmitter->textureMinFilter());
    mMipFilterComboBox.setCurrentEnum(mEmitter->textureMipFilter());

    // Populate variant slots
    mStartFrameList.setSource(frameInfo);
    mStartFrameList.setSlotMax(maxFrameCount());
    mStartFrameList.setSlotCount(mEmitter->numTexturePattern());

    const auto freq = mEmitter->texturePatternFrequency();
    const auto mode = freqToAnimMode(freq);

    mTexPatFreq.setValue(freq);
    mAnimModeComboBox.setCurrentIndex(static_cast<s32>(mode));
    mFrameTimeline.setAnimMode(mode);
    mFrameTimeline.setFrameStep(static_cast<s32>(freq));
    mFrameTimeline.setPtclLife(mEmitter->ptclLife());

    if (mode == FrameAnimMode::FitToLife) {
        mTexPatFreq.setDisabled(true);
    } else {
        mTexPatFreq.setDisabled(false);
    }

    mAnimFrameCountSpinBox.setValue(mEmitter->texturePatternFrameCount());

    updateFramesWarning();

    {
        const auto& tbl = mEmitter->texturePatternTable();
        std::vector<s32> frames(tbl.begin(), tbl.end());
        mFrameTimeline.setFrames(frames, mEmitter->texturePatternFrameCount());
    }

    const bool animEnabled = mEmitter->isTexturePatternAnim();
    mTexPatAnimCheckBox.setChecked(animEnabled);
    mTexPatGroupBox.setVisible(animEnabled);
}

void TextureInspector::updateFramesWarning() {
    if (!mEmitter) {
        return;
    }
    const s32 frameCount = mAnimFrameCountSpinBox.value();
    const s32 life = mEmitter->ptclLife();
    const bool showWarning = frameCount > life;
    mFramesWarningLabel->setVisible(showWarning);
}

void TextureInspector::changeTexture() {
    const auto& textureList = mDocument->textures();

    TextureSelectDialog dialog(textureList, this);
    if (dialog.exec() == QDialog::Accepted) {
        s32 selectedIndex = dialog.selectedIndex();
        if (selectedIndex >= 0 && static_cast<size_t>(selectedIndex) < textureList.size()) {
            const auto texture = textureList.at(selectedIndex).get();
            setEmitterProperty(
                "Set Texture",
                "SetEmitterTexture",
                &Ptcl::Emitter::texture,
                &Ptcl::Emitter::setTexture,
                texture
            );
        }
    }
}

s32 TextureInspector::maxFrameCount() const {
    return mEmitter->numTextureDivisionX() * mEmitter->numTextureDivisionY();
}

std::optional<Math::Vector2f> TextureInspector::calcFrameUVOffset(s32 frame) const {
    const auto divX = mEmitter->numTextureDivisionX();
    const auto divY = mEmitter->numTextureDivisionY();

    if (frame < 0 || divX <= 0 || divY <= 0) {
        return std::nullopt;
    }

    if (frame >= maxFrameCount()) {
        return std::nullopt;
    }

    const s32 frameX = frame % divX;
    const s32 frameY = frame / divX;

    const f32 stepX = 1.0f / static_cast<f32>(divX);
    const f32 stepY = 1.0f / static_cast<f32>(divY);

    return Math::Vector2f{
        stepX * static_cast<f32>(frameX),
        stepY * static_cast<f32>(frameY),
    };
}

QImage TextureInspector::getFrameTexture(s32 frame) const {
    if (!mEmitter->textureHandle().isValid()) {
        return {};
    }

    const s32 divX = mEmitter->numTextureDivisionX();
    const s32 divY = mEmitter->numTextureDivisionY();

    auto uv = calcFrameUVOffset(frame);
    if (!uv) {
        return {};
    }

    const auto& src = mEmitter->textureHandle()->textureData();

    const f32 texW = static_cast<f32>(src.width());
    const f32 texH = static_cast<f32>(src.height());

    const f32 u = uv->getX();
    const f32 v = uv->getY();

    const s32 x = static_cast<s32>(std::round(u * texW));
    const s32 y = static_cast<s32>(std::round(v * texH));

    const s32 w = static_cast<s32>(std::round(texW / static_cast<f32>(divX)));
    const s32 h = static_cast<s32>(std::round(texH / static_cast<f32>(divY)));

    QRect rect(x, y, w, h);
    rect = rect.intersected(src.rect());

    return rect.isValid() ? src.copy(rect) : QImage{};
}

QImage TextureInspector::applyUVRepetition(const QImage& image, f32 repeatX, f32 repeatY) const {
    if (image.isNull()) {
        return {};
    }

    const f32 baseW = static_cast<f32>(image.width());
    const f32 baseH = static_cast<f32>(image.height());

    const s32 outW = static_cast<s32>(std::round(baseW * repeatX));
    const s32 outH = static_cast<s32>(std::round(baseH * repeatY));

    QImage out(outW, outH, image.format());
    out.fill(Qt::transparent);

    if (out.isNull()) {
        return {};
    }

    QPainter p(&out);

    for (s32 y = 0; y < outH; y += image.height()) {
        bool mirrorY = (y / image.height()) % 2 != 0;
        for (s32 x = 0; x < outW; x += image.width()) {
            bool mirrorX = (x / image.width()) % 2 != 0;

            QImage tile = image;
            if (mirrorX) { tile = tile.flipped(Qt::Horizontal); }
            if (mirrorY) { tile = tile.flipped(Qt::Vertical); }
            p.drawImage(x, y, tile);
        }
    }

    return out;
}


// ========================================================================== //


} // namespace PtclEditor
