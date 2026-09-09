#pragma once

#include <QGraphicsScene>
#include <QHash>

#include "core/graph.h"

class NodeItem;

class BoardScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum class Mode { Idle, AddNode, AddLine, Delete, Solve };

    explicit BoardScene(Graph &graph, QObject *parent = nullptr);

    Mode mode() const;

public slots:
    void setAddNodeMode(bool active);
    void cancelInteraction();

signals:
    void modeChanged(BoardScene::Mode mode);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

private:
    void setMode(Mode mode);
    void addNodeAt(const QPointF &pos);

    Graph &m_graph;
    Mode m_mode = Mode::Idle;
    QHash<Graph::NodeId, NodeItem *> m_nodeItems;
};
