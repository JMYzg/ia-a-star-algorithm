#pragma once

#include <QProgressBar>

class QPropertyAnimation;

class SolveProgressBar : public QProgressBar
{
    Q_OBJECT

public:
    explicit SolveProgressBar(QWidget *parent = nullptr);

public slots:
    void setProgress(int stepIndex, int total);
    void reset();

private:
    QPropertyAnimation *m_animation;
};
