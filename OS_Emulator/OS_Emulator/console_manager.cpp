#include <stdlib.h>
#include <iostream>
#include "utils.h"
#include <mutex>
#include <map>
#include <sstream>


void printMenuTopBorder() {
    printf("%5c", 201);
    for (int i = 0; i < 90; i++) {
        printf("%c", 205);
    }
    printf("%c\n", 187);
}

void rowOne() {
    printf("%5c  ", 186);                                       //  + 2 spaces
    printf("%c", 201);                                          // C
    for (int i = 0; i < 8; i++) printf("%c", 205);              // ═ x8
    printf("%c ", 187);                                         //  + space

    printf("%c", 201);                                          //S
    for (int i = 0; i < 8; i++) printf("%c", 205);
    printf("%c ", 187);

    printf("%c", 201);                                          //O
    for (int i = 0; i < 12; i++) printf("%c", 205);
    printf("%c ", 187);

    printf("%c", 201);                                          //P
    for (int i = 0; i < 10; i++) printf("%c", 205);
    printf("%c ", 187);

    printf("%c", 201);                                          //E
    for (int i = 0; i < 9; i++) printf("%c", 205);
    printf("%c ", 187);

    printf("%c", 201);                                          //S
    for (int i = 0; i < 8; i++) printf("%c", 205);
    printf("%c ", 187);

    printf("%c", 201);                                          //Y
    for (int i = 0; i < 3; i++) printf("%c", 205);
    printf("%c", 187);

    printf("   "); // 3 spaces between small boxes

    printf("%c", 201);
    for (int i = 0; i < 3; i++) printf("%c", 205);
    printf("%c", 187);

    printf("%3c\n", 186);
}

void rowTwo() {
    printf("%5c  ", 186);                                       //Boarder

    printf("%c        %c ", 186, 186);                          //C
    printf("%c        %c ", 186, 186);
    printf("%c            %c ", 186, 186);
    printf("%c          %c ", 186, 186);
    printf("%c         %c ", 186, 186);
    printf("%c        %c", 186, 186);
    printf(" %c   %c   %c   %c", 186, 186, 186, 186);
    printf("  %c\n", 186);                                      //Boarder
}

void rowThree() {
    printf("%5c  ", 186);                                       //Boarder

    printf("%c   %c", 186, 201);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c ", 188);

    printf("%c   %c", 186, 201);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c ", 188);

    printf("%c   %c", 186, 201);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c   %c ", 187, 186);

    printf("%c   %c", 186, 201);
    for (int i = 0; i < 2; i++) printf("%c", 205);
    printf("%c   %c ", 187, 186);

    printf("%c   %c", 186, 201);
    for (int i = 0; i < 5; i++) printf("%c", 205);
    printf("%c ", 188);

    printf("%c   %c", 186, 201);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c ", 188);

    printf("%c", 200);
    printf("%c  ", 187);
    printf("%c", 200);
    printf("%c ", 187);
    printf("%c", 201);
    printf("%c  ", 188);
    printf("%c", 201);
    printf("%c  ", 188);

    printf("%c\n", 186);
}

void rowFour() {
    printf("%5c  ", 186);

    printf("%c   %c      ", 186, 186);//c

    printf("%c   %c", 186, 200);//s
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c ", 187);

    printf("%c   %c    %c   %c ", 186, 186, 186, 186);//o

    printf("%c   %c", 186, 200);//p
    for (int i = 0; i < 2; i++) printf("%c", 205);
    printf("%c   %c ", 188, 186);

    printf("%c   %c", 186, 200);
    for (int i = 0; i < 3; i++) printf("%c", 205);
    printf("%c   ", 187);

    printf("%c   %c", 186, 200);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c  ", 187);

    printf("%c", 200);
    printf("%c  ", 187);
    printf("%c", 200);
    printf("%c", 205);
    printf("%c  ", 188);
    printf("%c", 201);
    printf("%c   ", 188);

    printf("%c\n", 186);
}

void rowFive() {
    printf("%5c  ", 186);

    printf("%c   %c      %c        %c ", 186, 186, 186, 186);

    printf("%c   %c    %c   %c ", 186, 186, 186, 186);

    printf("%c          %c ", 186, 186);

    printf("%c       %c   ", 186, 186);

    printf("%c        %c   ", 186, 186);

    printf("%c", 200);
    printf("%c     ", 187);
    printf("%c", 201);
    printf("%c    ", 188);

    printf("%c\n", 186);
}

