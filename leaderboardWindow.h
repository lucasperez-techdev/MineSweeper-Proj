#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>

class LeaderboardWindow
{
public:
    LeaderboardWindow(int height, int width, std::map<std::string, std::string> vect)
        : windowWidth(width), windowHeight(height)
    {
        std::map<std::string, std::string> leaderboardEntry = vect;
        loadFont();
        loadLeaderboard();
    }

    void run()
    {
        windowHeight = windowHeight * 16 + 50;
        windowWidth = windowWidth * 16;
        sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Mine Sweeper");
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
            }

            window.clear(sf::Color::Blue);
            drawLeaderboard(window);
            window.display();
        }
    }

    int windowWidth;
    int windowHeight;
    std::map<std::string, std::string> leaderboardEntry;
    sf::Font font;

    bool loadFont()
    {
        if (!font.loadFromFile("files/font.ttf"))
        {
            std::cerr << "Couldn't load font file" << std::endl;
            return false;
        }
        return true;
    }

    void loadLeaderboard()
    {
        std::ifstream fileIn("files/leaderboard.txt");
        if (!fileIn.is_open())
        {
            std::cout << "Couldn't load file" << std::endl;
        }
        std::string str;
        std::string name;
        std::string time;
        while (getline(fileIn, str))
        {
            if (str == "")
            {
                continue;
            }
            else
            {
                int index = str.find(',');
                time = str.substr(0, index);
                name = str.substr(index + 1, str.length());

                leaderboardEntry[time] = name;
            }
        }
    }

    void drawLeaderboard(sf::RenderWindow &window)
    {

        sf::Text title("LEADERBOARD", font, 20);
        title.setStyle(sf::Text::Bold | sf::Text::Underlined);
        title.setFillColor(sf::Color::White);
        title.setPosition(windowWidth / 2 - 60, (windowHeight / 2) - 120);
        window.draw(title);

        float yOffset = (windowHeight / 2) - 50;
        auto iter = leaderboardEntry.begin();
        int count = 0;
        for (; iter != leaderboardEntry.end(); iter++)
        {
            if (count >= 5)
            {
                return;
            }
            std::stringstream ss;
            ss << iter->first << "," << iter->second;

            sf::Text entryText(ss.str(), font, 18);
            entryText.setStyle(sf::Text::Bold);
            entryText.setFillColor(sf::Color::White);
            entryText.setPosition(windowWidth / 2 - 60, yOffset);
            window.draw(entryText);
            count += 1;
            yOffset += 30;
        }
    }
};
