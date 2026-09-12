#include "set_table.h"

#include <QEvent>
#include <QHeaderView>
#include <QLabel>
#include <QResizeEvent>
#include <QTableWidget>
#include <QVBoxLayout>

namespace {
const QColor kHighlightBackground{QStringLiteral("#353b3c")};
const QColor kHighlightForeground{QStringLiteral("#eef0f2")};
}

SetTable::SetTable(Graph &graph, bool withCostColumn, const QString &emptyHint, QWidget *parent)
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

    m_hintLabel = new QLabel(emptyHint, m_table->viewport());
    m_hintLabel->setObjectName("emptyHint");
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    m_hintLabel->setGeometry(m_table->viewport()->rect());
    m_hintLabel->show();
    m_table->viewport()->installEventFilter(this);
}

bool SetTable::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_table->viewport() && event->type() == QEvent::Resize) {
        auto *resizeEvent = static_cast<QResizeEvent *>(event);
        m_hintLabel->setGeometry(QRect(QPoint(0, 0), resizeEvent->size()));
    }
    return QWidget::eventFilter(watched, event);
}

void SetTable::updateHint()
{
    m_hintLabel->setVisible(m_table->rowCount() == 0);
    m_hintLabel->raise();
}

QString SetTable::displayName(Graph::NodeId nodeId) const
{
    const Graph::Node *node = m_graph.node(nodeId);
    return node ? node->name : QString();
}

void SetTable::showNodes(const QList<Graph::NodeId> &nodeIds, const AStarStep &step)
{
    m_table->setRowCount(nodeIds.size());
    updateHint();
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
    updateHint();
}
