#include "editor/inspector/textureInspector.h"

#include "editor/textureSelectDialog.h"

#include "util/paintUtil.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QGroupBox>


namespace PtclEditor {


// ========================================================================== //


TextureDivisionSelector::TextureDivisionSelector(QWidget* parent) :
    QWidget{parent} {

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setMinimumSize(128, 128);
    setMaximumSize(512, 512);
}

QSize TextureDivisionSelector::sizeHint() const {
    return {256, 256};
}

void TextureDivisionSelector::setDivisions(s32 x, s32 y) {
    mDivX = x;
    mDivY = y;
    update();
}

void TextureDivisionSelector::setTexture(const QImage& image) {
    mTexture = image;
    updateLayoutCache();
    update();
}

void TextureDivisionSelector::updateLayoutCache() {
    if (mTexture.isNull()) {
        return;
    }

    const QSize imgSize = mTexture.size();
    const QSize widgetSize = size();

    mScale = std::min(
        widgetSize.width() / static_cast<qreal>(imgSize.width()),
        widgetSize.height() / static_cast<qreal>(imgSize.height())
        );

    mScaledSize = QSize{
        static_cast<s32>(std::floor(imgSize.width() * mScale)),
        static_cast<s32>(std::floor(imgSize.height() * mScale))
    };

    const QPoint topLeft{
        (width() - mScaledSize.width()) / 2,
        (height() - mScaledSize.height()) / 2
    };

    mTexRect = QRect(topLeft, mScaledSize);

    mScaledPixmap = QPixmap::fromImage(mTexture).scaled(mScaledSize);
}

void TextureDivisionSelector::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    const auto& pal = palette();

    painter.fillRect(rect(), pal.color(QPalette::Base));

    if (!mTexture.isNull()) {

        const QRect texRect = mTexRect;

        painter.save();
        painter.setClipRect(texRect);
        PaintUtil::drawCheckerboard(painter, texRect);
        painter.restore();

        const QPoint topLeft{
            (width() - mScaledSize.width()) / 2,
            (height() - mScaledSize.height()) / 2
        };

        painter.drawPixmap(mTexRect.topLeft(), mScaledPixmap);

        const s32 divX = mDragging ? mPreviewDivX : mDivX;
        const s32 divY = mDragging ? mPreviewDivY : mDivY;

        const f32 cellW = static_cast<f32>(texRect.width()) / static_cast<f32>(divX);
        const f32 cellH = static_cast<f32>(texRect.height()) / static_cast<f32>(divY);

        auto drawLines = [&painter, divX, divY, cellW, cellH, texRect]() {
            for (s32 x = 1; x < divX; ++x) {
                const s32 px = texRect.left() + (texRect.width() * x) / divX;
                painter.drawLine(px, texRect.top(), px, texRect.bottom());
            }
            for (s32 y = 1; y < divY; ++y) {
                const s32 py = texRect.top() + (texRect.height() * y) / divY;
                painter.drawLine(texRect.left(), py, texRect.right(), py);
            }
        };

        painter.save();

        if (mDragging) {
            painter.setCompositionMode(QPainter::CompositionMode_Difference);
            QColor color = Qt::white;
            color.setAlpha(180);
            painter.setPen(QPen(color, 2));
        } else {
            painter.setPen(QPen(Qt::black, 3));
            drawLines();
            painter.setPen(QPen(Qt::white, 1));
        }
        drawLines();

        painter.restore();
    }
}

void TextureDivisionSelector::mousePressEvent(QMouseEvent* event) {
    if(!mTexRect.contains(event->pos())) {
        return;
    }

    mDragging = true;
    setCursor(Qt::ClosedHandCursor);

    mDragStart = event->pos();
    mPreviewDivX = mDivX;
    mPreviewDivY = mDivY;
    update();
}

void TextureDivisionSelector::mouseMoveEvent(QMouseEvent* event) {
    if (!mDragging) {
        if (mTexRect.contains(event->pos())) {
            setCursor(Qt::OpenHandCursor);
        } else {
            unsetCursor();
        }
        return;
    }

    const QPoint delta = event->pos() - mDragStart;

    // TODO: should this be derrived from texture size somehow?
    const s32 maxDiv = 8;

    const f32 normX = static_cast<f32>(delta.x()) / static_cast<f32>(mTexRect.width());
    const f32 normY = static_cast<f32>(delta.y()) / static_cast<f32>(mTexRect.height());

    mPreviewDivX = std::clamp(mDivX + static_cast<s32>(normX * maxDiv), 1, maxDiv);
    mPreviewDivY = std::clamp(mDivY + static_cast<s32>(normY * maxDiv), 1, maxDiv);

    update();
}

