FLAGS_SDL = -lSDL2main -lSDL2 -lSDL2_image 
CC = g++
FLAGS = -Wall -g 

CODE_REP = ./Code  
OBJ_TEST = ./obj/main.o ./obj/game_situation.o
OBJ_SDL = ./obj/main_sdl.o ./obj/game_situation.o

EXE_SDL = ./bin/rush_hour_sdl 
EXE_TEST = ./bin/rush_hour_test

all: $(EXE_SDL) $(EXE_TEST)

$(EXE_SDL): $(OBJ_SDL)
	$(CC) -g $^ -o $@ $(FLAGS_SDL)

$(EXE_TEST): $(OBJ_TEST)
	$(CC) -g $^ -o $@

./obj/main_sdl.o: ./Code/main_sdl.cpp ./Code/game_situation.h
	$(CC) $(FLAGS) -c $< -o $@

./obj/main.o: ./Code/main.cpp ./Code/game_situation.h
	$(CC) $(FLAGS) -c $< -o $@

./obj/game_situation.o: ./Code/game_situation.cpp ./Code/game_situation.h
	$(CC) $(FLAGS) -c $< -o $@

clean:
	rm -rf ./bin/* ./obj/*
