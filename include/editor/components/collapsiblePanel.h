#pragma once

#include "typedefs.h"

#include <QFrame>
#include <QLabel>
#include <QWidget>

class QMouseEvent;
class QPainter;
class QPaintEvent;


namespace PtclEditor {


// ========================================================================== //


class CollapsiblePanel final : public QWidget {
    Q_OBJECT
    friend class PanelSplitter;
public:
    explicit CollapsiblePanel(const QString& title, QWidget* parent = nullptr);

    void setContent(QWidget* widget);

    bool isCollapsed() const { return mCollapsed; }
    void setCollapsed(bool collapsed);
    void toggleCollapse();

    QString title() const { return mTitleLabel.text(); }

    void setContentEnabled(bool enabled);

    s32 collapsedHeight() const { return headerHeight(); }
    s32 expandedMinHeight() const { return headerHeight() + contentMinHeight(); }

signals:
    void collapsedChanged(bool collapsed);

protected:
    QSize minimumSizeHint() const override;

private:
    class HeaderFrame final : public QFrame {
    public:
        explicit HeaderFrame(QWidget* parent = nullptr);

        void setCollapsed(bool collapsed);

    protected:
        void paintEvent(QPaintEvent* event) override;
        void mouseReleaseEvent(QMouseEvent* event) override;
        void mouseMoveEvent(QMouseEvent* event) override;

    private:
        QRect arrowRect() const;
        void drawBackground(QPainter& painter);
        void drawCollapseArrow(QPainter& painter);

    private:
        bool mCollapsed{false};
    };

    s32 contentMinHeight() const;
    s32 headerHeight() const;

private:
    HeaderFrame mHeaderFrame{};
    QLabel mTitleLabel{};
    QWidget* mContentWidget{nullptr};

    s32 mRestoreSize{0};
    bool mCollapsed{false};
};


// ========================================================================== //


} // namespace PtclEditor