void rowSix() {
    printf("%5c  ", 186);

    printf("%c   %c      %c", 186, 186, 200);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c   %c ", 187, 186);

    printf("%c   %c    %c   %c ", 186, 186, 186, 186);

    printf("%c    %c", 186, 201);
    for (int i = 0; i < 5; i++) printf("%c", 205);
    printf("%c ", 188);

    printf("%c   %c", 186, 201);
    for (int i = 0; i < 3; i++) printf("%c", 205);
    printf("%c   ", 188);

    printf("%c", 200);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c   %c    ", 187, 186);

    printf("%c", 200);
    printf("%c   ", 187);
    printf("%c", 201);
    printf("%c     ", 188);

    printf("%c\n", 186);
}

void rowSeven() {
    printf("%5c  ", 186);                                       //Boarder

    printf("%c   %c", 186, 200);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c ", 187);

    printf("%c", 201);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c   %c ", 188, 186);

    printf("%c   %c", 186, 200);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c   %c", 188, 186);

    printf(" %c    %c       %c ", 186, 186, 186);

    printf("  %c", 200);
    for (int i = 0; i < 5; i++) printf("%c", 205);
    printf("%c ", 187);

    printf("%c", 201);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c   %c ", 188, 186);

    printf("    %c   %c      %c\n", 186, 186, 186);
}

void rowEight() {
    printf("%5c  ", 186);

    printf("%c        %c ", 186, 186);
    printf("%c        %c ", 186, 186);
    printf("%c            %c ", 186, 186);
    printf("%c    %c       %c ", 186, 186, 186);
    printf("        %c ", 186);
    printf("%c        %c ", 186, 186);
    printf("    %c   %c      %c\n", 186, 186, 186);
}

void rowNine() {
    printf("%5c  ", 186);

    printf("%c", 200);
    for (int i = 0; i < 8; i++) printf("%c", 205);
    printf("%c ", 188);

    printf("%c", 200);
    for (int i = 0; i < 8; i++) printf("%c", 205);
    printf("%c ", 188);

    printf("%c", 200);
    for (int i = 0; i < 12; i++) printf("%c", 205);
    printf("%c ", 188);

    printf("%c", 200);
    for (int i = 0; i < 4; i++) printf("%c", 205);
    printf("%c       ", 188);

    printf("%c", 200);
    for (int i = 0; i < 9; i++) printf("%c", 205);
    printf("%c ", 188);

    printf("%c", 200);
    for (int i = 0; i < 8; i++) printf("%c", 205);
    printf("%c     ", 188);

    printf("%c", 200);
    for (int i = 0; i < 3; i++) printf("%c", 205);
    printf("%c      %c\n", 188, 186);
}

void printMenuMiddleBorder() {
    printf("%5c", 204);
    for (int i = 0; i < 90; i++) {
        printf("%c", 205);
    }
    printf("%c\n", 185);
}

void printMenuBottomBorder() {
    printf("%5c", 200);
    for (int i = 0; i < 90; i++) {
        printf("%c", 205);
    }
    printf("%c\n", 188);
}

void printCenteredGreeting(const char* message) {
    int totalWidth = 94;
    int messageLength = strlen(message);
    //int messageLength = static_cast<int>(strlen(message));
    int innerWidth = totalWidth - 4; // exclude 2 border chars + 2 space padding

    int leftPadding = (innerWidth - messageLength) / 2;
    int rightPadding = innerWidth - messageLength - leftPadding;

    printf("    %c", 186); // starting border with 4-space indent
    for (int i = 0; i < leftPadding; i++) printf(" ");
    printf("%s", message);
    for (int i = 0; i < rightPadding; i++) printf(" ");
    printf("%c\n", 186); // ending border
}

//Full opening page :D

void startUp() {
    printMenuTopBorder();
    rowOne();
    rowTwo();
    rowThree();
    rowFour();
    rowFive();
    rowSix();
    rowSeven();
    rowEight();
    rowNine();

    printMenuMiddleBorder();
    printCenteredGreeting("Welcome to CSOPESY Emulator!");
    printCenteredGreeting("Developed by: Group 3"); //Yes i actually wrote this >_>
    printCenteredGreeting("Type 'Initialize' to start");
    printCenteredGreeting("Type 'Exit' to close console");
    printMenuBottomBorder();
}

