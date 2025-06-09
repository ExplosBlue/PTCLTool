#pragma once

#include <QLabel>
#include <QPropertyAnimation>
#include <QToolButton>
#include <QWidget>


// ========================================================================== //


class CollapsibleWidget : public QWidget {
    Q_OBJECT
public:
    explicit CollapsibleWidget(const QString& title, QWidget* parent = nullptr);

    void setContent(QWidget* widget);

private slots:
    void toggle(bool expanded);

private:
    QLabel mTitleLabel;
    QToolButton mToggleButton;
    QFrame mContentFrame;
    QWidget* mContentWidget;
    QPropertyAnimation mAnimation;
};


// ========================================================================== //
