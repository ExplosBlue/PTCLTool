#pragma once

#include "math/matrix.h"
#include "typedefs.h"

#include <QObject>
#include <QSize>


// ========================================================================== //


class QPainter;

namespace Ptcl {
class EmitterSet;
} // namespace Ptcl


namespace PtclTool {


class ViewportDebugOverlay final : public QObject {
    Q_OBJECT
public:
    struct Context {
        const Ptcl::EmitterSet* emitterSet{nullptr};
        const Math::Matrix44f* viewProjection{nullptr};
        QSize viewportSize{};
        s32 selectedEmitterIndex{-1};
        bool hasSelection{false};
    };

    explicit ViewportDebugOverlay(QObject* parent = nullptr);

    bool isEmissionShapesVisible() const { return mEmissionShapesVisible; }
    bool isDebugSelectedOnly() const { return mDebugSelectedOnly; }

    void draw(QPainter& painter, const Context& context) const;

public slots:
    void setEmissionShapesVisible(bool visible);
    void setDebugSelectedOnly(bool selectedOnly);

signals:
    void emissionShapesVisibleChanged(bool visible);
    void debugSelectedOnlyChanged(bool selectedOnly);
    void changed();

private:
    bool mEmissionShapesVisible{false};
    bool mDebugSelectedOnly{false};
};


// ========================================================================== //


} // namespace PtclTool
