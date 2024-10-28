#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "gameWindow.h"

#pragma once
class welcomeWindow
{
public:
    void runWWindow()
    {
        std::vector<int> configValues = loadConfig("files/config.cfg");
        int colCount = configValues[0];
        int rowCount = configValues[1];
        int mineCount = configValues[2];
        int width = (configValues[0] * 32);
        int height = (configValues[1] * 32) + 100;

        sf::RenderWindow welcomeWindow(sf::VideoMode(width, height), "Minesweeper");

        sf::Font font;
        if (!font.loadFromFile("files/font.ttf"))
        {
            std::cerr << "Couldn't load font file" << std::endl;
            return;
        }

        sf::Text welcomeText("Welcome to Minesweeper!", font, 24); // Load Welcome Message
        welcomeText.setFillColor(sf::Color::White);
        welcomeText.setStyle(sf::Text::Bold | sf::Text::Underlined);
        CenterText(welcomeText, width / 2, (height / 2) - 150);

        sf::Text enterNameText("Enter your name:", font, 20); // Load Enter Name Message
        enterNameText.setFillColor(sf::Color::White);
        enterNameText.setStyle(sf::Text::Bold);
        CenterText(enterNameText, width / 2, (height / 2) - 75);

        sf::Text userInputText("", font, 20); // Load User Input Message
        userInputText.setFillColor(sf::Color::Yellow);
        userInputText.setStyle(sf::Text::Bold);
        CenterText(userInputText, width / 2, (height / 2) - 45);
        std::string userInput;

        sf::RectangleShape cursor(sf::Vector2f(2, userInputText.getCharacterSize()));
        cursor.setFillColor(sf::Color::White);
        bool showCursor = true;
        sf::Clock cursorClock;
        std::size_t cursorPosition = 0;

        while (welcomeWindow.isOpen())
        {
            sf::Event event;
            while (welcomeWindow.pollEvent(event)) // Close if x pressed
            {
                if (event.type == sf::Event::Closed)
                {
                    welcomeWindow.close();
                }

                if (event.type == sf::Event::TextEntered)
                {
                    if (event.text.unicode < 128)
                    { // Only handle ASCII characters
                        if (event.text.unicode == '\b' && cursorPosition > 0)
                        { // Handle backspace
                            userInput.erase(cursorPosition - 1, 1);
                            cursorPosition--;
                        }
                        else if (std::isalpha(static_cast<char>(event.text.unicode)) && userInput.length() < 10)
                        { // Handle alphabetical characters and limit length to 10
                            userInput.insert(cursorPosition, 1, static_cast<char>(event.text.unicode));
                            cursorPosition++;
                        }

                        // Transform the input to have the first letter uppercase and the rest lowercase
                        if (!userInput.empty())
                        {
                            userInput[0] = std::toupper(userInput[0]);
                            for (std::size_t i = 1; i < userInput.size(); ++i)
                            {
                                userInput[i] = std::tolower(userInput[i]);
                            }
                        }

                        userInputText.setString(userInput);
                        CenterText(userInputText, width / 2, (height / 2) - 45);
                    }
                }

                if (event.type == sf::Event::KeyPressed)
                {
                    if (event.key.code == sf::Keyboard::Enter)
                    {
                        if (!userInput.empty())
                        {
                            welcomeWindow.close();
                            gameWindow game(rowCount, colCount, mineCount, userInput += '*');
                            game.runGWindow();
                        }
                    }
                    else if (event.key.code == sf::Keyboard::Left && cursorPosition > 0)
                    {
                        cursorPosition--;
                    }
                    else if (event.key.code == sf::Keyboard::Right && cursorPosition < userInput.size())
                    {
                        cursorPosition++;
                    }
                }
            }

            // Update cursor position
            if (cursorClock.getElapsedTime().asSeconds() > 0.5f)
            {
                showCursor = !showCursor;
                cursorClock.restart();
            }
            cursor.setPosition(userInputText.findCharacterPos(cursorPosition).x, userInputText.getPosition().y - 10);

            welcomeWindow.clear(sf::Color::Blue);
            welcomeWindow.draw(welcomeText);
            welcomeWindow.draw(enterNameText);
            welcomeWindow.draw(userInputText);
            if (showCursor)
            {
                welcomeWindow.draw(cursor);
            }
            welcomeWindow.display();
        }
    }

    void CenterText(sf::Text &text, float width, float height)
    {
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(width, height));
    }

    std::vector<int> loadConfig(const std::string &filename)
    {
        std::vector<int> configValues;
        std::ifstream configFile(filename);

        if (!configFile)
        {
            std::cerr << "Couldn't open config file: " << filename << std::endl;
            return configValues;
        }

        std::string line;
        while (std::getline(configFile, line))
        {
            if (!line.empty())
            {
                try
                {
                    int value = std::stoi(line);
                    configValues.push_back(value);
                }
                catch (const std::invalid_argument &e)
                {
                    std::cerr << "Invalid value in config file: " << line << std::endl;
                }
                catch (const std::out_of_range &e)
                {
                    std::cerr << "Value out of range in config file: " << line << std::endl;
                }
            }
        }
        return configValues;
    }
};