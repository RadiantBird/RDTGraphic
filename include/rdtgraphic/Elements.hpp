#pragma once

#include "Triangle.hpp"

namespace RDTGraphic {

struct Rect {
    Vector2 position;
    Vector2 size;

    Tria t0;
    Tria t1;

    Color4 color;

    Rect():
        // This means redirecting(actually delegating) the constructor symbol __Rect
        Rect(
            {0.0f, 0.0f},
            {50.0f, 50.0f},
            Color4(1.0f, 0.0f, 1.0f, 1.0f)
        )
    {}
    
    // __Rect is this!
    Rect(Vector2 position, Vector2 size, Color4 color):
        position(position),
        size(size),
        t0(position, {
            -size.x, -size.y,
            size.x, -size.y,
            size.x,  size.y
        }, color, 0.5f), // zPosition = -1.0f for test
        t1(position, {
            -size.x, -size.y,
            size.x,  size.y,
            -size.x,  size.y
        }, color, 0.5f),
        color(color)
    {}
};


} // namespace RDTGraphic