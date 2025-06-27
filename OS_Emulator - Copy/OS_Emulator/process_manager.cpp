#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <chrono>
#include <string>

#include "utils.h"


void drawProcessScreen(const ProcessScreen& ps) {
    clearScreen();
    printf("=================================================\n");
    printf("   Process Screen: %s\n", ps.name.c_str());
    printf("   Instruction: %d / %d\n", ps.currentInstructions, ps.totalInstructions);
    printf("   Created: %s\n", ps.timestamp.c_str());
    printf("=================================================\n");
    printf("Type 'exit' to return to the main menu.\n");
}

void processScreenLoop(const std::string& name) {
    char input[100];
    while (true) {
        printf("\n(screen:%s) > ", name.c_str());
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        for (int i = 0; input[i]; i++) input[i] = tolower(input[i]);

        if (strcmp(input, "exit") == 0) {
            clearScreen();
            startUp();
            break;
        }
        else {
            printf("Unknown command. Type 'exit' to return to the main menu.\n");
        }
    }

}

ProcessScreen createProcess(std::string name, int id, int totalInstructions) {
    ProcessScreen ps;
    ps.name = name;
    ps.id = id;
    ps.currentInstructions = 0;
    ps.totalInstructions = totalInstructions;
    ps.timestamp = getCurrentTimestamp();

    return ps;

}
