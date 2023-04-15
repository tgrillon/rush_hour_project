#pragma once

#include <random>
#include <utility>
#include <iostream>
#include <stack>

#include "Graph.h"

#define PROMPT 0

void AddLine(std::string& output, const Vehicle* vehicle) {
    std::ofstream oStream(output, std::ios::app);
    if (!oStream.is_open()) {
        std::cerr << "Can't find/open this file: " << output << std::endl;
        exit(-1);
    }
    oStream << '\n';
    oStream << vehicle->Position.Row;
    oStream << ' ';
    oStream << vehicle->Position.Col;
    oStream << ' ';
    oStream << vehicle->Length;
    oStream << ' ';
    oStream << vehicle->IsHorizontal;
    oStream.close();
}

void InitFile(std::string& outputFile, const Vehicle* vehicle, int gridSize) {
    std::ofstream oStream(outputFile.c_str());
    if (!oStream.is_open()) {
        std::cerr << "Can't find/open this file: " << outputFile << std::endl;
        exit(-1);
    }
    // dimension
    oStream << gridSize;
    oStream << ' ';
    oStream << gridSize;
    oStream << '\n';

    // exit position 
    oStream << vehicle->Position.Row;
    oStream << ' ';
    oStream << gridSize - 1;
    oStream.close();

    AddLine(outputFile, vehicle);
}


namespace Generator {
    const int gridSize = 6;

    void RandPuzzle(std::string& outputFile, Vehicle* first = nullptr, bool fromScratch = true) {

        // Set the attributes of the first car
        if (fromScratch) {
            std::random_device rng;
            std::uniform_int_distribution<int> ur(0, gridSize - 1);
            std::uniform_int_distribution<int> uc(0, gridSize - 3);

            first->Position = Box(ur(rng), uc(rng));
            first->Length = 2;
            first->IsHorizontal = 1;
        }

        // Initialize the file with grid dimention and the exit position 
        InitFile(outputFile, first, gridSize);

        std::vector<Vehicle> possibilities;
        // all vehicles available 
        for (int h = 0; h < gridSize; ++h) {
            for (int w = 0; w < gridSize; ++w) {
                if (h == first->Position.Row && (w == first->Position.Col || w == first->Position.Col + 1))
                    continue;

                for (int l = 2; l <= 3; ++l) {
                    for (int d = 0; d <= 1; ++d) {
                        if (!d && h + l - 1 >= gridSize)
                            continue;
                        if (d && w + l - 1 >= gridSize)
                            continue;
                        if (d && h == first->Position.Row)
                            continue;
                        Vehicle vehicle;
                        vehicle.Position = Box(h, w);
                        vehicle.Length = l;
                        vehicle.IsHorizontal = d;
                        possibilities.push_back(vehicle);
                    }
                }
            }
        }

        std::vector<bool> boxes(gridSize * gridSize, true);
        boxes[first->Position.Row * gridSize + first->Position.Col] = false;
        boxes[first->Position.Row * gridSize + first->Position.Col + 1] = false;

        int count = 1;

        int totalVehicles = gridSize * gridSize / 3;
        int failes = -1;
        bool stop = false;
        while (count < gridSize * gridSize / 3 && !possibilities.empty() && !boxes.empty()) {
            Vehicle vehicle;
            int isValid;
            std::vector<bool> bufferBox;
            do {
                failes++;
                if (possibilities.empty()) {
                    stop = true;
                    break;
                }

                std::random_device rng;
                std::uniform_int_distribution<int> u(0, possibilities.size() - 1);

                bufferBox = boxes;
                int key = u(rng);
                vehicle = possibilities[key];
                possibilities.erase(possibilities.begin() + key);

                int len = 0;
                isValid = true;
                while (len < vehicle.Length) {
                    int idx = ((vehicle.Position.Row + (1 - vehicle.IsHorizontal) * len)) * gridSize + (vehicle.Position.Col + vehicle.IsHorizontal * len);
                    if (!bufferBox[idx]) {
                        isValid = false;
                        break;
                    }
                    bufferBox[idx] = false;
                    len++;
                }
            } while (!isValid);

            if (stop) {
#if PROMPT
                std::cout << "Stopped! " << (int)((float)count / (float)totalVehicles * 100) << "% (" << count << "/" << totalVehicles << ") has failed " << failes << " times" << std::endl;
#endif
                break;
            }

            GameSituation gs_test(outputFile);
            gs_test.AddVehicle(vehicle);

            std::vector<Graph::Node> graph;
            if (!FindPath(gs_test, graph)) {
                failes++;
                continue;
            }

            count++;
            boxes = bufferBox;

#if PROMPT
            std::cout << (int)((float)count / (float)totalVehicles * 100) << "% (" << count << "/" << totalVehicles << ") has failed " << failes << " times" << std::endl;
#endif

            failes = 0;
            AddLine(outputFile, &vehicle);
        }
    }

    void TargetPosition(std::string& outputFile) {
        Vehicle first;
        first.Position = Box(2, gridSize - 2);
        first.Length = 2;
        first.IsHorizontal = 1;

#if PROMPT
        std::cout << "Generating the target position:" << std::endl;
#endif

        do {
            RandPuzzle(outputFile, &first, true);
#if PROMPT
            std::cout << "\x1B[2J\x1B[H";
            std::cout << "Generating the target position:" << std::endl;
#endif
        } while (!(GameSituation(outputFile).IsMovable(0)));
    }

    void HardestPuzzle(std::string& outputFile) {


#define TEST 0
#if TEST
        GameSituation target("./data/files/b.txt");
#else
        TargetPosition(outputFile);
        GameSituation target(outputFile);
#endif

        std::vector<Graph::Node> graph;
        {
            Graph::Node node(target, -1);
            graph.push_back(node);
        }

        GameSituation init = target.MoveVehicle(0);

        {
            Graph::Node node(init, 0);
            graph.push_back(node);
        }

        std::queue<int> queue;
        queue.push(1);

        while (queue.size() > 0) {
            int cindex = queue.front();
            Graph::Node node = graph[cindex];
            for (int i = 0; i < node.Gs.NumOfMouvements(); ++i) {
                GameSituation buffGs = node.Gs.MoveVehicle(i);
                Graph::Node newNode = Graph::Node(buffGs, cindex);

                if (!AlreadyExists(cindex, newNode, graph)) {
                    queue.push(graph.size());
                    graph.push_back(newNode);
                }
            }
            queue.pop();
        }

        int size = graph.size();
        int maxMoves = 0;
        int iSituation = size - 1;

#if PROMPT
        std::cout << "Searching the initial position requesting the most car moves" << std::endl;
#endif

        for (int i = size - 1; i >= size - size / 4; --i) {
            GameSituation gs = graph[i].Gs;

            int moves = Graph::Path(gs).size();
            if (moves > maxMoves) {
                maxMoves = moves;
                iSituation = i;
            }

#if PROMPT
            std::cout << "\x1B[2J\x1B[H";
            std::cout << "Searching the initial position requesting the most car moves" << std::endl;
            std::cout << "Loading... " << float(size - i) / float(size / 4) * 100 << "% (current requested moves: " << maxMoves << ")" << std::endl;
#endif
        }

        graph[iSituation].Gs.WriteToFile(outputFile);
    }

}
