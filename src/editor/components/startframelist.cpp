#include "editor/components/startframelist.h"
#include "editor/components/thumbnailWidget.h"

#include <QLabel>
#include <QHBoxLayout>



namespace PtclEditor {


// ========================================================================== //


StartFrameList::StartFrameList(QWidget* parent)
    : QFrame{parent} {

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);

    auto* mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    // Header
    auto* headerLayout = new QHBoxLayout;

    auto* title = new QLabel("Spawn Frames");

    mCountSpinBox = new QSpinBox;
    mCountSpinBox->setRange(0, sMaxFrameCount);
    mCountSpinBox->setAlignment(Qt::AlignLeft);

    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(mCountSpinBox);

    mainLayout->addLayout(headerLayout);

    // Grid
    mGridLayout = new QGridLayout;

    mGridLayout->setContentsMargins(0, 0, 0, 0);
    mGridLayout->setHorizontalSpacing(8);
    mGridLayout->setVerticalSpacing(8);

    for (s32 i = 0; i < sMaxFrameCount; ++i) {
        auto* thumbnail = new ThumbnailWidget;

        thumbnail->setThumbnailSize({sThumbSize, sThumbSize});

        // Initial empty state
        thumbnail->setVisible(false);

        const s32 row = i / sColumnCount;
        const s32 col = i % sColumnCount;

        mGridLayout->addWidget(thumbnail, row, col, Qt::AlignCenter);

        mThumbnails[i] = thumbnail;
    }

    auto* gridContainer = new QWidget;
    gridContainer->setLayout(mGridLayout);

    mainLayout->addWidget(gridContainer, 0, Qt::AlignCenter);


    connect(mCountSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setSlotCount(static_cast<u16>(value));
        emit slotCountChanged(static_cast<u16>(value));
    });
}

void StartFrameList::setSource(const TextureFrameInfo& frameInfo) {
    mFrameInfo = frameInfo;

    mFrameInfo.divX = std::max(1, mFrameInfo.divX);
    mFrameInfo.divY = std::max(1, mFrameInfo.divY);
    mFrameInfo.repX = std::max(1, mFrameInfo.repX);
    mFrameInfo.repY = std::max(1, mFrameInfo.repY);

    updateGrid();
}

void StartFrameList::setSlotCount(u16 count) {
    count = std::min<u16>(count, sMaxFrameCount);

    if (mSlotCount == count) {
        return;
    }

    mSlotCount = count;


    QSignalBlocker blocker(mCountSpinBox);
    mCountSpinBox->setValue(count);

    updateGrid();
}

void StartFrameList::setSlotMax(u16 count) {
    mSlotMax = std::min<u16>(count, sMaxFrameCount);

    mCountSpinBox->setMaximum(mSlotMax);

    if (mSlotCount > mSlotMax) {
        setSlotCount(mSlotMax);
    }
}

u16 StartFrameList::slotCount() const {
    return mSlotCount;
}

void StartFrameList::updateGrid() {
    while (mGridLayout->count() > 0) {
        mGridLayout->takeAt(0);
    }

    for (s32 i = 0; i < sMaxFrameCount; ++i) {
        mThumbnails[i]->setVisible(false);
    }

    for (s32 row = 0; row < sRowCount; ++row) {
        const s32 rowStart = row * sColumnCount;
        const s32 rowCount = std::clamp(static_cast<s32>(mSlotCount) - rowStart, 0, sColumnCount);

        if (rowCount <= 0) {
            continue;
        }

        const s32 startColumn = (sColumnCount - rowCount) / 2;

        for (s32 col = 0; col < rowCount; ++col) {
            const s32 frameIndex = rowStart + col;

            auto* thumbnail = mThumbnails[frameIndex];
            thumbnail->setVisible(true);
            thumbnail->setPixmap(PaintUtil::renderTextureFrame(mFrameInfo, frameIndex, sThumbSize));

            mGridLayout->addWidget(thumbnail, row, startColumn + col, Qt::AlignCenter);
        }
    }
}


// ========================================================================== //


} // namespace PtclEditor
