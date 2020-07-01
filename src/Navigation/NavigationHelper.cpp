//
// Created by uniautonom on 11.03.20.
//

#include <Map/NavigationMarker.hpp>
#include "Vector2.h"
#include "NavigationHelper.h"

MapGraph NavigationHelper::mapGraph;

InstructionSet NavigationHelper::getDrivingInstructionsForSection(PassableElement *fromElement, PassableElement *toElement) {
    InstructionSet instructionSet;
    int fromLane = -1, toLane = -1;

    auto *fromElementAsSpecialMarking = dynamic_cast<SpecialMarking*>(fromElement);
    auto *fromElementAsLane = dynamic_cast<Lane*>(fromElement);
    if (fromElementAsSpecialMarking && fromElementAsSpecialMarking->isParkingType()) {
        instructionSet.emplace(instructionSet.begin(), "Leave parking spot.");
        fromLane = fromElementAsSpecialMarking->adjacentLane.front();
    }
    else if (fromElementAsLane) {
        fromLane = fromElementAsLane->getId();
    }
    else {
        throw runtime_error("Unsupported map object for route calculation.");
    }

    auto *toElementAsSpecialMarking = dynamic_cast<SpecialMarking*>(toElement);
    auto *toElementAsLane = dynamic_cast<Lane*>(toElement);
    bool isParkingSpot;
    if ((isParkingSpot = toElementAsSpecialMarking && toElementAsSpecialMarking->isParkingType())) {
        toLane = toElementAsSpecialMarking->adjacentLane.front();
        instructionSet.push_back("Drive into parking spot.");
    } else if (toElementAsLane) {
        toLane = toElementAsLane->getId();
    } else {
        throw runtime_error("Unsupported map object for route calculation.");
    }

    Route route = mapGraph.calculateRoute(fromLane, toLane);
    InstructionSet routeInstructions = getDrivingInstructionsForRoute(route);
    instructionSet.insert(isParkingSpot ? instructionSet.end() - 1 : instructionSet.end(),
                          routeInstructions.begin(), routeInstructions.end());
    instructionSet.push_back("Destination reached.");
    return instructionSet;
}

void NavigationHelper::makeMapGraph() {
    if (!mapGraph.getNodes().empty())
        mapGraph = MapGraph();
    mapGraph.fillMapGraphWithDatabase();
}

InstructionSet NavigationHelper::getDrivingInstructionsForRoute(const Route &lanesToDrive) {
    Route instructionRoute(lanesToDrive);

    InstructionSet instructions;
    if (!instructionRoute.empty()) {
        if (instructionRoute.size() > 1) {
            if (instructionRoute.front()->getType() == LaneType::InCrossing)
                instructionRoute.erase(instructionRoute.begin());
            if (instructionRoute.back()->getType() == LaneType::InCrossing)
                instructionRoute.erase(instructionRoute.end() - 1);

            // iterate over all lanes (until the second-last) and generate an instruction for each transition
            Lane *current = nullptr, *next = nullptr;
            for (unsigned int position = 0; position < instructionRoute.size() - 2; position++) {
                current = instructionRoute.at(position);
                next = instructionRoute.at(position + 1);
                if (next->isInCrossing()) {
                    string instruction = getInstructionForTransition(current, next);
                    instructions.push_back(instruction.substr(0, instruction.size() - 1) + " at the next crossing.");
                }
            }
        }
    } else
        instructions.push_back("No route available.");

    return instructions;
}

string NavigationHelper::getInstructionForTransition(Lane *fromLane, Lane *toLane) {
    auto reachableLanes = fromLane->getReachableLanes();

    if (reachableLanes.empty() || reachableLanes.find(toLane) == reachableLanes.end())
        return "No connection for: " + to_string(fromLane->getId()) + "->" + to_string(toLane->getId());

    int numberOfReachableLanes = fromLane->getNumberOfReachableLanes();
    if (numberOfReachableLanes == 1) {
        int direction_id = reachableLanes.at(toLane);
        if (direction_id == Direction::Straight)
            return "Follow the street.";
        else
            return "Illegal index.";
    }
    else if (numberOfReachableLanes == 2 || numberOfReachableLanes == 3) {
        int direction_id = reachableLanes.at(toLane);
        switch (direction_id) {
            case Direction::Right:
                return "Turn right.";
            case Direction::Straight:
                return "Drive straight.";
            case Direction::Left:
                return "Turn left.";
            case Direction::HalfwayRight:
                return "Turn halfway right.";
            case Direction::HalfwayLeft:
                return "Turn halfway left.";
            case ThreeArmCrossingGenericDirection::First:
            case ThreeArmCrossingGenericDirection::Second:
            case ThreeArmCrossingGenericDirection::Third:
                return getGenericInstruction(direction_id - THREE_ARM_CROSSING_GENERIC_INSTRUCTION_OFFSET);
            default:
                return "Illegal Index.";
        }
    }
    // create generic instruction
    int direction_id = reachableLanes.at(toLane);
    return getGenericInstruction(direction_id);
}

string NavigationHelper::getGenericInstruction(int direction_id) {
    string appendix = "th";
    int adjusted_direction_id = direction_id;
    if (direction_id < 10 || direction_id > 20)
        adjusted_direction_id = adjusted_direction_id % 10;
    if (adjusted_direction_id == 0)
        appendix = "st";
    else if (adjusted_direction_id == 1)
        appendix = "nd";
    else if (adjusted_direction_id == 2)
        appendix = "rd";
    return "Turn at the " + to_string(direction_id + 1) + appendix + " arm.";
}