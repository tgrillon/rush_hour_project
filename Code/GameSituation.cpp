#include "GameSituation.h"
#include <iostream>

#define LOG(STR, X) std::cout << STR << " " << X << std::endl ; 

GameSituation::GameSituation(const std::string& input_filepath) : m_Id(0), m_GridWidth(6), m_GridHeight(6) {
    ReadFromFile(input_filepath) ;
    UpdateBoxCondition() ;
    UpdateMovableVehicles() ;
    SetID();
}

GameSituation::GameSituation(const std::vector<Vehicle> vehicles, int width, int height, const Box& exitPosition, int id) 
    : m_Vehicles(vehicles), m_GridWidth(width), m_GridHeight(height), m_ExitPosition(exitPosition), m_Id(id) {
    UpdateBoxCondition() ;
    UpdateMovableVehicles() ;
}

int VehicleID(const Vehicle& vehicle, int wRow, int wCol) {
    int value = (vehicle.Position.Row + 1) * wRow + (vehicle.Position.Col + 1) * wCol;
    int i = 1;
    while (i < vehicle.Length) {
        if (vehicle.IsHorizontal) {
            value *= (((vehicle.Position.Row + 1) * wRow + ((vehicle.Position.Col + 1) + i) * wCol));
            i++;
            continue;
        }
        value *= ((((vehicle.Position.Row + 1) + i) * wRow + (vehicle.Position.Col + 1) * wCol));
        i++;
    }

    return value;
}

void GameSituation::SetID() {
    int wCol = m_GridWidth - 1;
    int wRow = m_GridHeight;

    for (const Vehicle& vehicle : m_Vehicles) 
        m_Id += VehicleID(vehicle, wRow, wCol);
}

bool GameSituation::FinalSituation() const { 
    return m_Vehicles[0].Position.Row == m_ExitPosition.Row &&
        m_Vehicles[0].Position.Col + m_Vehicles[0].Length - 1 == m_ExitPosition.Col; 
}

// linear complexity 
//bool GameSituation::SameSituation(const GameSituation& gs) const {
//    std::vector<Vehicle> Vehicles = gs.GetVehicles() ;
//    for (int i = 0; i < m_Vehicles.size(); ++i) {
//        Vehicle v1 = m_Vehicles[i] ;
//        Vehicle v2 = Vehicles[i] ;
//        if (v1.Position.Row != v2.Position.Row) return false ;  
//        if (v1.Position.Col != v2.Position.Col) return false ;  
//        if (v1.Length != v2.Length) return false ;  
//        if (v1.IsHorizontal != v2.IsHorizontal) return false ;  
//    }
//
//    return true ;
//}

// linear complexity 
void GameSituation::UpdateBoxCondition() {
    if (!m_BoxCondition.empty())
        m_BoxCondition.clear() ;
    m_BoxCondition.assign(m_GridWidth*m_GridHeight, true) ;

    for (Vehicle v : m_Vehicles) {
        int i = v.Length - 1 ;
        while (i >= 0) {
            if (v.IsHorizontal)
                m_BoxCondition[v.Position.Row*m_GridWidth + v.Position.Col + i] = false ;
            else // vertical 
                m_BoxCondition[(v.Position.Row + i)*m_GridWidth + v.Position.Col] = false ;
            --i ;
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
                } 

            if (v.Position.Col - 1 >= 0)
                if (m_BoxCondition[v.Position.Row*m_GridWidth+v.Position.Col-1]) {
                    m_MovableVehicles.push_back(Movement( i, -1 )) ; 
                }        
        } else { // vertical 
            if (v.Position.Row + d != m_GridHeight) 
                if (m_BoxCondition[(v.Position.Row+d)*m_GridWidth+v.Position.Col]) {
                    m_MovableVehicles.push_back(Movement( i, 1 )) ;
                } 

            if (v.Position.Row - 1 >= 0)
                if (m_BoxCondition[(v.Position.Row-1)*m_GridWidth+v.Position.Col]) {
                    m_MovableVehicles.push_back(Movement( i, -1 )) ; 
                }        
        }
    }
}

void GameSituation::AddVehicle(int Row, int col, int length, int direction) {
    Vehicle vh;
    vh.Length = length;
    vh.IsHorizontal = direction;
    vh.Position = Box(Row, col);
    m_Vehicles.push_back(vh);
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

    int wRow = m_GridHeight;
    int wCol = m_GridWidth - 1;
    int nextID = m_Id - VehicleID(v, wRow, wCol);

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

    nextID = m_Id + VehicleID(v, wRow, wCol);
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

