#include "mainwindow.h"

#include "board/board_scene.h"
#include "board/board_toolbar.h"
#include "board/board_view.h"
#include "board/edge_item.h"
#include "board/node_item.h"
#include "sidebar/edit_box.h"

#include <QFrame>
#include <QLabel>
#include <QShortcut>
#include <QSplitter>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Simulador A*");
    resize(1280, 800);

    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(1);

    m_board = new BoardView;
    m_toolbar = new BoardToolbar(m_board);
    m_board->setOverlay(m_toolbar);

    m_scene = new BoardScene(m_graph, this);
    m_board->setScene(m_scene);

    m_sidebar = new QFrame;
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setMinimumWidth(280);

    auto *sidebarLayout = new QVBoxLayout(m_sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    m_editBox = new EditBox(m_graph, *m_scene);
    sidebarLayout->addWidget(makeSidebarSection("Edit box", m_editBox), 3);
    sidebarLayout->addWidget(makeSidebarSection("Open set"), 3);
    sidebarLayout->addWidget(makeSidebarSection("Closed set"), 3);

    splitter->addWidget(m_board);
    splitter->addWidget(m_sidebar);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({960, 320});

    setCentralWidget(splitter);

    connect(m_toolbar, &BoardToolbar::nodeToggled, m_scene, &BoardScene::setAddNodeMode);
    connect(m_toolbar, &BoardToolbar::lineToggled, m_scene, &BoardScene::setAddLineMode);
    connect(m_toolbar, &BoardToolbar::deleteToggled, m_scene, &BoardScene::setDeleteMode);
    connect(m_toolbar, &BoardToolbar::arrangeClicked, m_scene, &BoardScene::arrangeNodes);
    connect(m_scene, &BoardScene::modeChanged, this, [this](BoardScene::Mode mode) {
        m_board->setPanEnabled(mode == BoardScene::Mode::Idle);
        m_toolbar->setNodeModeActive(mode == BoardScene::Mode::AddNode);
        m_toolbar->setLineModeActive(mode == BoardScene::Mode::AddLine);
        m_toolbar->setDeleteModeActive(mode == BoardScene::Mode::Delete);
    });

    connect(m_scene, &QGraphicsScene::selectionChanged, this, [this] {
        const QList<QGraphicsItem *> selected = m_scene->selectedItems();
        for (QGraphicsItem *item : selected) {
            if (auto *node = qgraphicsitem_cast<NodeItem *>(item)) {
                m_editBox->selectNode(node->nodeId());
                return;
            }
        }
        for (QGraphicsItem *item : selected) {
            if (auto *edge = qgraphicsitem_cast<EdgeItem *>(item)) {
                m_editBox->selectEdge(edge->edgeId());
                return;
            }
        }
        m_editBox->clearSelection();
    });

    auto *cancelShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(cancelShortcut, &QShortcut::activated, m_scene, &BoardScene::cancelInteraction);
}

QWidget *MainWindow::makeSidebarSection(const QString &title, QWidget *content) const
{
    auto *section = new QFrame;
    section->setObjectName("sidebarSection");

    auto *sectionLayout = new QVBoxLayout(section);
    sectionLayout->setContentsMargins(0, 0, 0, 0);
    sectionLayout->setSpacing(0);

    auto *titleLabel = new QLabel(title, section);
    titleLabel->setObjectName("sidebarSectionTitle");
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    sectionLayout->addWidget(titleLabel);
    if (content) {
        sectionLayout->addWidget(content, 1);
    } else {
        auto *placeholder = new QLabel("-", section);
        placeholder->setObjectName("sectionPlaceholder");
        placeholder->setAlignment(Qt::AlignCenter);
        sectionLayout->addWidget(placeholder, 1);
    }

    return section;
}
