#include "board_toolbar.h"

#include <QButtonGroup>
#include <QToolButton>
#include <QVBoxLayout>

namespace {
const QString kCancelText = QStringLiteral("\u00d7");
const QString kNodeText = QStringLiteral("Nodo");
const QString kLineText = QStringLiteral("L\u00ednea");
const QString kDeleteText = QStringLiteral("Borrar");
}

BoardToolbar::BoardToolbar(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("boardToolbar");
    setAttribute(Qt::WA_StyledBackground);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(4);

    m_nodeButton = makeButton(kNodeText, "nodeButton");
    m_lineButton = makeButton(kLineText, "lineButton");
    m_deleteButton = makeButton(kDeleteText, "deleteButton");
    m_arrangeButton = makeButton("Ordenar", "arrangeButton", false);
    m_solveButton = makeButton("Resolver", "solveButton");

    layout->addWidget(m_nodeButton);
    layout->addWidget(m_lineButton);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_arrangeButton);
    layout->addSpacing(6);
    layout->addWidget(m_solveButton);

    connect(m_nodeButton, &QToolButton::toggled, this, &BoardToolbar::nodeToggled);
    connect(m_lineButton, &QToolButton::toggled, this, &BoardToolbar::lineToggled);
    connect(m_deleteButton, &QToolButton::toggled, this, &BoardToolbar::deleteToggled);

    m_modeGroup = new QButtonGroup(this);
    m_modeGroup->setExclusive(true);
    m_modeGroup->addButton(m_nodeButton);
    m_modeGroup->addButton(m_lineButton);
    m_modeGroup->addButton(m_deleteButton);
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

void BoardToolbar::setModeButton(QToolButton *button, bool active, const QString &idleText)
{
    QSignalBlocker blocker(button);
    button->setChecked(active);
    button->setText(active ? kCancelText : idleText);
}

void BoardToolbar::setNodeModeActive(bool active)
{
    setModeButton(m_nodeButton, active, kNodeText);
}

void BoardToolbar::setLineModeActive(bool active)
{
    setModeButton(m_lineButton, active, kLineText);
}

void BoardToolbar::setDeleteModeActive(bool active)
{
    setModeButton(m_deleteButton, active, kDeleteText);
}
