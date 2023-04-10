#pragma once

#include <random>
#include <utility>
#include <fstream>
#include <iostream>
#include <stack>

void addLine(std::ostream& o_stream, int row, int col, int len, int dir) {
    o_stream << '\n';
    o_stream << row;
    o_stream << ' ';
    o_stream << col;
    o_stream << ' ';
    o_stream << len;
    o_stream << ' ';
    o_stream << dir;
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
            int exit_column = uc(rng);

            // dimension
            f_writing << size_grid;
            f_writing << ' ';
            f_writing << size_grid;
            f_writing << '\n';

            // exit position 
            f_writing << exit_row;
            f_writing << ' ';
            f_writing << size_grid - 1;

            addLine(f_writing, exit_row, exit_column, 2, 1);

            // all available boxes  
            for (int h = 0; h < size_grid; ++h) {
                for (int w = 0; w < size_grid; ++w) {
                    if (h == exit_row && (w == exit_column || w == exit_column + 1))
                        continue;
                    pairs.push_back({ h, w });
                }
            }

            std::cout << "[ ";
            for (const auto& p : pairs) {
                std::cout << "(" << p.first << ", " << p.second << "), " << " ";
            }
            std::cout << " ]\n";

            int count = 0;
            while (count < size_grid * size_grid / 3 - 1 && !pairs.empty()) {
                std::random_device rng;
                std::uniform_int_distribution<int> u(0, pairs.size() - 1);
                std::uniform_int_distribution<int> ul(2, 3);
                std::uniform_int_distribution<int> ud(0, 1);

                int length = ul(rng);
                int direction = ud(rng);
                int key = u(rng);

                std::pair<int, int> pair = pairs[key];
                if (pair.first == exit_row && direction) {
                    direction = 0;
                }

                if (pair.first + (1 - direction) * (length - 1) < size_grid &&
                    pair.second + direction * (length - 1) < size_grid) {
                    std::stack<std::pair<int, int>> s;
                    pairs.erase(pairs.begin() + key);

                    int n = 1;
                    int i = 0;
                    while (i < pairs.size()) {
                        if (n == length) break;
                        std::pair<int, int> p = pairs[i];
                        for (int l = 1; l < length; ++l) {
                            int hl = pair.first + (1 - direction) * l;
                            int wl = pair.second + direction * l;
                            if (wl == p.second && hl == p.first) {
                                std::pair<int, int> buff = pairs[i];
                                s.push(buff);
                                pairs.erase(pairs.begin() + i);
                                n++;
                            }
                        }
                        i++;
                    }

                    if (n != length) {
                        pairs.push_back(pair);
                        for (int i = 0; i < s.size(); ++i) {
                            pairs.push_back(s.top());
                            s.pop();
                        }
                    }
                    else {
                        count++;
                        addLine(f_writing, pair.first, pair.second, length, direction);
                    }
                }
            }
        }
        else {
            std::cout << "Error: No such file at '" << output_file << "'" << std::endl;
            exit(-1);
        }

        f_writing.close();
    }
}