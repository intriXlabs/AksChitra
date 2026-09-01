#include <iostream>
#include <string>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>


class akschitraEngine{

private:
    bool isDisplayInitialized = false;

private: // basic primitives and data structures
    // pixel structure to hold RGB values
    struct pixel{
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    // character structure to hold character and its color
    struct character{
        char ch;
        uint8_t fg_r, fg_g, fg_b; // foreground color
    };

private: // display grid for both character and pixel data
    pixel* pixelDisplay; // pixel display grid
    character* characterDisplay; // character display grid

private:
    int displayRows, displayCols; // dimensions of the display

public:
    int initializeDisplay(int rows, int cols) {
        // no protection coz same function going to refresh too

        struct winsize w;
        if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
            std::cerr << "Failed to get terminal size." << std::endl;
            return -1;
        }
        displayRows = w.ws_row;
        displayCols = w.ws_col;

        pixelDisplay = new pixel[displayRows * displayCols];
        characterDisplay = new character[displayRows * displayCols];

        isDisplayInitialized = true;

        return 0;
    }

private:
    struct requestedPixel {
        int row;
        int col;
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };
    struct requestedCharacter {
        int row;
        int col;
        char ch;
        uint8_t fg_r;
        uint8_t fg_g;
        uint8_t fg_b;
    };

    std::vector<requestedPixel> requestedPixels; // to hold requested pixel updates
    std::vector<requestedCharacter> requestedCharacters; // to hold requested character updates

    std::vector<requestedPixel> requestedStatelessPixels; // to hold requested stateless pixel updates
    std::vector<requestedCharacter> requestedStatelessCharacters; // to hold requested stateless character updates

private: //helper functions
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

    pixel convertCustomToPixelData(std::string customColorCode) {
        // Implement your custom color code conversion logic here
        // For example, if you have a predefined set of custom color codes:
        if (customColorCode == "red") {
            return {255, 0, 0};
        } else if (customColorCode == "green") {
            return {0, 255, 0};
        } else if (customColorCode == "blue") {
            return {0, 0, 255};
        }
        // Add more custom color codes as needed

        std::cerr << "Unknown custom color code: " << customColorCode << std::endl;
        return {0, 0, 0}; // Return black for unknown custom color code
    }

