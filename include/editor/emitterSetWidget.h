#pragma once

#include "emitterWidget/emitterWidget.h"
#include "ptcl/ptclEmitterSet.h"

#include <QGroupBox>
#include <QLineEdit>
#include <QWidget>


namespace PtclEditor {


// ========================================================================== //


class EmitterSetWidget final : public QWidget {
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
    void setupConnections();

    void updateStatusLabel();

private:
    enum class EditorMode {
        Standard,
        Child,
        Fluctuation,
        Field
    };

private:
    Ptcl::EmitterSet* mEmitterSetPtr{nullptr};
    s32 mCurEmitterIdx{};

    QLineEdit mNameLineEdit{};
    QSpinBox mUserDataSpinBox{};
    QSpinBox mLastUpdateSpinBox{};
    QGroupBox mEmitterGroup{};
    EmitterWidget mEmitterWidget{};

    EditorMode mEditorMode{EditorMode::Standard};
};


// ========================================================================== //


} // namespace PtclEditor
