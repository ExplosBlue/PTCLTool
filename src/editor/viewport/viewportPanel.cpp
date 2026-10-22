#include "editor/viewport/viewportPanel.h"

#include "editor/viewport/playbackPanel.h"
#include "editor/viewport/previewViewport.h"
#include "editor/viewport/viewportToolbar.h"

#include <QVBoxLayout>


namespace PtclTool {


// ========================================================================== //


ViewportPanel::ViewportPanel(QWidget* parent) :
    QWidget{parent} {
    setObjectName("ViewportPanel");

    mViewport = new PreviewViewport(this);
    auto* toolbar = new ViewportToolbar(mViewport, this);
    auto* playbackPanel = new PlaybackPanel(mViewport, this);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(toolbar);
    layout->addWidget(mViewport, 1);
    layout->addWidget(playbackPanel);
}

PreviewViewport* ViewportPanel::viewport() const {
    return mViewport;
}


// ========================================================================== //


} // namespace PtclTool
