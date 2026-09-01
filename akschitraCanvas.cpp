#include <iostream>
#include <string>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>
#include "akschitraEngine.cpp"
#include <algorithm>

/*

main plans

exposing both engine and canvas to the global scope so that dev can use both of them as per their needs
meanwhile also giving above abstraction to the devs who want to use it for their projects

means: i suing ladder abstraction where
    - each ladder step is usable independently
    - each extra step is an abstraction over the previous step
    - each new step makes experience more immersive and easy to use

means if there are n abstraction steps then dev can take out n-1 steps and use the last step for more control over the display
project tries to keep architecture as simple as possible so that dev can use it for their projects without any hassle

*/


/*

inside architecture:

user -> canvas -> engine -> terminal

so canvas and engine both should be initalized before rendering to terminal
engine is exposed to global scope so that canvas can use it to set pixels and render to terminal

engine is two sided means dev can use it for more control over the display and canvas is for more user friendly and easy to use interface
while canvas is more user friendly and easy to use interface for the devs who want to use it for their projects

*/

/*

architecture time:

priavte: handeling pixel primitive and few very basic data structures and booleans
private: handling display array and its dimensions
private: helper functions to convert color codes to pixel data
public: initialize the display
public: pixel only data
public: special shapes from now using same setPixel function to set pixels for shapes and other things
public: few special controls
public: rendering the display to the terminal

*/

class akschitraCanvas{

private:
    // pixel structure to hold RGB values
    struct pixel{
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
    };
    // to remember the terminal size
    struct terminalSize{
        int rows;
        int cols;
    }TS;

    // isinitialized and isTerminalSizeRefreshed flags to ensure proper initialization and terminal size refresh
    bool isInitialized = false;
    bool isTerminalSizeRefreshed = false;

private:
    // display array to hold pixel data and its dimensions
    pixel* display;
    int displayRows, displayCols;

private:
// helper functions to convert color codes to pixel data
    pixel convertHexToPixelData(std::string hexColor) {
        if (hexColor.length() != 7 || hexColor[0] != '#') {
            std::cerr << "Invalid hex color format. Use #RRGGBB." << std::endl;
            return {0, 0, 0}; // Return black for invalid input
        }
        uint8_t r = std::stoi(hexColor.substr(1, 2), nullptr, 16);
        uint8_t g = std::stoi(hexColor.substr(3, 2), nullptr, 16);
        uint8_t b = std::stoi(hexColor.substr(5, 2), nullptr, 16);
        return {r, g, b};

    }

    pixel convertRGBtoPixelData(std::string rgbCode) {
        if (rgbCode.substr(0, 4) != "rgb(" || rgbCode.back() != ')') {
            std::cerr << "Invalid RGB format. Use rgb(R,G,B)." << std::endl;
            return {0, 0, 0}; // Return black for invalid input
        }
        size_t firstComma = rgbCode.find(',', 4);
        size_t secondComma = rgbCode.find(',', firstComma + 1);
        if (firstComma == std::string::npos || secondComma == std::string::npos) {
            std::cerr << "Invalid RGB format. Use rgb(R,G,B)." << std::endl;
            return {0, 0, 0}; // Return black for invalid input
        }
        uint8_t r = std::stoi(rgbCode.substr(4, firstComma - 4));
        uint8_t g = std::stoi(rgbCode.substr(firstComma + 1, secondComma - firstComma - 1));
        uint8_t b = std::stoi(rgbCode.substr(secondComma + 1, rgbCode.length() - secondComma - 2));
        return {r, g, b};
    }

