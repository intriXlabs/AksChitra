#include <iostream>
#include <string>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>


/*

engine workflow:

user -> API -> setPixel() -> requestedPixels -> render() -> display

where:
    - user: is the dev who is using this engine for their project
    - API: is the public API of the engine which is exposed to the global scope so
    - pixels: setpixel add pixels into requestedPixels vector which simulates diff rendering like behavior
    - render: render function takes requestedPixels and renders them to the display array and then to the terminal by making a bug buffer of ansii escape codes and then writing it to the terminal

*/

// terminal is exposed to the global scope so that canvas can use it to set pixels and render to terminal
// and also because engine is rendering direct to terminal which means terminal is one and only space output for the engine
// which makes it more workable if it's globaly accessible
/*

architecture time:

private: handeling pixel primitive and few very basic data structures and booleans
private: handling display array and its dimensions
private: helper functions to convert color codes to pixel data
public: refreshing or getting the terminal size
private: request handeling and rendering the display to the terminal
public: main API to set pixel get pixel etc
private: stateless char to set a char at a pixel location with fg color and render it to the terminal
public: stateless char to set a char at a pixel location with fg color and render it to the terminal
[what is stateless char?] stateless char is a char which does not contain any data in memory and it is only used to render a char at a pixel location with fg color and it does not inherit any bg color from the pixel location where it is rendered
public: rendering the display to the terminal

*/

class akschitra{

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
    // char structure to hold char and its fg color
    struct Char{
        int row;
        int col;
        uint8_t bg_r;
        uint8_t bg_g;
        uint8_t bg_b;
        uint8_t fg_r;
        uint8_t fg_g;
        uint8_t fg_b;
        char c;
    };

private:
    Char* characterDisplay;
    std::vector<Char> requestedChars;

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

public:
    // Refresh the terminal size and store it in TS
    int refreshTerminalSize(){
        // Get the terminal size using ioctl
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
            std::cerr << "Failed to get terminal size." << std::endl;
            return -1;
        }
        TS.rows = w.ws_row;
        TS.cols = w.ws_col;
        return 0;
    }

private:
    // Structure to hold requested pixel information to make a proper and understandable render buffer
    struct requestedPixel{
        int row;
        int col;
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    std::vector<requestedPixel> requestedPixels;

public:
    int initialize(){

        if(isInitialized){
            std::cerr << "Display is already initialized." << std::endl;
            return -1;
        }

        if(!isTerminalSizeRefreshed){
            if(refreshTerminalSize() != 0){
                std::cerr << "Failed to refresh terminal size." << std::endl;
                return -1;
            }
            isTerminalSizeRefreshed = true;
        }
        if(requestedPixels.size() > 0){
            requestedPixels.clear();
        }

        displayRows = TS.rows;
        displayCols = TS.cols;
        display = new pixel[displayRows * displayCols];
        characterDisplay = new Char[displayRows * displayCols];

        isInitialized = true;
        return 0;

    }

    int setPixel(int row, int col, uint8_t r, uint8_t g, uint8_t b) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }
        if (row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Pixel coordinates out of bounds." << std::endl;
            return -1;
        }
        requestedPixels.push_back({row, col, r, g, b});
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
        } else {
            p = convertRGBtoPixelData(colorCode);
        }

        requestedPixels.push_back({row, col, p.r, p.g, p.b});
        return 0;
    }

    int setChar(int row, int col, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, char c) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }
        if (row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Pixel coordinates out of bounds." << std::endl;
            return -1;
        }

        pixel existinPixel = getPixel(row, col);
        uint8_t bg_r = existinPixel.r;
        uint8_t bg_g = existinPixel.g;
        uint8_t bg_b = existinPixel.b;

        requestedChars.push_back({row, col, bg_r, bg_g, bg_b, fg_r, fg_g, fg_b, c});
        return 0;
    }
    int setChar(int row, int col, std::string colorCode, char c) {
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
        } else {
            p = convertRGBtoPixelData(colorCode);
        }

        pixel existinPixel = getPixel(row, col);
        uint8_t bg_r = existinPixel.r;
        uint8_t bg_g = existinPixel.g;
        uint8_t bg_b = existinPixel.b;

        requestedChars.push_back({row, col, bg_r, bg_g, bg_b, p.r, p.g, p.b, c});
        return 0;
    }

    pixel getPixel(int row, int col) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return {0, 0, 0}; // Return black for uninitialized display
        }
        if (row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Pixel coordinates out of bounds." << std::endl;
            return {0, 0, 0}; // Return black for out of bounds
        }
        return display[row * displayCols + col];
    }

    terminalSize getTerminalSize() {
        if (!isTerminalSizeRefreshed) {
            std::cerr << "Terminal size is not refreshed." << std::endl;
            return {0, 0}; // Return zero size for unrefreshed terminal
        }
        return TS;
    }

    std::vector<requestedPixel> getRequestedPixels() {
        return requestedPixels;
    }

