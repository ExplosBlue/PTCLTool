#include "editor/components/frameSelectorPopup.h"

#include "util/paintUtil.h"

#include <QGuiApplication>
#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


FrameSelectorPopup::FrameSelectorPopup(const TextureFrameInfo& frameInfo, const SelectionState& state, QWidget* parent) :
    QWidget{parent, Qt::Popup}, mState{state}  {

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);

    initialize(frameInfo);
    adjustSize();
    positionAt(state.anchorGlobal);
}

void FrameSelectorPopup::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    constexpr qreal radius = 8.0;
    constexpr qreal arrowWidth = 20.0;
    constexpr auto arrowHeight = static_cast<qreal>(sArrowHeight);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const qreal arrowX = std::clamp(
        static_cast<qreal>(mArrowCenter),
        radius + arrowWidth / 2.0,
        width() - radius - arrowWidth / 2.0);

    QPainterPath path;

    path.moveTo(radius, arrowHeight);
    path.lineTo(arrowX - arrowWidth / 2.0, arrowHeight);

    path.lineTo(arrowX, 0.0);
    path.lineTo(arrowX + arrowWidth / 2.0, arrowHeight);

    path.lineTo(width() - radius, arrowHeight);
    path.quadTo(width(), arrowHeight, width(), arrowHeight + radius);

    path.lineTo(width(), height() - radius);
    path.quadTo(width(), height(), width() - radius, height());

    path.lineTo(radius, height());
    path.quadTo(0.0, height(), 0.0, height() - radius);

    path.lineTo(0.0, arrowHeight + radius);
    path.quadTo(0.0, arrowHeight, radius, arrowHeight);

    path.closeSubpath();

    painter.setPen(QPen(palette().mid().color(), 1));
    painter.setBrush(palette().window());

    painter.drawPath(path);
}

void FrameSelectorPopup::selectFrame(s32 frame) {
    if (frame != mState.currentFrame) {
        emit frameSelected(frame);
    }

    close();
}

void FrameSelectorPopup::positionAt(const QPoint& point) {
    QPoint pos(point.x() - width() / 2, point.y());
    s32 arrowCenter = point.x() - pos.x();

    mArrowCenter = std::clamp(arrowCenter, sPadding, width() - sPadding);

    const auto* screen = QGuiApplication::screenAt(pos);
    if (screen) {
        const auto screenRect = screen->availableGeometry();
        if (pos.x() + width() > screenRect.right()) {
            pos.setX(screenRect.right() - width());
        }
        if (pos.x() < screenRect.left()) {
            pos.setX(screenRect.left());
        }
        if (pos.y() + height() > screenRect.bottom()) {
            pos.setY(point.y() - height() - 1);
        }
        if (pos.y() < screenRect.top()) {
            pos.setY(screenRect.top());
        }
    }

    move(pos);
}

QToolButton* FrameSelectorPopup::makeFrameButton(const TextureFrameInfo& frameInfo, s32 frameIdx)  {
    const s32 btnSize = sThumbSize + sButtonPadding;

    const QPixmap thumb = PaintUtil::renderTextureFrame(frameInfo, frameIdx, sThumbSize);

    auto* btn = new QToolButton(this);
    btn->setIcon(QIcon(thumb));
    btn->setIconSize(QSize(sThumbSize, sThumbSize));
    btn->setText(QString::number(frameIdx));
    btn->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    btn->setFixedSize(btnSize, btnSize + sLabelHeight);
    btn->setToolTip(QString("Frame %1").arg(frameIdx));
    btn->setProperty("frameValue", frameIdx);
    btn->setCheckable(true);
    btn->setChecked(frameIdx == mState.currentFrame);

    connect(btn, &QToolButton::clicked, this, [this, frameIdx]() {
        selectFrame(frameIdx);
    });

    return btn;
}

void FrameSelectorPopup::initialize(const TextureFrameInfo& frameInfo) {
    auto* outer = new QVBoxLayout(this);
    outer->setSpacing(4);
    outer->setContentsMargins(sPadding, sArrowHeight + sPadding, sPadding, sPadding);

    auto* title = new QLabel("Select Frame", this);
    title->setAlignment(Qt::AlignCenter);
    outer->addWidget(title);

    auto* grid = new QGridLayout;
    grid->setSpacing(4);
    outer->addLayout(grid);

    for (s32 frameIdx = 0; frameIdx < mState.maxFrame; ++frameIdx) {
        auto btn = this->makeFrameButton(frameInfo, frameIdx);
        grid->addWidget(btn, frameIdx / sNumColumn, frameIdx % sNumColumn);
    }
}


// ========================================================================== //


} // namespace PtclEditor
