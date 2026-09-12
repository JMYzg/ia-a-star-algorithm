#include "set_table.h"

#include <QHeaderView>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
const QColor kHighlightBackground{QStringLiteral("#353b3c")};
const QColor kHighlightForeground{QStringLiteral("#eef0f2")};
}

SetTable::SetTable(Graph &graph, bool withCostColumn, QWidget *parent)
    : QWidget(parent)
    , m_graph(graph)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_table = new QTableWidget(0, withCostColumn ? 3 : 2, this);
    QStringList headers{QStringLiteral("N"), QStringLiteral("Father")};
    if (withCostColumn)
        headers << QStringLiteral("f()");
    m_table->setHorizontalHeaderLabels(headers);
    m_table->verticalHeader()->setVisible(false);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);
    m_table->setFocusPolicy(Qt::NoFocus);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionsClickable(false);

    layout->addWidget(m_table);
}

QString SetTable::displayName(Graph::NodeId nodeId) const
{
    const Graph::Node *node = m_graph.node(nodeId);
    return node ? node->name : QString();
}

void SetTable::showNodes(const QList<Graph::NodeId> &nodeIds, const AStarStep &step)
{
    m_table->setRowCount(nodeIds.size());
    for (int row = 0; row < nodeIds.size(); ++row) {
        const Graph::NodeId nodeId = nodeIds[row];

        auto *nameItem = new QTableWidgetItem(displayName(nodeId));
        m_table->setItem(row, 0, nameItem);

        const Graph::NodeId parentId = step.parents.value(nodeId, Graph::kInvalidNodeId);
        const QString fatherName = parentId == Graph::kInvalidNodeId
                                       ? QStringLiteral("-")
                                       : displayName(parentId);
        m_table->setItem(row, 1, new QTableWidgetItem(fatherName));

        if (m_table->columnCount() == 3) {
            m_table->setItem(row, 2,
                             new QTableWidgetItem(QString::number(step.f.value(nodeId), 'f', 1)));
        }

        if (nodeId == step.currentNode) {
            for (int col = 0; col < m_table->columnCount(); ++col) {
                QTableWidgetItem *item = m_table->item(row, col);
                if (!item)
                    continue;
                item->setBackground(kHighlightBackground);
                item->setForeground(kHighlightForeground);
            }
        }
    }
}

void SetTable::clearRows()
{
    m_table->setRowCount(0);
}
