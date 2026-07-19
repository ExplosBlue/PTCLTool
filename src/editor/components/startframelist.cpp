#include "editor/components/startframelist.h"

#include <QLabel>
#include <QPainter>
#include <QHBoxLayout>



namespace PtclEditor {


// ==========================================================================//


StartFrameList::StartFrameList(QWidget* parent)
    : QFrame{parent} {

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);

    auto* mainLayout = new QVBoxLayout(this);

    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(8);

    auto* headerLayout = new QHBoxLayout;

    mExpandButton = new QToolButton;
    mExpandButton->setCheckable(true);
    mExpandButton->setArrowType(Qt::RightArrow);
    mExpandButton->setAutoRaise(true);
    mExpandButton->setChecked(false);

    auto* title = new QLabel("Spawn Frames");

    mCountSpinBox = new QSpinBox;
    mCountSpinBox->setRange(0, sMaxFrameCount);
    mCountSpinBox->setAlignment(Qt::AlignLeft);

    headerLayout->addWidget(mExpandButton);
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(mCountSpinBox);

    mainLayout->addLayout(headerLayout);

    mZeroFrameLabel = new QLabel("No starting frames available.\nNo texture will be drawn.");
    mZeroFrameLabel->setStyleSheet("QLabel { color: #cc8800; font-weight: bold; }");
    mZeroFrameLabel->setWordWrap(true);
    mZeroFrameLabel->setVisible(false);
    mainLayout->addWidget(mZeroFrameLabel);

    mListWidget = new QListWidget;
    mListWidget->setViewMode(QListView::IconMode);
    mListWidget->setMovement(QListView::Static);
    mListWidget->setResizeMode(QListView::Fixed);
    mListWidget->setSpacing(sSpacing);
    mListWidget->setGridSize(QSize(sItemSize, sItemSize));
    mListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mListWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    mListWidget->setObjectName("SpawnFrameList");
    mListWidget->setFixedWidth(sColumns * sItemSize + (sColumns - 1) * sSpacing);
    mListWidget->setVisible(false);

    mainLayout->addWidget(mListWidget, 0, Qt::AlignCenter);

    connect(mCountSpinBox, &QSpinBox::valueChanged, this, [this](s32 value) {
        setSlotCount(static_cast<u16>(value));
        emit slotCountChanged(static_cast<u16>(value));
    });

    connect(mExpandButton, &QToolButton::toggled, this, [this](bool checked) {
        mExpandButton->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);
        mListWidget->setVisible(checked);

        if (checked) {
            mListWidget->setFixedHeight(expandedHeight());
        }

        updateGeometry();
    });
}

QPixmap StartFrameList::renderThumb(s32 frameIndex) const {
    const QPixmap frame = PaintUtil::renderTextureFrame(mFrameInfo, frameIndex, sThumbSize);

    QPixmap thumb(sThumbSize, sThumbSize);
    thumb.fill(Qt::transparent);

    QPainter painter(&thumb);
    PaintUtil::drawCheckerboard(painter, thumb.rect(), 8, thumb.size());
    painter.drawPixmap(0, 0, frame);

    return thumb;
}

void StartFrameList::setSource(const TextureFrameInfo& frameInfo) {
    mFrameInfo = frameInfo;

    mFrameInfo.divX = std::max(1, mFrameInfo.divX);
    mFrameInfo.divY = std::max(1, mFrameInfo.divY);
    mFrameInfo.repX = std::max(1, mFrameInfo.repX);
    mFrameInfo.repY = std::max(1, mFrameInfo.repY);

    updateList();
}

void StartFrameList::setSlotCount(u16 count) {
    count = std::min<u16>(count, sMaxFrameCount);

    if (mSlotCount == count) {
        return;
    }

    mSlotCount = count;

    QSignalBlocker blocker(mCountSpinBox);
    mCountSpinBox->setValue(count);

    updateList();
    updateZeroFrameLabel();
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

s32 StartFrameList::expandedHeight() const {
    const s32 count = static_cast<s32>(mSlotCount);
    if (count <= 0) {
        return 0;
    }

    constexpr s32 columns = sColumns;
    const s32 rows = (count + columns - 1) / columns;
    return rows * sItemSize + std::max(0, rows - 1) * sSpacing;
}

void StartFrameList::updateList() {
    const s32 prevCount = mListWidget->count();

    while (mListWidget->count() > static_cast<s32>(mSlotCount)) {
        delete mListWidget->takeItem(mListWidget->count() - 1);
    }

    for (s32 i = prevCount; i < static_cast<s32>(mSlotCount); ++i) {
        auto* item = new QListWidgetItem;
        item->setIcon(QIcon(renderThumb(i)));
        item->setSizeHint(QSize(sItemSize, sItemSize));
        item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
        mListWidget->addItem(item);
    }

    for (s32 i = 0; i < mListWidget->count(); ++i) {
        mListWidget->item(i)->setIcon(QIcon(renderThumb(i)));
    }

    if (mExpandButton->isChecked()) {
        mListWidget->setFixedHeight(expandedHeight());
        updateGeometry();
    }
}

void StartFrameList::updateZeroFrameLabel() {
    mZeroFrameLabel->setVisible(mSlotCount == 0);
}


// ==========================================================================//


} // namespace PtclEditor
