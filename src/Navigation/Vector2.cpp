//
// Created by uniautonom on 11.03.20.
//

#include <cmath>
#include "Vector2.h"


Vector2::Vector2(int x, int y) : x(x), y(y) {}

Vector2::Vector2(Node *node) : Vector2(node->x, node->y) {
}

Vector2::Vector2(Node *fromPoint, Node *ToPoint) {
    x = (ToPoint->x - fromPoint->x);
    y = (ToPoint->y - fromPoint->y);
}

void Vector2::setX(int x) {
    Vector2::x = x;
}

void Vector2::setY(int y) {
    Vector2::y = y;
}

int Vector2::getX() const {
    return x;
}

int Vector2::getY() const {
    return y;
}

int Vector2::dotProduct(const Vector2 &other) const {
    return x * other.getX() + y * other.getY();
}

double Vector2::angleTo(const Vector2 &other) const {
    return acos((dotProduct(other)) / (length() * other.length()));
}

double Vector2::length() const {
    return sqrt(pow(x, 2) + pow(y, 2));
}