void TextureDivisionSelector::mouseReleaseEvent(QMouseEvent* event) {
    if (!mDragging) {
        return;
    }

    mDragging = false;
    setCursor(Qt::OpenHandCursor);

    if (mPreviewDivX != mDivX || mPreviewDivY != mDivY) {
        mDivX = mPreviewDivX;
        mDivY = mPreviewDivY;
        emit divisionsChanged(mDivX, mDivY);
    }
    update();
}

void TextureDivisionSelector::resizeEvent(QResizeEvent* event) {
    const s32 side = std::min(width(), height());
    resize(side, side);

    updateLayoutCache();
    QWidget::resizeEvent(event);
}


// ========================================================================== //


TextureRepetitionSelector::TextureRepetitionSelector(QWidget* parent) :
    QWidget{parent} {

    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setMaximumSize(128, 128);
    setMaximumSize(512, 512);
}

QSize TextureRepetitionSelector::sizeHint() const {
    return {256, 256};
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
    update();
}

s32 mirrorIndex(s32 i, s32 max) {
    if (max <= 1) {
        return 0;
    }

    const s32 period = (max - 1) * 2;
    i = i % period;

    if (i >= max) {
        i = period - i;
    }

    return i;
}

void TextureRepetitionSelector::updateLayoutCache() {
    if (mTexture.isNull()) {
        return;
    }

    const QSize imgSize = mTexture.size();
    const QSize widgetSize = size();

    mScale = std::min(
        static_cast<f32>(widgetSize.width()) / static_cast<f32>(imgSize.width()),
        static_cast<f32>(widgetSize.height()) / static_cast<f32>(imgSize.height())
        );

    mScaledSize = QSize{
        static_cast<s32>(std::floor(imgSize.width() * mScale)),
        static_cast<s32>(std::floor(imgSize.height() * mScale))
    };

    const QPoint topLeft{
        (width() - mScaledSize.width()) / 2,
        (height() - mScaledSize.height()) / 2
    };

    mTexRect = QRect(topLeft, mScaledSize);
}

f32 mirrorCoord(f32 t) {
    const s32 i = static_cast<s32>(std::floor(t));
    const f32 f = t - static_cast<f32>(i);
    return (i % 2 == 0) ? f : (1.0f - f);
}

void TextureRepetitionSelector::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.fillRect(rect(), palette().color(QPalette::Base));

    if (mTexture.isNull()) {
        return;
    }

    const s32 repX = mDragging ? mPreviewRepX : mRepX;
    const s32 repY = mDragging ? mPreviewRepY : mRepY;

    painter.save();
    painter.setClipRect(mTexRect);
    PaintUtil::drawCheckerboard(painter, mTexRect);
    painter.restore();

    const s32 texW = mTexture.width();
    const s32 texH = mTexture.height();

    const s32 w = mTexRect.width();
    const s32 h = mTexRect.height();

    const f32 uMax = static_cast<f32>(mRepX) / static_cast<f32>(mDivX);
    const f32 vMax = static_cast<f32>(mRepY) / static_cast<f32>(mDivY);

    QImage output(w, h, QImage::Format::Format_ARGB32);

    for (s32 y = 0; y < h; ++y) {
        QRgb* scanLine = reinterpret_cast<QRgb*>(output.scanLine(y));

        for (s32 x = 0; x < w; ++x) {
            const f32 u  = (static_cast<f32>(x) / w) * uMax;
            const f32 v  = (static_cast<f32>(y) / h) * vMax;

            const f32 mu = mirrorCoord(u);
            const f32 mv = mirrorCoord(v);

            const s32 tx = std::clamp(static_cast<s32>(mu * texW), 0, texW - 1);
            const s32 ty = std::clamp(static_cast<s32>(mv * texH), 0, texH - 1);

            scanLine[x] = mTexture.pixel(tx, ty);
        }
    }

    painter.drawImage(mTexRect.topLeft(), output);

    const f32 cellW = mTexRect.width() / static_cast<f32>(repX);
    const f32 cellH = mTexRect.height() / static_cast<f32>(repY);

    painter.setPen(QPen(Qt::black, 1, Qt::DashLine));

    for (s32 x = 1; x < repX; ++x) {
        const s32 px = mTexRect.left() + static_cast<s32>(x * cellW);
        painter.drawLine(px, mTexRect.top(), px, mTexRect.bottom());
    }

    for (s32 y = 1; y < repY; ++y) {
        const s32 py = mTexRect.top() + static_cast<s32>(y * cellH);
        painter.drawLine(mTexRect.left(), py, mTexRect.right(), py);
    }

    painter.setPen(Qt::white);
    painter.drawText(
        mTexRect.adjusted(4, 4, -4, -4),
        Qt::AlignTop | Qt::AlignLeft,
        QString("%1 x %2").arg(repX).arg(repY)
        );
}

