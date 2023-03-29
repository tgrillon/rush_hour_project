#if __linux__

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#elif _WIN32

#include <SDL.h>
#include <SDL_image.h>

#endif

#include <iostream>
#include <cstdlib>
#include <queue>
#include <cassert>
#include <map>
#include <thread>


#include "game_situation.h"

int WINDOW_WIDTH = 800 ;
int WINDOW_HEIGHT = 800 ;

const int BOX_SIZE = 64 ;

const uint32_t MAX_HEIGHT = 100 ;

struct Node {
    int height ;
    game_situation gs;
    std::vector<uint32_t> neighbors ;
};

uint32_t alreadyExists(uint32_t cindex, Node& node, const std::vector<std::vector<uint32_t>>& heights, std::vector<Node>& graph) {
    for (uint32_t i = 0; i < graph.size(); ++i) {
        if (node.gs.sameSituation(graph[i].gs)) {
            graph[i].neighbors.push_back(cindex) ;
            return i ;
        }
    }

    return -1 ;
}

std::vector<Node> buildGraph(std::vector<uint32_t>& final_sit_idx, const game_situation& init_situation) {
    std::vector<std::vector<uint32_t>> heights ;
    heights.assign(MAX_HEIGHT, {}) ;
    heights[0].push_back(0) ;

    std::vector<Node> graph ;
    graph.push_back(Node {0, init_situation, { 0 }}) ;

    std::queue<uint32_t> queue ; 
    queue.push(0) ;

    while (!queue.empty()) {
        uint32_t cindex = queue.front() ;

        Node node = graph[cindex] ;

        if (node.gs.finalSituation())
            final_sit_idx.push_back(cindex) ;

        for (int i = 0; i < node.gs.numOfMouvements(); ++i) {
            game_situation buff_gs = node.gs.moveVehicle(i) ;
            Node new_node = Node {node.height + 1, buff_gs, { cindex }} ;
            uint32_t nei_idx ;
            if ((nei_idx = alreadyExists(cindex, new_node, heights, graph)) == -1) {
                queue.push(graph.size()) ; // on ajoute le noeud dans la file 
                node.neighbors.push_back(uint32_t(graph.size())) ; // voisin du noeud qui l'a généré 
                heights[new_node.height].push_back(graph.size()) ; // hauteur = hauteur du noeud père + 1
                graph.push_back(new_node) ; // on ajoute le noeud dans le graph
            } else {
                node.neighbors.push_back(nei_idx) ;
            }
        }

        queue.pop() ;   
    }

    return graph ;
}

struct Dijkstra_Node {
    uint32_t idx ; 
    uint32_t d ;
    int p ;
} ;

const uint32_t INF = 1569325055 ;

struct compareLess {
    bool operator()(const Dijkstra_Node& n1, const Dijkstra_Node& n2) { return n1.d > n2.d ; }
} ;

bool isInQueue(uint32_t idx, const std::priority_queue<Dijkstra_Node, std::vector<Dijkstra_Node>, compareLess>& p_queue) {
    std::priority_queue<Dijkstra_Node, std::vector<Dijkstra_Node>, compareLess> buff = p_queue ;

    while(!buff.empty()) {
        if (buff.top().idx == idx)
            return true ;
        buff.pop() ;
    }

    return false ;
}

void updateQueueElt(const Dijkstra_Node& elt, std::priority_queue<Dijkstra_Node, std::vector<Dijkstra_Node>, compareLess>& p_queue) {
    assert(isInQueue(elt.idx, p_queue)) ;

    std::priority_queue<Dijkstra_Node, std::vector<Dijkstra_Node>, compareLess> new_queue ;

    while (!p_queue.empty()) {
        if (p_queue.top().idx == elt.idx)
            new_queue.push(elt) ;
        else 
            new_queue.push(p_queue.top()) ;
        p_queue.pop() ;
    }

    while (!new_queue.empty()) {
        p_queue.push(new_queue.top()) ;
        new_queue.pop() ;
    }
}

