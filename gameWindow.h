#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include "leaderboardWindow.h"

class gameWindow
{
public:
    gameWindow(int rows, int cols, int mines, const std::string &user)
        : rows(rows), cols(cols), mineCount(mines), username(user), gameOver(false), gameWon(false), flaggedCount(0), debugMode(false), paused(false), showingLeaderboard(false), timeElapsed(0)
    {
        srand(static_cast<unsigned>(time(0)));
        createBoard();
        loadTextures();
        loadFont();
        startTime = std::chrono::high_resolution_clock::now();
    }

    void runGWindow()
    {
        int windowWidth = cols * 32;
        int windowHeight = (rows * 32) + 100;

        sf::RenderWindow gameWindow(sf::VideoMode(windowWidth, windowHeight), "Minesweeper Game");
        while (gameWindow.isOpen())
        {
            sf::Event event;
            while (gameWindow.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    gameWindow.close();
                }
                if (!gameOver && !gameWon && !showingLeaderboard)
                {
                    if (event.type == sf::Event::MouseButtonPressed)
                    {
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            if (isPausePlayButtonClicked(event.mouseButton.x, event.mouseButton.y))
                            {
                                togglePausePlay();
                            }
                            if (isLeaderboardButtonClicked(event.mouseButton.x, event.mouseButton.y))
                            {
                                showingLeaderboard = true;
                                togglePausePlay();
                                paused = true;
                                LeaderboardWindow leaderboard(rows, cols, leaderboardEnt);
                                gameWindow.setVisible(false);
                                leaderboard.run();
                                showingLeaderboard = false;
                                togglePausePlay();
                                gameWindow.setVisible(true);
                            }
                        }
                    }
                }
                if (!gameOver && !gameWon && !showingLeaderboard && !paused)
                {
                    if (event.type == sf::Event::MouseButtonPressed)
                    {
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            handleLeftClick(event.mouseButton.x, event.mouseButton.y);
                            if (isDebugButtonClicked(event.mouseButton.x, event.mouseButton.y))
                            {
                                toggleDebugMode();
                            }
                        }
                        else if (event.mouseButton.button == sf::Mouse::Right)
                        {
                            handleRightClick(event.mouseButton.x, event.mouseButton.y);
                        }
                    }
                }

                if (gameOver || gameWon || paused || showingLeaderboard)
                {
                    if (gameWon)
                    {
                        int time = static_cast<int>(timeElapsed);
                        int minutes = time / 60;
                        int seconds = time % 60;
                        std::ofstream file;
                        file.open("files/leaderboard.txt", std::ios::app);
                        std::string minutesStr;
                        std::string secondsStr;
                        if (minutes <= 10)
                        {
                            minutesStr = "0";
                            minutesStr += std::to_string(minutes);
                        }
                        else
                        {
                            minutesStr = std::to_string(minutes);
                        }
                        if (seconds <= 10)
                        {
                            secondsStr = "0";
                            secondsStr += std::to_string(seconds);
                        }
                        else
                        {
                            secondsStr = std::to_string(seconds);
                        }
                        file << std::endl
                             << minutesStr << ":" << secondsStr << ", " << username << std::endl;

                        std::string leaderboardTime = std::to_string(minutes);
                        leaderboardTime += ":";
                        leaderboardTime += std::to_string(seconds);
                        leaderboardEnt[leaderboardTime] = username;

                        showingLeaderboard = true;
                        LeaderboardWindow leaderboard(rows, cols, leaderboardEnt);
                        gameWindow.setVisible(false);
                        leaderboard.run();
                        gameWon = false;
                        paused = true;
                        gameWindow.setVisible(true);
                    }
                    if (event.type == sf::Event::MouseButtonPressed)
                    {
                        if (event.mouseButton.button == sf::Mouse::Left)
                        {
                            if (isResetButtonClicked(event.mouseButton.x, event.mouseButton.y))
                            {
                                resetGame();
                            }
                            if (isLeaderboardButtonClicked(event.mouseButton.x, event.mouseButton.y))
                            {
                                LeaderboardWindow leaderboard(rows, cols, leaderboardEnt);
                                gameWindow.setVisible(false);
                                leaderboard.run();
                                gameWindow.setVisible(true);
                            }
                        }
                    }
                }
            }

            if (!paused && !gameOver && !gameWon && !showingLeaderboard)
            {
                auto currentTime = std::chrono::high_resolution_clock::now();
                timeElapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime - pausedDuration).count();
            }

            gameWindow.clear(sf::Color::White);
            drawBoard(gameWindow);
            drawUI(gameWindow);
            gameWindow.display();
        }
    }

