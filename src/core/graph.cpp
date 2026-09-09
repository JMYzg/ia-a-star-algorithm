#include "graph.h"

QString Graph::nodeNameForIndex(int index)
{
    QString name;
    int n = index;
    do {
        name.prepend(QChar('A' + (n % 26)));
        n = n / 26 - 1;
    } while (n >= 0);
    return name;
}

Graph::NodeId Graph::addNode(const QPointF &position)
{
    Node node;
    node.id = m_nextNodeId++;
    node.name = nodeNameForIndex(m_nextNameIndex++);
    node.color = QColor("#c6c7c4");
    node.position = position;
    m_nodes.append(node);
    return node.id;
}

void Graph::removeNode(NodeId id)
{
    for (int i = 0; i < m_nodes.size(); ++i) {
        if (m_nodes[i].id == id) {
            m_nodes.removeAt(i);
            return;
        }
    }
}

bool Graph::hasNode(NodeId id) const
{
    return node(id) != nullptr;
}

const Graph::Node *Graph::node(NodeId id) const
{
    for (const Node &n : m_nodes) {
        if (n.id == id)
            return &n;
    }
    return nullptr;
}

Graph::Node *Graph::node(NodeId id)
{
    for (Node &n : m_nodes) {
        if (n.id == id)
            return &n;
    }
    return nullptr;
}

void Graph::setNodeName(NodeId id, const QString &name)
{
    if (Node *n = node(id))
        n->name = name;
}

void Graph::setNodeColor(NodeId id, const QColor &color)
{
    if (Node *n = node(id))
        n->color = color;
}

void Graph::setNodePosition(NodeId id, const QPointF &position)
{
    if (Node *n = node(id))
        n->position = position;
}

Graph::NodeId Graph::startNodeId() const
{
    for (const Node &n : m_nodes) {
        if (n.isStart)
            return n.id;
    }
    return kInvalidNodeId;
}

void Graph::setStartNode(NodeId id)
{
    for (Node &n : m_nodes)
        n.isStart = (n.id == id);
}

Graph::NodeId Graph::goalNodeId() const
{
    for (const Node &n : m_nodes) {
        if (n.isGoal)
            return n.id;
    }
    return kInvalidNodeId;
}

void Graph::setGoalNode(NodeId id)
{
    for (Node &n : m_nodes)
        n.isGoal = (n.id == id);
}

const QVector<Graph::Node> &Graph::nodes() const
{
    return m_nodes;
}