private:
    std::vector<Char> requestedStatelessChars;

public:
    int setStatelessChar(int row, int col, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b, char c) {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }
        if (row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Pixel coordinates out of bounds." << std::endl;
            return -1;
        }

        pixel existinPixel = getPixel(row, col);
        uint8_t bg_r = existinPixel.r;
        uint8_t bg_g = existinPixel.g;
        uint8_t bg_b = existinPixel.b;

        requestedStatelessChars.push_back({row, col, bg_r, bg_g, bg_b, fg_r, fg_g, fg_b, c});
        return 0;
    }    
    
public:

    int render() {
        if (!isInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        std::string buffer;

        // rendering the requested pixels to the display array and making a buffer of ansii escape codes to render to the terminal
        for (const auto& pixel : requestedPixels) {
            // Move the cursor to the specified position
            buffer += "\033[" + std::to_string(pixel.row + 1) + ";" + std::to_string(pixel.col + 1) + "H";
            // Set the color using ANSI escape codes
            buffer += "\033[48;2;" + std::to_string(static_cast<int>(pixel.r)) + ";" 
                      + std::to_string(static_cast<int>(pixel.g)) + ";" 
                      + std::to_string(static_cast<int>(pixel.b)) + "m" 
                      + " "; // Use a block character to represent the pixel
            display[pixel.row * displayCols + pixel.col] = {pixel.r, pixel.g, pixel.b};
        }


        // stateless char rendering
        for (const auto& schar : requestedStatelessChars) {
            // Move the cursor to the specified position
            buffer += "\033[" + std::to_string(schar.row + 1) + ";" + std::to_string(schar.col + 1) + "H";
            // Set the background color using ANSI escape codes
            buffer += "\033[48;2;" + std::to_string(static_cast<int>(schar.bg_r)) + ";" 
                      + std::to_string(static_cast<int>(schar.bg_g)) + ";" 
                      + std::to_string(static_cast<int>(schar.bg_b)) + "m" 
                      + "\033[38;2;" + std::to_string(static_cast<int>(schar.fg_r)) + ";"
                      + std::to_string(static_cast<int>(schar.fg_g)) + ";"
                      + std::to_string(static_cast<int>(schar.fg_b)) + "m"
                      + schar.c; // Render the character
        }
        // Reset color
        buffer += "\033[0m";

        for(const auto& c : requestedChars) {
            // Move the cursor to the specified position
            buffer += "\033[" + std::to_string(c.row + 1) + ";" + std::to_string(c.col + 1) + "H";
            // Set the background color using ANSI escape codes
            buffer += "\033[48;2;" + std::to_string(static_cast<int>(c.bg_r)) + ";" 
                      + std::to_string(static_cast<int>(c.bg_g)) + ";" 
                      + std::to_string(static_cast<int>(c.bg_b)) + "m" 
                      + "\033[38;2;" + std::to_string(static_cast<int>(c.fg_r)) + ";"
                      + std::to_string(static_cast<int>(c.fg_g)) + ";"
                      + std::to_string(static_cast<int>(c.fg_b)) + "m"
                      + c.c; // Render the character
        }

        // cleaning the vectors
        requestedPixels.clear();
        requestedStatelessChars.clear();
        requestedChars.clear();

        // rendering the buffer to the terminal
        write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        return 0;
    }

}akschitraExposedEngine;