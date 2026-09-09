#include "board_toolbar.h"

#include <QToolButton>
#include <QVBoxLayout>

namespace {
const QString kCancelText = QStringLiteral("\u00d7");
}

BoardToolbar::BoardToolbar(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("boardToolbar");
    setAttribute(Qt::WA_StyledBackground);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(4);

    m_nodeButton = makeButton("Nodo", "nodeButton");
    m_lineButton = makeButton("Línea", "lineButton");
    m_deleteButton = makeButton("Borrar", "deleteButton");
    m_arrangeButton = makeButton("Ordenar", "arrangeButton", false);
    m_solveButton = makeButton("Resolver", "solveButton");

    layout->addWidget(m_nodeButton);
    layout->addWidget(m_lineButton);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_arrangeButton);
    layout->addSpacing(6);
    layout->addWidget(m_solveButton);

    connect(m_nodeButton, &QToolButton::toggled, this, &BoardToolbar::nodeToggled);
}

QToolButton *BoardToolbar::makeButton(const QString &text, const QString &objectName,
                                      bool checkable)
{
    auto *button = new QToolButton(this);
    button->setText(text);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setCheckable(checkable);
    if (!objectName.isEmpty())
        button->setObjectName(objectName);
    return button;
}

void BoardToolbar::setNodeModeActive(bool active)
{
    QSignalBlocker blocker(m_nodeButton);
    m_nodeButton->setChecked(active);
    m_nodeButton->setText(active ? kCancelText : QStringLiteral("Nodo"));
}
