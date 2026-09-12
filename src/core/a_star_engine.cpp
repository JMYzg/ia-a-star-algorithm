#include "a_star_engine.h"

#include <cmath>

AStarEngine::AStarEngine(const Graph &graph)
    : m_graph(graph)
{
}

AStarEngine::Status AStarEngine::status() const
{
    return m_status;
}

AStarEngine::Status AStarEngine::run(Graph::NodeId startId, Graph::NodeId goalId)
{
    m_steps.clear();
    m_open.clear();
    m_closed.clear();
    m_closedList.clear();
    m_parents.clear();
    m_g.clear();
    m_h.clear();
    m_found = false;
    m_goalId = goalId;

    if (!m_graph.hasNode(startId)) {
        m_status = Status::MissingStart;
        return m_status;
    }
    if (!m_graph.hasNode(goalId)) {
        m_status = Status::MissingGoal;
        return m_status;
    }
    m_status = Status::Ready;

    for (const Graph::Node &node : m_graph.nodes())
        m_h.insert(node.id, realDistance(node.id, goalId));

    Record start;
    start.id = startId;
    m_open.append(start);
    m_parents.insert(startId, Graph::kInvalidNodeId);
    m_g.insert(startId, 0.0);

    while (!m_open.isEmpty()) {
        int bestIndex = 0;
        qreal bestF = m_g.value(m_open[0].id) + m_h.value(m_open[0].id);
        for (int i = 1; i < m_open.size(); ++i) {
            const qreal f = m_g.value(m_open[i].id) + m_h.value(m_open[i].id);
            if (f < bestF) {
                bestF = f;
                bestIndex = i;
            }
        }
        const Record current = m_open.takeAt(bestIndex);
        m_closed.insert(current.id);
        m_closedList.append(current.id);

        const bool reached = (current.id == goalId);
        if (!reached)
            expandNeighbors(current);
        snapshot(current.id, reached);

        if (reached) {
            m_found = true;
            break;
        }
    }

    return m_status;
}

const QList<AStarStep> &AStarEngine::steps() const
{
    return m_steps;
}

bool AStarEngine::foundPath() const
{
    return m_found;
}

QList<Graph::NodeId> AStarEngine::path() const
{
    if (!m_found)
        return {};
    QList<Graph::NodeId> result;
    Graph::NodeId id = m_goalId;
    while (id != Graph::kInvalidNodeId) {
        result.prepend(id);
        id = m_parents.value(id, Graph::kInvalidNodeId);
    }
    return result;
}

qreal AStarEngine::heuristic(Graph::NodeId nodeId) const
{
    return m_h.value(nodeId, 0.0);
}

qreal AStarEngine::realDistance(Graph::NodeId a, Graph::NodeId b) const
{
    const Graph::Node *na = m_graph.node(a);
    const Graph::Node *nb = m_graph.node(b);
    if (!na || !nb)
        return 0.0;
    const qreal dx = na->position.x() - nb->position.x();
    const qreal dy = na->position.y() - nb->position.y();
    return std::sqrt(dx * dx + dy * dy);
}

void AStarEngine::expandNeighbors(const Record &current)
{
    for (const Graph::Edge &edge : m_graph.edges()) {
        Graph::NodeId other = Graph::kInvalidNodeId;
        if (edge.a == current.id)
            other = edge.b;
        else if (edge.b == current.id)
            other = edge.a;
        else
            continue;
        if (m_closed.contains(other))
            continue;

        const qreal tentativeG = current.g + realDistance(current.id, other);

        Record *existing = nullptr;
        for (Record &record : m_open) {
            if (record.id == other) {
                existing = &record;
                break;
            }
        }

        if (!existing) {
            Record record;
            record.id = other;
            record.parent = current.id;
            record.g = tentativeG;
            m_open.append(record);
            m_parents[other] = current.id;
            m_g[other] = tentativeG;
        } else if (tentativeG < existing->g) {
            existing->g = tentativeG;
            existing->parent = current.id;
            m_parents[other] = current.id;
            m_g[other] = tentativeG;
        }
    }
}

void AStarEngine::snapshot(Graph::NodeId currentNodeId, bool goalReached)
{
    AStarStep step;
    step.currentNode = currentNodeId;
    step.goalReached = goalReached;

    for (const Record &record : m_open)
        step.openSet.append(record.id);
    step.closedSet = m_closedList;

    step.parents = m_parents;
    step.g = m_g;
    step.h = m_h;
    for (auto it = m_g.constBegin(); it != m_g.constEnd(); ++it)
        step.f.insert(it.key(), it.value() + m_h.value(it.key(), 0.0));

    m_steps.append(step);
}
