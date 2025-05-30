#include "editor/collapsibleWidget.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QStyleOption>

CollapsibleWidget::CollapsibleWidget(const QString& title, QWidget* parent) :
    QWidget{parent},
    mContentWidget{nullptr} {
    QStyleOption opt;
    QPalette pal = opt.palette;
    QColor bg = pal.color(QPalette::Button);
    QColor fg = pal.color(QPalette::ButtonText);
    QString style = QString(R"(
    QFrame#HeaderFrame {
        background-color: %1;
        color: %2;
        border: 1px solid %1;
        border-bottom: none;
        border-top-left-radius: 4px;
        border-top-right-radius: 4px;
    }
)").arg(bg.name(), fg.name());

    // Header Frame
    auto headerFrame = new QFrame;
    headerFrame->setObjectName("HeaderFrame");
    headerFrame->setStyleSheet(style);

    // Toggle Button
    mToggleButton.setStyleSheet("QToolButton {border: none; }");
    mToggleButton.setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    mToggleButton.setArrowType(Qt::RightArrow);
    mToggleButton.setCheckable(true);
    mToggleButton.setChecked(false);
    connect(&mToggleButton, &QToolButton::toggled, this, &CollapsibleWidget::toggle);

    // Title Label
    mTitleLabel.setText(title);
    mTitleLabel.setStyleSheet("QLabel { font-weight: bold; }");

    // Header Layout
    auto headerLayout = new QHBoxLayout(headerFrame);
    headerLayout->setContentsMargins(8, 4, 8, 4);
    headerLayout->setSpacing(8);
    headerLayout->addWidget(&mToggleButton);
    headerLayout->addWidget(&mTitleLabel);
    headerLayout->addStretch();

    // Content Frame
    mContentFrame.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mContentFrame.setMaximumHeight(0);
    mContentFrame.setMinimumHeight(0);
    mContentFrame.setLayout(new QVBoxLayout);
    mContentFrame.layout()->setContentsMargins(0, 0, 0, 0);
    mContentFrame.layout()->setSpacing(0);

    QString contentFrameStyle = QString(R"(
    QFrame#ContentFrame {
        border: 1px solid %1;
        border-top: none;
        border-bottom-left-radius: 4px;
        border-bottom-right-radius: 4px;
    }
)").arg(bg.name());

    mContentFrame.setObjectName("ContentFrame");
    mContentFrame.setStyleSheet(contentFrameStyle);

    // Main Layout
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(headerFrame);
    mainLayout->addWidget(&mContentFrame);

    // Animation
    mAnimation.setTargetObject(&mContentFrame);
    mAnimation.setPropertyName("maximumHeight");
    mAnimation.setDuration(100);
    mAnimation.setEasingCurve(QEasingCurve::InOutQuad);
}

void CollapsibleWidget::setContent(QWidget* widget) {
    if (mContentWidget) {
        mContentWidget->setParent(nullptr);
    }

    mContentWidget = widget;
    mContentFrame.layout()->addWidget(mContentWidget);

    mContentWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    mContentWidget->adjustSize();

    const int height = mContentWidget->sizeHint().height() + 16;
    mAnimation.setEndValue(height);
}

void CollapsibleWidget::toggle(bool expanded) {
    mToggleButton.setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);

    if (!mContentWidget) {
        return;
    }

    const int fullHeight = mContentWidget->sizeHint().height() + 16;

    mAnimation.stop();
    mAnimation.setStartValue(expanded ? 0 : fullHeight);
    mAnimation.setEndValue(expanded ? fullHeight : 0);
    mAnimation.start();
}
