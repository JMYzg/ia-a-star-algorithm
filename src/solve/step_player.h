#pragma once

#include <QObject>
#include <QTimer>

#include "core/a_star_engine.h"

class Graph;

class StepPlayer : public QObject
{
    Q_OBJECT

public:
    explicit StepPlayer(QObject *parent = nullptr);

    void start(const Graph &graph, Graph::NodeId startId, Graph::NodeId goalId);
    void stop();

    bool isActive() const;
    int stepIndex() const;
    int stepCount() const;
    const AStarStep &currentStep() const;
    bool foundPath() const;
    bool isAutoRunning() const;

public slots:
    void next();
    void previous();
    void setAutoRunning(bool running);

signals:
    void started();
    void stopped();
    void stepChanged(int stepIndex, int stepCount);
    void runFinished(bool found);
    void autoRunningChanged(bool running);
    void validationFailed(const QString &message);

private:
    QList<AStarStep> m_steps;
    int m_index = -1;
    bool m_autoRunning = false;
    QTimer m_autoTimer;
};