    pixel convertCustomToPixelData(std::string customCode) {
        if(customCode.empty()) {
            std::cerr << "Custom color code cannot be empty." << std::endl;
            return {0, 0, 0}; // Return black for invalid input
        }
        
        if(customCode.length() > 12 || customCode.length() < 3) { // Arbitrary limit for custom color code length
            std::cerr << "Custom color code is too long or too short." << std::endl;
            return {0, 0, 0}; // Return black for invalid input
        }

        for(auto &c: customCode) {
            if(c >= 'A' && c <= 'Z') {
                c = tolower(c);
            }
        }

        if(customCode == "red") {
            return {255, 0, 0};
        } else if(customCode == "green") {
            return {0, 255, 0};
        } else if(customCode == "blue") {
            return {0, 0, 255};
        } else if(customCode == "yellow") {
            return {255, 255, 0};
        } else if(customCode == "cyan") {
            return {0, 255, 255};
        } else if(customCode == "magenta") {
            return {255, 0, 255};
        } else if(customCode == "white") {
            return {255, 255, 255};
        } else if(customCode == "black") {
            return {0, 0, 0};
        } else if(customCode == "orange") {
            return {255, 165, 0};
        } else if(customCode == "purple") {
            return {128, 0, 128};
        } else if(customCode == "pink") {
            return {255, 192, 203};
        } else if(customCode == "brown") {
            return {165, 42, 42};
        } else if(customCode == "gray" || customCode == "grey") {
            return {128, 128, 128};
        } else if(customCode == "lightgray" || customCode == "lightgrey") {
            return {211, 211, 211};
        } else if(customCode == "darkgray" || customCode == "darkgrey") {
            return {169, 169, 169};
        } else if(customCode == "lightblue") {
            return {173, 216, 230};
        } else if(customCode == "darkblue") {
            return {0, 0, 139};
        } else if(customCode == "lightgreen") {
            return {144, 238, 144};
        } else if(customCode == "darkgreen") {
            return {0, 100, 0};
        } else if(customCode == "lightred") {
            return {255, 102, 102};
        } else if(customCode == "darkred") {
            return {139, 0, 0};
        } else if(customCode == "lightyellow") {
            return {255, 255, 224};
        } else if(customCode == "darkyellow") {
            return {204, 204, 0};
        } else if(customCode == "lightcyan") {
            return {224, 255, 255};
        } else if(customCode == "darkcyan") {
            return {0, 139, 139};
        } else if(customCode == "lightmagenta") {
            return {255, 182, 193};
        } else if(customCode == "darkmagenta") {
            return {139, 0, 139};
        } else if(customCode == "lightorange") {
            return {255, 200, 150};
        } else if(customCode == "darkorange") {
            return {255, 140, 0};
        } else if(customCode == "lightpurple") {
            return {200, 162, 200};
        } else if(customCode == "darkpurple") {
            return {75, 0, 130};
        } else if(customCode == "lightpink") {
            return {255, 182, 193};
        } else if(customCode == "darkpink") {
            return {231, 84, 128};
        } else if(customCode == "lightbrown") {
            return {181, 101, 29};
        } else if(customCode == "darkbrown") {
            return {101, 67, 33};
        } else if(customCode == "white"){
            return {255, 255, 255};
        } else if(customCode == "black"){
            return {0, 0, 0};
        } else {
            std::cerr << "Unknown custom color code: " << customCode << std::endl;
            return {0, 0, 0}; // Return black for unknown custom color code
        }

        return {0, 0, 0}; // Placeholder return value
    }

public:
    int initalize(){
        if(isInitialized){
            std::cerr << "Display is already initialized." << std::endl;
            return -1;
        }

        auto [rows, cols] = akschitraExposedEngine.getTerminalSize();
        displayRows = rows;
        displayCols = cols;
        display = new pixel[displayRows * displayCols];

        isInitialized = true;
        return 0;

    }

public:    

    int setPixel(int row, int col, uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        if(akschitraExposedEngine.setPixel(row, col, r, g, b) != 0){
            std::cerr << "Failed to set pixel." << std::endl;
            return -1;
        }

        return 0;
    }

    int setPixel(int row, int col, std::string colorCode) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }
        if (row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Pixel coordinates out of bounds." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        akschitraExposedEngine.setPixel(row, col, p.r, p.g, p.b);
        return 0;
    }

    int getPixel(int row, int col, uint8_t &r, uint8_t &g, uint8_t &b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        auto p = akschitraExposedEngine.getPixel(row, col);
        r = p.r;
        g = p.g;
        b = p.b;

        return 0;
    }


