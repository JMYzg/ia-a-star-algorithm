#include "board_toolbar.h"

#include <QToolButton>
#include <QVBoxLayout>

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
    m_arrangeButton = makeButton("Ordenar", "arrangeButton");
    m_solveButton = makeButton("Resolver", "solveButton");

    layout->addWidget(m_nodeButton);
    layout->addWidget(m_lineButton);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_arrangeButton);
    layout->addSpacing(6);
    layout->addWidget(m_solveButton);
}

QToolButton *BoardToolbar::makeButton(const QString &text, const QString &objectName)
{
    auto *button = new QToolButton(this);
    button->setText(text);
    button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    button->setCheckable(true);
    if (!objectName.isEmpty())
        button->setObjectName(objectName);
    return button;
}
