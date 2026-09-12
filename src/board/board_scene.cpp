#include "board_scene.h"

#include "edge_item.h"
#include "node_item.h"

#include <QGraphicsSceneMouseEvent>
#include <QVariantAnimation>

#include <algorithm>

namespace {
constexpr qreal kIdealDistance = 110.0;
constexpr int kArrangeIterations = 300;
constexpr qreal kArrangeStartTemp = 70.0;
constexpr qreal kArrangeTempDecay = 0.95;
constexpr int kArrangeDurationMs = 400;
}

BoardScene::BoardScene(Graph &graph, QObject *parent)
    : QGraphicsScene(parent)
    , m_graph(graph)
{
}

BoardScene::Mode BoardScene::mode() const
{
    return m_mode;
}

NodeItem *BoardScene::nodeItem(Graph::NodeId nodeId) const
{
    return m_nodeItems.value(nodeId);
}

EdgeItem *BoardScene::edgeItem(Graph::EdgeId edgeId) const
{
    return m_edgeItems.value(edgeId);
}

void BoardScene::refreshNodeRoles()
{
    for (auto it = m_nodeItems.constBegin(); it != m_nodeItems.constEnd(); ++it) {
        const Graph::Node *node = m_graph.node(it.key());
        if (node)
            it.value()->setRole(node->isStart, node->isGoal);
    }
}

void BoardScene::applySolveState(const AStarStep &step)
{
    for (auto it = m_nodeItems.constBegin(); it != m_nodeItems.constEnd(); ++it) {
        const Graph::NodeId nodeId = it.key();
        NodeItem::SolveVisual visual = NodeItem::SolveVisual::None;
        if (step.currentNode == nodeId)
            visual = NodeItem::SolveVisual::Current;
        else if (step.closedSet.contains(nodeId))
            visual = NodeItem::SolveVisual::Closed;
        else if (step.openSet.contains(nodeId))
            visual = NodeItem::SolveVisual::Open;
        it.value()->setSolveVisual(visual);
    }
}

void BoardScene::clearSolveState()
{
    for (auto it = m_nodeItems.constBegin(); it != m_nodeItems.constEnd(); ++it)
        it.value()->setSolveVisual(NodeItem::SolveVisual::None);
    for (auto it = m_edgeItems.constBegin(); it != m_edgeItems.constEnd(); ++it)
        it.value()->setHighlight(false);
}

void BoardScene::highlightPath(const QList<Graph::NodeId> &path)
{
    for (int i = 0; i + 1 < path.size(); ++i) {
        for (auto it = m_edgeItems.constBegin(); it != m_edgeItems.constEnd(); ++it) {
            EdgeItem *edgeItem = it.value();
            const bool matches =
                (edgeItem->endpointA() == path[i] && edgeItem->endpointB() == path[i + 1])
                || (edgeItem->endpointA() == path[i + 1] && edgeItem->endpointB() == path[i]);
            if (matches) {
                edgeItem->setHighlight(true);
                break;
            }
        }
    }
}

void BoardScene::deleteSelection()
{
    if (m_mode != Mode::Idle)
        return;
    const QList<QGraphicsItem *> selected = selectedItems();
    for (QGraphicsItem *item : selected) {
        if (auto *node = qgraphicsitem_cast<NodeItem *>(item))
            removeNodeVisual(node->nodeId());
        else if (auto *edge = qgraphicsitem_cast<EdgeItem *>(item))
            removeEdgeVisual(edge->edgeId());
    }
}

void BoardScene::setAddNodeMode(bool active)
{
    setMode(active ? Mode::AddNode : Mode::Idle);
}

void BoardScene::setAddLineMode(bool active)
{
    setMode(active ? Mode::AddLine : Mode::Idle);
}

void BoardScene::setDeleteMode(bool active)
{
    setMode(active ? Mode::Delete : Mode::Idle);
}

void BoardScene::setSolveMode(bool active)
{
    setMode(active ? Mode::Solve : Mode::Idle);
}

void BoardScene::cancelInteraction()
{
    stopGhostLine();
    m_lineAnchorId = Graph::kInvalidNodeId;
    setMode(Mode::Idle);
}

void BoardScene::setMode(Mode mode)
{
    if (m_mode == mode)
        return;
    if (m_mode == Mode::AddLine && mode != Mode::AddLine)
        stopGhostLine();
    m_mode = mode;
    emit modeChanged(m_mode);
}