public:
    // special shapes from now using same setPixel function to set pixels for shapes and other things
    int drawRectangleHollow(int startRow, int startCol, int height, int width, uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        // Draw top and bottom borders
        for (int col = startCol; col < startCol + width; ++col) {
            setPixel(startRow, col, r, g, b); // Top border
            setPixel(startRow + height - 1, col, r, g, b); // Bottom border
        }

        // Draw left and right borders
        for (int row = startRow; row < startRow + height; ++row) {
            setPixel(row, startCol, r, g, b); // Left border
            setPixel(row, startCol + width - 1, r, g, b); // Right border
        }

        return 0;
    }
    int drawRectangleHollow(int startRow, int startCol, int height, int width, std::string colorCode) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        drawRectangleHollow(startRow, startCol, height, width, p.r, p.g, p.b);
        return 0;
    }
    int drawRectangleFilled(int startRow, int startCol, int height, int width, uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        for (int row = startRow; row < startRow + height; ++row) {
            for (int col = startCol; col < startCol + width; ++col) {
                setPixel(row, col, r, g, b);
            }
        }

        return 0;
    }
    int drawRectangleFilled(int startRow, int startCol, int height, int width, std::string colorCode) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        drawRectangleFilled(startRow, startCol, height, width, p.r, p.g, p.b);
        return 0;
    }

    int drawLine(int startRow, int startCol, int endRow, int endCol, uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        // Bresenham's line algorithm
        int dx = abs(endCol - startCol);
        int dy = abs(endRow - startRow);
        int sx = (startCol < endCol) ? 1 : -1;
        int sy = (startRow < endRow) ? 1 : -1;
        int err = dx - dy;

        while (true) {
            setPixel(startRow, startCol, r, g, b);

            if (startRow == endRow && startCol == endCol) break;
            int e2 = err * 2;
            if (e2 > -dy) { err -= dy; startCol += sx; }
            if (e2 < dx) { err += dx; startRow += sy; }
        }

        return 0;
    }
    int drawLine(int startRow, int startCol, int endRow, int endCol, std::string colorCode) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        drawLine(startRow, startCol, endRow, endCol, p.r, p.g, p.b);
        return 0;
    }

    int drawCircleHollow(int centerRow, int centerCol, int radius, uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        // Midpoint circle algorithm
        int x = radius;
        int y = 0;
        int err = 0;

        while (x >= y) {
            setPixel(centerRow + y, centerCol + x, r, g, b);
            setPixel(centerRow + y, centerCol - x, r, g, b);
            setPixel(centerRow - y, centerCol + x, r, g, b);
            setPixel(centerRow - y, centerCol - x, r, g, b);
            setPixel(centerRow + x, centerCol + y, r, g, b);
            setPixel(centerRow + x, centerCol - y, r, g, b);
            setPixel(centerRow - x, centerCol + y, r, g, b);
            setPixel(centerRow - x, centerCol - y, r, g, b);

            if (err <= 0) {
                y += 1;
                err += 2*y + 1;
            }
            if (err > 0) {
                x -= 1;
                err -= 2*x + 1;
            }
        }

        return 0;
    }
    int drawCircleHollow(int centerRow, int centerCol, int radius, std::string colorCode) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        drawCircleHollow(centerRow, centerCol, radius, p.r, p.g, p.b);
        return 0;
    }

    int drawCircleFilled(int centerRow, int centerCol, int radius, uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        // Midpoint circle algorithm for filled circle
        for (int y = -radius; y <= radius; y++) {
            for (int x = -radius; x <= radius; x++) {
                if (x*x + y*y <= radius*radius) {
                    setPixel(centerRow + y, centerCol + x, r, g, b);
                }
            }
        }

        return 0;
    }
    int drawCircleFilled(int centerRow, int centerCol, int radius, std::string colorCode) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        drawCircleFilled(centerRow, centerCol, radius, p.r, p.g, p.b);
        return 0;
    }

    int drawTriangleHollow(int row1, int col1, int row2, int col2, int row3, int col3, uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        drawLine(row1, col1, row2, col2, r, g, b);
        drawLine(row2, col2, row3, col3, r, g, b);
        drawLine(row3, col3, row1, col1, r, g, b);

        return 0;
    }
    int drawTriangleHollow(int row1, int col1, int row2, int col2, int row3, int col3, std::string colorCode) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        drawTriangleHollow(row1, col1, row2, col2, row3, col3, p.r, p.g, p.b);
        return 0;
    }

    int drawTriangleFilled(int row1, int col1, int row2, int col2, int row3, int col3, uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        // Sort vertices by y-coordinate
        if (row2 < row1) { std::swap(row1, row2); std::swap(col1, col2); }
        if (row3 < row1) { std::swap(row1, row3); std::swap(col1, col3); }
        if (row3 < row2) { std::swap(row2, row3); std::swap(col2, col3); }

        // Draw the triangle using horizontal lines
        for (int y = row1; y <= row3; ++y) {
            int xStart = col1 + (col2 - col1) * (y - row1) / (row2 - row1);
            int xEnd = col1 + (col3 - col1) * (y - row1) / (row3 - row1);
            if (xStart > xEnd) std::swap(xStart, xEnd);
            for (int x = xStart; x <= xEnd; ++x) {
                setPixel(y, x, r, g, b);
            }
        }

        return 0;
    }
    int drawTriangleFilled(int row1, int col1, int row2, int col2, int row3, int col3, std::string colorCode) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        drawTriangleFilled(row1, col1, row2, col2, row3, col3, p.r, p.g, p.b);
        return 0;
    }

    int drawPolygonHollow(const std::vector<std::pair<int, int>>& vertices, uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        for (size_t i = 0; i < vertices.size(); ++i) {
            const auto& start = vertices[i];
            const auto& end = vertices[(i + 1) % vertices.size()];
            drawLine(start.first, start.second, end.first, end.second, r, g, b);
        }

        return 0;
    }
    int drawPolygonHollow(const std::vector<std::pair<int, int>>& vertices, std::string colorCode) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        drawPolygonHollow(vertices, p.r, p.g, p.b);
        return 0;
    }

    int drawPolygonFilled(const std::vector<std::pair<int, int>>& vertices, uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        // Simple scanline fill algorithm for convex polygons
        int minY = vertices[0].first, maxY = vertices[0].first;
        for (const auto& vertex : vertices) {
            minY = std::min(minY, vertex.first);
            maxY = std::max(maxY, vertex.first);
        }

        for (int y = minY; y <= maxY; ++y) {
            std::vector<int> intersections;
            for (size_t i = 0; i < vertices.size(); ++i) {
                const auto& start = vertices[i];
                const auto& end = vertices[(i + 1) % vertices.size()];
                if ((start.first <= y && end.first > y) || (start.first > y && end.first <= y)) {
                    int x = start.second + (end.second - start.second) * (y - start.first) / (end.first - start.first);
                    intersections.push_back(x);
                }
            }
            std::sort(intersections.begin(), intersections.end());
            for (size_t i = 0; i < intersections.size(); i += 2) {
                for (int x = intersections[i]; x <= intersections[i + 1]; ++x) {
                    setPixel(y, x, r, g, b);
                }
            }
        }

        return 0;
    }
    int drawPolygonFilled(const std::vector<std::pair<int, int>>& vertices, std::string colorCode) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        drawPolygonFilled(vertices, p.r, p.g, p.b);
        return 0;
    }

