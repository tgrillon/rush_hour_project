#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdlib>

struct movement {
    movement(int i, int d) : index(i), direction(d) {}
    size_t index ;
    int direction ;
} ;

struct box {
    int row ;
    int column ;
} ;

struct vehicle {
    inline bool isHorizontal() { return t_hor_f_vert ; }
    inline bool isVertical() { return !t_hor_f_vert ; }

    bool t_hor_f_vert ;
    box position ;
    int length ;
} ;

class game_situation {
public:
    game_situation(const std::string& input_filepath) ;
    game_situation(const std::vector<vehicle> vehicles, int width, int height, const box& exit_position) ;
    ~game_situation() = default ;

    bool sameSituation(const game_situation& gs) const ;
    bool finalSituation() const ;

    game_situation moveVehicle(size_t i) const;
    game_situation moveVehicleRand() const;

    inline int getGridHeight() const { return m_grid_height ; }
    inline int getGridWidth() const { return m_grid_width ; }
    inline box getExitPosition() const { return m_exit_position ; }
    inline std::vector<vehicle> getVehicles() const { return m_vehicles ; }
    inline size_t numOfMouvements() const { return m_movable_vehicles.size() ; }

private:
    void writeToFile(const std::string& output_filepath) const ;
    void readFromFile(const std::string& input_filepath) ;  
    
    void updateBoxCondition() ;
    void updateMovableVehicles() ;
private:
    std::vector<vehicle> m_vehicles ;
    std::vector<bool> m_box_condition ;
    std::vector<movement> m_movable_vehicles ;
    box m_exit_position ;
    int m_grid_height ;
    int m_grid_width ;
};

