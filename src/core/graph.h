#pragma once

#include <QColor>
#include <QPointF>
#include <QString>
#include <QVector>

class Graph
{
public:
    using NodeId = int;
    static constexpr NodeId kInvalidNodeId = -1;
    using EdgeId = int;
    static constexpr EdgeId kInvalidEdgeId = -1;

    struct Node {
        NodeId id = kInvalidNodeId;
        QString name;
        QColor color;
        QPointF position;
        bool isStart = false;
        bool isGoal = false;
    };

    struct Edge {
        EdgeId id = kInvalidEdgeId;
        NodeId a = kInvalidNodeId;
        NodeId b = kInvalidNodeId;
        QColor color;
    };

    NodeId addNode(const QPointF &position);
    void removeNode(NodeId id);
    bool hasNode(NodeId id) const;
    const Node *node(NodeId id) const;
    Node *node(NodeId id);

    void setNodeName(NodeId id, const QString &name);
    void setNodeColor(NodeId id, const QColor &color);
    void setNodePosition(NodeId id, const QPointF &position);

    EdgeId addEdge(NodeId a, NodeId b);
    void removeEdge(EdgeId id);
    bool hasEdgeBetween(NodeId a, NodeId b) const;
    const Edge *edge(EdgeId id) const;
    void setEdgeColor(EdgeId id, const QColor &color);

    NodeId startNodeId() const;
    void setStartNode(NodeId id);
    NodeId goalNodeId() const;
    void setGoalNode(NodeId id);

    const QVector<Node> &nodes() const;
    const QVector<Edge> &edges() const;

private:
    static QString nodeNameForIndex(int index);

    QVector<Node> m_nodes;
    QVector<Edge> m_edges;
    NodeId m_nextNodeId = 0;
    int m_nextNameIndex = 0;
    EdgeId m_nextEdgeId = 0;
};
