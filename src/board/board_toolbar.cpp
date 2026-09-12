#include "board_toolbar.h"

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
    m_continueButton = makeButton("Continuar", "continueButton", false);
    m_rewindButton = makeButton("Retroceder", "rewindButton", false);
    m_autoButton = makeButton("Auto", "autoButton");

    m_continueButton->setVisible(false);
    m_rewindButton->setVisible(false);
    m_autoButton->setVisible(false);

    layout->addWidget(m_nodeButton);
    layout->addWidget(m_lineButton);
    layout->addWidget(m_deleteButton);
    layout->addWidget(m_arrangeButton);
    layout->addSpacing(6);
    layout->addWidget(m_solveButton);
    layout->addWidget(m_continueButton);
    layout->addWidget(m_rewindButton);
    layout->addWidget(m_autoButton);

    connect(m_nodeButton, &QToolButton::toggled, this, [this](bool checked) {
        if (checked)
            uncheckOthersSilently(m_nodeButton);
        emit nodeToggled(checked);
    });
    connect(m_lineButton, &QToolButton::toggled, this, [this](bool checked) {
        if (checked)
            uncheckOthersSilently(m_lineButton);
        emit lineToggled(checked);
    });
    connect(m_deleteButton, &QToolButton::toggled, this, [this](bool checked) {
        if (checked)
            uncheckOthersSilently(m_deleteButton);
        emit deleteToggled(checked);
    });
    connect(m_arrangeButton, &QToolButton::clicked, this, &BoardToolbar::arrangeClicked);
    connect(m_solveButton, &QToolButton::toggled, this, &BoardToolbar::solveToggled);
    connect(m_continueButton, &QToolButton::clicked, this, &BoardToolbar::continueClicked);
    connect(m_rewindButton, &QToolButton::clicked, this, &BoardToolbar::rewindClicked);
    connect(m_autoButton, &QToolButton::toggled, this, &BoardToolbar::autoToggled);
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

void BoardToolbar::uncheckOthersSilently(QToolButton *active)
{
    for (QToolButton *button : { m_nodeButton, m_lineButton, m_deleteButton }) {
        if (button == active)
            continue;
        QSignalBlocker blocker(button);
        button->setChecked(false);
    }
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

void BoardToolbar::setSolveModeActive(bool active)
{
    QSignalBlocker blocker(m_solveButton);
    m_solveButton->setChecked(active);
    m_solveButton->setText(active ? QStringLiteral("Detener") : QStringLiteral("Resolver"));

    m_nodeButton->setVisible(!active);
    m_lineButton->setVisible(!active);
    m_deleteButton->setVisible(!active);
    m_arrangeButton->setVisible(!active);
    m_continueButton->setVisible(active);
    m_rewindButton->setVisible(active);
    m_autoButton->setVisible(active);

    if (!active) {
        QSignalBlocker autoBlocker(m_autoButton);
        m_autoButton->setChecked(false);
    }
    updateGeometry();
}

void BoardToolbar::setAutoRunning(bool running)
{
    QSignalBlocker blocker(m_autoButton);
    m_autoButton->setChecked(running);
}
