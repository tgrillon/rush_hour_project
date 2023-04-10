#include <iostream>
#include <cmath>
#include <queue>
#include <cassert>
#include <map>
#include <cstdlib>
#include <random>
#include <utility>
#include <stack>

#include "game_situation.h"

struct Node {
    game_situation gs ;
    int id ;
} ;

///CECI EST LA BRANCHE MASTER ET UNIQUEMENT MASTER 

bool alreadyExists(int cindex, Node& node, std::vector<Node>& graph) {
   
    // Complexity : Theta(n) (with n the number of elements in graph) 
    for (int i = 0; i < graph.size(); ++i) {
        if (node.gs.sameSituation(graph[i].gs)) {
            return true ;
        }
    }

   return false ;
}

bool findPath(const game_situation& init, std::vector<Node>& graph) {
    graph.push_back({ init, -1 }) ;
    
    std::queue<int> queue ;
    queue.push(0) ;

    while (queue.size() > 0) {
        int cindex = queue.front() ;
        Node node = graph[cindex] ; 
        
        for (int i = 0; i < node.gs.numOfMouvements(); ++i) {
            game_situation buff_gs = node.gs.moveVehicle(i) ;
            Node new_node = Node { buff_gs, cindex } ;
            if (new_node.gs.finalSituation()) {
                graph.push_back(new_node) ;
                return true ;
            }
            if (!alreadyExists(cindex, new_node, graph)) {
                queue.push(graph.size()) ;
                graph.push_back(new_node) ;
            }
        }

        queue.pop() ;
    }

    return false ;
}

const int size_grid = 4 ;

void generate(std::string& output_file) {

    std::ofstream f_writing(output_file.c_str()) ;

    if (f_writing) {
        std::vector<std::pair<int, int>> couples(size_grid * size_grid) ;

        // all available boxes  
        for (int h = 0; h < size_grid; ++h) {
            for (int w = 0; w < size_grid; ++w) {
                couples[h * size_grid + w] = { w, h } ;
            }  
        }

        std::cout << "[ ";
        for (const auto& p : couples ) {
            std::cout << "(" << p.first << ", " << p.second << "), " << " " ;
        }
        std::cout << " ]\n" ;

        // dimension
        f_writing << size_grid ;
        f_writing << ' ' ;
        f_writing << size_grid ;
        f_writing << '\n' ;

        // exit position 
        f_writing << 2 ;
        f_writing << ' ' ;
        f_writing << size_grid - 1 ;
        
        int count = 0 ;
        while (count < size_grid * size_grid / 3) {
            std::random_device rng ;
            std::uniform_int_distribution<int> u(0, couples.size()) ;
            std::uniform_int_distribution<int> ul(2, 3) ;
            std::uniform_int_distribution<int> ud(0, 1) ;

            int length = ul(rng) ;
            int direction = ud(rng) ;
            int key = u(rng) ;

            if (couples[key].first + direction * (length - 1) < size_grid && 
            couples[key].second + (1 - direction) * (length - 1) < size_grid) {
                std::pair<int, int> pair = couples[key] ;
                // std::cout << "w: " << (pair.first + direction * (length - 1)) << " h: " 
                //     << (pair.second + (1 - direction) * (length - 1)) << std::endl ;
                std::stack<std::pair<int, int>> s ;
                std::cout << "(" << pair.first << ", " << pair.second << ")" << std::endl ;
                couples.erase(couples.begin()+key) ;

                std::cout << "[ ";
                for (const auto& p : couples ) {
                    std::cout << "(" << p.first << ", " << p.second << "), " << " " ;
                }
                std::cout << " ]\n" ;

                std::cin.get() ;
                
                int n = 1 ;
                int i = 0 ;
                while (i < couples.size()) {
                    // std::cout << i << " " << couples.size() << std::endl ;
                    if (n == length) break ;
                    std::pair<int, int> p = couples[i] ;
                    for (int l = 1; l < length; ++l) {
                        int wl = pair.first + direction * l ;
                        int hl = pair.second + (1 - direction) * l ;
                        if (wl == p.first && hl == p.second) {
                            std::pair<int, int> buff = couples[i] ;
                            s.push(buff) ;
                            // std::cout << "(" << p.first << ", " << p.second << ")" << std::endl ;
                            couples.erase(couples.begin()+i) ;
                            n++ ;
                        }   
                    }
                    ++i ;
                }

                if (n != length) {
                    // couples.push_back(pair) ;
                    // for (int i = 0; i < s.size(); ++i) {
                    //     couples.push_back(s.top()) ;
                    //     s.pop() ;
                    // }
                } else {
                    int column = couples[key].first ;
                    int row = couples[key].second ;

                    count++ ;

                    f_writing << '\n' ;
                    
                    f_writing << column ;
                    f_writing << ' ' ;

                    f_writing << row ;
                    f_writing << ' ' ;

                    f_writing << length ;
                    f_writing << ' ' ;

                    f_writing << direction ;
                }

            }
        }
    } else {
        std::cout << "Error: No such file at '" << output_file << "'" << std::endl ;
        exit(-1) ;
    }

    f_writing.close() ;
}

std::string filepath = "./data/files/puzzle.txt" ;

int main(int argc, char** argv) {

    if (argc > 1)
        filepath = argv[1] ;

    game_situation init_situation(filepath) ;

    std::vector<Node> graph ;

    if (findPath(init_situation, graph)) {
        Node node = graph[graph.size() - 1] ;

        int n = 1 ;
        while (node.id != -1) {
            // std::cout << node->previous << std::endl ;
            n++ ;
            node = graph[node.id] ;
        }

        std::cout << "nombre de coup pour resoudre le puzzle: " << std::endl ;
   }

    return 0;
} 
