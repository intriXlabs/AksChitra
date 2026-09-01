#include <iostream>
#include <string>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <random>
#include <algorithm>
#include <iomanip>
#include <cstdint>

// ============ YOUR ENGINE CODE (COPY-PASTE FROM YOUR FILE) ============
class akschitraEngine {
private:
    bool isDisplayInitialized = false;

private:
    struct pixel {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    struct character {
        char ch;
        uint8_t fg_r, fg_g, fg_b;
    };

private:
    pixel* pixelDisplay;
    character* characterDisplay;
    int displayRows, displayCols;

public:
    int initializeDisplay(int rows, int cols) {
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) {
            std::cerr << "Failed to get terminal size." << std::endl;
            return -1;
        }
        displayRows = w.ws_row;
        displayCols = w.ws_col;

        pixelDisplay = new pixel[displayRows * displayCols];
        characterDisplay = new character[displayRows * displayCols];

        // Initialize arrays
        for (int i = 0; i < displayRows * displayCols; i++) {
            pixelDisplay[i] = {0, 0, 0};
            characterDisplay[i] = {' ', 255, 255, 255};
        }

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

    std::vector<requestedPixel> requestedPixels;
    std::vector<requestedCharacter> requestedCharacters;
    std::vector<requestedPixel> requestedStatelessPixels;
    std::vector<requestedCharacter> requestedStatelessCharacters;

public:
    int setPixel(int row, int col, uint8_t r, uint8_t g, uint8_t b) {
        if (!isDisplayInitialized || row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            return -1;
        }
        requestedPixels.push_back({row, col, r, g, b});
        return 0;
    }

    int setCharacter(int row, int col, char ch, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b) {
        if (!isDisplayInitialized || row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            return -1;
        }
        requestedCharacters.push_back({row, col, ch, fg_r, fg_g, fg_b});
        return 0;
    }

    int setStatelessPixel(int row, int col, uint8_t r, uint8_t g, uint8_t b) {
        if (!isDisplayInitialized || row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            return -1;
        }
        requestedStatelessPixels.push_back({row, col, r, g, b});
        return 0;
    }

    int setStatelessCharacter(int row, int col, char ch, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b) {
        if (!isDisplayInitialized || row < 0 || row >= displayRows || col < 0 || col >= displayCols) {
            return -1;
        }
        requestedStatelessCharacters.push_back({row, col, ch, fg_r, fg_g, fg_b});
        return 0;
    }

private:
    std::string processRequestedPixels(std::string &buffer) {
        for (auto &pixel : requestedPixels) {
            buffer += "\033[" + std::to_string(pixel.row + 1) + ";" + std::to_string(pixel.col + 1) + "H";
            buffer += "\033[48;2;" + std::to_string(pixel.r) + ";" + std::to_string(pixel.g) + ";" + std::to_string(pixel.b) + "m ";
            pixelDisplay[pixel.row * displayCols + pixel.col] = {pixel.r, pixel.g, pixel.b};
        }
        buffer += "\033[0m";
        requestedPixels.clear();
        return buffer;
    }

    std::string processRequestedCharacters(std::string &buffer) {
        for (auto &character : requestedCharacters) {
            pixel existingPixel = pixelDisplay[character.row * displayCols + character.col];
            buffer += "\033[" + std::to_string(character.row + 1) + ";" + std::to_string(character.col + 1) + "H";
            buffer += "\033[48;2;" + std::to_string(existingPixel.r) + ";" + std::to_string(existingPixel.g) + ";" + std::to_string(existingPixel.b) + "m";
            buffer += "\033[38;2;" + std::to_string(character.fg_r) + ";" + std::to_string(character.fg_g) + ";" + std::to_string(character.fg_b) + "m";
            buffer += character.ch;
            characterDisplay[character.row * displayCols + character.col] = {character.ch, character.fg_r, character.fg_g, character.fg_b};
        }
        buffer += "\033[0m";
        requestedCharacters.clear();
        return buffer;
    }

    std::string processRequestedStatelessPixels(std::string &buffer) {
        for (auto &pixel : requestedStatelessPixels) {
            buffer += "\033[" + std::to_string(pixel.row + 1) + ";" + std::to_string(pixel.col + 1) + "H";
            buffer += "\033[48;2;" + std::to_string(pixel.r) + ";" + std::to_string(pixel.g) + ";" + std::to_string(pixel.b) + "m ";
        }
        buffer += "\033[0m";
        requestedStatelessPixels.clear();
        return buffer;
    }

    std::string processRequestedStatelessCharacters(std::string &buffer) {
        for (auto &character : requestedStatelessCharacters) {
            pixel existingPixel = pixelDisplay[character.row * displayCols + character.col];
            buffer += "\033[" + std::to_string(character.row + 1) + ";" + std::to_string(character.col + 1) + "H";
            buffer += "\033[48;2;" + std::to_string(existingPixel.r) + ";" + std::to_string(existingPixel.g) + ";" + std::to_string(existingPixel.b) + "m";
            buffer += "\033[38;2;" + std::to_string(character.fg_r) + ";" + std::to_string(character.fg_g) + ";" + std::to_string(character.fg_b) + "m";
            buffer += character.ch;
        }
        buffer += "\033[0m";
        requestedStatelessCharacters.clear();
        return buffer;
    }

public:
    int render() {
        std::string buffer;
        buffer.reserve(1024 * 1024); // Reserve 1MB for performance

        processRequestedPixels(buffer);
        processRequestedCharacters(buffer);
        processRequestedStatelessPixels(buffer);
        processRequestedStatelessCharacters(buffer);

        if (!buffer.empty()) {
            write(STDOUT_FILENO, buffer.c_str(), buffer.size());
        }
        return 0;
    }
};
// ============ END OF ENGINE CODE ============

// ============ FPS BENCHMARK CODE ============
class FPSBenchmark {
private:
    akschitraEngine* engine;
    int rows, cols;
    std::mt19937 rng;
    
public:
    FPSBenchmark(akschitraEngine* eng) : engine(eng), rng(42) {
        struct winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        rows = w.ws_row;
        cols = w.ws_col;
    }

