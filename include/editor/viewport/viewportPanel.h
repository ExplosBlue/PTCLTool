#pragma once

#include <QWidget>


// ========================================================================== //


namespace PtclTool {


class PreviewViewport;


class ViewportPanel final : public QWidget {
public:
    explicit ViewportPanel(QWidget* parent = nullptr);

    PreviewViewport* viewport() const;

private:
    PreviewViewport* mViewport{nullptr};
};


// ========================================================================== //


} // namespace PtclTool
