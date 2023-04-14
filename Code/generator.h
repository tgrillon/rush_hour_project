#pragma once

#include <random>
#include <utility>
#include <fstream>
#include <iostream>
#include <stack>

#include "Graph.h"

void AddLine(std::string& output, const Vehicle& vehicle) {
    std::ofstream oStream(output, std::ios::app);
    if (!oStream.is_open()) {
        std::cerr << "Can't find this file: " << output << std::endl;
        exit(-1);
    }
    oStream << '\n';
    oStream << vehicle.Position.Row;
    oStream << ' ';
    oStream << vehicle.Position.Col;
    oStream << ' ';
    oStream << vehicle.Length;
    oStream << ' ';
    oStream << vehicle.IsHorizontal;
    oStream.close();
}

namespace Generator {
    const int gridSize = 6;

    void RandPuzzle(std::string& outputFile) {
       /* std::random_device rng;
        std::uniform_int_distribution<int> ur(0, size_grid - 1);
        std::uniform_int_distribution<int> uc(0, size_grid - 3);*/

        Vehicle first;
        first.Position = Box(1, 0);
        first.Length = 2;
        first.IsHorizontal = 1;

        std::ofstream oStream(outputFile.c_str());
        if (!oStream.is_open()) {
            std::cerr << "Can't find this file: " << outputFile << std::endl;
            exit(-1);
        }
        // dimension
        oStream << gridSize;
        oStream << ' ';
        oStream << gridSize;
        oStream << '\n';

        // exit position 
        oStream << first.Position.Row;
        oStream << ' ';
        oStream << gridSize - 1;
        oStream.close();

        AddLine(outputFile, first);

        std::vector<Vehicle> possibilities;
        // all available vehicles 
        for (int h = 0; h < gridSize; ++h) {
            for (int w = 0; w < gridSize; ++w) {
                if (h == first.Position.Row && (w == first.Position.Col || w == first.Position.Col + 1))
                    continue;

                for (int l = 2; l <= 3; ++l) {
                    for (int d = 0; d <= 1; ++d) {
                        if (!d && h + l - 1 >= gridSize)
                            continue;
                        if (d && w + l - 1 >= gridSize)
                            continue;
                        if (d && h == first.Position.Row)
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

        std::cout << "num of possibilities: " << possibilities.size() << std::endl;

        std::vector<bool> boxes(gridSize * gridSize, true);
        boxes[first.Position.Row * gridSize + first.Position.Col] = false;
        boxes[first.Position.Row * gridSize + first.Position.Col + 1] = false;

        /*std::cout << "[ ";
        for (const auto& p : possibilities) {
            std::cout << "(" << p.Position.Row << ", " << p.Position.Col << ", " << p.Length << ", " << p.IsHorizontal << "), " << " ";
        }
        std::cout << " ]\n";*/

        int count = 1;
            
        int totalVehicles = gridSize * gridSize / 3;
        int failes = -1;
        bool stop = false;
        int erases = 0;
        while (count < gridSize * gridSize / 3 && !possibilities.empty() && !boxes.empty()) {
            //std::vector<Vehicle> bufferVhc = possibilities;
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
                erases++;

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
                std::cout << "Stopped! " << (int)((float)count / (float)totalVehicles * 100) << "% (" << count << "/" << totalVehicles << ") has failed " << failes << " times" << std::endl;
                std::cout << "Erases: " << erases << std::endl;
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
            std::cout << (int)((float)count / (float)totalVehicles * 100) << "% (" << count << "/" << totalVehicles << ") has failed " << failes << " times" << std::endl;
            failes = 0;
            AddLine(outputFile, vehicle);
        }
    }
}