#pragma once

#include <QHash>
#include <QList>
#include <QSet>

#include "graph.h"

struct AStarStep {
    Graph::NodeId currentNode = Graph::kInvalidNodeId;
    QList<Graph::NodeId> openSet;
    QList<Graph::NodeId> closedSet;
    QHash<Graph::NodeId, Graph::NodeId> parents;
    QHash<Graph::NodeId, qreal> g;
    QHash<Graph::NodeId, qreal> h;
    QHash<Graph::NodeId, qreal> f;
    bool goalReached = false;
};

class AStarEngine
{
public:
    enum class Status { Ready, MissingStart, MissingGoal };

    explicit AStarEngine(const Graph &graph);

    Status status() const;

    Status run(Graph::NodeId startId, Graph::NodeId goalId);

    const QList<AStarStep> &steps() const;
    bool foundPath() const;
    QList<Graph::NodeId> path() const;

private:
    struct Record {
        Graph::NodeId id = Graph::kInvalidNodeId;
        Graph::NodeId parent = Graph::kInvalidNodeId;
        qreal g = 0.0;
    };

    qreal heuristic(Graph::NodeId nodeId) const;
    qreal realDistance(Graph::NodeId a, Graph::NodeId b) const;
    void expandNeighbors(const Record &current);
    void snapshot(Graph::NodeId currentNodeId, bool goalReached);

    const Graph &m_graph;
    Status m_status = Status::Ready;
    QList<AStarStep> m_steps;
    QList<Record> m_open;
    QSet<Graph::NodeId> m_closed;
    QList<Graph::NodeId> m_closedList;
    QHash<Graph::NodeId, Graph::NodeId> m_parents;
    QHash<Graph::NodeId, qreal> m_g;
    QHash<Graph::NodeId, qreal> m_h;
    Graph::NodeId m_goalId = Graph::kInvalidNodeId;
    bool m_found = false;
};
