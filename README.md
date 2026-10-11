# MyTerminal

A custom, lightweight, and stylish terminal emulator built with C++ (Qt6), designed for Linux enthusiasts (tested and running smoothly on CachyOS & KDE Plasma)!

---

##  Features

- Custom Aesthetic: Deep dark theme (#0b0f19), striking blue command prompts (#38bdf8), and sharp retro-red output (#ef4444).
- ASCII Startup Banner: Features a custom startup screen complete with a sleek ASCII cat and system specifications.
- **Interactive stdin Support**: Unlike basic GUI wrappers, you can actually interact with running processes (e.g., answering [Y/n] prompts during pacman updates).
- **Graphical sudo Integration**: Automatically routes sudo commands through pkexec for seamless password prompts.

---

##  Quick Start

### Prerequisites
Make sure you have Qt6 and CMake installed on your system:
`bash
sudo pacman -S qt6-base cmake make gcc

Building from Source

git clone [https://github.com/YOUR_USERNAME/MyTerminal.git](https://github.com/YOUR_USERNAME/MyTerminal.git)
cd MyTerminal
mkdir build && cd build
cmake ..
make

Running

./MyTerminal
