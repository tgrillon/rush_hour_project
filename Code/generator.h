#pragma once

#include <random>
#include <utility>
#include <fstream>
#include <iostream>
#include <stack>

namespace Generator {
    const int size_grid = 6;

    void rand_puzzle(std::string& output_file) {

        std::ofstream f_writing(output_file.c_str());

        if (f_writing) {
            std::vector<std::pair<int, int>> pairs;

            std::random_device rng;
            std::uniform_int_distribution<int> ur(0, size_grid - 1);
            std::uniform_int_distribution<int> uc(0, size_grid - 3);

            int exit_row = ur(rng);
            int exit_column = uc(rng);

            std::cout << exit_row << " " << exit_column << std::endl;

            // dimension
            f_writing << size_grid;
            f_writing << ' ';
            f_writing << size_grid;
            f_writing << '\n';

            // exit position 
            f_writing << exit_row;
            f_writing << ' ';
            f_writing << size_grid - 1;

            f_writing << '\n';

            f_writing << exit_row;
            f_writing << ' ';

            f_writing << exit_column;
            f_writing << ' ';

            f_writing << 2;
            f_writing << ' ';

            f_writing << 1;

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
                    // std::cout << "w: " << (pair.first + direction * (length - 1)) << " h: " 
                    //     << (pair.second + (1 - direction) * (length - 1)) << std::endl ;
                    std::stack<std::pair<int, int>> s;
                    //std::cout << "(" << pair.first << ", " << pair.second << ", " << length << ", " << direction << " )" << std::endl;
                    pairs.erase(pairs.begin() + key);

                    int n = 1;
                    int i = 0;
                    while (i < pairs.size()) {
                        // std::cout << i << " " << pairs.size() << std::endl ;
                        if (n == length) break;
                        std::pair<int, int> p = pairs[i];
                        for (int l = 1; l < length; ++l) {
                            int hl = pair.first + (1 - direction) * l;
                            int wl = pair.second + direction * l;
                            if (wl == p.second && hl == p.first) {
                                std::pair<int, int> buff = pairs[i];
                                s.push(buff);
                                //std::cout << "erased : (" << p.first << ", " << p.second << ")" << std::endl;
                                pairs.erase(pairs.begin() + i);
                                n++;
                            }
                        }
                        i++;
                    }

                    /*std::cout << "n=" << n << "[";
                    for (const auto& p : pairs) {
                        std::cout << "(" << p.first << ", " << p.second << "), " << " ";
                    }
                    std::cout << " ]\n";

                    std::cin.get();*/

                    if (n != length) {
                        //std::cout << "Annulation!" << std::endl;
                        pairs.push_back(pair);
                        for (int i = 0; i < s.size(); ++i) {
                            pairs.push_back(s.top());
                            s.pop();
                        }
                    }
                    else {
                        //std::cout << "Ajout!" << std::endl;
                        int row = pair.first;
                        int column = pair.second;

                        count++;

                        f_writing << '\n';

                        f_writing << row;
                        f_writing << ' ';

                        f_writing << column;
                        f_writing << ' ';

                        f_writing << length;
                        f_writing << ' ';

                        f_writing << direction;
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