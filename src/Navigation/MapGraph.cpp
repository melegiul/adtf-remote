//
// Created by uniautonom on 11.03.20.
//

#include "Vector2.h"
#include "NavigationHelper.h"
#include <Map/NavigationMarker.hpp>
#include <Map/ContentManager.hpp>
#include "MapGraph.h"
#include "Helper.h"
#include <fstream>

void MapGraph::fillMapGraphWithDatabase() {
    ContentManager &cm = ContentManager::getInstance();

    vector<Lane *> allLanes = Helper::getValuesFromMap(cm.lane_map);

    fillMapGraphWithLanes(allLanes);
}

void MapGraph::fillMapGraphWithLanes(const vector<Lane *> &lanes) {
    // Connect all lanes that are connected

    for (auto fromLane : lanes) {
        vector<LaneAngle> laneAnglePairs;
        for (auto toLane : lanes) {
            if (fromLane == toLane) {
                setIsConnected(fromLane->getId(), toLane->getId(), 0);
            } else if (fromLane->getRightLine()->path.back()->equals(toLane->getRightLine()->path.front())
                       && fromLane->getLeftEndNode()->equals(toLane->getLeftStartNode())
                       && !(fromLane->isInCrossing() && toLane->isInCrossing())) {
                setIsConnected(fromLane->getId(), toLane->getId(), 1);

                //setting lane as adjacentLane
                //         +
                //
                //
                //  +----->+
                Vector2 incomingLaneVector(
                        fromLane->getLeftEndNode(),
                        fromLane->getRightLine()->path.back());

                //         +
                //         ^
                //         |
                //  +      +
                Vector2 targetVector(
                        fromLane->getRightLine()->path.back(),
                        toLane->getRightLine()->path.back());

                double angle = incomingLaneVector.angleTo(targetVector);

                //when right line is 1 point, angle will be "nan" because of zero division
                if (isnan(angle))
                    angle = 0;

                laneAnglePairs.emplace_back(toLane, angle);
            } else
                setIsConnected(fromLane->getId(), toLane->getId(), numeric_limits<float>::infinity());
        }
        fromLane->setAdjacentLanes(laneAnglePairs);
    }
}

void MapGraph::setIsConnected(int sourceLane, int destinationLane, float weight) {
    A[sourceLane][destinationLane] = weight;
    upToDate = false;
}

Route MapGraph::calculateRoute(int startLane, int targetLane) {
    //check if startLane or targetLane even exist
    if (A.find(startLane) == A.end() || A.find(targetLane) == A.end())
        return Route();

    if (!upToDate) { floyd_warshall(); }

    Route route;
    if (startLane == targetLane)
        route.push_back(ContentManager::getInstance().getLaneFromId(startLane));
    else if (P.at(startLane).at(targetLane) == -1) {
        // Are start lane and target lane directly connected?
        if (!isinf(A.at(startLane).at(targetLane))) {
            route.push_back(ContentManager::getInstance().getLaneFromId(startLane));
            route.push_back(ContentManager::getInstance().getLaneFromId(targetLane));
        }
    } else {
        auto intermediate = P.at(startLane).at(targetLane);
        auto routeA = calculateRoute(startLane, intermediate);
        auto routeB = calculateRoute(intermediate, targetLane);

        //return empty route in case P is corrupted
        if (routeA.empty() || routeB.empty())
            return Route();

        route = routeA;
        route.pop_back();
        route.insert(route.end(), routeB.begin(), routeB.end());
    }
    return route;
}

void MapGraph::floyd_warshall() {
    const vector<int> &nodes = getNodes();
    for (auto i: nodes)
        for (auto j: nodes) {
            D[i][j] = A[i][j];
            P[i][j] = -1;
        }
    for (auto k: nodes)
        for (auto i: nodes)
            for (auto j: nodes) {
                if (D.at(i).at(k) + D.at(k).at(j) < D.at(i).at(j)) {
                    D.at(i).at(j) = D.at(i).at(k) + D.at(k).at(j);
                    P.at(i).at(j) = k;
                }
            }
    upToDate = true;
}

vector<int> MapGraph::getNodes() const {
    vector<int> nodes;
    for (const auto &element : A)
        nodes.push_back(element.first);
    return nodes;
}

MapGraph::MapGraph() = default;