#include "path_summary.h"

#include <QLabel>
#include <QVBoxLayout>

PathSummary::PathSummary(const QString &emptyHint, QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(4);

    m_hintLabel = new QLabel(emptyHint, this);
    m_hintLabel->setObjectName("emptyHint");
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setWordWrap(true);

    m_pathLabel = new QLabel(this);
    m_pathLabel->setObjectName("pathSummaryText");
    m_pathLabel->setWordWrap(true);
    m_pathLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    m_costLabel = new QLabel(this);
    m_costLabel->setObjectName("pathSummaryCost");

    layout->addWidget(m_hintLabel);
    layout->addWidget(m_pathLabel);
    layout->addWidget(m_costLabel);
    layout->addStretch(1);
    updateVisibility();
}

void PathSummary::showPath(const QString &pathText, qreal totalCost)
{
    m_pathLabel->setText(pathText);
    m_costLabel->setText(
        QStringLiteral("Costo total: %1").arg(QString::number(totalCost, 'f', 1)));
    updateVisibility();
}

void PathSummary::showNoPath()
{
    m_pathLabel->setText(QStringLiteral("No se encontr\u00f3 camino al destino."));
    m_costLabel->clear();
    updateVisibility();
}

void PathSummary::clear()
{
    m_pathLabel->clear();
    m_costLabel->clear();
    updateVisibility();
}

void PathSummary::updateVisibility()
{
    const bool hasContent = !m_pathLabel->text().isEmpty();
    m_pathLabel->setVisible(hasContent);
    m_costLabel->setVisible(!m_costLabel->text().isEmpty());
    m_hintLabel->setVisible(!hasContent);
}
