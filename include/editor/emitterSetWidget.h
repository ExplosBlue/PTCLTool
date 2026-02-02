#pragma once

#include "emitterWidget/emitterWidget.h"
#include "ptcl/ptclEmitterSet.h"

#include <QLabel>
#include <QLineEdit>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterSetWidget : public QWidget {
    Q_OBJECT
public:
    explicit EmitterSetWidget(QWidget* parent = nullptr);

    void setEmitterSet(Ptcl::EmitterSet* emitterSet);
    void setTextureList(const Ptcl::TextureList& textureList);

    void selectEmitter(s32 emitterIndex);

    void showStandardEditor();
    void showChildEditor();
    void showFluctuationEditor();
    void showFieldEditor();

    void clear();

signals:
    void textureUpdated(s32 oldIndex, s32 index);

    void emitterTypeChanged();
    void emitterNameChanged();
    void emitterSetNamedChanged();
    void emitterComplexFlagsChanged();

private:
    void populateProperties();

private:
    Ptcl::EmitterSet* mEmitterSetPtr;

    QVBoxLayout mMainLayout;
    QLineEdit mNameLineEdit;
    QLabel mEmitterCountLabel;
    EmitterWidget mEmitterWidget;
};


// ========================================================================== //


} // namespace PtclEditor