Graph::NodeId BoardScene::addNodeAt(const QPointF &pos)
{
    const Graph::NodeId id = m_graph.addNode(pos);
    const Graph::Node *node = m_graph.node(id);

    auto *item = new NodeItem(id, node->name, node->color);
    item->setPos(pos);
    addItem(item);
    m_nodeItems.insert(id, item);

    connect(item, &NodeItem::positionChanged, this, [this](Graph::NodeId nodeId) {
        m_graph.setNodePosition(nodeId, m_nodeItems.value(nodeId)->pos());
        updateEdgeItemsForNode(nodeId);
    });

    return id;
}

NodeItem *BoardScene::nodeItemAt(const QPointF &pos) const
{
    for (QGraphicsItem *item : items(pos)) {
        if (auto *node = qgraphicsitem_cast<NodeItem *>(item))
            return node;
    }
    return nullptr;
}

EdgeItem *BoardScene::edgeItemAt(const QPointF &pos) const
{
    for (QGraphicsItem *item : items(pos)) {
        if (auto *edge = qgraphicsitem_cast<EdgeItem *>(item))
            return edge;
    }
    return nullptr;
}

void BoardScene::startGhostLineFrom(NodeItem *node)
{
    m_lineAnchorId = node->nodeId();
    m_ghostLine = new QGraphicsLineItem;
    m_ghostLine->setPen(QPen(QColor("#353b3c"), 2.0, Qt::DashLine));
    m_ghostLine->setZValue(5.0);
    m_ghostLine->setLine(QLineF(node->pos(), node->pos()));
    addItem(m_ghostLine);
}

void BoardScene::stopGhostLine()
{
    if (m_ghostLine) {
        removeItem(m_ghostLine);
        delete m_ghostLine;
        m_ghostLine = nullptr;
    }
    m_lineAnchorId = Graph::kInvalidNodeId;
}

void BoardScene::createEdgeItem(Graph::EdgeId edgeId)
{
    const Graph::Edge *edge = m_graph.edge(edgeId);
    if (!edge)
        return;

    NodeItem *a = m_nodeItems.value(edge->a);
    NodeItem *b = m_nodeItems.value(edge->b);
    if (!a || !b)
        return;

    auto *item = new EdgeItem(edgeId, edge->a, edge->b);
    item->setEndpoints(a->pos(), b->pos());
    addItem(item);
    m_edgeItems.insert(edgeId, item);
}

void BoardScene::updateEdgeItemsForNode(Graph::NodeId nodeId)
{
    for (auto it = m_edgeItems.constBegin(); it != m_edgeItems.constEnd(); ++it) {
        EdgeItem *edgeItem = it.value();
        if (edgeItem->endpointA() != nodeId && edgeItem->endpointB() != nodeId)
            continue;
        NodeItem *a = m_nodeItems.value(edgeItem->endpointA());
        NodeItem *b = m_nodeItems.value(edgeItem->endpointB());
        if (a && b)
            edgeItem->setEndpoints(a->pos(), b->pos());
    }
}

void BoardScene::removeNodeVisual(Graph::NodeId nodeId)
{
    NodeItem *item = m_nodeItems.take(nodeId);
    if (!item)
        return;

    for (auto it = m_edgeItems.begin(); it != m_edgeItems.end();) {
        if (it.value()->endpointA() == nodeId || it.value()->endpointB() == nodeId) {
            m_graph.removeEdge(it.key());
            removeItem(it.value());
            delete it.value();
            it = m_edgeItems.erase(it);
        } else {
            ++it;
        }
    }

    m_graph.removeNode(nodeId);
    removeItem(item);
    delete item;
}

void BoardScene::removeEdgeVisual(Graph::EdgeId edgeId)
{
    m_graph.removeEdge(edgeId);
    if (auto *item = m_edgeItems.take(edgeId)) {
        removeItem(item);
        delete item;
    }
}

