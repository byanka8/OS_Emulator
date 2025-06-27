// Console design/layout : Groupwork made for week 3
// Version 1.1 of Console design/layout?
// it does what is expected when input is  "screen -s test" the screen test and clearing
#include <iostream>
#include <string>
#include <algorithm> // for std::transform
#include <cstdlib>
#include <cstring>
#include <ctime> //gets time
#include <map> // 
#include <thread>

// import header file
#include "utils.h"

// variables
std::map<std::string, ProcessScreen> processScreens;
std::map<std::string, ProcessScreen> runningProcesses;
std::map<std::string, ProcessScreen> finishedProcesses;
int numCpu, quantumCycles, batchFreq, minIns, maxIns, delay;
std::string schedulerType;
std::mutex processMutex;

void handleCommand(bool initialized) {

    char command[100];

    while (initialized) {
        printf("\n> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        //lower case converter
        for (int i = 0; command[i]; i++) command[i] = tolower(command[i]);
        // i forget if i typed it already lol
        if (runningProcesses.size() == 0) {
            break;
        }
        else if (strncmp(command, "screen", 6) == 0) { //this reads if there is more than just screen

            handleScreenCommand(command, processScreens, maxIns, runningProcesses, finishedProcesses, processMutex);
        }
        else if (strcmp(command, "scheduler-start") == 0) {
            printf("scheduler-test command recognized. Doing something.\n");
        }
        else if (strcmp(command, "scheduler-stop") == 0) {
            printf("scheduler-stop command recognized. Doing something.\n");
        }
        else if (strcmp(command, "report-util") == 0) {
            printf("report-util command recognized. Doing something.\n");
        }
        else if (strcmp(command, "clear") == 0) {
            printf("clear command recognized. Reprinting screen...\n");
            clearScreen();
            startUp();
        }
        else if (strcmp(command, "exit") == 0) {
            printf("EXIT command recognized. Terminating application.\n");
            break;
        }
        else {
            printf("Unrecognized command. Try again.\n");
        }
    }
}


int main() {

    std::srand(static_cast<unsigned>(std::time(0)));
    
    char command[100];
    bool initialized = false;

    startUp();

    while (!initialized) {
        printf("\n> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;
        for (int i = 0; command[i]; i++) command[i] = tolower(command[i]);

        if (strcmp(command, "initialize") == 0) {
            initialized = true;
            readConfig("config.txt", numCpu, schedulerType, quantumCycles, batchFreq, minIns, maxIns, delay);
            std::cout << "CPU: " << numCpu << "\n";
            std::cout << "Scheduler: " << schedulerType << "\n";
            std::cout << "Quantum: " << quantumCycles << "\n";
            std::cout << "Batch Freq: " << batchFreq << "\n";
            std::cout << "Min Ins: " << minIns << "\n";
            std::cout << "Max Ins: " << maxIns << "\n";
            std::cout << "Delay: " << delay << "\n";

            printf("INITIALIZE command recognized. Initializing System.\n");
        }
        else if (strcmp(command, "exit") == 0) {
            printf("EXIT command recognized. Terminating application.\n");
            return 0;
        }
        else if (strcmp(command, "clear") == 0) {
            printf("clear command recognized. Reprinting screen...\n");
            clearScreen();
            startUp();
        }
        else {
            printf("System not initialized. Type 'initialize' or 'exit'.\n");
        }
    }

    while (initialized) {
        printf("\n> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        //lower case converter
        for (int i = 0; command[i]; i++) command[i] = tolower(command[i]);
        // i forget if i typed it already lol
        if (strcmp(command, "initialize") == 0 && initialized == true) {
            printf("Already initialized please continue.\n");
        }
        else if (strncmp(command, "screen", 6) == 0) { //this reads if there is more than just screen

            handleScreenCommand(command, processScreens, maxIns, runningProcesses, finishedProcesses, processMutex);
        }
        else if (strcmp(command, "scheduler-start") == 0) {
            printf("scheduler-test command recognized. Doing something.\n");
            // test scheduler
            processScreens.emplace("p1", createProcess("p1", 1, 100));
            processScreens.emplace("p2", createProcess("p2", 2, 100));
            processScreens.emplace("p3", createProcess("p3", 3, 100));

            processScreens.emplace("p4", createProcess("p4", 4, 100));
            processScreens.emplace("p5", createProcess("p5", 5, 100));
            processScreens.emplace("p6", createProcess("p6", 6, 100));

            processScreens.emplace("p7", createProcess("p7", 7, 100));
            processScreens.emplace("p8", createProcess("p8", 8, 100));
            processScreens.emplace("p9", createProcess("p9", 9, 100));
        

            Scheduler scheduler(numCpu, runningProcesses, finishedProcesses, processMutex);

            /*for (const auto& process : processScreens) {
                scheduler.addProcess(process);
            }*/

            for (const auto& pair : processScreens) {
                scheduler.addProcess(pair.second);
            }

            std::thread schedulerThread;

            if (schedulerType == "fcfs") {
                schedulerThread = std::thread(&Scheduler::runSchedulerFCFS, &scheduler);
            }
            else if (schedulerType == "rr") {
                schedulerThread = std::thread(&Scheduler::runSchedulerRR, &scheduler,
                    quantumCycles);
            }
            else {
                printf("Invalid scheduler type.\n");


            }

            // Optionally, wait for the thread to finish:
            /*if (schedulerThread.joinable()) {
                schedulerThread.join();
            }*/
            std::thread handleCmdThread(handleCommand, initialized);

            schedulerThread.join();
            handleCmdThread.join();

            /*schedulerThread.join();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));*/

            /*if (schedulerType == "fcfs")
                scheduler.runSchedulerFCFS();
            else if (schedulerType == "rr")
                scheduler.runSchedulerRR(quantumCycles);
            else
                printf("Invalid scheduler type.");*/


                // end of test scheduler
        }
        else if (strcmp(command, "scheduler-stop") == 0) {
            printf("scheduler-stop command recognized. Doing something.\n");
        }
        else if (strcmp(command, "report-util") == 0) {
            printf("report-util command recognized. Doing something.\n");
        }
        else if (strcmp(command, "clear") == 0) {
            printf("clear command recognized. Reprinting screen...\n");
            clearScreen();
            startUp();
        }
        else if (strcmp(command, "exit") == 0) {
            printf("EXIT command recognized. Terminating application.\n");
            break;
        }
        else {
            printf("Unrecognized command. Try again.\n");
        }
    }

    return 0;
}