#include "save_state.hpp"
#include <iostream>
#include <fstream>
#include <string>

SaveState::SaveState() {
    loaded = 0;
}

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

        // Close the file
        outputFile.close();

        std::cout << "File written successfully at path: " << filePath << std::endl;
    }
    else {
        std::cerr << "Unable to open the file for writing!" << std::endl;
    }
}

void SaveState::loadInventory() {
    if (loaded >= 2) {
        return;
    }
    // Open the file in input mode
    std::ifstream inputFile(filePath);

    // Check if the file is opened successfully
    if (inputFile.is_open()) {
        int itemValue;
        Inventory& inventory = registry.inventory.components[0];
        int i = 0;
        // Read and convert each integer value to enum
        while (inputFile >> itemValue) {
            // Cast the integer to enum and output its value
            if (i >= inventory.holding.size()) {
                break;
            }
            Item item = static_cast<Item>(itemValue);
            inventory.holding[i] = item;
            i++;
        }

        // Close the file
        inputFile.close();

        std::cout << "Enum values read successfully from the file at path: " << filePath << std::endl;
    }
    else {
        std::cerr << "Unable to open the file for reading!" << std::endl;
    }

    loaded++;

}