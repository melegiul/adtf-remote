//
// Created by uniautonom on 11.03.20.
//

#ifndef AADC_NAVIGATIONHELPER_H
#define AADC_NAVIGATIONHELPER_H

#include <Map/Lane.hpp>
#include "MapGraph.h"

using namespace std;

typedef vector<std::string> InstructionSet;
typedef pair<Lane *, double> LaneAngle;

#define THREE_ARM_CROSSING_GENERIC_INSTRUCTION_OFFSET 5

class NavigationHelper {
private:
    static MapGraph mapGraph;

    static string getInstructionForTransition(Lane *fromLane, Lane *toLane);

    static string getGenericInstruction(int direction_id);

public:
    static InstructionSet getDrivingInstructionsForRoute(const Route &lanesToDrive);

    static InstructionSet getDrivingInstructionsForSection(PassableElement *fromElement, PassableElement *toElement);

    static void makeMapGraph();
};


#endif //AADC_NAVIGATIONHELPER_H
