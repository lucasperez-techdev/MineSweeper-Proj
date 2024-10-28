Minesweeper Game

This Minesweeper project, developed as a programming class assignment, recreates the classic puzzle game with a modern graphical interface and includes a welcome screen, leaderboard functionality, and a scalable grid display.

Table of Contents

•	Introduction
•	Features
•	Installation
•	Usage
•	File Structure
•	Technologies Used
•	Future Improvements

Introduction

This Minesweeper game is implemented in C++ and uses the SFML (Simple and Fast Multimedia Library) for rendering graphics. It offers the classic gameplay experience, with features like a start screen, game window with interactive cells, and a leaderboard to track high scores.

Features

•	Graphical User Interface (GUI): Uses SFML for a smooth and interactive graphical interface.
•	Configurable Board Size: Players can adjust grid dimensions and mine count to customize difficulty.
•	Leaderboard: Tracks and displays top scores.
•	Win/Loss Detection: Automatically determines if the player has won or lost based on cell interactions.
•	Responsive Controls: Left-click to reveal a cell, right-click to flag a potential mine.

Installation

1.	Clone the Repository
```
git clone https://github.com/prism-bjj/MineSweeper-proj.git
```

2.	Navigate to the Project Directory
```
cd MineSweeper-proj
```

3.	Build the Project
Use the provided Makefile to compile the project:
```
make
```

Ensure SFML is installed on your system:
```
sudo apt-get install libsfml-dev
```

4.	Run the Game
 ```
./prog
```

Usage

1.	Welcome Screen: Start the game by navigating the welcome screen.
2.	Playing Minesweeper:
•	Left Click: Reveal a cell.
•	Right Click: Flag a suspected mine cell.
3.	Leaderboard: Check the leaderboard to see top scores after a game ends.

File Structure

•	main.cpp: The entry point of the program. Initializes the welcomeWindow and starts the game.
•	welcomeWindow.h: Handles the initial game window, displaying options like starting a new game or viewing the leaderboard.
•	gameWindow.h: Manages the main gameplay, including grid display, mine placement, and cell interactions.
•	leaderboardWindow.h: Displays and updates high scores for players.
•	Makefile: Compiles and links the project files, handling dependencies for SFML and other libraries.
•	prog: The compiled executable for the game.

Technologies Used

•	C++: Core programming language.
•	SFML (Simple and Fast Multimedia Library): Provides the graphical rendering for the game’s interface.

Future Improvements

•	Additional Game Modes: Adding different modes like “Timed Mode” or “Infinite Grid.”
•	Dynamic Difficulty: Adjust difficulty based on player performance.
•	Enhanced Graphics: Implement animations and better visual effects.
•	Online Leaderboard: Track and compare high scores across players online.
