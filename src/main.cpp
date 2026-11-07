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
    for (auto v : pushing) {
        target.push_back(v);
    }
}

using namespace RDTGraphic;
int main() {
    print("Welcome!\n");

    Renderer renderer = Renderer();
    bool success = renderer.init();
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
    vector<char> ASCIIwords = {'H', 'e', 'l', 'l', 'o', '!'};

    for (int i = 0; char& c : ASCIIwords) {
        Contours contours = parser.getContoursOfASCIICodepoint(c);
        float scale = parser.getFontScaleOf(0.5f);

        { // tess scope
        vector<Tria> t = tessa.getTrianglesFromContours(contours, scale, {-350.0f + (float)i * 125.0f, 0.0f});
        unpackAndPush(triangles, std::move(t));
        } // tess scope

        i++;
    }

    print("triangles.size() = {}\n", triangles.size());
    renderer.registerTriangles(std::move(triangles));

    // --------------------------------------------------

    DrawMode mode = DrawMode::Pixel;

    while (bool keepRendering = !renderer.draw(mode)) {
        // just sleep...
    }

    return 0;
}