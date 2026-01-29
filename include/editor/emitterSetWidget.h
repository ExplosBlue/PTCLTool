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
    void setEmitterTab(int emitterIndex);
    void setTextureList(const Ptcl::TextureList& textureList);
    void showStandardEditor();
    void showChildEditor();
    void showFluctuationEditor();

    void clear();

signals:
    void textureUpdated(int oldIndex, int index);
    void nameUpdated(const QString& name);
    void emitterNameUpdated(u32 emitterIndex, const QString& name);

    void emitterAdded();
    void emitterRemoved();

private slots:
    void selectedEmitterChanged(u32 index);
    void emitterTabClosed(int index);

private:
    void populateEmitterPicker();
    void populateProperties();

private:
    Ptcl::EmitterSet* mEmitterSetPtr;

    QVBoxLayout mMainLayout;
    QLineEdit mNameLineEdit;
    QLabel mEmitterCountLabel;
    QTabWidget mEmitterTabs;
    EmitterWidget mEmitterWidget;

    std::vector<QWidget*> mEmitterTabPlaceholders;
};


// ========================================================================== //


} // namespace PtclEditor
