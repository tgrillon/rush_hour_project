#include <iostream>
#include <queue>
#include <cassert>
#include <map>
#include <cstdlib>

#include "game_situation.h"
#include "generator.h"

struct Node {
    game_situation gs ;
    int id ;
} ;

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

std::string filepath = "./data/files/puzzle.txt" ;

int main(int argc, char** argv) {

   // if (argc > 1)
   //     filepath = argv[1] ;

   // game_situation init_situation(filepath) ;

   // std::vector<Node> graph ;

   // if (findPath(init_situation, graph)) {
   //     Node node = graph[graph.size() - 1] ;

   //     int n = 1 ;
   //     while (node.id != -1) {
   //         // std::cout << node->previous << std::endl ;
   //         n++ ;
   //         node = graph[node.id] ;
   //     }

   //     std::cout << "nombre de coup pour resoudre le puzzle: " << std::endl ;
   //}

    std::string outputfile = "./data/files/generated_gs.txt";

    Generator::rand_puzzle(outputfile) ;

    return 0;
} 
