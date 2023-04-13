#pragma once

#include <random>
#include <utility>
#include <fstream>
#include <iostream>
#include <stack>

#include "Graph.h"

void addLine(std::string& output, int row, int col, int len, int dir) {
    std::ofstream o_stream(output, std::ios::app);
    if (!o_stream.is_open()) {
        std::cerr << "Can't find this file: " << output << std::endl;
        exit(-1);
    }
    o_stream << '\n';
    o_stream << row;
    o_stream << ' ';
    o_stream << col;
    o_stream << ' ';
    o_stream << len;
    o_stream << ' ';
    o_stream << dir;
    o_stream.close();
}

namespace Generator {
    const int size_grid = 6;

    void rand_puzzle(std::string& output_file) {

        std::ofstream f_writing(output_file.c_str());

        if (f_writing) {
            std::vector<std::pair<int, int>> pairs;

            std::random_device rng;
            std::uniform_int_distribution<int> ur(0, size_grid - 1);
            std::uniform_int_distribution<int> uc(0, size_grid - 3);

            int exit_row = 2;
            int exit_column = 2;


            std::ofstream f_writing(output_file.c_str());
            // dimension
            f_writing << size_grid;
            f_writing << ' ';
            f_writing << size_grid;
            f_writing << '\n';

            // exit position 
            f_writing << exit_row;
            f_writing << ' ';
            f_writing << size_grid - 1;
            f_writing.close();

            addLine(output_file, exit_row, exit_column, 2, 1);

            // all available boxes  
            for (int h = 0; h < size_grid; ++h) {
                for (int w = 0; w < size_grid; ++w) {
                    if (h == exit_row && (w == exit_column || w == exit_column + 1))
                        continue;
                    pairs.push_back({ h, w });
                }
            }

           /* std::cout << "[ ";
            for (const auto& p : pairs) {
                std::cout << "(" << p.first << ", " << p.second << "), " << " ";
            }
            std::cout << " ]\n";*/

            int count = 1;
            
            int total_vehicles = size_grid * size_grid / 3;
            int failes = 0;
            while (count < size_grid * size_grid / 3 && !pairs.empty()) {
                std::vector<std::pair<int, int>> buffer = pairs; 

                std::random_device rng;
                std::uniform_int_distribution<int> u(0, buffer.size() - 1);
                std::uniform_int_distribution<int> ul(2, 3);
                std::uniform_int_distribution<int> ud(0, 1);

                int length = ul(rng);
                int direction = ud(rng);
                int key = u(rng);

                std::pair<int, int> pair = buffer[key];
                if (pair.first == exit_row && direction) {
                    direction = 0;
                }

                if (pair.first + (1 - direction) * (length - 1) < size_grid &&
                    pair.second + direction * (length - 1) < size_grid) {
                    std::stack<std::pair<int, int>> s;
                    buffer.erase(buffer.begin() + key);

                    int n = 1;
                    int i = 0;
                    while (i < buffer.size()) {
                        if (n == length) break;
                        std::pair<int, int> p = buffer[i];
                        for (int l = 1; l < length; ++l) {
                            int hl = pair.first + (1 - direction) * l;
                            int wl = pair.second + direction * l;
                            if (wl == p.second && hl == p.first) {
                                std::pair<int, int> buff = pairs[i];
                                s.push(buff);
                                buffer.erase(buffer.begin() + i);
                                n++;
                            }
                        }
                        i++;
                    }

                    if (n != length) {
                        failes++;
                        continue;
                    }
                    
                    GameSituation gs_test(output_file);
                    gs_test.AddVehicle(pair.first, pair.second, length, direction);

                    std::vector<Graph::Node> graph;
                    if (!FindPath(gs_test, graph)) {
                        failes++;
                        continue;
                    }

                    pairs = buffer;
                    count++;
                    std::cout << (int)((float)count / (float)total_vehicles * 100) << "% (" << count << "/" << total_vehicles << ") has failed " << failes << " times" << std::endl;
                    failes = 0;
                    addLine(output_file, pair.first, pair.second, length, direction);
                }
            }
        }
    }
}