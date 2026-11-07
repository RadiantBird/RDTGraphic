#pragma once

#include <array>
#include <cstdint>

namespace RDTGraphic {
struct Vector2 {
    float x = 0.0f;
    float y = 0.0f;
};

struct Color4 {
    float red, green, blue, alpha;

    Color4(float red, float green, float blue, float alpha)
        : red(red), green(green), blue(blue), alpha(alpha) {}

    static Color4 fromRGBA(int32_t R, int32_t G, int32_t B, int32_t A) {
        return {
            R / 255.0f,
            G / 255.0f,
            B / 255.0f,
            A / 255.0f
        };
    }

    std::array<float, 4> to_array() const {
        return {red, green, blue, alpha};
    }
};

struct Tria {
    Vector2 position;
    std::array<float, 6> vertices;
    uint32_t vao_id;
    uint32_t vbo_id;
    Color4 color;

    Tria(): 
        position{0.0f, 0.0f},
        vertices{},
        vao_id(0),
        vbo_id(0),
        color(1.0f, 0.0f, 1.0f, 1.0f) 
    {}

    Tria(Vector2 position, std::array<float, 6> vertices, Color4 color):
        position(position),
        vertices(vertices),
        vao_id(0),
        vbo_id(0),
        color(color)
    {}
};

}