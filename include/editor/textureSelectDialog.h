#ifndef TEXTURESELECTDIALOG_H
#define TEXTURESELECTDIALOG_H

#include <QDialog>
#include <QListWidget>

#include "ptcl/ptcl.h"

class TextureSelectDialog : public QDialog {
    Q_OBJECT
public:
    explicit TextureSelectDialog(const Ptcl::TextureList& textures, QWidget* parent = nullptr);
    int selectedIndex() const;

private:
    QListWidget mListWidget;
    int mSelectedIndex;
};

#endif // TEXTURESELECTDIALOG_H
