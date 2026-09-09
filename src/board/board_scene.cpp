#include "board_scene.h"

#include "node_item.h"

#include <QGraphicsSceneMouseEvent>

BoardScene::BoardScene(Graph &graph, QObject *parent)
    : QGraphicsScene(parent)
    , m_graph(graph)
{
}

BoardScene::Mode BoardScene::mode() const
{
    return m_mode;
}

void BoardScene::setAddNodeMode(bool active)
{
    setMode(active ? Mode::AddNode : Mode::Idle);
}

void BoardScene::cancelInteraction()
{
    setMode(Mode::Idle);
}

void BoardScene::setMode(Mode mode)
{
    if (m_mode == mode)
        return;
    m_mode = mode;
    emit modeChanged(m_mode);
}

void BoardScene::addNodeAt(const QPointF &pos)
{
    const Graph::NodeId id = m_graph.addNode(pos);
    const Graph::Node *node = m_graph.node(id);

    auto *item = new NodeItem(id, node->name, node->color);
    item->setPos(pos);
    addItem(item);
    m_nodeItems.insert(id, item);

    connect(item, &NodeItem::positionChanged, this, [this](Graph::NodeId nodeId) {
        m_graph.setNodePosition(nodeId, m_nodeItems.value(nodeId)->pos());
    });
}

void BoardScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    if (m_mode == Mode::AddNode) {
        if (items(event->scenePos()).isEmpty())
            addNodeAt(event->scenePos());
        event->accept();
        return;
    }

    QGraphicsScene::mousePressEvent(event);
}
