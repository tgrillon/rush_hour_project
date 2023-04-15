#include "GameSituation.h"
#include <iostream>

#define LOG(STR, X) std::cout << STR << " " << X << std::endl ; 

GameSituation::GameSituation(const std::string& input_filepath) : m_Mask(0), m_GridWidth(6), m_GridHeight(6), m_CanBeATarget(false) {
    ReadFromFile(input_filepath) ;
    UpdateBoxCondition() ;
    UpdateMovableVehicles() ;
    m_WCol = m_GridWidth - 1;
    m_WRow = m_GridHeight;
    SetMask();
}

GameSituation::GameSituation(const std::vector<Vehicle> vehicles, int width, int height, const Box& exitPosition, int mask) 
    : m_Vehicles(vehicles), m_GridWidth(width), m_GridHeight(height), m_ExitPosition(exitPosition), m_Mask(mask), m_CanBeATarget(false) {
    UpdateBoxCondition() ;
    UpdateMovableVehicles() ;
    m_WCol = m_GridWidth - 1;
    m_WRow = m_GridHeight;
}

int GameSituation::VehicleMask(const Vehicle& vehicle) const {
    int value = (vehicle.Position.Row + 1) * m_WRow + (vehicle.Position.Col + 1) * m_WCol;
    int i = 1;
    while (i < vehicle.Length) {
        if (vehicle.IsHorizontal) {
            value *= (((vehicle.Position.Row + 1) * m_WRow + ((vehicle.Position.Col + 1) + i) * m_WCol));
            i++;
            continue;
        }
        value *= ((((vehicle.Position.Row + 1) + i) * m_WRow + (vehicle.Position.Col + 1) * m_WCol));
        i++;
    }

    return vehicle.IsHorizontal ? value : -value;
}

void GameSituation::SetMask() {
    for (const Vehicle& vehicle : m_Vehicles) 
        m_Mask += VehicleMask(vehicle);
}

bool GameSituation::FinalSituation() const { 
    return m_Vehicles[0].Position.Row == m_ExitPosition.Row &&
        m_Vehicles[0].Position.Col + m_Vehicles[0].Length - 1 == m_ExitPosition.Col; 
}

// linear complexity 
void GameSituation::UpdateBoxCondition() {
    if (!m_BoxCondition.empty())
        m_BoxCondition.clear() ;
    m_BoxCondition.assign(m_GridWidth*m_GridHeight, true) ;

    for (Vehicle v : m_Vehicles) {
        int l = 0 ;
        while (l < v.Length) {
            int idx = ((v.Position.Row + (1 - v.IsHorizontal) * l)) * m_GridWidth + (v.Position.Col + v.IsHorizontal * l);
            m_BoxCondition[idx] = false ;
            l++ ;
        }
    }
}

// linear complexity
void GameSituation::UpdateMovableVehicles() {
    if (!m_MovableVehicles.empty())
        m_MovableVehicles.clear() ;

    for (int i = 0; i < m_Vehicles.size(); ++i) {
        Vehicle v = m_Vehicles[i] ;
        int d = v.Length ;
        if (v.IsHorizontal) {
            if (v.Position.Col + d != m_GridWidth) 
                if (m_BoxCondition[v.Position.Row*m_GridWidth+v.Position.Col+d]) {
                    m_MovableVehicles.push_back(Movement( i, 1 )) ;
                    if (i == 0) 
                        m_CanBeATarget = true;
                } 

            if (v.Position.Col - 1 >= 0)
                if (m_BoxCondition[v.Position.Row*m_GridWidth+v.Position.Col-1]) {
                    m_MovableVehicles.push_back(Movement( i, -1 )) ; 
                    if (i == 0)
                        m_CanBeATarget = true;
                }        
        } else { // vertical 
            if (v.Position.Row + d != m_GridHeight) 
                if (m_BoxCondition[(v.Position.Row+d)*m_GridWidth+v.Position.Col]) {
                    m_MovableVehicles.push_back(Movement( i, 1 )) ;
                    if (i == 0)
                        m_CanBeATarget = true;
                } 

            if (v.Position.Row - 1 >= 0)
                if (m_BoxCondition[(v.Position.Row-1)*m_GridWidth+v.Position.Col]) {
                    m_MovableVehicles.push_back(Movement( i, -1 )) ; 
                    if (i == 0)
                        m_CanBeATarget = true;
                }        
        }
    }
}

