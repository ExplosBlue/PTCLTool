#pragma once

#include "emitterWidget.h"
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
    EmitterWidget& getEmitterWidget();

signals:
    void textureUpdated(int oldIndex, int index);

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
