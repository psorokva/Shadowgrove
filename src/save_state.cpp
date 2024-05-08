#include <iostream>
#include <fstream>
#include <string>

//#include "world_init.hpp"
#include "world_system.hpp"
#include "save_state.hpp"


void SaveState::saveGame(GameState state) 
{
    std::ofstream outputFile(filePath);

    // Check if the file is opened successfully
    if (outputFile.is_open()) 
    {

        // --- player inventory ---
        for (Item item : state.player_inventory) {
            outputFile << static_cast<int>(item) << std::endl;
        }

        // --- crafted potions ---
        outputFile << state.orange_crafted << std::endl;
        outputFile << state.green_crafted << std::endl;
        outputFile << state.purple_crafted << std::endl;

        // --- level ---
        outputFile << state.level << std::endl;

        // Close the file
        outputFile.close();
        std::cout << "Game State saved successfully at: " << filePath << std::endl;
    }

    else 
    {
        //std::cerr << "Unable to open the file for writing!" << std::endl;
    }
}


GameState SaveState::loadGame() 
{
    GameState state;

    std::ifstream inputFile(filePath);

    // Check if the file is opened successfully
    if (inputFile.is_open()) 
    {
        int itemValue;
        // --- player inventory ---
        for (int i = 0; i < inventory_size; i++)
        {
            inputFile >> itemValue;
            Item item = static_cast<Item>(itemValue);
            state.player_inventory[i] = item;
        }

        // --- crafted potions ---
        // TYPE: 0 = GREEN , PURPLE = 1,  ORANGE = default
        bool potion_crafted;
        for (int i = 0; i < potions_size; i++)
        {
            //std::cout << "potionType: " << potionType << std::endl;
            inputFile >> potion_crafted;
            if (i == 0 && potion_crafted)
            {
                state.orange_crafted = true;
            }
            else if (i == 1 && potion_crafted)
            {
                state.green_crafted = true;
            }
            else if (i == 2 && potion_crafted)
            {
                state.purple_crafted = true;
            }
        }

        int level;
        for (int i = 0; i < levels_size; i++)
        {
            inputFile >> level;
            state.level = level;
        }

        // Close the file
        inputFile.close();
        std::cout << "Game State read successfully at: " << filePath << std::endl;
    }
    else 
    {
        //std::cerr << "Unable to open the file for reading!" << std::endl;
    }

    return state;
}


void SaveState::clearSave() 
{
    // Delete everything in file
    std::ofstream outputFile(filePath, std::ofstream::out | std::ofstream::trunc);

    GameState state;

    saveGame(state);
}