void GameSituation::AddVehicle(const Vehicle& vehicle) {
    Vehicle vh;
    vh.Length = vehicle.Length;
    vh.IsHorizontal = vehicle.IsHorizontal;
    vh.Position = Box(vehicle.Position.Row, vehicle.Position.Col);
    m_Vehicles.push_back(vh);
    m_Mask += VehicleMask(vh);
    UpdateBoxCondition();
    UpdateMovableVehicles();
}

// Theta(1) complexity 
GameSituation GameSituation::MoveVehicle(size_t i) {
    Movement m = m_MovableVehicles[i] ;
    int index = m.Index ;
    int direction = (m.Direction < 0 ) ? -1 : 1 ;
    int delta = direction ;
    Vehicle v = m_Vehicles[index] ;

    int nextID = m_Mask - VehicleMask(v);

    if (v.IsHorizontal) {
        if (direction < 0) {
            while (v.Position.Col-1+delta >= 0) {
                if (!m_BoxCondition[v.Position.Row*m_GridWidth+v.Position.Col-1+delta]) {
                    break ;
                }
                delta += direction ;
            }
        } else {
            while (v.Position.Col+v.Length+delta < m_GridWidth) {
                if (!m_BoxCondition[v.Position.Row*m_GridWidth+v.Position.Col+v.Length+delta]) {
                    break ;
                }
                delta += direction ;
            }
        }
        v.Position.Col += delta ;
    } else { // vertical
        if (direction < 0) { 
            while (v.Position.Row-1+delta >= 0) {
                if (!m_BoxCondition[(v.Position.Row-1+delta)*m_GridWidth+v.Position.Col]) {
                    break ;
                }   
                delta += direction ;
            }   
        } else {
            while (v.Position.Row+v.Length+delta < m_GridHeight) {
                if (!m_BoxCondition[(v.Position.Row+v.Length+delta)*m_GridWidth+v.Position.Col]) {
                    break ;
                }
                delta += direction ;
            }
        }      
        v.Position.Row += delta ;
    }
    std::vector<Vehicle> new_Vehicle_array = m_Vehicles ;

    nextID += VehicleMask(v);
    new_Vehicle_array[index] = v ;
    return GameSituation(new_Vehicle_array, m_GridWidth, m_GridHeight, m_ExitPosition, nextID) ;
}

void GameSituation::ReadFromFile(const std::string& input_filepath) {
    std::ifstream f_reading(input_filepath.c_str()) ;

    if (f_reading) {
        f_reading >> m_GridWidth ;
        f_reading >> m_GridHeight ;

        f_reading >> m_ExitPosition.Row ;
        f_reading >> m_ExitPosition.Col ;

        int v;
        while (f_reading >> v) {
            Vehicle new_Vehicle ;
            new_Vehicle.Position.Row = v ;
            f_reading >> new_Vehicle.Position.Col ;
            f_reading >> new_Vehicle.Length ;
            f_reading >> new_Vehicle.IsHorizontal ;
            m_Vehicles.push_back(new_Vehicle) ;
        }
    } else {
        std::cout << "Error: No such file at '" << input_filepath << "' !" << std::endl ;
        exit(-1) ;
    }

    f_reading.close() ;
}

void GameSituation::WriteToFile(const std::string& output_filepath) const {
    std::ofstream f_writing(output_filepath.c_str()) ;

    if (f_writing) {
        f_writing << m_GridWidth ;
        f_writing << ' ' ;
        f_writing << m_GridHeight ;
        f_writing << '\n' ;

        f_writing << m_ExitPosition.Row ;
        f_writing << ' ' ;
        f_writing << m_ExitPosition.Col ;

        for (Vehicle v : m_Vehicles) {
            f_writing << '\n' ;
            
            f_writing << v.Position.Row ;
            f_writing << ' ' ;

            f_writing << v.Position.Col ;
            f_writing << ' ' ;

            f_writing << v.Length ;
            f_writing << ' ' ;

            f_writing << v.IsHorizontal ;
        }
    } else {
        std::cout << "Error: No such file at '" << output_filepath << "'" << std::endl ;
        exit(-1) ;
    }

    f_writing.close() ;
}

