#pragma once

#include <QGraphicsScene>
#include <QHash>
#include <QPointer>
#include <QVariantAnimation>

#include "core/graph.h"

class NodeItem;
class EdgeItem;

class BoardScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum class Mode { Idle, AddNode, AddLine, Delete, Solve };

    explicit BoardScene(Graph &graph, QObject *parent = nullptr);

    Mode mode() const;
    NodeItem *nodeItem(Graph::NodeId nodeId) const;
    EdgeItem *edgeItem(Graph::EdgeId edgeId) const;
    void refreshNodeRoles();

public slots:
    void setAddNodeMode(bool active);
    void setAddLineMode(bool active);
    void setDeleteMode(bool active);
    void cancelInteraction();
    void arrangeNodes();

signals:
    void modeChanged(BoardScene::Mode mode);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void setMode(Mode mode);
    Graph::NodeId addNodeAt(const QPointF &pos);
    NodeItem *nodeItemAt(const QPointF &pos) const;
    EdgeItem *edgeItemAt(const QPointF &pos) const;
    void startGhostLineFrom(NodeItem *node);
    void stopGhostLine();
    void createEdgeItem(Graph::EdgeId edgeId);
    void updateEdgeItemsForNode(Graph::NodeId nodeId);
    void removeNodeVisual(Graph::NodeId nodeId);
    void removeEdgeVisual(Graph::EdgeId edgeId);

    Graph &m_graph;
    Mode m_mode = Mode::Idle;
    QHash<Graph::NodeId, NodeItem *> m_nodeItems;
    QHash<Graph::EdgeId, EdgeItem *> m_edgeItems;
    Graph::NodeId m_lineAnchorId = Graph::kInvalidNodeId;
    QGraphicsLineItem *m_ghostLine = nullptr;
    QPointer<QVariantAnimation> m_arrangeAnimation;
};
