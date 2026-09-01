# Akshchitra Engine

<div align="center">

![Version](https://img.shields.io/badge/version-1.0.0-blue.svg)
![C++](https://img.shields.io/badge/C%2B%2B-11%2B-orange.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS-lightgrey.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

**A Wierd yet high potential terminal graphics engine**

*I dicide what terminal do - not others*

</div>

---

## 📖 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Architecture](#-architecture)
- [Quick Start](#-quick-start)
- [API Reference](#-api-reference)
- [Performance Benchmarks](#-performance-benchmarks)
- [Use Cases](#-use-cases)
- [Technical Deep Dive](#-technical-deep-dive)
- [Contributing](#-contributing)
- [License](#-license)

---

## 🌟 Overview

**Akshchitra Engine** is a engine based upon unique and weird methods and architectures i developed over experience and ressearch on it over 2+ years. i design this not to beat FPS, i already had done beat them. main goal of it to give a serious immersive experience to terminal graphics.

The name "Akshchitra" derives from Sanskrit, where "Aksh" (अक्ष) means "eye" and "Chitra" (चित्र) means "picture" — literally "eye-pictures" or "visual art for the eyes." This philosophy drives every aspect of the engine's design. yeah - i like sanskrit, urdu, hindi and more so i keep drive weird names. good weird names.

### Why Terminal Graphics?

why normal graphics? right - yeah i like terminal graphics coz of sharp, uniqye texture which even gui fails to delivers - so why not i make something for others too -

it is my opensource premium project - but i have better ones which are private property.

for this one:
- **Zero Dependencies**: No external libraries, no graphics drivers except ANSI escape sequences and c++ standard library
- **Lightweight**: Minimal memory footprint, instant startup [i expect atleast basic c++ knowledge from user to use it]
- **Retro Aesthetic**: Beautiful pixel-art style rendering [terminals are retro by design]
- **Practical**: Perfect for TUI applications, monitoring dashboards, and games [more like uniqueness and weirdness]

---

## ✨ Features

### 🎨 Rich Color Support
- **24-bit True Color**: Full RGB spectrum (16.7 million colors)
- **Foreground & Background**: Independent color control
- **Hex Color Support**: `#FF5733` format
- **RGB Function Support**: `rgb(255,87,51)` format
- **Named Colors**: Built-in color presets

### 🚀 High Performance
- **Optimized Rendering Pipeline**: Batched ANSI escape sequences
- **Stateful & Stateless Modes**: Choose between persistence and speed
- **Memory-Efficient**: Flat array-based display grid
- **Zero-Copy Operations**: Direct buffer manipulation

### 🔧 Developer-Friendly API
- **Intuitive Methods**: Simple `setPixel()`, `setCharacter()` calls
- **Automatic Bounds Checking**: Safe rendering with error handling
- **Flexible Color Input**: Multiple color format support
- **Clean Abstraction**: Engine handles complexity, you focus on creativity

### 📐 Advanced Architecture
- **Request-Based Rendering**: Queue and batch updates efficiently
- **Dual Display Grids**: Separate pixel and character layers
- **Buffer Management**: Optimized string concatenation
- **Terminal Size Detection**: Automatic dimension adaptation

---

## 🏗 Architecture
my best of best work here - even though it emerges during the process!

```
┌─────────────────────────────────────────────────────────────┐
│                     Application Layer                       │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  │
│  │setPixel()│  │setChar() │  │Stateless │  │  Mixed   │  │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘  │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                     Request Queue Layer                     │
│  ┌──────────────────────────────────────────────────────┐  │
│  │         Requested Updates (Stateful & Stateless)      │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                   Processing Pipeline                      │
│  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐  │
│  │  Pixels  │→ │  Chars   │→ │Stateless │→ │Stateless │  │
│  │Processor │  │Processor │  │  Pixels  │  │  Chars   │  │
│  └──────────┘  └──────────┘  └──────────┘  └──────────┘  │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                    Display Grid Layer                      │
│  ┌─────────────────┐      ┌─────────────────┐             │
│  │  Pixel Display  │      │ Character Display│             │
│  │  (Background)   │      │   (Foreground)   │             │
│  └─────────────────┘      └─────────────────┘             │
└─────────────────────────────────────────────────────────────┘
                            ↓
┌─────────────────────────────────────────────────────────────┐
│                    ANSI Escape Layer                       │
│  ┌──────────────────────────────────────────────────────┐  │
│  │   \033[48;2;R;G;Bm  - Background Color               │  │
│  │   \033[38;2;R;G;Bm  - Foreground Color               │  │
│  │   \033[row;colH     - Cursor Positioning            │  │
│  └──────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
                            ↓
                    Terminal Display
```

---

## 🚀 Quick Start

### Prerequisites

- **C++11 or higher** compiler
- **Linux/macOS** terminal with True Color support
- **Terminal** with ANSI escape sequence support

### Installation

```bash
# Clone the repository
git clone https://github.com/intriXlabs/AksChitra-Initiality.git

# Compile with optimizations
g++ -O3 -std=c++11 -o CODE_FILE

# Run the demo
./akshchitra
```

### Minimal Example

```cpp
#include "akshchitra_engine.cpp" // i do recomend durect cpp file - coz of autmatic linking and no need to worry about linking errors: yeah it is design choice

int main() {
    akschitraEngine engine;
    
    // Initialize with terminal dimensions
    engine.initializeDisplay(0, 0);
    
    // Draw a red pixel at position (10, 10)
    engine.setPixel(10, 10, 255, 0, 0);
    
    // Draw a green character at position (20, 20)
    engine.setCharacter(20, 20, 'A', 0, 255, 0);
    
    // Render everything to the terminal
    engine.render();
    
    return 0;
}
```

---

## 📚 API Reference

### Class: `akschitraEngine`

#### Initialization

```cpp
int initialize()
```
Initializes the display grid. size detection is automatic.

**Returns:** `0` on success, `-1` on failure.

---

#### Pixel Operations

```cpp
int setPixel(int row, int col, uint8_t r, uint8_t g, uint8_t b)
```
Sets a pixel with RGB values (0-255).

```cpp
int setPixel(int row, int col, std::string colorCode)
```
Sets a pixel using color code:
- Hex: `"#FF5733"`
- RGB: `"rgb(255,87,51)"`
- Named: `"red"`, `"green"`, `"blue"`

---

#### Character Operations

```cpp
int setCharacter(int row, int col, char ch, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b)
```
Sets a character with foreground color.

```cpp
int setCharacter(int row, int col, char ch, std::string colorCode)
```
Sets a character with color code (same formats as pixels).

---

#### Stateless Operations

Stateless operations render without updating the internal display grid, perfect for temporary effects:

```cpp
int setStatelessPixel(int row, int col, uint8_t r, uint8_t g, uint8_t b)
int setStatelessCharacter(int row, int col, char ch, uint8_t fg_r, uint8_t fg_g, uint8_t fg_b)
```

---

#### Rendering

```cpp
int render()
```
Processes all queued updates and renders to terminal.

---

## 📊 Performance Benchmarks

### Test Environment
- **CPU**: ryzen 5 5600g 6 cores 12 threads 4.4GHz
- **RAM**: 24gb DDR4
- **Terminal**: different terminals
- **OS**: Ubuntu 22.04.4 LTS

### Results

check yourself images on directory "engine/"

direct results: heavy hard rendering

### GNOME Terminal
| Metric | Value |
| --- | --- |
| **Time Elapsed** | 10.01 seconds |
| **Total Frames** | 963 frames |
| **FPS** | 96.16 frames/sec |
| **Total Pixels** | 4645136 pixels |
| **Total Characters** | 4353955 characters |
| **Pixels/sec** | 463826 px/s |
| **Characters/sec** | 434751 chars/s |
| **Total Elements/s** | 898577 elem/s |
| **Status** | EXCELLENT - Can handle 60+ FPS |

### kitty-terminal:
| Metric | Value |
| --- | --- |
| **Time Elapsed** | 10.00 seconds |
| **Total Frames** | 1559 frames |
| **FPS** | 155.84 frames/sec |
| **Total Pixels** | 7591644 pixels |
| **Total Characters** | 7675785 characters |
| **Pixels/sec** | 758891 px/s |
| **Characters/sec** | 767302 chars/s |
| **Total Elements/s** | 1526193 elem/s |
| **Status** | EXCELLENT - Can handle 60+ FPS |

### Side-by-Side Comparison

| Metric | Kitty Terminal (Image 1) | GNOME Terminal (Image 2) |
| --- | --- | --- |
| **Time Elapsed** | 10.00 seconds | 10.01 seconds |
| **Total Frames** | 1,559 frames | 963 frames |
| **FPS** | 155.84 frames/sec | 96.16 frames/sec |
| **Total Pixels** | 7,591,644 pixels | 4,645,136 pixels |
| **Total Characters** | 7,675,785 characters | 4,353,955 characters |
| **Pixels/sec** | 758,891 px/s | 463,826 px/s |
| **Characters/sec** | 767,302 chars/s | 434,751 chars/s |
| **Total Elements/s** | 1,526,193 elem/s | 898,577 elem/s |
| **Status** | EXCELLENT - Can handle 60+ FPS | EXCELLENT - Can handle 60+ FPS |


---

## 🎯 Use Cases

### 1. Terminal Dashboards
```cpp
// Real-time system monitoring
engine.setCharacter(0, 0, '█', 0, 255, 0); // CPU bar
engine.setCharacter(0, 1, '█', 0, 255, 0);
engine.setCharacter(0, 2, '░', 128, 128, 128);
```

### 2. Terminal Games
```cpp
// Snake game rendering
engine.setCharacter(snakeHead.row, snakeHead.col, '@', 0, 255, 0);
engine.setPixel(food.row, food.col, 255, 0, 0);
```

### 3. Data Visualization
```cpp
// Bar charts
engine.setPixel(row, col, intensity, 0, 255 - intensity);
```

### 4. Text Editors
```cpp
// Syntax highlighting
engine.setCharacter(row, col, keyword[i], 255, 128, 0);
```

---

## 🔬 Technical Deep Dive

### ANSI Escape Sequence Optimization

The engine batches multiple ANSI sequences into a single write operation:

```cpp
// Instead of multiple writes:
write(STDOUT, "\033[10;10H", 8);
write(STDOUT, "\033[48;2;255;0;0m", 16);
write(STDOUT, " ", 1);

// The engine does:
buffer += "\033[10;10H\033[48;2;255;0;0m ";
write(STDOUT, buffer.c_str(), buffer.size()); // Single write
```

### Memory Layout

The display grid uses a flat array for cache efficiency:

```cpp
// Instead of 2D array:
pixel display[rows][cols]; // Cache-unfriendly

// Flat array:
pixel* display = new pixel[rows * cols]; // Contiguous memory
// Access: display[row * cols + col]
```

### Stateful vs Stateless Rendering

**Stateful** updates maintain the display state:
- Updates internal `pixelDisplay` and `characterDisplay`
- Subsequent renders know previous state
- Perfect for persistent UI elements
- bassicaly each data has memory and stored in display

**Stateless** updates are transient:
- Only affects current render
- No state maintenance overhead
- Ideal for particles, animations, effects
- no data stored in display - just render and forget

---

## 🤝 Contributing

not ready for contribution yet - but you can contribute if you want to - but i will not accept any pull request for now - coz of my private work on it.

---

## 📈 Roadmap

### v1.1 - Performance Optimization
- [ ] Double buffering support
- [ ] Multi-threaded rendering
- [ ] Bresenham's line algorithm
- [ ] Circle drawing primitives

### v1.2 - Extended Features
- [ ] Sprite support
- [ ] Animation framework
- [ ] Event handling system
- [ ] Window management

### v2.0 - Advanced Graphics
note: current version is 2.1 due to restart of this repo because of some decisions i took

- [ ] 3D projection support
- [ ] Texture mapping
- [ ] Alpha blending
- [ ] Gradient rendering

---

## 🔧 Troubleshooting

### Common Issues

**Q: Colors not displaying correctly?**
- Ensure terminal supports True Color (24-bit)
- Check `TERM` environment variable
- Try: `export TERM=xterm-256color`

**Q: Screen flickering?**
- Reduce render frequency
- Use stateless rendering for animations
- Enable double buffering (coming in v1.1)

**Q: Performance issues?**
- Compile with `-O3` optimization
- Reduce elements per frame
- Use batch updates instead of individual calls

---

## 💡 Tips & Tricks

### Performance Optimization

1. **Batch Operations**: Queue multiple updates before calling `render()`
2. **Use Stateless**: For temporary effects, stateless is faster
3. **Pre-allocate**: If possible, reuse vectors to avoid reallocation
4. **Optimize Loops**: Minimize color string parsing in hot loops

### Visual Effects

```cpp
// Gradient effect
for (int i = 0; i < cols; i++) {
    uint8_t intensity = (i * 255) / cols;
    engine.setPixel(row, i, intensity, 0, 255 - intensity);
}

// Animation without clearing
engine.setStatelessPixel(prevRow, prevCol, 0, 0, 0); // Clear
engine.setPixel(newRow, newCol, 255, 255, 255);     // Draw
```

---

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.


---

## 🙏 Acknowledgments

- **ANSI Escape Sequences**: The foundation of terminal graphics
- **Terminal Emulators**: For supporting True Color
- **Open Source Community**: For inspiration and support

---

## 📞 Contact

- **GitHub Issues**: [Report bugs](https://github.com/intriXlabs/AksChitra-Initiality/issues)
- **Discussions**: [Join the conversation](https://github.com/intriXlabs/AksChitra-Initiality/discussions)
- **Email**: intrinxlabs@gmail.com
- **website**: [https://intrixlabs.vercel.app](https://intrixlabs.vercel.app)

---

## 🌟 Star History

If you find this project useful, please consider giving it a star! ⭐

---

<div align="center">

**Built with ❤️ for the terminal**

*"In the world of pixels and escape sequences, every character tells a story"*

[⬆ Back to Top](#akshchitra-engine)

</div>

---

## 📊 Project Statistics

- **Lines of Code**: 500+
- **Memory Footprint**: < 1MB
- **Dependencies**: 0 (Zero!)
- **Compile Time**: < 1 second
- **Startup Time**: < 10ms
- **API Methods**: 12+
- **Color Formats**: 3

---

## 🎓 Learning Resources

Want to understand how terminal graphics work?

1. **ANSI Escape Codes**: [Wikipedia Article](https://en.wikipedia.org/wiki/ANSI_escape_code)
2. **True Color Support**: [Terminal Guide](https://gist.github.com/XVilka/8346728)
3. **Terminal Graphics**: [History & Evolution](https://en.wikipedia.org/wiki/Text-based_user_interface)

- i suggest "fuck arround and find out"

---

This README represents the culmination of countless hours of optimization, testing, and refinement. The Akshchitra Engine isn't just code—it's a testament to what's possible when we push the boundaries of conventional thinking.

**Welcome to the weird world of terminal graphics.** 🚀