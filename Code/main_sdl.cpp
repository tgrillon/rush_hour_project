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
#include <random>
#include <utility>
#include <stack>

#include "game_situation.h"

int WINDOW_WIDTH = 800 ;
int WINDOW_HEIGHT = 800 ;

const int BOX_SIZE = 64 ;

const uint32_t MAX_HEIGHT = 100 ;

struct Node {
    game_situation gs;
    int id ;
};

bool alreadyExists(int cindex, Node& node, std::vector<Node>& graph) {
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

std::string filepath = "./data/files/puzzle.txt" ;

int main(int argc, char** argv)
{

    if (argc > 1)
        filepath = argv[1] ;

    // initialisation de SDL2
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[debug] %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    // Creation de la fenêtre
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    game_situation init_situation(filepath) ;

    std::vector<Node> graph ;
    findPath(init_situation, graph) ;

    Node node = graph[graph.size() - 1] ;
    std::vector<game_situation> path ; 
    while (node.id != -1) {
        path.push_back(node.gs) ;
        node = graph[node.id] ;
    }

    path.push_back(node.gs) ;

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

    SDL_Surface* side_l = IMG_Load("./data/tiles/side_l.png") ;
    SDL_Surface* side_r = IMG_Load("./data/tiles/side_r.png") ;
    SDL_Surface* side_h = IMG_Load("./data/tiles/side_h.png") ;
    SDL_Surface* side_b = IMG_Load("./data/tiles/side_b.png") ;
    SDL_Surface* side_l_r = IMG_Load("./data/tiles/side_l_r.png") ;
    SDL_Surface* side_r_r = IMG_Load("./data/tiles/side_r_r.png") ;
    SDL_Surface* side_h_r = IMG_Load("./data/tiles/side_h_r.png") ;
    SDL_Surface* side_b_r = IMG_Load("./data/tiles/side_b_r.png") ;

    SDL_Surface* mid_v = IMG_Load("./data/tiles/mid_v.png") ;
    SDL_Surface* mid_h = IMG_Load("./data/tiles/mid_h.png") ;
    SDL_Surface* mid_v_r = IMG_Load("./data/tiles/mid_v_r.png") ;
    SDL_Surface* mid_h_r = IMG_Load("./data/tiles/mid_h_r.png") ;

    SDL_Surface* exit = IMG_Load("./data/tiles/exit.png") ;

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

    textures.push_back(SDL_CreateTextureFromSurface(renderer, exit)) ; // 12

    uint32_t iter = path.size() - 1 ;

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
                    if (iter < path.size() - 1)
                        ++iter ;
                    else iter = path.size() - 1 ;
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

        drawGameState(textures, renderer, path[iter].getVehicles(), init_situation.getExitPosition(), grid_width, grid_height) ;

        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // if (iter > 0) iter--;
        // else {
        //     iter = path.size() - 1;
        // }
        // if (iter == path.size() - 2)
        //     std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        SDL_RenderPresent(renderer);
    }

    for (SDL_Texture* t : textures) 
        SDL_DestroyTexture(t);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return EXIT_SUCCESS;
}

