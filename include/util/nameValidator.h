#pragma once

#include <QValidator>


// ========================================================================== //


class EmitterNameValidator final : public QValidator {
    Q_OBJECT
public:
    explicit EmitterNameValidator(QObject* parent = nullptr);

    State validate(QString& input, int& pos) const final;
};


// ========================================================================== //
