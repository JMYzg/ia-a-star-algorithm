#include "step_player.h"

#include <QTimer>

namespace {
constexpr int kAutoStepDelayMs = 700;
}

StepPlayer::StepPlayer(QObject *parent)
    : QObject(parent)
{
    m_autoTimer.setInterval(kAutoStepDelayMs);
    connect(&m_autoTimer, &QTimer::timeout, this, &StepPlayer::next);
}

void StepPlayer::start(const Graph &graph, Graph::NodeId startId, Graph::NodeId goalId)
{
    m_autoTimer.stop();
    m_autoRunning = false;

    AStarEngine engine(graph);
    const AStarEngine::Status status = engine.run(startId, goalId);
    if (status == AStarEngine::Status::MissingStart) {
        emit validationFailed(QStringLiteral("No hay nodo inicio definido. Selecciona uno en el edit box."));
        return;
    }
    if (status == AStarEngine::Status::MissingGoal) {
        emit validationFailed(QStringLiteral("No hay nodo destino definido. Selecciona uno en el edit box."));
        return;
    }

    m_steps = engine.steps();
    m_path = engine.path();
    m_index = -1;
    emit started();
    next();
}

void StepPlayer::stop()
{
    m_autoTimer.stop();
    m_steps.clear();
    m_path.clear();
    m_index = -1;
    if (m_autoRunning) {
        m_autoRunning = false;
        emit autoRunningChanged(false);
    }
    emit stopped();
}

bool StepPlayer::isActive() const
{
    return !m_steps.isEmpty();
}

int StepPlayer::stepIndex() const
{
    return m_index;
}

int StepPlayer::stepCount() const
{
    return m_steps.size();
}

const AStarStep &StepPlayer::currentStep() const
{
    static const AStarStep kEmptyStep;
    if (m_index < 0 || m_index >= m_steps.size())
        return kEmptyStep;
    return m_steps[m_index];
}

bool StepPlayer::foundPath() const
{
    return !m_steps.isEmpty() && m_steps.last().goalReached;
}

QList<Graph::NodeId> StepPlayer::path() const
{
    return m_path;
}

bool StepPlayer::isAutoRunning() const
{
    return m_autoRunning;
}

void StepPlayer::next()
{
    if (m_steps.isEmpty() || m_index >= m_steps.size() - 1) {
        setAutoRunning(false);
        return;
    }
    ++m_index;
    emit stepChanged(m_index, m_steps.size());
    if (m_index == m_steps.size() - 1) {
        setAutoRunning(false);
        emit runFinished(foundPath());
    }
}

void StepPlayer::previous()
{
    if (m_steps.isEmpty() || m_index <= 0)
        return;
    --m_index;
    emit stepChanged(m_index, m_steps.size());
}

void StepPlayer::setAutoRunning(bool running)
{
    if (m_autoRunning == running)
        return;
    m_autoRunning = running;
    if (running)
        m_autoTimer.start();
    else
        m_autoTimer.stop();
    emit autoRunningChanged(running);
}
