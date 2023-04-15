#include <iostream>
#include <queue>
#include <cassert>
#include <map>
#include <cstdlib>

#include "Generator.h"

std::string filepath = "./data/files/puzzle.txt" ;

int main(int argc, char** argv) {

    if (argc > 1)
        filepath = argv[1] ;

    //GameSituation init_situation(filepath) ;

    //std::vector<Graph::Node> graph ;

    //if (Graph::FindPath(init_situation, graph)) {
    //    Graph::Node node = graph[graph.size() - 1] ;

    //    int n = 1 ;
    //    while (node.id != -1) {
    //        // std::cout << node->previous << std::endl ;
    //        n++ ;
    //        node = graph[node.id] ;
    //    }

    //    std::cout << "nombre de coup pour resoudre le puzzle: " << std::endl ;
    //}

    std::string outputfile = "./data/files/generated_gs.txt";

    //Generator::RandPuzzle(outputfile) ;

    return 0;
} 