    void runIntenseBenchmark(int durationSeconds = 10) {
        std::cout << "\033[2J\033[H"; // Clear screen
        std::cout << "\033[?25l"; // Hide cursor
        
        auto start = std::chrono::high_resolution_clock::now();
        auto end = start + std::chrono::seconds(durationSeconds);
        
        long long frameCount = 0;
        long long pixelCount = 0;
        long long charCount = 0;
        
        std::cout << "\033[1;1H╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    INTENSE FPS BENCHMARK                       ║\n";
        std::cout << "║                    Running for " << durationSeconds << " seconds...              ║\n";
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        
        while (std::chrono::high_resolution_clock::now() < end) {
            // Random mode selection
            int mode = rng() % 4;
            
            switch (mode) {
                case 0: // Full screen random pixels
                    for (int r = 0; r < rows; r++) {
                        for (int c = 0; c < cols; c++) {
                            engine->setPixel(r, c, rng() % 256, rng() % 256, rng() % 256);
                            pixelCount++;
                        }
                    }
                    break;
                    
                case 1: // Full screen random characters
                    for (int r = 0; r < rows; r++) {
                        for (int c = 0; c < cols; c++) {
                            engine->setCharacter(r, c, 33 + (rng() % 94), rng() % 256, rng() % 256, rng() % 256);
                            charCount++;
                        }
                    }
                    break;
                    
                case 2: // Mixed content
                    for (int r = 0; r < rows; r++) {
                        for (int c = 0; c < cols; c++) {
                            if ((r + c) % 2 == 0) {
                                engine->setPixel(r, c, rng() % 256, rng() % 256, rng() % 256);
                                pixelCount++;
                            } else {
                                engine->setCharacter(r, c, 33 + (rng() % 94), rng() % 256, rng() % 256, rng() % 256);
                                charCount++;
                            }
                        }
                    }
                    break;
                    
                case 3: // Stateless rendering
                    for (int r = 0; r < rows; r += 2) {
                        for (int c = 0; c < cols; c += 2) {
                            if (rng() % 2 == 0) {
                                engine->setStatelessPixel(r, c, rng() % 256, rng() % 256, rng() % 256);
                                pixelCount++;
                            } else {
                                engine->setStatelessCharacter(r, c, 33 + (rng() % 94), rng() % 256, rng() % 256, rng() % 256);
                                charCount++;
                            }
                        }
                    }
                    break;
            }
            
            engine->render();
            frameCount++;
        }
        
        auto actualEnd = std::chrono::high_resolution_clock::now();
        double elapsedSeconds = std::chrono::duration_cast<std::chrono::microseconds>(actualEnd - start).count() / 1000000.0;
        
        // Calculate stats
        double fps = frameCount / elapsedSeconds;
        double pixelsPerSecond = pixelCount / elapsedSeconds;
        double charsPerSecond = charCount / elapsedSeconds;
        double totalElementsPerSecond = (pixelCount + charCount) / elapsedSeconds;
        
        // Display results
        std::cout << "\033[2J\033[H";
        std::cout << "\033[1;1H╔══════════════════════════════════════════════════════════════╗\n";
        std::cout << "║                    BENCHMARK RESULTS                           ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        std::cout << "║ Time Elapsed:     " << std::fixed << std::setprecision(2) << std::setw(8) << elapsedSeconds << " seconds              ║\n";
        std::cout << "║ Total Frames:     " << std::setw(8) << frameCount << " frames               ║\n";
        std::cout << "║ FPS:              " << std::setprecision(2) << std::setw(8) << fps << " frames/sec           ║\n";
        std::cout << "║ Total Pixels:     " << std::setw(8) << pixelCount << " pixels               ║\n";
        std::cout << "║ Total Characters: " << std::setw(8) << charCount << " characters           ║\n";
        std::cout << "║ Pixels/sec:       " << std::setprecision(0) << std::setw(10) << pixelsPerSecond << " px/s          ║\n";
        std::cout << "║ Characters/sec:   " << std::setprecision(0) << std::setw(10) << charsPerSecond << " chars/s        ║\n";
        std::cout << "║ Total Elements/s: " << std::setprecision(0) << std::setw(10) << totalElementsPerSecond << " elem/s        ║\n";
        std::cout << "╠══════════════════════════════════════════════════════════════╣\n";
        
        if (fps > 60) {
            std::cout << "║ STATUS: EXCELLENT - Can handle 60+ FPS                        ║\n";
        } else if (fps > 30) {
            std::cout << "║ STATUS: GOOD - Can handle 30+ FPS                             ║\n";
        } else if (fps > 15) {
            std::cout << "║ STATUS: MODERATE - Can handle 15+ FPS                         ║\n";
        } else {
            std::cout << "║ STATUS: POOR - Below 15 FPS, optimization needed              ║\n";
        }
        
        std::cout << "╚══════════════════════════════════════════════════════════════╝\n";
        
        std::cout << "\033[?25h"; // Show cursor
        std::cout << "\033[0m"; // Reset colors
    }
};

int main() {
    akschitraEngine engine;
    
    if (engine.initializeDisplay(0, 0) != 0) {
        std::cerr << "Failed to initialize display" << std::endl;
        return -1;
    }
    
    FPSBenchmark benchmark(&engine);
    benchmark.runIntenseBenchmark(10); // Run for 10 seconds
    
    return 0;
}