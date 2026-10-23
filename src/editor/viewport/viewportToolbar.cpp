#include "editor/viewport/viewportToolbar.h"

#include "editor/viewport/previewViewport.h"
#include "editor/viewport/viewportDebugOverlay.h"

#include <QAction>
#include <QMenu>
#include <QSignalBlocker>
#include <QSize>
#include <QToolButton>


namespace PtclTool {


// ========================================================================== //


ViewportToolbar::ViewportToolbar(PreviewViewport* viewport, QWidget* parent) :
    QToolBar{parent},
    mViewport{viewport},
    mDebugOverlay{&viewport->debugOverlay()} {
    setMovable(false);
    setFloatable(false);
    setIconSize(QSize(18, 18));
    setToolButtonStyle(Qt::ToolButtonIconOnly);

    buildActions();
    connectSignals();
    syncFromViewport();
}

void ViewportToolbar::buildActions() {
    auto* viewButton = new QToolButton(this);
    viewButton->setText("View");
    viewButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    viewButton->setPopupMode(QToolButton::InstantPopup);
    viewButton->setToolTip("Camera views (Numpad 1/3/7, Ctrl for opposite)");

    auto* viewMenu = new QMenu(viewButton);
    const auto addViewAction = [this, viewMenu](const QString& text, ViewportCamera::ViewDirection direction) {
        QAction* action = viewMenu->addAction(text);
        connect(action, &QAction::triggered, this, [this, direction] {
            mViewport->setViewDirection(direction);
        });
    };

    addViewAction("Front",  ViewportCamera::ViewDirection::Front);
    addViewAction("Back",   ViewportCamera::ViewDirection::Back);
    addViewAction("Left",   ViewportCamera::ViewDirection::Left);
    addViewAction("Right",  ViewportCamera::ViewDirection::Right);
    addViewAction("Top",    ViewportCamera::ViewDirection::Top);
    addViewAction("Bottom", ViewportCamera::ViewDirection::Bottom);

    viewMenu->addSeparator();

    QAction* resetViewAction = viewMenu->addAction("Reset View");
    connect(resetViewAction, &QAction::triggered, mViewport, &PreviewViewport::resetCamera);

    viewMenu->addSeparator();

    mGridAction = viewMenu->addAction("Grid (Shift+Alt+Z)");
    mGridAction->setCheckable(true);
    connect(mGridAction, &QAction::toggled, mViewport, &PreviewViewport::setGridVisible);

    viewButton->setMenu(viewMenu);
    addWidget(viewButton);

    mProjectionButton = new QToolButton(this);
    mProjectionButton->setText("Orthographic");
    mProjectionButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mProjectionButton->setToolTip("Toggle orthographic/perspective (Numpad 5)");
    connect(mProjectionButton, &QToolButton::clicked, mViewport, &PreviewViewport::toggleProjection);
    addWidget(mProjectionButton);

    auto* debugButton = new QToolButton(this);
    debugButton->setText("Debug");
    debugButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    debugButton->setPopupMode(QToolButton::InstantPopup);
    debugButton->setToolTip("Viewport debug overlays");

    auto* debugMenu = new QMenu(debugButton);
    debugMenu->setToolTipsVisible(true);

    mEmissionShapesAction = debugMenu->addAction("Emission Shapes");
    mEmissionShapesAction->setCheckable(true);
    mEmissionShapesAction->setToolTip("Render emission shapes as wireframes.");
    connect(mEmissionShapesAction, &QAction::toggled, mDebugOverlay, &ViewportDebugOverlay::setEmissionShapesVisible);

    mDebugSelectedOnlyAction = debugMenu->addAction("Selected Only");
    mDebugSelectedOnlyAction->setCheckable(true);
    mDebugSelectedOnlyAction->setToolTip("Render emission shapes for the current selection only.");
    connect(mDebugSelectedOnlyAction, &QAction::toggled, mDebugOverlay, &ViewportDebugOverlay::setDebugSelectedOnly);

    debugButton->setMenu(debugMenu);
    addWidget(debugButton);
}

void ViewportToolbar::connectSignals() {
    connect(mViewport, &PreviewViewport::projectionChanged, this, [this](bool orthographic) {
        mProjectionButton->setText(orthographic ? "Orthographic" : "Perspective");
    });

    connect(mViewport, &PreviewViewport::gridVisibleChanged, this, [this](bool visible) {
        const QSignalBlocker blocker(mGridAction);
        mGridAction->setChecked(visible);
    });

    connect(mDebugOverlay, &ViewportDebugOverlay::emissionShapesVisibleChanged, this, [this](bool visible) {
        const QSignalBlocker blocker(mEmissionShapesAction);
        mEmissionShapesAction->setChecked(visible);
    });

    connect(mDebugOverlay, &ViewportDebugOverlay::debugSelectedOnlyChanged, this, [this](bool selectedOnly) {
        const QSignalBlocker blocker(mDebugSelectedOnlyAction);
        mDebugSelectedOnlyAction->setChecked(selectedOnly);
    });
}

void ViewportToolbar::syncFromViewport() {
    mGridAction->setChecked(mViewport->isGridVisible());
    mEmissionShapesAction->setChecked(mDebugOverlay->isEmissionShapesVisible());
    mDebugSelectedOnlyAction->setChecked(mDebugOverlay->isDebugSelectedOnly());
    mProjectionButton->setText(
        mViewport->projection() == ViewportCamera::ProjectionType::Orthographic ? "Orthographic" : "Perspective"
    );
}


// ========================================================================== //


} // namespace PtclTool
