#include "headers/collision.h"
#include <cmath>

bool checkCollision(SDL_Rect rect, double circleX, double circleY, double radius) {
    double closestX = fmax(rect.x, fmin(circleX, rect.x + rect.w));
    double closestY = fmax(rect.y, fmin(circleY, rect.y + rect.h));

    double dx = circleX - closestX;
    double dy = circleY - closestY;

    return (dx * dx + dy * dy) <= (radius * radius);
}
