#pragma once

#include <QToolBar>


// ========================================================================== //


class QAction;
class QToolButton;


namespace PtclTool {


// ========================================================================== //


class PreviewViewport;


class ViewportToolbar final : public QToolBar {
    Q_OBJECT
public:
    explicit ViewportToolbar(PreviewViewport* viewport, QWidget* parent = nullptr);

private:
    void buildActions();
    void connectSignals();
    void syncFromViewport();

    PreviewViewport* mViewport{nullptr};

    QToolButton* mProjectionButton{nullptr};
    QAction* mGridAction{nullptr};
};


// ========================================================================== //


} // namespace PtclTool
