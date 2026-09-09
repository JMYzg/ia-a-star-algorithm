#pragma once

#include <QWidget>

class QToolButton;

class BoardToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit BoardToolbar(QWidget *parent = nullptr);

private:
    QToolButton *makeButton(const QString &text, const QString &objectName = {});

    QToolButton *m_nodeButton = nullptr;
    QToolButton *m_lineButton = nullptr;
    QToolButton *m_deleteButton = nullptr;
    QToolButton *m_arrangeButton = nullptr;
    QToolButton *m_solveButton = nullptr;
};
