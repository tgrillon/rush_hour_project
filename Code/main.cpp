//#include <iostream>
//#include <cmath>
//#include <queue>
//#include <cassert>
//#include <map>
//
//#include "game_situation.h"

//struct Node {
//    uint32_t height ;
//    game_situation gs;
//    std::vector<size_t> neighbors ;
//} ;
//
//uint32_t alreadyExists(uint32_t cindex, Node& node, std::vector<Node>& graph) {
//    for (uint32_t i = 0; i < graph.size(); ++i) {
//        if (node.gs.sameSituation(graph[i].gs)) {
//            graph[i].neighbors.push_back(cindex) ;
//            return i ;
//        }
//    }
//
//    return -1 ;
//}
//
//std::vector<Node> buildGraph(const game_situation& init_situation) {
//    std::vector<Node> graph ;
//    graph.push_back(Node {0, init_situation, { 0 }}) ;
//
//    std::queue<size_t> queue ; 
//    queue.push(0) ;
//
//    while (!queue.empty()) {
//        size_t cindex = queue.front() ;
//
//        Node node = graph[cindex] ;
//        for (uint32_t i = 0; i < node.gs.numOfMouvements(); ++i) {
//            game_situation* buff_gs = node.gs.moveVehicle(i) ;
//            Node new_node = Node {node.height + 1, *buff_gs, { cindex }} ;
//            int nei_idx ;
//            if ((nei_idx = alreadyExists(cindex, new_node, graph)) == -1) {
//                queue.push(graph.size()) ; // on ajoute le noeud dans la file 
//                node.neighbors.push_back(graph.size()) ; // voisin du noeud qui l'a généré 
//                graph.push_back(new_node) ; // on ajoute le noeud dans le graph
//            } else {
//                node.neighbors.push_back(nei_idx) ;
//            }
//        }
//
//        queue.pop() ;
//    }
//
//    return graph ;
//}
//
//struct Dijkstra_Node {
//    uint32_t idx ; 
//    uint32_t d ;
//    int p ;
//} ;
//
//const uint32_t INF = 1569325055 ;
//
//struct compareLess {
//    bool operator()(const Dijkstra_Node& n1, const Dijkstra_Node& n2) { return n1.d > n2.d ; }
//} ;
//
//bool isInQueue(uint32_t idx, const std::priority_queue<Dijkstra_Node, std::vector<Dijkstra_Node>, compareLess>& p_queue) {
//    std::priority_queue<Dijkstra_Node, std::vector<Dijkstra_Node>, compareLess> buff = p_queue ;
//
//    while(!buff.empty()) {
//        if (buff.top().idx == idx)
//            return true ;
//        buff.pop() ;
//    }
//
//    return false ;
//}
//
//void updateQueueElt(const Dijkstra_Node& elt, std::priority_queue<Dijkstra_Node, std::vector<Dijkstra_Node>, compareLess>& p_queue) {
//    assert(isInQueue(elt.idx, p_queue)) ;
//
//    std::priority_queue<Dijkstra_Node, std::vector<Dijkstra_Node>, compareLess> new_queue ;
//
//    while (!p_queue.empty()) {
//        if (p_queue.top().idx == elt.idx)
//            new_queue.push(elt) ;
//        else 
//            new_queue.push(p_queue.top()) ;
//        p_queue.pop() ;
//    }
//
//    while (!new_queue.empty()) {
//        p_queue.push(new_queue.top()) ;
//        new_queue.pop() ;
//    }
//}
//
//std::vector<Dijkstra_Node> shortest_paths_dijkstra(const std::vector<Node>& graph) {
//    std::vector<Dijkstra_Node> shortest_paths ;
//    shortest_paths.assign(graph.size(), Dijkstra_Node { 0, INF, -1 }) ;
//
//    for (int i = 1; i < graph.size(); ++i) 
//        shortest_paths[i].idx = i ;
//
//    int start = 0 ; 
//
//    // file a priorité minimale 
//    std::priority_queue<Dijkstra_Node, std::vector<Dijkstra_Node>, compareLess> s; 
//
//    s.push({ 0, 0, start }) ;
//    shortest_paths[start].d = 0 ;
//    shortest_paths[start].p = 0 ;

  /*  while (!s.empty()) {
        Dijkstra_Node v = s.top() ;
        s.pop() ;
        
        for (uint32_t iw : graph[v.idx].neighbors) {
            uint32_t d_vw = std::abs(graph[iw].height - graph[v.idx].height) ;
            if (shortest_paths[iw].d > v.d + d_vw) {
                shortest_paths[iw].d = v.d + d_vw ;
                shortest_paths[iw].p = v.idx ;
                if (!isInQueue(shortest_paths[iw].idx, s)) 
                    s.push(shortest_paths[iw]) ;
                else updateQueueElt(shortest_paths[iw], s) ;
            }
        }
    }

    return shortest_paths ; 
}


int main(int argc, char** argv) {

    game_situation init_situation("./data/puzzle.txt") ;

    std::vector<Node> graph = buildGraph(init_situation) ;

    std::vector<Dijkstra_Node> sp = shortest_paths_dijkstra(graph) ;

    for (Dijkstra_Node dn : sp) {
        std::cout << " idx : " << dn.idx << ", d : " << dn.d << ", p : " << dn.p << std::endl ;
    }
   
    return EXIT_SUCCESS;
}*/