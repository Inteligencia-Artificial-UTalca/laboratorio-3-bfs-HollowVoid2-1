#include "Map.h"
#include "Search.h"
#include "ColorMap.h"
#include <cassert>
#include <iostream>

int main(int argc, char *argv[]){

    //Verify that the amount of arguments is correct
    //6 arguments: program name, map file, x1, y1, x2, y2
    if (argc != 6) {
        std::cerr << "Uso: " << argv[0] << " <mapa> <x_inicio> <y_inicio> <x_fin> <y_fin>\n";
        return 1;
    }

    int x1, y1, x2, y2;
    try {
        x1 = std::stoi(argv[2]);
        y1 = std::stoi(argv[3]);
        x2 = std::stoi(argv[4]);
        y2 = std::stoi(argv[5]);
    } catch (...) {
        std::cerr << "Error: las coordenadas deben ser numeros enteros\n";
        return 1;
    }

    //Load map with class Map
    Map map(argv[1]);
    ColorMap colorMap(map);
    colorMap.print();

    if (x1 < 0 || x1 >= map.getHeight() || y1 < 0 || y1 >= map.getWidth() ||
        x2 < 0 || x2 >= map.getHeight() || y2 < 0 || y2 >= map.getWidth()) {
        
        std::cerr << "Error: coordenadas fuera del mapa\n";
        return 1;
    }
    if (map._map[x1][y1] == 1 || map._map[x2][y2] == 1) {
        std::cerr << "Error: inicio o fin es un obstáculo\n";
        return 1;
    }

    auto path = Search::Greedy(map,{atoi(argv[2]),atoi(argv[3])},{atoi(argv[4]),atoi(argv[5])}); 
    if (path.empty()) {
        std::cout << "No path found\n";
    } else {
        colorMap.print(path);
    }
    
    //Calculate path distance
    if (path.empty()) {
    std::cout << "No path found\n";
    } else {

        //Print path distance
        colorMap.print(path);

        int distance = path.size() - 1;
        std::cout << "Distancia: " << distance << std::endl;
    }
    
    return 0;
}