void TextureRepetitionSelector::mousePressEvent(QMouseEvent* event) {
    if (!mTexRect.contains(event->pos())) {
        return;
    }

    mDragging = true;
    mDragStart = event->pos();

    mPreviewRepX = mRepX;
    mPreviewRepY = mRepY;

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

    const QPoint delta = event->pos() - mDragStart;

    const s32 maxRep = 16;

    const f32 normX = static_cast<f32>(delta.x()) / static_cast<f32>(mTexRect.width());
    const f32 normY = static_cast<f32>(delta.y()) / static_cast<f32>(mTexRect.height());

    mPreviewRepX = std::clamp(mRepX + static_cast<s32>(normX * maxRep), 1, maxRep);
    mPreviewRepY = std::clamp(mRepY + static_cast<s32>(normY * maxRep), 1, maxRep);

    update();
}

void TextureRepetitionSelector::mouseReleaseEvent(QMouseEvent* event) {
    Q_UNUSED(event);

    if (!mDragging) {
        return;
    }

    mDragging = false;

    if (mPreviewRepX != mRepX || mPreviewRepY != mRepY) {
        mRepX = mPreviewRepX;
        mRepY = mPreviewRepY;
        emit repetitionsChanged(mRepX, mRepY);
    }

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
    mTexturePreview.setThumbnailSize(QSize(64, 64));
    mTexPatFreq.setMinimum(1);
    mTexPatTblUse.setRange(2, 16);


    // Texture pattern table
    const s32 referenceHeight = mWrapTComboBox.sizeHint().height(); // Make TexPat table same hight as a comboBox
    mTexPatTbl.setFixedHeight(referenceHeight);
    mTexPatTbl.setRowCount(1);
    mTexPatTbl.setRowHeight(0, referenceHeight);
    mTexPatTbl.horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    mTexPatTbl.verticalHeader()->setVisible(false);
    mTexPatTbl.horizontalHeader()->setVisible(false);
    mTexPatTbl.setSelectionMode(QAbstractItemView::NoSelection);
    mTexPatTbl.setEditTriggers(QAbstractItemView::AllEditTriggers);

    // Populate initially
    for (s32 i = 0; i < 16; ++i) {
        auto* item = new QTableWidgetItem("0");
        item->setTextAlignment(Qt::AlignCenter);
        mTexPatTbl.setItem(0, i, item);
    }

    // mTexRepetitionsX.setMinimum(1);
    // mTexRepetitionsY.setMinimum(1);

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
    settingsLayout->addWidget(new QLabel("Texture Divisions:"), 3, 0);
    settingsLayout->addWidget(&mDivisionSelector, 3, 1, 1, 3);

    settingsLayout->addWidget(new QLabel("Texture Repetitions:"), 4, 0);
    settingsLayout->addWidget(&mRepetitionSelector, 4, 1, 1, 4);

    // settingsLayout->addWidget(new QLabel("Repetitions X:"), 4, 0);
    // settingsLayout->addWidget(&mTexRepetitionsX, 4, 1);
    // settingsLayout->addWidget(new QLabel("Repetitions Y:"), 4, 2);
    // settingsLayout->addWidget(&mTexRepetitionsY, 4, 3);

    // Anim Mode
    mAnimModeComboBox.addItems({"Fit to Speed", "Fit to Life"});

    // Texture Pattern Anim
    mTexPatGroupBox.setTitle("Texture Pattern Animation");
    mTexPatGroupBox.setCheckable(true);

    auto texPatSettingsLayout = new QGridLayout;
    texPatSettingsLayout->addWidget(new QLabel("Animation Mode:"), 0, 0);
    texPatSettingsLayout->addWidget(&mAnimModeComboBox, 0, 1);
    texPatSettingsLayout->addWidget(new QLabel("Frames Per Step:"), 1, 0);
    texPatSettingsLayout->addWidget(&mTexPatFreq, 1, 1);
    texPatSettingsLayout->addWidget(new QLabel("Animation Frames:"), 1, 2);
    texPatSettingsLayout->addWidget(&mTexPatTblUse, 1, 3);
    texPatSettingsLayout->addWidget(new QLabel("Frame Order:"), 2, 0, 1, 2);
    texPatSettingsLayout->addWidget(&mTexPatTbl, 2, 0, 1, 4);

    mTexPatGroupBox.setLayout(texPatSettingsLayout);

    // Main Layout
    auto mainLayout = new QGridLayout(this);
    mainLayout->addLayout(settingsLayout, 0, 0, 1, 1);
    mainLayout->addWidget(&mTexturePreview, 0, 1, 1, 1);
    mainLayout->addWidget(&mTexPatGroupBox, 2, 0, 1, 2);

    mainLayout->addWidget(new QLabel("Initial Frame Variants:"), 3, 0, 1, 1);
    mainLayout->addWidget(&mNumTexPat, 3, 1, 1, 1);

    setLayout(mainLayout);
    setupConnections();
}

