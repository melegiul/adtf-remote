//
// Created by uniautonom on 11.03.20.
//

#ifndef AADC_MAPGRAPH_H
#define AADC_MAPGRAPH_H

#include <map>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

typedef vector<Lane *> Route;
typedef map<int, map<int, int>> AdjacentMatrix;
typedef map<int, map<int, float>> FloatAdjacentMatrix;

class MapGraph {
private:
    //Matrix for edges
    FloatAdjacentMatrix A;

    //Matrix for distances
    FloatAdjacentMatrix D;

    //Matrix for paths
    AdjacentMatrix P;

    bool upToDate = false;
public:
    MapGraph();

    void fillMapGraphWithDatabase();

    void fillMapGraphWithLanes(const vector<Lane *> &lanes);

    void setIsConnected(int sourceLane, int destinationLane, float weight);

    vector<int> getNodes() const;

    Route calculateRoute(int startLane, int targetLane);

    void floyd_warshall();
};


#endif //AADC_MAPGRAPH_H