// Clear screen no longer does Startup
// Reason: With the condition that the process should clear the screen completely when requesting for <screen>,
// applying StartUp at the bottom would contradict it so it's been removed, just simply call StartUp when needing
// the menu
void clearScreen() {
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    system("clear");
#endif
}

std::string getProcessStatus(
    const std::map<std::string, ProcessScreen>& runningProcesses,
    const std::map<std::string, ProcessScreen>& finishedProcesses,
    const std::map<std::string, ProcessScreen>& processScreens,
    std::mutex& processMutex
) {
    /*std::lock_guard<std::mutex> lock(processMutex);*/

    std::ostringstream oss;
    if (runningProcesses.size() > 0 || finishedProcesses.size() > 0) {
        oss << "-------------------------------\n";
        oss << "Running processes:\n";

        for (const auto& pair : runningProcesses) {
            const std::string& processName = pair.first;
            const ProcessScreen& proc = pair.second;
            oss << processName
                << " (" << proc.startTime << ")"
                << "  Core: " << proc.coreAssigned
                << "  [ " << proc.currentInstructions << " / " << proc.totalInstructions << " ]"
                << std::endl;
        }

        oss << "\nFinished processes:\n";

        for (const auto& pair : finishedProcesses) {
            const std::string& processName = pair.first;
            const ProcessScreen& proc = pair.second;
            oss << processName
                << " (" << proc.endTime << ")"
                << "  Finished  "
                << "  [ " << proc.currentInstructions << " / " << proc.totalInstructions << " ]"
                << std::endl;
        }

        oss << "-------------------------------\n";
    }
    else {
        oss << "\nCreated processes:\n";

        for (const auto& pair : processScreens) {
            const std::string& processName = pair.first;
            const ProcessScreen& proc = pair.second;
            oss << processName
                << " (" << proc.timestamp << ")"
                << "  [ " << proc.currentInstructions << " / " << proc.totalInstructions << " ]"
                << std::endl;
        }

        oss << "-------------------------------\n";
    }
    

    return oss.str();
}

void handleScreenCommand(const char* input, std::map<std::string, ProcessScreen> &processScreens, int maxIns, std::map<std::string, ProcessScreen>& runningProcesses,
    std::map<std::string, ProcessScreen>& finishedProcesses, std::mutex& processMutex) {
    char cmd[20] = { 0 }, flag[6] = { 0 }, name[51] = { 0 };

    int stringCount = sscanf_s(input, "%s %s %s", cmd, (unsigned)_countof(cmd), flag, (unsigned)_countof(flag), name, (unsigned)_countof(name));

    if (stringCount == 3) {
        if (strcmp(flag, "-s") == 0) {
            //std::string timestamp = getCurrentTimestamp();
            ProcessScreen ps = createProcess(name, processScreens.size(), maxIns);
            processScreens[name] = ps;
            drawProcessScreen(ps);
            processScreenLoop(name);
        }
        else if (strcmp(flag, "-r") == 0) {
            if (runningProcesses.count(name)) {
                drawProcessScreen(runningProcesses[name]);
                processScreenLoop(name);
            }
            else if (finishedProcesses.count(name)) {
                drawProcessScreen(finishedProcesses[name]);
                processScreenLoop(name);
            }
            else if (processScreens.count(name)) {
                drawProcessScreen(processScreens[name]);
                processScreenLoop(name);
            }
            else {
                printf("No screen found with name '%s'.\n", name);
            }
        }
        else {
            printf("Invalid screen flag. Use '-s' or '-r'.\n");
        }
    }
    else if (stringCount == 2) { // If screen -ls is entered
        if (strcmp(flag, "-ls") == 0) {
            //printf("CPU Utilization: 100%\n");  // This will be moved to cpu_manager
            //printf("Cores used: 4\n");
            //printf("Cores available: 0\n");
            //std::ostringstream oss; //originally ossCPU
            //std::ostringstream ossProcess;

            std::string processReport = getCPUUtilization() + getProcessStatus(runningProcesses, finishedProcesses, processScreens, processMutex);

            setProcessReport(processReport);

            std::cout << processReport;
        }
    }
    else {
        printf("Usage: screen -s <name>, screen -r <name>, or screen -ls\n");
    }
}