void TextureInspector::setupConnections() {
    connect(&mTexturePreview, &ThumbnailWidget::clicked, this, &TextureInspector::changeTexture);

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
    connect(&mNumTexPat, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture Pattern Count",
            "SetNumTexPat",
            &Ptcl::Emitter::numTexturePattern,
            &Ptcl::Emitter::setNumTexturePattern,
            static_cast<u16>(value)
            );
    });

    connect(&mDivisionSelector, &TextureDivisionSelector::divisionsChanged, this, [this](s32 x, s32 y) {
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
    });

    connect(&mTexPatFreq, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture Anim Frame Step",
            "SetTexPatFreq",
            &Ptcl::Emitter::texturePatternFrequency,
            &Ptcl::Emitter::setTexturePatternFrequency,
            static_cast<u16>(value)
            );
    });

    connect(&mTexPatTblUse, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
        setEmitterProperty(
            "Set Texture Anim Frame Count",
            "SetTexPatTblUse",
            &Ptcl::Emitter::texturePatternTableUse,
            &Ptcl::Emitter::setTexturePatternTableUse,
            static_cast<u16>(value)
            );
    });

    connect(&mTexPatTbl, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
        QSignalBlocker b1(mTexPatTbl);

        bool ok = false;
        s32 value = item->text().toInt(&ok);

        const bool valid = ok && value >= 0 && value < maxFrameCount();
        if (!valid) {
            value = 0;
            item->setText("0");
        }

        auto table = mEmitter->texturePatternTable();
        const s32 col = item->column();

        if (table[col] == value) {
            return;
        }

        table[col] = value;

        setEmitterProperty(
            "Set Texture Anim Frame Order",
            QString("SetTexPatTbl_%1").arg(col),
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

    // connect(&mTexRepetitionsY, &SizedSpinBoxBase::valueChanged, this, [this](u64 value) {
    //     setEmitterProperty(
    //         "Set Texture Y Repetition",
    //         "SetTexRepY",
    //         &Ptcl::Emitter::numTextureRepetitionsY,
    //         &Ptcl::Emitter::setNumTextureRepetitionsY,
    //         static_cast<s32>(value)
    //     );
    // });

    connect(&mAnimModeComboBox, &QComboBox::currentIndexChanged, this, [this](s32 index) {
        auto mode = static_cast<AnimMode>(index);

        u16 freq;
        if (mode == AnimMode::FitToLife) {
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

    connect(&mTexPatGroupBox, &QGroupBox::clicked, this, [this](bool checked) {
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
    QSignalBlocker b6(mNumTexPat);

    QSignalBlocker b9(mTexPatFreq);
    QSignalBlocker b10(mTexPatTblUse);
    QSignalBlocker b11(mTexPatTbl);
    QSignalBlocker b12(mTexPatGroupBox);
    // QSignalBlocker b13(mTexRepetitionsX);
    // QSignalBlocker b14(mTexRepetitionsY);
    QSignalBlocker b15(mAnimModeComboBox);

    qDebug() << "Setting Divisions X:" << mEmitter->numTextureDivisionX();
    qDebug() << "Setting Divisions Y:" << mEmitter->numTextureDivisionY();
    qDebug() << "Setting Repetitions X:" << mEmitter->numTextureRepetitionsX();
    qDebug() << "Setting Repetitions Y:" << mEmitter->numTextureRepetitionsY();

    mTexturePreview.setPixmap(QPixmap::fromImage(mEmitter->textureHandle()->textureData()));

    mDivisionSelector.setTexture(mEmitter->textureHandle()->textureData());
    mDivisionSelector.setDivisions(mEmitter->numTextureDivisionX(), mEmitter->numTextureDivisionY());

    mRepetitionSelector.setSource(mEmitter->textureHandle()->textureData(), mEmitter->numTextureDivisionX(), mEmitter->numTextureDivisionY());
    // mRepetitionSelector.setFrame(mEmitter->textureHandle()->textureData());
    mRepetitionSelector.setRepetitions(mEmitter->numTextureRepetitionsX(), mEmitter->numTextureRepetitionsY());


    mWrapTComboBox.setCurrentEnum(mEmitter->textureWrapT());
    mWrapSComboBox.setCurrentEnum(mEmitter->textureWrapS());
    mMagFilterComboBox.setCurrentEnum(mEmitter->textureMagFilter());
    mMinFilterComboBox.setCurrentEnum(mEmitter->textureMinFilter());
    mMipFilterComboBox.setCurrentEnum(mEmitter->textureMipFilter());

    mNumTexPat.setValue(mEmitter->numTexturePattern());

    const auto freq = mEmitter->texturePatternFrequency();
    const auto mode = freqToAnimMode(freq);

    mTexPatFreq.setValue(freq);
    mAnimModeComboBox.setCurrentIndex(static_cast<s32>(mode));

    if (mode == AnimMode::FitToLife) {
        mTexPatFreq.setDisabled(true);
    } else {
        mTexPatFreq.setDisabled(false);
    }

    mTexPatTblUse.setValue(mEmitter->texturePatternTableUse());

    const auto& tbl = mEmitter->texturePatternTable();
    for (s32 i = 0; i < tbl.size(); ++i) {
        QTableWidgetItem* item = mTexPatTbl.item(0, i);
        item->setText(QString::number(tbl[i]));

        if (i < mEmitter->numTexturePattern()) {
            item->setIcon(QIcon(createFramePreview(tbl[i])));
        }
    }

    mTexPatGroupBox.setChecked(mEmitter->isTexturePatternAnim());

    // mTexRepetitionsX.setValue(mEmitter->numTextureRepetitionsX());
    // mTexRepetitionsY.setValue(mEmitter->numTextureRepetitionsY());

    updateTexPatTblColumns();
}

void TextureInspector::updateTexPatTblColumns() {
    const QPalette& palette = mTexPatTbl.palette();

    for (s32 i = 0; i < 16; ++i) {
        QTableWidgetItem* item = mTexPatTbl.item(0, i);
        if (!item) {
            continue;
        }

        if (i >= mEmitter->texturePatternTableUse()) {
            item->setFlags(Qt::NoItemFlags);
            item->setBackground(palette.color(QPalette::Disabled, QPalette::Base));
            item->setForeground(palette.color(QPalette::Disabled, QPalette::Text));

            s32 frame = mEmitter->texturePatternTable()[i];
            item->setText(QString::number(frame));
            item->setIcon(QIcon());
        } else {
            item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled);
            item->setBackground(palette.color(QPalette::Base));
            item->setForeground(palette.color(QPalette::Text));

            s32 frame = mEmitter->texturePatternTable()[i];
            item->setText(QString::number(frame));
            item->setIcon(QIcon(createFramePreview(frame)));
        }
    }
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

    const auto& uvScale = mEmitter->textureUVScale();

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

    const s32 w = static_cast<s32>(std::round(texW / divX));
    const s32 h = static_cast<s32>(std::round(texH / divY));

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

QPixmap TextureInspector::createFramePreview(s32 frame) const {
    QImage base = getFrameTexture(frame);
    if (base.isNull()) {
        return {};
    }

    // TODO
    const f32 repeatX = static_cast<f32>(mEmitter->numTextureRepetitionsX());
    const f32 repeatY = static_cast<f32>(mEmitter->numTextureRepetitionsY());

    QImage repeated = applyUVRepetition(base, repeatX, repeatY);
    if (repeated.isNull()) {
        return {};
    }

    return QPixmap::fromImage(repeated.scaled(32, 32, Qt::KeepAspectRatio));
}


// ========================================================================== //


} // namespace PtclEditor
