#include "mainwindow.h"

#include "board/board_scene.h"
#include "board/board_toolbar.h"
#include "board/board_view.h"
#include "board/edge_item.h"
#include "board/node_item.h"
#include "board/solve_progress_bar.h"
#include "sidebar/edit_box.h"
#include "sidebar/set_table.h"
#include "solve/step_player.h"

#include <QFrame>
#include <QLabel>
#include <QMessageBox>
#include <QShortcut>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Simulador A*");
    resize(1280, 800);

    auto *splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setHandleWidth(1);

    auto *boardPane = new QWidget;
    auto *boardPaneLayout = new QVBoxLayout(boardPane);
    boardPaneLayout->setContentsMargins(0, 0, 0, 0);
    boardPaneLayout->setSpacing(0);

    m_board = new BoardView;
    m_toolbar = new BoardToolbar(m_board);
    m_board->setOverlay(m_toolbar);

    m_progressBar = new SolveProgressBar(boardPane);
    m_progressBar->setObjectName("solveProgressBar");
    m_progressBar->setVisible(false);

    boardPaneLayout->addWidget(m_board, 1);
    boardPaneLayout->addWidget(m_progressBar);

    m_scene = new BoardScene(m_graph, this);
    m_board->setScene(m_scene);

    m_sidebar = new QFrame;
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setMinimumWidth(280);

    auto *sidebarLayout = new QVBoxLayout(m_sidebar);
    sidebarLayout->setContentsMargins(0, 0, 0, 0);
    sidebarLayout->setSpacing(0);

    m_editBox = new EditBox(m_graph, *m_scene);
    m_openTable = new SetTable(m_graph, true);
    m_closedTable = new SetTable(m_graph, false);
    sidebarLayout->addWidget(makeSidebarSection("Edit box", m_editBox), 3);
    sidebarLayout->addWidget(makeSidebarSection("Open set", m_openTable), 3);
    sidebarLayout->addWidget(makeSidebarSection("Closed set", m_closedTable), 3);

    splitter->addWidget(boardPane);
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
        m_board->setPanEnabled(mode == BoardScene::Mode::Idle
                               || mode == BoardScene::Mode::Solve);
        m_toolbar->setNodeModeActive(mode == BoardScene::Mode::AddNode);
        m_toolbar->setLineModeActive(mode == BoardScene::Mode::AddLine);
        m_toolbar->setDeleteModeActive(mode == BoardScene::Mode::Delete);
        m_toolbar->setSolveModeActive(mode == BoardScene::Mode::Solve);
        m_board->refreshOverlay();
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

    m_player = new StepPlayer(this);
    connect(m_toolbar, &BoardToolbar::solveToggled, this, [this](bool checked) {
        if (checked)
            m_player->start(m_graph, m_graph.startNodeId(), m_graph.goalNodeId());
        else
            m_player->stop();
    });
    connect(m_player, &StepPlayer::started, this, [this] {
        m_progressBar->reset();
        m_progressBar->setVisible(true);
        m_scene->setSolveMode(true);
    });
    connect(m_player, &StepPlayer::stopped, this, [this] {
        m_scene->setSolveMode(false);
        m_scene->clearSolveState();
        m_openTable->clearRows();
        m_closedTable->clearRows();
        m_progressBar->reset();
        m_progressBar->setVisible(false);
    });
    connect(m_player, &StepPlayer::stepChanged, this, [this](int stepIndex, int stepCount) {
        const AStarStep &step = m_player->currentStep();
        m_scene->applySolveState(step);
        m_openTable->showNodes(step.openSet, step);
        m_closedTable->showNodes(step.closedSet, step);
        m_progressBar->setProgress(stepIndex, stepCount);
        if (stepIndex == stepCount - 1 && !m_player->foundPath())
            statusBar()->showMessage(
                QStringLiteral("El algoritmo termin\u00f3 sin encontrar camino al destino"),
                4000);
    });
    connect(m_player, &StepPlayer::validationFailed, this, [this](const QString &message) {
        QMessageBox::warning(this, QStringLiteral("Resolver"), message);
        m_toolbar->setSolveModeActive(false);
    });
    connect(m_player, &StepPlayer::runFinished, this, [this](bool found) {
        if (found)
            statusBar()->showMessage(QStringLiteral("Camino encontrado"), 4000);
    });
    connect(m_player, &StepPlayer::autoRunningChanged, m_toolbar, &BoardToolbar::setAutoRunning);
    connect(m_toolbar, &BoardToolbar::continueClicked, m_player, &StepPlayer::next);
    connect(m_toolbar, &BoardToolbar::rewindClicked, m_player, &StepPlayer::previous);
    connect(m_toolbar, &BoardToolbar::autoToggled, m_player, &StepPlayer::setAutoRunning);
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
