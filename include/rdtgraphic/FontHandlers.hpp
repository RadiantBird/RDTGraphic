#pragma once

#include <string>
#include <iostream>
#include <print>
#include <fstream>
#include <iterator>

#include "Triangle.hpp"

#include "stb_truetype.h"
#include "tesselator.h"

namespace RDTGraphic {

using std::string;
using std::print;

Vector2 bezier(Vector2 p0, Vector2 p1, Vector2 p2, float t) {
    float u = 1.0f - t;

    return {
        u*u*p0.x + 2*u*t*p1.x + t*t*p2.x,
        u*u*p0.y + 2*u*t*p1.y + t*t*p2.y
    };
}

using Contours = vector<vector<Vector2>>;

class FontParser {
    vector<unsigned char> bytes;
    stbtt_fontinfo font{};
    stbtt_vertex* vertices = nullptr;

public:
    FontParser() {
        // just sleep...
    }

    bool init(const string& filepath) {
        // returns true if succeed

        std::ifstream file(filepath, std::ios::binary);
        if (!file.is_open()) {
            print("Failed to read {}!", filepath);
            return false;
        }

        bytes = {
            std::istreambuf_iterator<char>{file},
            std::istreambuf_iterator<char>{}
        };

        int offset = stbtt_GetFontOffsetForIndex(bytes.data(), 0);

        if (offset < 0 || !stbtt_InitFont(&font, bytes.data(), offset)) {
            return false;
        }

        return true;
    }

    Contours getContoursOfASCIICodepoint(char character) {
        int count = stbtt_GetCodepointShape(&font, character, &vertices);

        if (count <= 0 || !vertices) {
            print("Failed to get codepoint shape: {}", character);
            return {};
        }

        Contours contours;
        Vector2 current{0, 0};

        for (int i = 0; i < count; i++) {
            auto& v = vertices[i];

            Vector2 end{(float)v.x, (float)v.y};

            switch (v.type) {
                case STBTT_vmove: {
                    contours.emplace_back();
                    contours.back().push_back(end);
                    current = end;
                    break;
                }

                case STBTT_vline: {
                    contours.back().push_back(end);
                    current = end;
                    break;
                }

                case STBTT_vcurve: {
                    Vector2 control{(float)v.cx, (float)v.cy};
                    const int divisions = 10;

                    for (int j = 1; j <= divisions; j++) {
                        float t = (float)j / (float)divisions;

                        Vector2 point = bezier(
                            current,
                            control,
                            end,
                            t
                        );

                        contours.back().push_back(point);
                    }

                    current = end;
                    break;
                }
            }
        }

        stbtt_FreeShape(&font, vertices);
        vertices = nullptr;

        return contours;
    }

    float getFontScaleOf(float s) {
        float scale = stbtt_ScaleForMappingEmToPixels(&font, s);
        return scale;
    }
    
    ~FontParser() {
        print("FontParser has destructed!\n");
    }

};

class Tesselator {
    TESStesselator* tess = nullptr;

    void regisiterContours(const Contours& C) {
        for (const auto& contour : C) {
            if (contour.size() < 3) {
                continue;
            }

            vector<TESSreal> coords;
            for (const auto& p : contour) {
                coords.push_back(p.x);
                coords.push_back(p.y);
            }

            tessAddContour(
                tess, 2, coords.data(),
                sizeof(TESSreal) * 2,
                (int)contour.size()
            );
        }
    }
public:
    Tesselator() {
        // just sleep...
    }

    bool init() {
        tess = tessNewTess(nullptr);
        if (!tess) {
            print("Failed to make TESS\n");
            return false;
        }
        return true;
    }

    vector<Tria> getTrianglesFromContours(const Contours& C, float scale, Vector2 position = {0.0f, 0.0f}) {
        regisiterContours(C);

        vector<Tria> triangles;

        if (!tessTesselate(
            tess,
            TESS_WINDING_NONZERO,
            TESS_POLYGONS, 3, 2, nullptr)
        ) {
            return {};
        }

        const TESSreal* points = tessGetVertices(tess);
        const TESSindex* indices = tessGetElements(tess);

        for (int i = 0; i < tessGetElementCount(tess); i++) {
            TESSindex a = indices[i * 3];
            TESSindex b = indices[i * 3 + 1];
            TESSindex c = indices[i * 3 + 2];

            if (a == TESS_UNDEF || b == TESS_UNDEF || c == TESS_UNDEF) {
                continue;
            }

            triangles.emplace_back(
                position,
                std::array<float, 6>{
                    points[a * 2] * scale, points[a * 2 + 1] * scale,
                    points[b * 2] * scale, points[b * 2 + 1] * scale,
                    points[c * 2] * scale, points[c * 2 + 1] * scale
                },
                Color4::fromRGBA(255, 255, 255, 255)
            );
        }
        
        return triangles;
    }

    ~Tesselator() {
        tessDeleteTess(tess);
        tess = nullptr;
        print("Tesselator has destructed!\n");
    }
};

} // namespace RDTGraphic