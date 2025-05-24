#ifndef EMITTERSETWIDGET_H
#define EMITTERSETWIDGET_H

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QTabWidget>

#include "emitterWidget.h"
#include "ptcl/ptclEmitterSet.h"

namespace PtclEditor {

class EmitterSetWidget : public QWidget {
    Q_OBJECT
public:
    explicit EmitterSetWidget(QWidget* parent = nullptr);

    void setEmitterSet(Ptcl::EmitterSet* emitterSet);

private:
    void populateEmitterPicker();
    void populateProperties();

private slots:
    void selectedEmitterChanged(u32 index);

private:
    // TODO: this should not be a raw pointer...
    Ptcl::EmitterSet* mEmitterSetPtr;

    QVBoxLayout mMainLayout;

    QLineEdit mNameLineEdit;
    QLabel mEmitterCountLabel;

    QTabWidget mEmitterTabs;

    EmitterWidget mEmitterWidget;

    std::vector<QWidget*> mEmitterTabPlaceholders;
};

} // namespace PtclEditor

#endif // EMITTERSETWIDGET_H
