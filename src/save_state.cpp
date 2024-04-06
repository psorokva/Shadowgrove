#include <iostream>
#include <fstream>
#include <string>

#include "world_init.hpp"
#include "save_state.hpp"

void SaveState::saveInventory() {
    // Open the file in output mode
    std::ofstream outputFile(filePath);

    // Check if the file is opened successfully
    if (outputFile.is_open()) {
        // Write some content to the file
        //registry.inventory
        for (Item item : registry.inventory.components[0].holding) {
            outputFile << static_cast<int>(item) << std::endl;
        }

        for (const UIComp& ui : registry.uicomps.components)
        {
            outputFile << ui.potion_type;
        }

        // Close the file
        outputFile.close();

        std::cout << "File written successfully at path: " << filePath << std::endl;
    }
    else {
        std::cerr << "Unable to open the file for writing!" << std::endl;
    }
}

void SaveState::loadInventory(RenderSystem* renderer) {
    // Open the file in input mode
    std::ifstream inputFile(filePath);

    // Check if the file is opened successfully
    if (inputFile.is_open()) {
        int itemValue;
        Inventory& inventory = registry.inventory.components[0];

        // Read and convert each integer value to enum
        for (int i = 0; i < inventory.holding.size(); i++)
        {
            inputFile >> itemValue;
            Item item = static_cast<Item>(itemValue);
            inventory.holding[i] = item;
        }

        // TYPE: 0 = GREEN , PURPLE = 1,  ORANGE = default 
        int potionType;
        while (inputFile >> potionType) 
        {
            std::cout << "potionType: " << potionType << std::endl;
            if (potionType == 2)
            {
                vec2 orange_potion_pos = { 850, 660 };
                Entity orange = createConversationPotion(renderer, orange_potion_pos, 2);
            }
            else if (potionType == 0) 
            {
                vec2 green_potion_pos = { 900, 660 };
                Entity green = createConversationPotion(renderer, green_potion_pos, 0);
            }
            else if (potionType == 1) 
            {
                vec2 purple_potion_pos = { 950, 660 };
                Entity purple = createConversationPotion(renderer, purple_potion_pos, 1);
            }
        }

        // Close the file
        inputFile.close();

        std::cout << "Enum values read successfully from the file at path: " << filePath << std::endl;
    }
    else {
        std::cerr << "Unable to open the file for reading!" << std::endl;
    }
}

void SaveState::clearSave() {
    // Open the file in output mode
    std::ofstream outputFile(filePath, std::ofstream::out | std::ofstream::trunc);

    // Check if the file is opened successfully
    if (outputFile.is_open()) {
        int inventory_size = registry.inventory.components[0].holding.size();

        for (int i = 0; i < inventory_size; i++)
        {
            outputFile << Item::nullval << std::endl;
        }

        std::cout << "cleared file" << std::endl;
    }
    else {
        std::cerr << "Unable to open the file for writing!" << std::endl;
    }
}