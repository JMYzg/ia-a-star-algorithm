#include "solve_progress_bar.h"

#include <QPropertyAnimation>

#include <algorithm>

SolveProgressBar::SolveProgressBar(QWidget *parent)
    : QProgressBar(parent)
{
    setTextVisible(true);
    setFormat(QStringLiteral("Paso %v / %m"));
    setAlignment(Qt::AlignCenter);
    setFixedHeight(18);
    m_animation = new QPropertyAnimation(this, "value", this);
    m_animation->setDuration(250);
    m_animation->setEasingCurve(QEasingCurve::InOutCubic);
}

void SolveProgressBar::setProgress(int stepIndex, int total)
{
    setRange(0, std::max(total, 1));
    m_animation->stop();
    m_animation->setStartValue(value());
    m_animation->setEndValue(stepIndex + 1);
    m_animation->start();
}

void SolveProgressBar::reset()
{
    m_animation->stop();
    setRange(0, 1);
    setValue(0);
}
