#pragma once

#include <queue>
#include "GameSituation.h"

namespace Graph {
    struct Node {
        Node(GameSituation gs, int id) : Gs(gs), Id(id) {}
        GameSituation Gs;
        int Id;
    };

    // Linear complexity
    bool AlreadyExists(int cindex, Node& node, std::vector<Node>& graph) {
        for (int i = 0; i < graph.size(); ++i) {
            if (node.Gs.SameSituation(graph[i].Gs)) {
                return true;
            }
        }

        return false;
    }

    // Linear complexity
    bool FindPath(const GameSituation& init, std::vector<Node>& graph) {
        graph.push_back({ init, -1 });

        std::queue<int> queue;
        queue.push(0);

        while (queue.size() > 0) {
            int cindex = queue.front();
            Node node = graph[cindex];

            for (int i = 0; i < node.Gs.NumOfMouvements(); ++i) {
                GameSituation buffGs = node.Gs.MoveVehicle(i);
                Node newNode = Node{ buffGs, cindex };
                if (newNode.Gs.FinalSituation()) {
                    graph.push_back(newNode);
                    return true;
                }

                if (!AlreadyExists(cindex, newNode, graph)) {
                    queue.push(graph.size());
                    graph.push_back(newNode);
                }
            }

            queue.pop();
        }

        return false;
    }

    // Linear complexity
    std::vector<GameSituation> Path(const GameSituation& init) {
        std::vector<Graph::Node> graph;
        Graph::FindPath(init, graph);

        Graph::Node node = graph[graph.size() - 1];
        std::vector<GameSituation> path;
        while (node.Id != -1) {
            path.push_back(node.Gs);
            node = graph[node.Id];
        }

        path.push_back(node.Gs);

        return path;
    }
}