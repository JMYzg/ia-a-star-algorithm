#include <cstdio>

#include "core/a_star_engine.h"
#include "core/graph.h"

static int failures = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            std::printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);         \
            ++failures;                                                        \
        }                                                                      \
    } while (0)

static void checkLinearPath()
{
    Graph graph;
    const Graph::NodeId a = graph.addNode(QPointF(0, 0));
    const Graph::NodeId b = graph.addNode(QPointF(0, 10));
    const Graph::NodeId c = graph.addNode(QPointF(0, 20));
    const Graph::NodeId d = graph.addNode(QPointF(0, 30));
    graph.addEdge(a, b);
    graph.addEdge(b, c);
    graph.addEdge(c, d);

    AStarEngine engine(graph);
    const AStarEngine::Status status = engine.run(a, d);
    CHECK(status == AStarEngine::Status::Ready);
    CHECK(engine.foundPath());
    const QList<Graph::NodeId> path = engine.path();
    CHECK(path.size() == 4);
    CHECK(path[0] == a);
    CHECK(path[1] == b);
    CHECK(path[2] == c);
    CHECK(path[3] == d);

    const QList<AStarStep> &steps = engine.steps();
    CHECK(steps.size() == 4);
    CHECK(steps[0].currentNode == a);
    CHECK(steps[0].goalReached == false);
    CHECK(steps[0].openSet.contains(b));
    CHECK(steps[1].currentNode == b);
    CHECK(steps[2].currentNode == c);
    CHECK(steps[3].currentNode == d);
    CHECK(steps[3].goalReached == true);
    CHECK(!steps[3].openSet.contains(d));
    CHECK(steps[3].closedSet.contains(d));
    CHECK(steps[3].parents.value(d) == c);
    CHECK(steps[3].parents.value(a) == Graph::kInvalidNodeId);

    const AStarStep &last = steps[3];
    CHECK(std::abs(last.f.value(d) - 30.0) < 1e-6);
    CHECK(std::abs(last.g.value(d) - 30.0) < 1e-6);
    CHECK(std::abs(last.h.value(a) - 30.0) < 1e-6);


}

static void checkShorterRouteChosen()
{
    Graph graph;
    const Graph::NodeId a = graph.addNode(QPointF(0, 0));
    const Graph::NodeId b = graph.addNode(QPointF(10, 0));
    const Graph::NodeId c = graph.addNode(QPointF(0, 20));
    const Graph::NodeId d = graph.addNode(QPointF(10, 20));
    graph.addEdge(a, b);
    graph.addEdge(b, d);
    graph.addEdge(a, c);
    graph.addEdge(c, d);

    AStarEngine engine(graph);
    engine.run(a, d);
    CHECK(engine.foundPath());
    const QList<Graph::NodeId> path = engine.path();
    CHECK(path.size() == 3);
    CHECK(path[1] == b);
}

static void checkRouteRelaxation()
{
    Graph graph;
    const Graph::NodeId a = graph.addNode(QPointF(0, 0));
    const Graph::NodeId b = graph.addNode(QPointF(1, 0));
    const Graph::NodeId c = graph.addNode(QPointF(5, 0));
    graph.addEdge(a, b);
    graph.addEdge(b, c);
    graph.addEdge(a, c);

    AStarEngine engine(graph);
    engine.run(a, c);
    CHECK(engine.foundPath());
    const QList<AStarStep> &steps = engine.steps();
    CHECK(steps.last().parents.value(c) == a);
    const QList<Graph::NodeId> path = engine.path();
    CHECK(path.size() == 2);
    CHECK(path[1] == c);
}

static void checkUnreachableGoal()
{
    Graph graph;
    const Graph::NodeId a = graph.addNode(QPointF(0, 0));
    const Graph::NodeId b = graph.addNode(QPointF(0, 10));
    const Graph::NodeId c = graph.addNode(QPointF(100, 100));
    graph.addEdge(a, b);

    AStarEngine engine(graph);
    engine.run(a, c);
    CHECK(!engine.foundPath());
    CHECK(engine.path().isEmpty());
    CHECK(!engine.steps().isEmpty());
    CHECK(engine.steps().last().openSet.isEmpty());
}

static void checkValidation()
{
    Graph graph;
    const Graph::NodeId a = graph.addNode(QPointF(0, 0));
    const Graph::NodeId b = graph.addNode(QPointF(0, 10));

    AStarEngine engine(graph);
    CHECK(engine.run(Graph::kInvalidNodeId, b) == AStarEngine::Status::MissingStart);
    CHECK(engine.run(a, Graph::kInvalidNodeId) == AStarEngine::Status::MissingGoal);
    CHECK(engine.steps().isEmpty());
}

static void checkStartEqualsGoal()
{
    Graph graph;
    const Graph::NodeId a = graph.addNode(QPointF(0, 0));
    const Graph::NodeId b = graph.addNode(QPointF(0, 10));

    AStarEngine engine(graph);
    engine.run(a, a);
    CHECK(engine.foundPath());
    CHECK(engine.path().size() == 1);
    CHECK(engine.steps().size() == 1);
    CHECK(engine.steps()[0].goalReached);
}

int main()
{
    checkLinearPath();
    checkShorterRouteChosen();
    checkRouteRelaxation();
    checkUnreachableGoal();
    checkValidation();
    checkStartEqualsGoal();

    if (failures == 0) {
        std::printf("All A* engine tests passed\n");
        return 0;
    }
    std::printf("%d failure(s)\n", failures);
    return 1;
}
