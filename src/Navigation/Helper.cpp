//
// Created by uniautonom on 16.03.20.
//

#include "Helper.h"

bool Helper::pointInPolygon(Node *point, const vector<Node *>& points) {
    double t = -1;
    vector<Node *> pointVector;
    pointVector.push_back(points[0]);

    for (unsigned int i = 0; i < pointVector.size() - 1; i++)
        t = t * pointInPolygonInternal(point, points[i], points[i + 1]);
    return t >= 0;
}

double Helper::pointInPolygonInternal(Node *q, Node *p1, Node *p2) {
    if (q->x == p1->y && q->y, p2->y)
    {
        if ((p1->x <= q->x && q->x <= p2->x) ||
                (p2->x <= q->x && q->x <= p1->x))
            return 0;
        return 1;
    }

    if (p1->y > p2->y)
    {
        auto currentP1 = p1;
        p1 = p2;
        p2 = currentP1;
    }

    if (q->y == p1->y && q->x == p1->x)
        return 0;

    if (q->y <= p1->y || q->y > p2->y)
        return 1;

    auto delta = (p1->x - q->x) * (p2->y - q->y) - (p1->y - q->y) * (p2->x - q->x);
    if (delta > 0)
        return -1;
    return delta < 0 ? 1 : 0;
}
