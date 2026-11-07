#include <iostream>
#include <print>
#include <array>

#include <vector>
#include <memory>

#include <string>

#define STB_TRUETYPE_IMPLEMENTATION

#include "RDTGraphic.h"

using std::vector;
using std::string;
using std::print;

template <typename T>
void unpackAndPush(vector<T>& target, vector<T>&& pushing) {
    for (auto& v : pushing) {
        target.push_back(std::move(v));
    }
}

using namespace RDTGraphic;
int main() {
    print("Welcome!\n");

    DrawMode mode = DrawMode::Pixel;
    const int width = 800;
    const int height = 600;

    Renderer renderer = Renderer();
    bool success = renderer.init(width, height, mode);
    if (!success) {
        print("Initilaizing failed!");
        return 1;
    }

    // --------------------------------------------------

    string filepath = "res/MPLUS1p-Regular.ttf";
    FontParser parser = FontParser();
    success = parser.init(filepath);
    if (!success) {
        print("Initilaizing failed!");
        return 1;
    }

    // --------------------------------------------------

    Tesselator tessa = Tesselator();
    success = tessa.init();
    if (!success) {
        print("Initilaizing failed!");
        return 1;
    }

    vector<Tria> triangles = {};
    string ASCIIwords = "Hello!";

    for (int i = 0; char& c : ASCIIwords) {
        Contours contours = parser.getContoursOfASCIICodepoint(c);
        float scale = parser.getFontScaleOf(150.0f);

        { // tess scope
        vector<Tria> t = tessa.getTrianglesFromContours(contours, scale, {-350.0f + (float)i * 125.0f, 0.0f});
        unpackAndPush(triangles, std::move(t));
        } // tess scope

        i++;
    }

    print("triangles.size() = {}\n", triangles.size());
    renderer.registerTriangles(std::move(triangles));

    // --------------------------------------------------

    {
        vector<Rect> rectangles = {
            Rect()
        };
        renderer.registerRectangles(std::move(rectangles));
    }

    while (bool keepRendering = !renderer.draw()) {
        // just sleep...
    }

    return 0;
}