void BoardScene::arrangeNodes()
{
    if (m_arrangeAnimation && m_arrangeAnimation->state() == QAbstractAnimation::Running)
        return;
    const QVector<Graph::Node> nodes = m_graph.nodes();
    if (nodes.size() < 2)
        return;

    const int count = nodes.size();
    QHash<Graph::NodeId, int> index;
    QVector<QPointF> pos(count);
    QPointF originalCentroid(0, 0);
    for (int i = 0; i < count; ++i) {
        index.insert(nodes[i].id, i);
        pos[i] = nodes[i].position;
        originalCentroid += nodes[i].position;
    }
    originalCentroid /= count;

    qreal temp = kArrangeStartTemp;
    for (int iter = 0; iter < kArrangeIterations && temp > 0.5; ++iter) {
        QVector<QPointF> disp(count);
        for (int i = 0; i < count; ++i) {
            for (int j = i + 1; j < count; ++j) {
                const QPointF delta = pos[i] - pos[j];
                const qreal dist = std::max(QLineF(QPointF(0, 0), delta).length(), 0.01);
                const qreal force = (kIdealDistance * kIdealDistance) / dist;
                const QPointF unit = delta / dist;
                disp[i] += unit * force;
                disp[j] -= unit * force;
            }
        }
        for (const Graph::Edge &edge : m_graph.edges()) {
            const int a = index.value(edge.a, -1);
            const int b = index.value(edge.b, -1);
            if (a < 0 || b < 0)
                continue;
            const QPointF delta = pos[a] - pos[b];
            const qreal dist = std::max(QLineF(QPointF(0, 0), delta).length(), 0.01);
            const qreal force = (dist * dist) / kIdealDistance;
            const QPointF unit = delta / dist;
            disp[a] -= unit * force;
            disp[b] += unit * force;
        }
        for (int i = 0; i < count; ++i) {
            const qreal len = QLineF(QPointF(0, 0), disp[i]).length();
            if (len > temp)
                disp[i] = disp[i] / len * temp;
            pos[i] += disp[i];
        }
        QPointF centroid(0, 0);
        for (int i = 0; i < count; ++i)
            centroid += pos[i];
        centroid /= count;
        const QPointF correction = centroid - originalCentroid;
        for (int i = 0; i < count; ++i)
            pos[i] -= correction;
        temp *= kArrangeTempDecay;
    }

    QHash<Graph::NodeId, QPointF> startPositions;
    QHash<Graph::NodeId, QPointF> endPositions;
    for (int i = 0; i < count; ++i) {
        startPositions.insert(nodes[i].id, nodes[i].position);
        endPositions.insert(nodes[i].id, pos[i]);
    }

    m_arrangeAnimation = new QVariantAnimation(this);
    m_arrangeAnimation->setDuration(kArrangeDurationMs);
    m_arrangeAnimation->setEasingCurve(QEasingCurve::OutCubic);
    m_arrangeAnimation->setStartValue(0.0);
    m_arrangeAnimation->setEndValue(1.0);
    connect(m_arrangeAnimation, &QVariantAnimation::valueChanged, this,
            [this, startPositions, endPositions](const QVariant &value) {
                const qreal t = value.toReal();
                for (auto it = endPositions.constBegin(); it != endPositions.constEnd(); ++it) {
                    if (NodeItem *item = m_nodeItems.value(it.key())) {
                        const QPointF start = startPositions.value(it.key());
                        item->setPos(start + (it.value() - start) * t);
                    }
                }
            });
    m_arrangeAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void BoardScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QGraphicsScene::mousePressEvent(event);
        return;
    }

    const QPointF pos = event->scenePos();

    switch (m_mode) {
    case Mode::AddNode: {
        if (items(pos).isEmpty())
            addNodeAt(pos);
        event->accept();
        return;
    }
    case Mode::AddLine: {
        NodeItem *target = nodeItemAt(pos);
        if (m_lineAnchorId == Graph::kInvalidNodeId) {
            if (target) {
                startGhostLineFrom(target);
            } else {
                const Graph::NodeId newId = addNodeAt(pos);
                startGhostLineFrom(m_nodeItems.value(newId));
            }
        } else if (target && target->nodeId() != m_lineAnchorId) {
            const Graph::EdgeId edgeId = m_graph.addEdge(m_lineAnchorId, target->nodeId());
            if (edgeId != Graph::kInvalidEdgeId)
                createEdgeItem(edgeId);
            stopGhostLine();
        } else if (!target) {
            const Graph::NodeId newId = addNodeAt(pos);
            const Graph::EdgeId edgeId = m_graph.addEdge(m_lineAnchorId, newId);
            if (edgeId != Graph::kInvalidEdgeId)
                createEdgeItem(edgeId);
            stopGhostLine();
            startGhostLineFrom(m_nodeItems.value(newId));
        }
        event->accept();
        return;
    }
    case Mode::Delete: {
        if (NodeItem *node = nodeItemAt(pos))
            removeNodeVisual(node->nodeId());
        else if (EdgeItem *edge = edgeItemAt(pos))
            removeEdgeVisual(edge->edgeId());
        event->accept();
        return;
    }
    case Mode::Idle:
        break;
    case Mode::Solve:
        event->accept();
        return;
    }

    QGraphicsScene::mousePressEvent(event);
}

void BoardScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (m_mode == Mode::AddLine && m_ghostLine
        && m_lineAnchorId != Graph::kInvalidNodeId) {
        m_ghostLine->setLine(QLineF(m_nodeItems.value(m_lineAnchorId)->pos(),
                                    event->scenePos()));
        event->accept();
        return;
    }

    QGraphicsScene::mouseMoveEvent(event);
}
