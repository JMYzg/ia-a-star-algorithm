#pragma once

#include <QWidget>

class QLabel;

class PathSummary : public QWidget
{
    Q_OBJECT

public:
    explicit PathSummary(const QString &emptyHint, QWidget *parent = nullptr);

public slots:
    void showPath(const QString &pathText, qreal totalCost);
    void showNoPath();
    void clear();

private:
    void updateVisibility();

    QLabel *m_hintLabel = nullptr;
    QLabel *m_pathLabel = nullptr;
    QLabel *m_costLabel = nullptr;
};
