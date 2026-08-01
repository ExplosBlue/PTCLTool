#include "editor/components/collapsiblePanel.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOption>
#include <QVBoxLayout>


namespace PtclEditor {


// ========================================================================== //


CollapsiblePanel::HeaderFrame::HeaderFrame(QWidget* parent) :
    QFrame{parent} {
    setMouseTracking(true);
}

QRect CollapsiblePanel::HeaderFrame::arrowRect() const {
    return {8, 0, 20, height()};
}

void CollapsiblePanel::HeaderFrame::setCollapsed(bool collapsed) {
    if (mCollapsed == collapsed) {
        return;
    }

    mCollapsed = collapsed;
    update();
}

void CollapsiblePanel::HeaderFrame::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawCollapseArrow(painter);
}

void CollapsiblePanel::HeaderFrame::drawBackground(QPainter& painter) {
    const QRectF r = QRectF(rect()).adjusted(0.5, 0.5, -0.5, -0.5);
    constexpr qreal radius = 4.0;

    QPainterPath path;
    path.moveTo(r.left(), r.bottom());
    path.lineTo(r.left(), r.top() + radius);
    path.quadTo(r.left(), r.top(), r.left() + radius, r.top());
    path.lineTo(r.right() - radius, r.top());
    path.quadTo(r.right(), r.top(), r.right(), r.top() + radius);
    path.lineTo(r.right(), r.bottom());
    path.closeSubpath();

    painter.fillPath(path, palette().color(QPalette::Button));
}

void CollapsiblePanel::HeaderFrame::drawCollapseArrow(QPainter& painter) {
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect = arrowRect();

    const QStyle::PrimitiveElement arrow = mCollapsed
        ? QStyle::PE_IndicatorArrowRight
        : QStyle::PE_IndicatorArrowDown;

    style()->drawPrimitive(arrow, &opt, &painter, this);
}

void CollapsiblePanel::HeaderFrame::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && arrowRect().contains(event->pos())) {
        if (auto* panel = qobject_cast<CollapsiblePanel*>(parentWidget())) {
            panel->toggleCollapse();
        }
        return;
    }

    QFrame::mouseReleaseEvent(event);
}

void CollapsiblePanel::HeaderFrame::mouseMoveEvent(QMouseEvent* event) {
    setCursor(arrowRect().contains(event->pos()) ? Qt::PointingHandCursor : Qt::ArrowCursor);
    QFrame::mouseMoveEvent(event);
}


// ========================================================================== //


CollapsiblePanel::CollapsiblePanel(const QString& title, QWidget* parent) :
    QWidget{parent} {
    // Title Label
    QFont titleFont = mTitleLabel.font();
    titleFont.setBold(true);
    mTitleLabel.setFont(titleFont);
    mTitleLabel.setText(title);

    // Header Layout
    auto* headerLayout = new QHBoxLayout(&mHeaderFrame);
    headerLayout->setContentsMargins(32, 4, 8, 4);
    headerLayout->setSpacing(8);
    headerLayout->addWidget(&mTitleLabel);
    headerLayout->addStretch();

    // Main Layout
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(&mHeaderFrame);
}

void CollapsiblePanel::setContent(QWidget* widget) {
    if (mContentWidget) {
        disconnect(mContentWidget, nullptr, this, nullptr);
        mContentWidget->setParent(nullptr);
    }

    mContentWidget = widget;
    if (mContentWidget) {
        connect(mContentWidget, &QObject::destroyed, this, [this] {
            mContentWidget = nullptr;
        });
    }
    layout()->addWidget(mContentWidget);
    mContentWidget->setVisible(!mCollapsed);
    updateGeometry();
}

void CollapsiblePanel::setContentEnabled(bool enabled) {
    if (mContentWidget) {
        mContentWidget->setEnabled(enabled);
    }
}

void CollapsiblePanel::setCollapsed(bool collapsed) {
    if (mCollapsed == collapsed) {
        return;
    }

    mCollapsed = collapsed;

    if (mContentWidget) {
        mContentWidget->setVisible(!collapsed);
    }

    mHeaderFrame.setCollapsed(mCollapsed);

    updateGeometry();
    emit collapsedChanged(collapsed);
}

void CollapsiblePanel::toggleCollapse() {
    setCollapsed(!mCollapsed);
}

s32 CollapsiblePanel::headerHeight() const {
    return mHeaderFrame.sizeHint().height();
}

QSize CollapsiblePanel::minimumSizeHint() const {
    const s32 headerH = headerHeight();
    return mCollapsed ? QSize(0, headerH) : QSize(0, headerH + contentMinHeight());
}

s32 CollapsiblePanel::contentMinHeight() const {
    return mContentWidget ? mContentWidget->minimumSizeHint().height() : 0;
}


// ========================================================================== //


} // namespace PtclEditor
