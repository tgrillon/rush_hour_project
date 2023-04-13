#pragma once

#include <queue>
#include "GameSituation.h"

namespace Graph {
    struct Node {
        GameSituation gs;
        int id;
    };

    bool AlreadyExists(int cindex, Node& node, std::vector<Node>& graph) {
        for (int i = 0; i < graph.size(); ++i) {
            if (node.gs.SameSituation(graph[i].gs)) {
                return true;
            }
        }

        return false;
    }

    bool FindPath(const GameSituation& init, std::vector<Node>& graph) {
        graph.push_back({ init, -1 });

        std::queue<int> queue;
        queue.push(0);

        while (queue.size() > 0) {
            int cindex = queue.front();
            Node node = graph[cindex];

            for (int i = 0; i < node.gs.NumOfMouvements(); ++i) {
                if (graph.size() > 2000)
                    return false;
                GameSituation buff_gs = node.gs.MoveVehicle(i);
                Node new_node = Node{ buff_gs, cindex };
                if (new_node.gs.FinalSituation()) {
                    graph.push_back(new_node);
                    return true;
                }
                if (!AlreadyExists(cindex, new_node, graph)) {
                    queue.push(graph.size());
                    graph.push_back(new_node);
                }
            }

            queue.pop();
        }

        return false;
    }
}