    pixel connectorToHelper(std::string colorCode) {
        pixel p;
        if (colorCode[0] == '#') {
            p = convertHexToPixelData(colorCode);
        } else if (colorCode.substr(0, 3) == "rgb") {
            p = convertRGBtoPixelData(colorCode);
        } else {
            p = convertCustomToPixelData(colorCode);
        }
        return p;
    }

public:
    int setPixel(int row, int col, uint8_t r, uint8_t g, uint8_t b) {
        if (!isDisplayInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        if(row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Pixel coordinates out of bounds." << std::endl;
            return -1;
        }

        requestedPixels.push_back({r, g, b});
        return 0;
    }
    int setPixel(int row, int col, std::string colorCode) {
        if (!isDisplayInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        if(row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Pixel coordinates out of bounds." << std::endl;
            return -1;
        }

        pixel p = connectorToHelper(colorCode);
        requestedPixels.push_back({row, col, p.r, p.g, p.b});
        return 0;
    }


    int setCharacter(int row, int col, char ch, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b) {
        if (!isDisplayInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        if(row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Character coordinates out of bounds." << std::endl;
            return -1;
        }

        requestedCharacters.push_back({row, col, ch, fg_r, fg_g, fg_b});
        return 0;
    }
    int setCharacter(int row, int col, char ch, std::string colorCode) {
        if (!isDisplayInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        if(row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Character coordinates out of bounds." << std::endl;
            return -1;
        }

        pixel p = connectorToHelper(colorCode);
        requestedCharacters.push_back({row, col, ch, p.r, p.g, p.b});
        return 0;
    }

    int setStatelessPixel(int row, int col, uint8_t r, uint8_t g, uint8_t b) {
        if (!isDisplayInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        if(row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Pixel coordinates out of bounds." << std::endl;
            return -1;
        }

        requestedStatelessPixels.push_back({row, col, r, g, b});
        return 0;
    }
    int setStatelessPixel(int row, int col, std::string colorCode) {
        if (!isDisplayInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        if(row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Pixel coordinates out of bounds." << std::endl;
            return -1;
        }

        pixel p = connectorToHelper(colorCode);
        requestedStatelessPixels.push_back({row, col, p.r, p.g, p.b});
        return 0;
    }

    int setStatelessCharacter(int row, int col, char ch, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b) {
        if (!isDisplayInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        if(row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Character coordinates out of bounds." << std::endl;
            return -1;
        }

        requestedStatelessCharacters.push_back({row, col, ch, fg_r, fg_g, fg_b});
        return 0;
    }
    int setStatelessCharacter(int row, int col, char ch, std::string colorCode) {
        if (!isDisplayInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return -1;
        }

        if(row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Character coordinates out of bounds." << std::endl;
            return -1;
        }

        pixel p = connectorToHelper(colorCode);
        requestedStatelessCharacters.push_back({row, col, ch, p.r, p.g, p.b});
        return 0;
    }

    pixel getPixel(int row, int col) {
        if (!isDisplayInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return {0, 0, 0}; // Return black for uninitialized display
        }

        if(row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Pixel coordinates out of bounds." << std::endl;
            return {0, 0, 0}; // Return black for out of bounds
        }

        return pixelDisplay[row * displayCols + col];
    }

    character getCharacter(int row, int col) {
        if (!isDisplayInitialized) {
            std::cerr << "Display is not initialized." << std::endl;
            return {' ', 0, 0, 0}; // Return space character for uninitialized display
        }

        if(row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            std::cerr << "Character coordinates out of bounds." << std::endl;
            return {' ', 0, 0, 0}; // Return space character for out of bounds
        }

        return characterDisplay[row * displayCols + col];
    }

private:
    std::string processRequestedPixels(std::string &buffer){
        for(auto &pixel : requestedPixels){
            buffer += "\033[" + std::to_string(pixel.row) + ";" + std::to_string(pixel.col) + "H"; // Move cursor
            buffer += "\033[48;2;" + std::to_string(pixel.r) + ";" + std::to_string(pixel.g) + ";" + std::to_string(pixel.b) + " " + "m"; // Set color
            pixelDisplay[pixel.row * displayCols + pixel.col] = {pixel.r, pixel.g, pixel.b}; // Update display grid
        }
        buffer += "\033[0m"; // Reset color
        requestedPixels.clear(); // Clear after processing
        return buffer;
    }

    std::string processRequestedCharacters(std::string &buffer){
        pixel existingPixel;
        for(auto &character : requestedCharacters){
            existingPixel = pixelDisplay[character.row * displayCols + character.col];

            buffer += "\033[" + std::to_string(character.row) + ";" + std::to_string(character.col) + "H"; // Move cursor
            buffer += "\033[48;2;" + std::to_string(existingPixel.r) + ";" + std::to_string(existingPixel.g) + ";" + std::to_string(existingPixel.b) + "m"; // Set background color
            buffer += "\033[38;2;" + std::to_string(character.fg_r) + ";" + std::to_string(character.fg_g) + ";" + std::to_string(character.fg_b) + "m"; // Set foreground color
            buffer += character.ch; // Print character
            characterDisplay[character.row * displayCols + character.col] = {character.ch, character.fg_r, character.fg_g, character.fg_b}; // Update display grid
        }
        buffer += "\033[0m"; // Reset color
        requestedCharacters.clear(); // Clear after processing
        return buffer;
    }

    std::string processRequestedStatelessPixels(std::string &buffer){
        for(auto &pixel : requestedStatelessPixels){
            buffer += "\033[" + std::to_string(pixel.row) + ";" + std::to_string(pixel.col) + "H"; // Move cursor
            buffer += "\033[48;2;" + std::to_string(pixel.r) + ";" + std::to_string(pixel.g) + ";" + std::to_string(pixel.b) + " " + "m"; // Set color
        }
        buffer += "\033[0m"; // Reset color
        requestedStatelessPixels.clear(); // Clear after processing
        return buffer;
    }

    std::string processRequestedStatelessCharacters(std::string &buffer){
        pixel existingPixel;
        for(auto &character : requestedStatelessCharacters){
            existingPixel = pixelDisplay[character.row * displayCols + character.col];

            buffer += "\033[" + std::to_string(character.row) + ";" + std::to_string(character.col) + "H"; // Move cursor
            buffer += "\033[48;2;" + std::to_string(existingPixel.r) + ";" + std::to_string(existingPixel.g) + ";" + std::to_string(existingPixel.b) + "m"; // Set background color
            buffer += "\033[38;2;" + std::to_string(character.fg_r) + ";" + std::to_string(character.fg_g) + ";" + std::to_string(character.fg_b) + "m"; // Set foreground color
            buffer += character.ch; // Print character
        }
        buffer += "\033[0m"; // Reset color
        requestedStatelessCharacters.clear(); // Clear after processing
        return buffer;
    }

public:
    int render(){
        std::string buffer;

        processRequestedPixels(buffer);
        processRequestedCharacters(buffer);
        processRequestedStatelessPixels(buffer);
        processRequestedStatelessCharacters(buffer);

        write(STDOUT_FILENO, buffer.c_str(), buffer.size());

        return 0;
    }


}akschitraExposedEngine;