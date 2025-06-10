#include "util/nameValidator.h"


// ========================================================================== //


EmitterNameValidator::EmitterNameValidator(QObject* parent) :
    QValidator{parent} {}

QValidator::State EmitterNameValidator::validate(QString& input, int& pos) const {
    return input.contains(' ') ? Invalid : Acceptable;
}


// ========================================================================== //
