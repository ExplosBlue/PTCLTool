#pragma once

#include "ptcl/ptclEmitter.h"
#include "ptcl/ptclDocument.h"

#include <QCheckBox>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterWidgetBase : public QWidget {
    Q_OBJECT
public:
    explicit EmitterWidgetBase(QWidget* parent = nullptr);

    virtual void setDocument(Ptcl::Document* document);
    virtual void setSelection(Ptcl::Selection* selection);

protected:
    virtual void populateProperties() = 0;

    QString formatLabel(const QString& label) const;

    template<typename T, typename Getter, typename Setter>
    void setEmitterProperty(const QString& label, QString key, Getter getter, Setter setter, const T& value) {
        if (!mDocument || !mSelection) {
            return;
        }

        mDocument->setEmitterProperty<T>(mSelection->emitterSetIndex(), mSelection->emitterIndex(), formatLabel(label), key, getter, setter, value);
    }

protected slots:
    virtual void onEmitterChanged(s32 setIndex, s32 emitterIndex);

protected:
    Ptcl::Document* mDocument{nullptr};
    const Ptcl::Selection* mSelection{nullptr};
    Ptcl::Emitter* mEmitter{nullptr};
};


// ========================================================================== //


} // namespace PtclEditor
