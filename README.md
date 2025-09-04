# Queens-Solver

A computer vision and automation tool that **detects a Queens puzzle grid directly from the screen** and **solves it automatically**.  

![VdeosinttuloHechoconClipchamp3-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/8056aed1-b90f-456d-acd7-cbf644ec4730)

[Full Demo](https://www.youtube.com/watch?v=QNjWrdYeiVg)
## Features

- Detects the Queens puzzle grid in real time using OpenCV.  
- Automatically extracts rows, columns, and colors from the detected grid.  
- Solves the puzzle respecting its rules (one queen per row, column, and color).  
- Can place queens back on the screen automatically.  
- Configurable click count and delay for puzzle interaction.  

## How It Works

1. **Screen Capture**  
   Captures the current screen and searches for a grid-like structure using edge detection and contour analysis.  

2. **Grid Detection**  
   Uses Hough Transform to find horizontal and vertical lines. Merges close lines to determine the rows and columns of the puzzle.  

3. **Color Matrix Extraction**  
   Creates a matrix representing the color of each cell by sampling its center.  

4. **Puzzle Solving**  
   Runs the solver algorithm to find a valid queen placement based on the detected grid.  

5. **Automated Interaction**  
   Clicks on the cells to place queens on the screen using the configured delay and number of clicks.  

## Usage

1. Launch the program.  
2. Open the Queens puzzle on your screen.  
3. Press **ENTER** to start solving.  
4. Press **END** to exit.  

The program will:  
- Detect the puzzle grid.  
- Print the detected grid size and color matrix.  
- Solve the puzzle and display the time taken.  

## Configuration

You can adjust the following defines in `entry_point.cpp`:  
```cpp
#define DELAY 0   // Delay between clicks (milliseconds)
#define CLICKS 1  // Number of clicks per cell