private:
    int rows;
    int cols;
    int mineCount;
    bool gameOver;
    bool gameWon;
    int flaggedCount;
    bool debugMode;
    bool paused;
    bool showingLeaderboard;
    long timeElapsed;
    std::string username;
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> pauseStartTime;
    std::chrono::duration<long> pausedDuration;

    struct Tile
    {
        bool hasMine = false;
        bool revealed = false;
        bool flagged = false;
        int adjacentMines = 0;
    };

    std::map<std::string, std::string> leaderboardEnt;
    std::vector<std::vector<Tile>> board;
    sf::Texture hiddenTileTexture;
    sf::Texture revealedTileTexture;
    sf::Texture mineTexture;
    sf::Texture flagTexture;
    sf::Texture faceHappyTexture;
    sf::Texture faceWinTexture;
    sf::Texture faceLoseTexture;
    sf::Texture digitsTexture;
    sf::Texture numberTextures[8]; // Array for number textures
    sf::Texture debugTexture;
    sf::Texture pauseTexture;
    sf::Texture playTexture;
    sf::Texture leaderboardTexture;
    sf::Font font;
    sf::Sprite faceButton;
    sf::Sprite debugButton;
    sf::Sprite pausePlayButton;
    sf::Sprite leaderboardButton;

    void createBoard()
    {
        board.resize(rows, std::vector<Tile>(cols));

        // Place mines
        int placedMines = 0;
        while (placedMines < mineCount)
        {
            int r = rand() % rows;
            int c = rand() % cols;
            if (!board[r][c].hasMine)
            {
                board[r][c].hasMine = true;
                placedMines++;
            }
        }

        // Calculate adjacent mines
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                if (!board[r][c].hasMine)
                {
                    int count = 0;
                    for (int dr = -1; dr <= 1; ++dr)
                    {
                        for (int dc = -1; dc <= 1; ++dc)
                        {
                            int nr = r + dr;
                            int nc = c + dc;
                            if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && board[nr][nc].hasMine)
                            {
                                count++;
                            }
                        }
                    }
                    board[r][c].adjacentMines = count;
                }
            }
        }
    }

    void clearAll()
    {
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                if (board[r][c].hasMine == true || board[r][c].flagged == true || board[r][c].revealed == true || board[r][c].adjacentMines > 0)
                {
                    board[r][c].hasMine = false;
                    board[r][c].flagged = false;
                    board[r][c].revealed = false;
                    board[r][c].adjacentMines = 0;
                }
            }
        }
    }

    void handleLeftClick(int x, int y)
    {
        int row = y / 32;
        int col = x / 32;

        if (row >= rows || col >= cols || board[row][col].revealed || board[row][col].flagged)
        {
            return;
        }

        if (board[row][col].hasMine)
        {
            gameOver = true;
            revealAllMines();
            faceButton.setTexture(faceLoseTexture);
        }
        else
        {
            revealTile(row, col);
            if (checkWinCondition())
            {
                gameWon = true;
                faceButton.setTexture(faceWinTexture);
                flagAllMines();
            }
        }
    }

    void handleRightClick(int x, int y)
    {
        int row = y / 32;
        int col = x / 32;

        if (row >= rows || col >= cols || board[row][col].revealed)
        {
            return;
        }

        board[row][col].flagged = !board[row][col].flagged;
        flaggedCount += board[row][col].flagged ? 1 : -1;
    }

    void revealTile(int row, int col)
    {
        if (row < 0 || row >= rows || col < 0 || col >= cols || board[row][col].revealed || board[row][col].flagged)
        {
            return;
        }

        board[row][col].revealed = true;

        if (board[row][col].adjacentMines == 0)
        {
            for (int dr = -1; dr <= 1; ++dr)
            {
                for (int dc = -1; dc <= 1; ++dc)
                {
                    revealTile(row + dr, col + dc);
                }
            }
        }
    }

    void flagAllMines()
    {
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                if (board[r][c].hasMine)
                {
                    board[r][c].flagged = true;
                    flaggedCount += 1;
                }
            }
        }
    }
    void revealAllMines()
    {
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                if (board[r][c].hasMine)
                {
                    board[r][c].revealed = true;
                }
            }
        }
    }

    void hideAllMines()
    {
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                if (board[r][c].hasMine)
                {
                    board[r][c].revealed = false;
                }
            }
        }
    }

    bool checkWinCondition()
    {
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                if (!board[r][c].hasMine && !board[r][c].revealed)
                {
                    return false;
                }
            }
        }
        return true;
    }

    void drawBoard(sf::RenderWindow &window)
    {
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                sf::Sprite tile;
                tile.setPosition(c * 32, r * 32);

                if (board[r][c].revealed)
                {
                    tile.setTexture(revealedTileTexture);
                    window.draw(tile);
                    if (board[r][c].hasMine)
                    {
                        sf::Sprite mineSprite;
                        mineSprite.setTexture(mineTexture);
                        mineSprite.setPosition(c * 32, r * 32);
                        window.draw(mineSprite);
                    }
                    else if (board[r][c].adjacentMines > 0)
                    {
                        sf::Sprite numberSprite;
                        numberSprite.setTexture(numberTextures[board[r][c].adjacentMines - 1]);
                        numberSprite.setPosition(c * 32, r * 32);
                        window.draw(numberSprite);
                    }
                }
                else
                {
                    tile.setTexture(hiddenTileTexture);
                    window.draw(tile);
                    if (debugMode && board[r][c].hasMine)
                    {
                        sf::Sprite mineSprite;
                        mineSprite.setTexture(mineTexture);
                        mineSprite.setPosition(c * 32, r * 32);
                        window.draw(mineSprite);
                    }
                }

                if (board[r][c].flagged)
                {
                    sf::Sprite flagSprite;
                    flagSprite.setTexture(flagTexture);
                    flagSprite.setPosition(c * 32, r * 32);
                    window.draw(flagSprite);
                }
            }
        }
    }

    void drawUI(sf::RenderWindow &window)
    {
        faceButton.setPosition((cols * 32) / 2 - 32, (rows + .5) * 32);
        window.draw(faceButton);
        debugButton.setPosition((cols * 32) - 304, (rows + .5) * 32);
        window.draw(debugButton);

        pausePlayButton.setPosition((cols * 32) - 240, (rows + .5) * 32);
        window.draw(pausePlayButton);

        leaderboardButton.setPosition((cols * 32) - 176, (rows + .5) * 32);
        window.draw(leaderboardButton);

        drawCounter(window);
        drawTimer(window);
    }

    void drawDigit(sf::RenderWindow &window, int digit, float x, float y)
    {
        sf::Sprite digitSprite;
        digitSprite.setTexture(digitsTexture);
        digitSprite.setTextureRect(sf::IntRect(digit * 21, 0, 21, 32)); // Assuming each digit is 21x32 pixels
        digitSprite.setPosition(x, y);
        window.draw(digitSprite);
    }

    void drawCounter(sf::RenderWindow &window)
    {
        int count = mineCount - flaggedCount;
        float x = 33;
        float y = (rows + .5) * 32 + 16;

        if (count < 0)
        {
            drawDigit(window, 10, x, y); // Draw minus sign (assuming it's the last sprite in digits.png)
            x += 21;
            count = -count;
        }

        std::string countStr = std::to_string(count);
        for (char digitChar : countStr)
        {
            int digit = digitChar - '0';
            drawDigit(window, digit, x, y);
            x += 21;
        }
    }

    void drawTimer(sf::RenderWindow &window)
    {
        int totalSeconds = static_cast<int>(timeElapsed);
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        float x = cols * 32 - 97;
        float y = (rows + .5) * 32 + 16;

        // Convert minutes and seconds to strings
        std::string minutesStr = std::to_string(minutes);
        if (minutesStr.length() < 2)
        {
            minutesStr.insert(0, 2 - minutesStr.length(), '0'); // Pad with leading zeros
        }

        std::string secondsStr = std::to_string(seconds);
        if (secondsStr.length() < 2)
        {
            secondsStr.insert(0, 2 - secondsStr.length(), '0'); // Pad with leading zeros
        }

        // Draw the minutes digits
        for (char digitChar : minutesStr)
        {
            int digit = digitChar - '0';
            drawDigit(window, digit, x, y);
            x += 21; // Move to the next digit position
        }

        x += 1;

        // Draw the seconds digits
        for (char digitChar : secondsStr)
        {
            int digit = digitChar - '0';
            drawDigit(window, digit, x, y);
            x += 21; // Move to the next digit position
        }
    }

    bool loadTextures()
    {
        if (!hiddenTileTexture.loadFromFile("files/images/tile_hidden.png") ||
            !revealedTileTexture.loadFromFile("files/images/tile_revealed.png") ||
            !mineTexture.loadFromFile("files/images/mine.png") ||
            !flagTexture.loadFromFile("files/images/flag.png") ||
            !faceHappyTexture.loadFromFile("files/images/face_happy.png") ||
            !faceWinTexture.loadFromFile("files/images/face_win.png") ||
            !faceLoseTexture.loadFromFile("files/images/face_lose.png") ||
            !digitsTexture.loadFromFile("files/images/digits.png") || // Load digits.png
            !debugTexture.loadFromFile("files/images/debug.png") ||
            !pauseTexture.loadFromFile("files/images/pause.png") ||
            !playTexture.loadFromFile("files/images/play.png") ||
            !leaderboardTexture.loadFromFile("files/images/leaderboard.png"))
        {
            std::cerr << "Couldn't load one or more textures" << std::endl;
            return false;
        }

        for (int i = 0; i < 8; ++i)
        {
            std::string filePath = "files/images/number_" + std::to_string(i + 1) + ".png";
            if (!numberTextures[i].loadFromFile(filePath))
            {
                std::cerr << "Couldn't load texture: " << filePath << std::endl;
                return false;
            }
        }

        // Set face button textures
        faceButton.setTexture(faceHappyTexture);
        debugButton.setTexture(debugTexture);
        pausePlayButton.setTexture(pauseTexture);
        leaderboardButton.setTexture(leaderboardTexture);

        return true;
    }

    bool loadFont()
    {
        if (!font.loadFromFile("files/font.ttf"))
        {
            std::cerr << "Couldn't load font file" << std::endl;
            return false;
        }
        return true;
    }

    bool isResetButtonClicked(int x, int y)
    {
        return x >= faceButton.getPosition().x && x <= faceButton.getPosition().x + 64 && y >= faceButton.getPosition().y && y <= faceButton.getPosition().y + 64;
    }

    bool isDebugButtonClicked(int x, int y)
    {
        return x >= debugButton.getPosition().x && x <= debugButton.getPosition().x + 64 &&
               y >= debugButton.getPosition().y && y <= debugButton.getPosition().y + 64;
    }

    bool isPausePlayButtonClicked(int x, int y)
    {
        return x >= pausePlayButton.getPosition().x && x <= pausePlayButton.getPosition().x + 64 &&
               y >= pausePlayButton.getPosition().y && y <= pausePlayButton.getPosition().y + 64;
    }

    bool isLeaderboardButtonClicked(int x, int y)
    {
        return x >= leaderboardButton.getPosition().x && x <= leaderboardButton.getPosition().x + 64 &&
               y >= leaderboardButton.getPosition().y && y <= leaderboardButton.getPosition().y + 64;
    }

    void resetGame()
    {
        gameOver = false;
        gameWon = false;
        showingLeaderboard = false;
        flaggedCount = 0;
        paused = false;
        pausePlayButton.setTexture(pauseTexture);
        faceButton.setTexture(faceHappyTexture);
        clearAll();
        createBoard();
        startTime = std::chrono::high_resolution_clock::now();
        pausedDuration = std::chrono::duration<long>::zero();
    }

    void toggleDebugMode()
    {
        debugMode = !debugMode;
    }

    void togglePausePlay()
    {
        if (paused)
        {
            auto pauseEndTime = std::chrono::high_resolution_clock::now();
            pausedDuration += std::chrono::duration_cast<std::chrono::seconds>(pauseEndTime - pauseStartTime);
            pausePlayButton.setTexture(pauseTexture);
        }
        else
        {
            pauseStartTime = std::chrono::high_resolution_clock::now();
            pausePlayButton.setTexture(playTexture);
        }
        paused = !paused;
    }
};
