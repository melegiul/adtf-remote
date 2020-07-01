//
// Created by uniautonom on 16.03.20.
//

#ifndef AADC_HELPER_H
#define AADC_HELPER_H

#include <Map/Node.hpp>
#include <unordered_map>
#include <map>
#include <vector>

using namespace std;

class Helper {
private:
    static double pointInPolygonInternal(Node *q, Node *p1, Node *p2);

public:
    template<class I, class T>
    static vector<T> getValuesFromMap(map<I, T> map) {
        vector<T> target;
        target.reserve(map.size());
        for (auto kv : map) {
            target.push_back(kv.second);
        }
        return target;
    }


    template<class I, class T>
    static vector<T> getValuesFromMap(unordered_map<I, T> map) {
        vector<T> target;
        target.reserve(map.size());
        for (auto kv : map) {
            target.push_back(kv.second);
        }
        return target;
    }

    static bool pointInPolygon(Node *point, const vector<Node *>& points);
};


#endif //AADC_HELPER_H