std::vector<Dijkstra_Node> shortest_paths_dijkstra(const std::vector<Node>& graph) {
    std::vector<Dijkstra_Node> shortest_paths ;
    shortest_paths.assign(graph.size(), Dijkstra_Node { 0, INF, -1 }) ;

    for (int i = 1; i < graph.size(); ++i) 
        shortest_paths[i].idx = i ;

    int start = 0 ; 

    // file a priorité minimale 
    std::priority_queue<Dijkstra_Node, std::vector<Dijkstra_Node>, compareLess> s; 

    s.push({ 0, 0, start }) ;
    shortest_paths[start].d = 0 ;
    shortest_paths[start].p = 0 ;

    while (!s.empty()) {
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

void drawGameState(const std::vector<SDL_Texture*>& textures, SDL_Renderer*& renderer, const std::vector<vehicle>& vehicles, const box& exit_position, int grid_width, int grid_height) {
    
    int grid_width_pxl = grid_width * BOX_SIZE ; 
    int grid_height_pxl = grid_height * BOX_SIZE ; 

    int xmin = WINDOW_WIDTH/2 - (grid_width_pxl/2) ;
    int ymin = WINDOW_HEIGHT/2 - (grid_height_pxl/2) ;

    SDL_Rect src { 0, 0, BOX_SIZE, BOX_SIZE } ;

    int xcurrent = 0 ;
    int ycurrent = ymin + exit_position.row * BOX_SIZE ;

    if (exit_position.column == 0) 
        xcurrent = xmin - BOX_SIZE ;

    if (exit_position.column == grid_width - 1) 
        xcurrent = xmin + (exit_position.column + 1) * BOX_SIZE ;

    SDL_Rect dst { xcurrent, ycurrent, BOX_SIZE, BOX_SIZE } ;

    SDL_RenderCopy(renderer, textures[12], &src, &dst) ;

    for (size_t it = 0; it < vehicles.size(); ++it) {
        vehicle v = vehicles[it] ;

        int i = v.length - 1 ;
        xcurrent = xmin + v.position.column * BOX_SIZE ;
        ycurrent = ymin + v.position.row * BOX_SIZE ;

        dst = { 
            xmin + v.position.column * BOX_SIZE , 
            ymin + v.position.row * BOX_SIZE , 
            BOX_SIZE, BOX_SIZE } ;

        int d = (it == 0) ? 6 : 0 ;  
        if (v.t_hor_f_vert) {
            SDL_RenderCopy(renderer, textures[0+d], &src, &dst) ;
            while (i > 1) {
                xcurrent += BOX_SIZE ;
                dst = { xcurrent , ycurrent , BOX_SIZE , BOX_SIZE } ;
                SDL_RenderCopy(renderer, textures[5+d], &src, &dst) ;
                --i;
            } 
            dst = { xcurrent + BOX_SIZE , ycurrent , BOX_SIZE , BOX_SIZE } ;
            SDL_RenderCopy(renderer, textures[1+d], &src, &dst) ;
        } else {
            SDL_RenderCopy(renderer, textures[2+d], &src, &dst) ;
            while (i > 1) {
                ycurrent += BOX_SIZE ;
                dst = { xcurrent , ycurrent , BOX_SIZE , BOX_SIZE } ;
                SDL_RenderCopy(renderer, textures[4+d], &src, &dst) ;
                --i;
            } 
            dst = { xcurrent , ycurrent + BOX_SIZE , BOX_SIZE , BOX_SIZE } ;
            SDL_RenderCopy(renderer, textures[3+d], &src, &dst) ;
        }
    }
}

int main(int argc, char* argv[])
{
    srand (time(NULL));

    // initialisation de SDL2
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[debug] %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Creation de la fenêtre
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    game_situation init_situation("./data/puzzle.txt") ;

    std::vector<uint32_t> final_sit_idx ;

    std::vector<Node> graph ;
    graph = buildGraph(final_sit_idx, init_situation) ;

    std::vector<Dijkstra_Node> shortest_paths = shortest_paths_dijkstra(graph) ;

    uint32_t idx_best_path = final_sit_idx[0] ;
    for (size_t i = 1; i < final_sit_idx.size(); ++i) {
        if (shortest_paths[final_sit_idx[i]].d < shortest_paths[idx_best_path].d)
            idx_best_path = final_sit_idx[i] ;
    }

    std::vector<Node> best_solution ;

    uint32_t current_index = idx_best_path ;
    while(current_index > 0) {
        best_solution.push_back(graph[current_index]) ;
        current_index = shortest_paths[current_index].p ;
    } 
    best_solution.push_back(graph[0]) ;
    
    int grid_width = init_situation.getGridWidth() ;
    int grid_height = init_situation.getGridHeight() ;

    WINDOW_WIDTH = grid_width * BOX_SIZE + 2 * BOX_SIZE;
    WINDOW_HEIGHT = grid_height * BOX_SIZE + 2 * BOX_SIZE;

    if (SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[DEBUG] > %s", SDL_GetError()) ;         
        SDL_Quit() ;         
        return EXIT_FAILURE ;   
    }

    SDL_SetWindowTitle(window, "rush hour game in C++") ;
    SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED) ;

    SDL_Rect grille { 
        WINDOW_WIDTH / 2 - ((grid_width * BOX_SIZE) / 2) ,  
        WINDOW_HEIGHT / 2 - ((grid_height  * BOX_SIZE) / 2) ,  
        grid_width * BOX_SIZE , 
        grid_height * BOX_SIZE
    } ;

    SDL_Surface* side_l = IMG_Load("./data/side_l.png") ;
    SDL_Surface* side_r = IMG_Load("./data/side_r.png") ;
    SDL_Surface* side_h = IMG_Load("./data/side_h.png") ;
    SDL_Surface* side_b = IMG_Load("./data/side_b.png") ;
    SDL_Surface* side_l_r = IMG_Load("./data/side_l_r.png") ;
    SDL_Surface* side_r_r = IMG_Load("./data/side_r_r.png") ;
    SDL_Surface* side_h_r = IMG_Load("./data/side_h_r.png") ;
    SDL_Surface* side_b_r = IMG_Load("./data/side_b_r.png") ;

    SDL_Surface* mid_v = IMG_Load("./data/mid_v.png") ;
    SDL_Surface* mid_h = IMG_Load("./data/mid_h.png") ;
    SDL_Surface* mid_v_r = IMG_Load("./data/mid_v_r.png") ;
    SDL_Surface* mid_h_r = IMG_Load("./data/mid_h_r.png") ;

    SDL_Surface* output = IMG_Load("./data/output.png") ;

    std::vector<SDL_Texture*> textures ; 
    textures.push_back(SDL_CreateTextureFromSurface(renderer, side_l)) ; // 0
    textures.push_back(SDL_CreateTextureFromSurface(renderer, side_r)) ; // 1
    textures.push_back(SDL_CreateTextureFromSurface(renderer, side_h)) ; // 2
    textures.push_back(SDL_CreateTextureFromSurface(renderer, side_b)) ; // 3
    textures.push_back(SDL_CreateTextureFromSurface(renderer, mid_v)) ; // 4
    textures.push_back(SDL_CreateTextureFromSurface(renderer, mid_h)) ; // 5

    textures.push_back(SDL_CreateTextureFromSurface(renderer, side_l_r)) ; // 6
    textures.push_back(SDL_CreateTextureFromSurface(renderer, side_r_r)) ; // 7
    textures.push_back(SDL_CreateTextureFromSurface(renderer, side_h_r)) ; // 8
    textures.push_back(SDL_CreateTextureFromSurface(renderer, side_b_r)) ; // 9
    textures.push_back(SDL_CreateTextureFromSurface(renderer, mid_v_r)) ; // 10
    textures.push_back(SDL_CreateTextureFromSurface(renderer, mid_h_r)) ; // 11

    textures.push_back(SDL_CreateTextureFromSurface(renderer, output)) ; // 12

    uint32_t iter = best_solution.size() - 1 ;

    SDL_Event events ;
    bool isOpen = true ;
    while(isOpen) {
        while (SDL_PollEvent(&events)) {
            switch (events.type) {
            case SDL_QUIT: 
                isOpen = false ;
                break ;
            case SDL_KEYDOWN: 
                if (events.key.keysym.scancode == SDL_SCANCODE_ESCAPE) 
                    isOpen = false; 
                else if (events.key.keysym.scancode == SDL_SCANCODE_R)
                    iter = 0 ;
                else if (events.key.keysym.scancode == SDL_SCANCODE_B) { 
                    if (iter < best_solution.size() - 1)
                        ++iter ;
                    else iter = best_solution.size() - 1 ;
                } else if (events.key.keysym.scancode == SDL_SCANCODE_N) {
                    if (iter > 0)
                        --iter ;
                    else iter = 0 ;
                }

                break;

            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255) ;
        SDL_RenderClear(renderer) ;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) ;
        SDL_RenderDrawRect(renderer, &grille);

        drawGameState(textures, renderer, best_solution[iter].gs.getVehicles(), init_situation.getExitPosition(), grid_width, grid_height) ;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (iter > 0) iter--;
        else {
            iter = best_solution.size() - 1;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        SDL_RenderPresent(renderer);
    }

    for (SDL_Texture* t : textures) 
        SDL_DestroyTexture(t);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return EXIT_SUCCESS;
}

