FLAGS_SDL = -lSDL2main -lSDL2 -lSDL2_image 
CC = g++
FLAGS = -Wall -g 

CODE_REP = ./Code  
OBJ_UTILS = ./obj/GameSituation.o ./obj/Graph.o ./obj/Generator.o
OBJ_TEST = ./obj/RushHourTest.o $(OBJ_UTILS)
OBJ_SDL = ./obj/RushHourApp.o $(OBJ_UTILS)

EXE_SDL = ./bin/RushHourApp 
EXE_TEST = ./bin/RushHourTest

ALL_EXE = $(EXE_SDL) $(EXE_TEST)

all: $(EXE_SDL) $(EXE_TEST)

$(EXE_SDL): $(OBJ_SDL)
	$(CC) -g $^ -o $@ $(FLAGS_SDL)

$(EXE_TEST): $(OBJ_TEST)
	$(CC) -g $^ -o $@

./obj/RushHourApp.o: ./Code/RushHourApp.cpp ./Code/GameSituation.h
	$(CC) $(FLAGS) -c $< -o $@

./obj/RushHourTest.o: ./Code/RushHourTest.cpp ./Code/GameSituation.h
	$(CC) $(FLAGS) -c $< -o $@

./obj/GameSituation.o: ./Code/GameSituation.cpp ./Code/GameSituation.h
	$(CC) $(FLAGS) -c $< -o $@

./obj/Graph.o: ./Code/Graph.h ./Code/GameSituation.h
$(CC) $(FLAGS) -c $< -o $@

./obj/Generator.o: ./Code/Generator.h ./Code/Graph.h 
$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf $(ALL_EXE) ./obj/*
