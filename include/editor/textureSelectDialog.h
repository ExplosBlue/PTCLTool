#pragma once

#include "ptcl/ptcl.h"

#include <QDialog>
#include <QListWidget>


// ========================================================================== //


class TextureSelectDialog : public QDialog {
    Q_OBJECT
public:
    explicit TextureSelectDialog(const Ptcl::TextureList& textures, QWidget* parent = nullptr);
    int selectedIndex() const;

private:
    QListWidget mListWidget;
    int mSelectedIndex;
};


// ========================================================================== //
