FLAGS_SDL = -lSDL2main -lSDL2 -lSDL2_image 
CC = g++
FLAGS = -Wall -g 

CODE_REP = ./Code  
HEADERS = ./Code/Timer.h ./Code/Graph.h ./Code/Generator.h ./Code/GameSituation.h 
OBJ_TEST = ./obj/RushHourTest.o ./obj/GameSituation.o 
OBJ_SDL = ./obj/RushHourApp.o ./obj/GameSituation.o 

EXE_SDL = ./bin/RushHourApp 
EXE_TEST = ./bin/RushHourTest

ALL_EXE = $(EXE_SDL) $(EXE_TEST)

all: $(EXE_SDL) $(EXE_TEST)

$(EXE_SDL): $(OBJ_SDL)
	$(CC) -g $^ -o $@ $(FLAGS_SDL)

$(EXE_TEST): $(OBJ_TEST)
	$(CC) -g $^ -o $@

./obj/RushHourApp.o: ./Code/RushHourApp.cpp $(HEADERS)
	$(CC) $(FLAGS) -c $< -o $@

./obj/RushHourTest.o: ./Code/RushHourTest.cpp $(HEADERS) 
	$(CC) $(FLAGS) -c $< -o $@

./obj/GameSituation.o: ./Code/GameSituation.cpp ./Code/GameSituation.h
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(ALL_EXE) ./obj/*
