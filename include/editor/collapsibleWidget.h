#ifndef COLLAPSIBLEWIDGET_H
#define COLLAPSIBLEWIDGET_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QToolButton>
#include <QLabel>

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

#endif // COLLAPSIBLEWIDGET_H
