#include "edit_box.h"

#include "board/board_scene.h"
#include "board/edge_item.h"
#include "board/node_item.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QPushButton>

EditBox::EditBox(Graph &graph, BoardScene &scene, QWidget *parent)
    : QWidget(parent)
    , m_graph(graph)
    , m_scene(scene)
{
    auto *layout = new QFormLayout(this);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(10);

    m_nameEdit = new QLineEdit(this);
    m_colorButton = new QPushButton(this);
    m_colorButton->setObjectName("colorSwatch");
    m_colorButton->setFixedSize(48, 22);
    m_startCheck = new QCheckBox("Inicio", this);
    m_goalCheck = new QCheckBox("Destino", this);

    layout->addRow("Nombre", m_nameEdit);
    layout->addRow("Color", m_colorButton);
    layout->addRow(m_startCheck);
    layout->addRow(m_goalCheck);

    connect(m_nameEdit, &QLineEdit::textEdited, this, &EditBox::applyName);
    connect(m_colorButton, &QPushButton::clicked, this, &EditBox::applyColor);
    connect(m_startCheck, &QCheckBox::toggled, this, &EditBox::applyStart);
    connect(m_goalCheck, &QCheckBox::toggled, this, &EditBox::applyGoal);

    refresh();
}

void EditBox::selectNode(Graph::NodeId nodeId)
{
    m_nodeId = nodeId;
    m_edgeId = Graph::kInvalidEdgeId;
    refresh();
}

void EditBox::selectEdge(Graph::EdgeId edgeId)
{
    m_edgeId = edgeId;
    m_nodeId = Graph::kInvalidNodeId;
    refresh();
}

void EditBox::clearSelection()
{
    m_nodeId = Graph::kInvalidNodeId;
    m_edgeId = Graph::kInvalidEdgeId;
    refresh();
}

QColor EditBox::currentColor() const
{
    if (const Graph::Node *node = m_graph.node(m_nodeId))
        return node->color;
    if (const Graph::Edge *edge = m_graph.edge(m_edgeId))
        return edge->color;
    return QColor("#eef0f2");
}

void EditBox::refresh()
{
    const Graph::Node *node = m_graph.node(m_nodeId);
    const bool hasNode = node != nullptr;
    const bool hasColor = hasNode || m_graph.edge(m_edgeId) != nullptr;

    {
        QSignalBlocker nameBlocker(m_nameEdit);
        QSignalBlocker startBlocker(m_startCheck);
        QSignalBlocker goalBlocker(m_goalCheck);
        m_nameEdit->setText(node ? node->name : QString());
        m_startCheck->setChecked(node && node->isStart);
        m_goalCheck->setChecked(node && node->isGoal);
    }

    m_nameEdit->setEnabled(hasNode);
    m_startCheck->setEnabled(hasNode);
    m_goalCheck->setEnabled(hasNode);
    m_colorButton->setEnabled(hasColor);
    m_colorButton->setStyleSheet(QString("background: %1;").arg(currentColor().name()));
}

void EditBox::applyName()
{
    if (m_nodeId == Graph::kInvalidNodeId)
        return;
    const QString name = m_nameEdit->text().trimmed();
    if (name.isEmpty())
        return;
    m_graph.setNodeName(m_nodeId, name);
    if (NodeItem *item = m_scene.nodeItem(m_nodeId))
        item->setName(name);
}

void EditBox::applyColor()
{
    const QColor color = QColorDialog::getColor(currentColor(), this, "Elegir color");
    if (!color.isValid())
        return;

    if (NodeItem *item = m_scene.nodeItem(m_nodeId)) {
        m_graph.setNodeColor(m_nodeId, color);
        item->setColor(color);
    } else if (EdgeItem *item = m_scene.edgeItem(m_edgeId)) {
        m_graph.setEdgeColor(m_edgeId, color);
        item->setColor(color);
    }
    refresh();
}

void EditBox::applyStart(bool checked)
{
    if (m_nodeId == Graph::kInvalidNodeId)
        return;
    m_graph.setStartNode(checked ? m_nodeId : Graph::kInvalidNodeId);
    m_scene.refreshNodeRoles();
}

void EditBox::applyGoal(bool checked)
{
    if (m_nodeId == Graph::kInvalidNodeId)
        return;
    m_graph.setGoalNode(checked ? m_nodeId : Graph::kInvalidNodeId);
    m_scene.refreshNodeRoles();
}