public:
    int backgroundSetColor(uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        for (int row = 0; row < displayRows; ++row) {
            for (int col = 0; col < displayCols; ++col) {
                setPixel(row, col, r, g, b);
            }
        }

        return 0;
    }
    int backgroundSetColor(std::string colorCode) {

        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        backgroundSetColor(p.r, p.g, p.b);
        return 0;

    }

    int drawTemporaryText(int startRow, int startCol, const std::string& text, uint8_t fgr, uint8_t fgg, uint8_t fgb) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        for(auto &c: text){
            akschitraExposedEngine.setStatelessChar(startRow, startCol, fgr, fgg, fgb, c);
            startCol++; // Move to the next column for the next character
        }   

        return 0;
    }
    int drawTemporaryText(int startRow, int startCol, const std::string& text, std::string colorCode) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }

        drawTemporaryText(startRow, startCol, text, p.r, p.g, p.b);

        return 0;
    }

public:
    int render() {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        auto requestedPixels = akschitraExposedEngine.getRequestedPixels();
        
        for(auto &pixel : requestedPixels){
            display[pixel.row * displayCols + pixel.col] = {pixel.r, pixel.g, pixel.b};
        }

        akschitraExposedEngine.render();

        return 0;
    }

};


int main() {
    akschitraExposedEngine.initialize(); // Initialize the engine first
    akschitraCanvas canvas;
    canvas.initalize();
    canvas.backgroundSetColor("#000000"); // Set background to black
    canvas.drawRectangleHollow(5, 10, 10, 20, "#FF0000"); // Draw a red hollow rectangle
    canvas.drawCircleFilled(15, 40, 5, "green"); // Draw a filled green circle
    canvas.drawTemporaryText(20, 5, "Hello World!", "#FFFFFF"); // Draw white text
    canvas.render(); // Render the display to the terminal

    return 0;
}