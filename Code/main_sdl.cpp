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

const int size_grid = 6;

void generate(std::string& output_file) {

    std::ofstream f_writing(output_file.c_str());

    if (f_writing) {
        std::vector<std::pair<int, int>> couples ;

        std::random_device rng;
        std::uniform_int_distribution<int> ur(0, size_grid - 1);
        std::uniform_int_distribution<int> uc(0, size_grid - 3);

        int exit_row = ur(rng);
        int exit_column = uc(rng) ;

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
                couples.push_back({ h, w });   
            }
        }

        std::cout << "[ ";
        for (const auto& p : couples) {
            std::cout << "(" << p.first << ", " << p.second << "), " << " ";
        }
        std::cout << " ]\n";

        int count = 0;
        while (count < size_grid * size_grid / 3 - 1 && !couples.empty()) {
            std::random_device rng;
            std::uniform_int_distribution<int> u(0, couples.size() - 1);
            std::uniform_int_distribution<int> ul(2, 3);
            std::uniform_int_distribution<int> ud(0, 1);

            int length = ul(rng);
            int direction = ud(rng);
            int key = u(rng);

            std::pair<int, int> pair = couples[key];
            if (pair.first == exit_row && direction) {
                direction = 0;
            }

            if (pair.first + (1 - direction) * (length - 1) < size_grid &&
                pair.second + direction * (length - 1) < size_grid) {
                // std::cout << "w: " << (pair.first + direction * (length - 1)) << " h: " 
                //     << (pair.second + (1 - direction) * (length - 1)) << std::endl ;
                std::stack<std::pair<int, int>> s;
                //std::cout << "(" << pair.first << ", " << pair.second << ", " << length << ", " << direction << " )" << std::endl;
                couples.erase(couples.begin() + key);

                int n = 1;
                int i = 0;
                while (i < couples.size()) {
                    // std::cout << i << " " << couples.size() << std::endl ;
                    if (n == length) break;
                    std::pair<int, int> p = couples[i];
                    for (int l = 1; l < length; ++l) {
                        int hl = pair.first + (1 - direction) * l;
                        int wl = pair.second + direction * l;
                        if (wl == p.second && hl == p.first) {
                            std::pair<int, int> buff = couples[i];
                            s.push(buff);
                            //std::cout << "erased : (" << p.first << ", " << p.second << ")" << std::endl;
                            couples.erase(couples.begin() + i);
                            n++;
                        }
                    }
                    i++;
                }

                /*std::cout << "n=" << n << "[";
                for (const auto& p : couples) {
                    std::cout << "(" << p.first << ", " << p.second << "), " << " ";
                }
                std::cout << " ]\n";

                std::cin.get();*/

                if (n != length) {
                    //std::cout << "Annulation!" << std::endl;
                    couples.push_back(pair);
                    for (int i = 0; i < s.size(); ++i) {
                        couples.push_back(s.top());
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

    std::string auto_generated = "data/files/generated_gs.txt" ;

    generate(auto_generated) ;

    game_situation init_situation(auto_generated) ;

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

        drawGameState(textures, renderer, path[iter].getVehicles(), init_situation.getExitPosition(), grid_width, grid_height);

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

