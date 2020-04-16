//Copyright 2020 Iris Technologies, All Rights Reserved
#pragma once
#include <SFML\Graphics.hpp>
#include <Database.hpp>

namespace ProjectIris
{
    namespace VisualDebugger
    {
        __forceinline void Start() noexcept
        {
            //Soon it will have more astonishing uses, just wait and see :D
            sf::RenderWindow Window(sf::VideoMode(1200, 600), "Project Iris - Visual Debugger");
            sf::Font font;
            if(font.loadFromFile("C:\\ProjectIris\\Crimson-Bold.ttf"))
            {
                sf::Text text;
                text.setFont(font);
                text.setCharacterSize(28);
                sf::Text text2;
                text2.setFont(font);
                text2.setCharacterSize(28);
                text2.setPosition(0, 50);
                while(Window.isOpen())
                {
                    sf::Event Event;
                    while(Window.pollEvent(Event))
                        if(Event.type == sf::Event::Closed)
                            Window.close();
                    Window.clear();
                    text.setString("File: " + Database::ActualFile + ", Line:  " + to_string(Database::ActualLine) + ", Character:  " + to_string(Database::ActualCharacter));
                    if(Database::Report.Message.size() > 0)
                        text2.setString("Error report at '" + Database::Report.File + "', " + to_string(Database::Report.Line) + " : " + to_string(Database::Report.Character) + " -> " + Database::Report.Message);
                    Window.draw(text);
                    Window.draw(text2);
                    Window.display();
                }
            }
        }
    }
}
