#pragma once

#include <vector>
#include <string>
#include <cstring>
#include <fstream>
#include <cstdlib>

struct Movement {
    Movement(int i, int d) : Index(i), Direction(d) {}
    size_t Index ;
    int Direction ;
} ;

struct Box {
    Box() = default;
    Box(int r, int c) : Row(r), Col(c) {}

    int Row ;
    int Col ;
} ;

struct Vehicle {
    Vehicle() = default;

    int IsHorizontal ;
    int Length ;
    Box Position ;
} ;

class GameSituation {
public:
    GameSituation(const std::string& inputFilepath) ;
    GameSituation(const std::vector<Vehicle> vehicles, int width, int height, const Box& exitPosition, int id) ;
    ~GameSituation() = default ;

    bool FinalSituation() const ;

    GameSituation MoveVehicle(size_t i);

    inline bool SameSituation(const GameSituation& gs) const { return gs.GetID() == m_Id; }
    inline int GetGridHeight() const { return m_GridHeight ; }
    inline int GetGridWidth() const { return m_GridWidth ; }
    inline Box GetExitPosition() const { return m_ExitPosition ; }
    inline std::vector<Vehicle> GetVehicles() const { return m_Vehicles ; }
    inline size_t NumOfMouvements() const { return m_MovableVehicles.size() ; }
    inline int GetID() const { return m_Id; }

    void AddVehicle(const Vehicle& vehicle);

private:
    void WriteToFile(const std::string& outputFilepath) const ;
    void ReadFromFile(const std::string& inputFilepath) ;  
    
    void UpdateBoxCondition() ;
    void UpdateMovableVehicles() ;

    int VehicleID(const Vehicle& vehicle) const;
    void SetID() ;
private:
    std::vector<Vehicle> m_Vehicles ;
    std::vector<bool> m_BoxCondition ;
    std::vector<Movement> m_MovableVehicles ;
    Box m_ExitPosition ;
    int m_GridHeight ;
    int m_GridWidth ;
    int m_Id;
    int m_WRow, m_WCol;
};

