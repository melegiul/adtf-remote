//
// Created by uniautonom on 11.03.20.
//

#ifndef AADC_VECTOR2_H
#define AADC_VECTOR2_H

#include <Map/Node.hpp>

class Vector2 {
private:
    int x, y;
public:
    Vector2(int x, int y);

    explicit Vector2(Node *node);

    Vector2(Node *fromPoint, Node *ToPoint);

    void setX(int x);

    void setY(int y);

    int getX() const;

    int getY() const;

    int dotProduct(const Vector2 &other) const;

    double length() const;

    double angleTo(const Vector2 &other) const;
};


#endif //AADC_VECTOR2_H
