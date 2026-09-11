#pragma once

#include <QWidget>

class QToolButton;

class BoardToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit BoardToolbar(QWidget *parent = nullptr);

public slots:
    void setNodeModeActive(bool active);
    void setLineModeActive(bool active);
    void setDeleteModeActive(bool active);

signals:
    void nodeToggled(bool checked);
    void lineToggled(bool checked);
    void deleteToggled(bool checked);
    void arrangeClicked();

private:
    QToolButton *makeButton(const QString &text, const QString &objectName = {},
                            bool checkable = true);
    void setModeButton(QToolButton *button, bool active, const QString &idleText);
    void uncheckOthersSilently(QToolButton *active);

    QToolButton *m_nodeButton = nullptr;
    QToolButton *m_lineButton = nullptr;
    QToolButton *m_deleteButton = nullptr;
    QToolButton *m_arrangeButton = nullptr;
    QToolButton *m_solveButton = nullptr;
};
