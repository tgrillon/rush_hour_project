#include "game_situation.h"
#include <iostream>

#define LOG(STR, X) std::cout << STR << " " << X << std::endl ; 

game_situation::game_situation(const std::string& input_filepath) {
    readFromFile(input_filepath) ;
    updateBoxCondition() ;
    updateMovableVehicles() ;
}

game_situation::game_situation(const std::vector<vehicle> vehicles, int width, int height, const box& exit_position) 
    : m_vehicles(vehicles), m_grid_width(width), m_grid_height(height), m_exit_position(exit_position) {
    updateBoxCondition() ;
    updateMovableVehicles() ;
}

game_situation::~game_situation() {
}

bool game_situation::finalSituation() const { 
        return m_vehicles[0].position.row == m_exit_position.row &&
            m_vehicles[0].position.column + m_vehicles[0].length - 1 == m_exit_position.column ; }

bool game_situation::sameSituation(const game_situation& gs) const {
    std::vector<vehicle> vehicles = gs.getVehicles() ;
    for (int i = 0; i < m_vehicles.size(); ++i) {
        vehicle v1 = m_vehicles[i] ;
        vehicle v2 = vehicles[i] ;
        if (v1.position.row != v2.position.row) return false ;  
        if (v1.position.column != v2.position.column) return false ;  
        if (v1.length != v2.length) return false ;  
        if (v1.t_hor_f_vert != v2.t_hor_f_vert) return false ;  
    }

    return true ;
}

void game_situation::updateBoxCondition() {
    if (!m_box_condition.empty())
        m_box_condition.clear() ;
    m_box_condition.assign(m_grid_width*m_grid_height, true) ;

    for (vehicle v : m_vehicles) {
        int i = v.length - 1 ;
        while (i >= 0) {
            if (v.isHorizontal())
                m_box_condition[v.position.row*m_grid_width + v.position.column + i] = false ;
            else // vertical 
                m_box_condition[(v.position.row + i)*m_grid_width + v.position.column] = false ;
            --i ;
        }
    }
}

void game_situation::updateMovableVehicles() {
    if (!m_movable_vehicles.empty())
        m_movable_vehicles.clear() ;

    for (int i = 0; i < m_vehicles.size(); ++i) {
        vehicle v = m_vehicles[i] ;
        int d = v.length ;
        if (v.isHorizontal()) {
            if (v.position.column + d != m_grid_width) 
                if (m_box_condition[v.position.row*m_grid_width+v.position.column+d]) {
                    m_movable_vehicles.push_back(movement( i, 1 )) ;
                } 

            if (v.position.column - 1 >= 0)
                if (m_box_condition[v.position.row*m_grid_width+v.position.column-1]) {
                    m_movable_vehicles.push_back(movement( i, -1 )) ; 
                }        
        } else { // vertical 
            if (v.position.row + d != m_grid_height) 
                if (m_box_condition[(v.position.row+d)*m_grid_width+v.position.column]) {
                    m_movable_vehicles.push_back(movement( i, 1 )) ;
                } 

            if (v.position.row - 1 >= 0)
                if (m_box_condition[(v.position.row-1)*m_grid_width+v.position.column]) {
                    m_movable_vehicles.push_back(movement( i, -1 )) ; 
                }        
        }
    }
}

game_situation game_situation::moveVehicle(size_t i) const {
    movement m = m_movable_vehicles[i] ;
    int index = m.index ;
    int direction = (m.direction < 0 ) ? -1 : 1 ;
    int delta = direction ;
    vehicle v = m_vehicles[index] ;

    if (v.isHorizontal()) {
        if (direction < 0) {
            while (v.position.column-1+delta >= 0) {
                if (!m_box_condition[v.position.row*m_grid_width+v.position.column-1+delta]) {
                    break ;
                }
                delta += direction ;
            }
        } else {
            while (v.position.column+v.length+delta < m_grid_width) {
                if (!m_box_condition[v.position.row*m_grid_width+v.position.column+v.length+delta]) {
                    break ;
                }
                delta += direction ;
            }
        }
        v.position.column += delta ;
    } else { // vertical
        if (direction < 0) { 
            while (v.position.row-1+delta >= 0) {
                if (!m_box_condition[(v.position.row-1+delta)*m_grid_width+v.position.column]) {
                    break ;
                }   
                delta += direction ;
            }   
        } else {
            while (v.position.row+v.length+delta < m_grid_height) {
                if (!m_box_condition[(v.position.row+v.length+delta)*m_grid_width+v.position.column]) {
                    break ;
                }
                delta += direction ;
            }
        }      
        v.position.row += delta ;
    }
    std::vector<vehicle> new_vehicle_array = m_vehicles ;

    new_vehicle_array[index] = v ;
    return game_situation(new_vehicle_array, m_grid_width, m_grid_height, m_exit_position) ;
}

game_situation game_situation::moveVehicleRand() const {
    return moveVehicle(rand() % m_movable_vehicles.size()) ;
}

void game_situation::readFromFile(const std::string& input_filepath) {
    std::ifstream f_en_lecture(input_filepath.c_str()) ;

    if (f_en_lecture) {
        f_en_lecture >> m_grid_width ;
        f_en_lecture >> m_grid_height ;

        f_en_lecture >> m_exit_position.row ;
        f_en_lecture >> m_exit_position.column ;

        int v;
        while (f_en_lecture >> v) {
            vehicle vehicle_a_ajouter ;
            vehicle_a_ajouter.position.row = v ;
            f_en_lecture >> vehicle_a_ajouter.position.column ;
            f_en_lecture >> vehicle_a_ajouter.length ;
            f_en_lecture >> vehicle_a_ajouter.t_hor_f_vert ;
            m_vehicles.push_back(vehicle_a_ajouter) ;
        }
    } else {
        std::cout << "Erreur à l'ouverture du fichier '" << input_filepath << "' !" << std::endl ;
    }

    f_en_lecture.close() ;
}

void game_situation::writeToFile(const std::string& output_filepath) const {
    std::ofstream f_en_ecriture(output_filepath.c_str()) ;

    if (f_en_ecriture) {
        f_en_ecriture << m_grid_width ;
        f_en_ecriture << ' ' ;
        f_en_ecriture << m_grid_height ;
        f_en_ecriture << '\n' ;

        f_en_ecriture << m_exit_position.row ;
        f_en_ecriture << ' ' ;
        f_en_ecriture << m_exit_position.column ;

        for (vehicle v : m_vehicles) {
            f_en_ecriture << '\n' ;
            
            f_en_ecriture << v.position.row ;
            f_en_ecriture << ' ' ;

            f_en_ecriture << v.position.column ;
            f_en_ecriture << ' ' ;

            f_en_ecriture << v.length ;
            f_en_ecriture << ' ' ;

            f_en_ecriture << v.t_hor_f_vert ;
        }
    } else {
        std::cout << "Erreur lors de l'ouverture en écriture du fichier: '" << output_filepath << "'" << std::endl ;
    }

    f_en_ecriture.close() ;
}

