#include <iostream>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <map>
#include <thread>
#include <atomic>
#include <mutex>

#include "utils.h"

// Globals
std::map<std::string, ProcessScreen> processScreens;
std::map<std::string, ProcessScreen> runningProcesses;
std::map<std::string, ProcessScreen> finishedProcesses;

int numCpu, quantumCycles, batchFreq, minIns, maxIns, delay;
std::string schedulerType;

std::mutex processMutex;
int nextProcessId = 1;

std::atomic<int> cpuTick(0);
std::atomic<bool> batch_scheduler_enabled(false);

// Threads
Scheduler* schedulerPtr = nullptr;
std::thread schedulerThread;
std::thread cpuTickThread;
std::thread handleCmdThread;

void handleCommand(bool initialized) {
    char command[100];

    while (initialized) {
        printf("\n> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = 0;

        // Lowercase for command parsing
        for (int i = 0; command[i]; i++) command[i] = tolower(command[i]);

        if (strncmp(command, "screen", 6) == 0) {
            handleScreenCommand(command, processScreens, maxIns, runningProcesses, finishedProcesses, processMutex);
        }
        else if (strcmp(command, "scheduler-start") == 0) {
            printf("scheduler-start command recognized. Starting scheduling system.\n");

            schedulerPtr = new Scheduler(numCpu, runningProcesses, finishedProcesses, processMutex);
            generateProcess(1, *schedulerPtr);  // Initial process batch
            batch_scheduler_enabled = true;

            cpuTickThread = std::thread(checkCPUTicks, 1, std::ref(*schedulerPtr));

            if (schedulerType == "fcfs") {
                schedulerThread = std::thread(&Scheduler::runSchedulerFCFS, schedulerPtr);
            }
            else if (schedulerType == "rr") {
                schedulerThread = std::thread(&Scheduler::runSchedulerRR, schedulerPtr, quantumCycles);
            }
            else {
                printf("Invalid scheduler type.\n");
            }
        }
        else if (strcmp(command, "scheduler-stop") == 0) {
            printf("scheduler-stop command recognized. Stopping batch generation only.\n");
            batch_scheduler_enabled = false;

            if (schedulerPtr) {
                schedulerPtr->noMoreProcesses = true;
                schedulerPtr->queueCV.notify_all();
            }
        }
        else if (strcmp(command, "report-util") == 0) {
            printf("report-util command recognized. Generating report.\n");
            std::string report = getProcessReport();

            if (report.empty()) {
                report = getCPUUtilization() + getProcessStatus(runningProcesses, finishedProcesses, processScreens, processMutex);
            }
            saveToFile(report);
        }
        else if (strcmp(command, "clear") == 0) {
            printf("clear command recognized. Reprinting screen...\n");
            clearScreen();
            startUp();
        }
        else if (strcmp(command, "exit") == 0) {
            printf("EXIT command recognized. Terminating application.\n");

            batch_scheduler_enabled = false;

            if (schedulerPtr) {
                schedulerPtr->noMoreProcesses = true;
                schedulerPtr->queueCV.notify_all();
            }

            break;
        }
        else {
            printf("Unrecognized command. Try again.\n");
        }
    }
}

void generateProcess(int batchSize, Scheduler& scheduler) {
    for (int i = 0; i < batchSize; ++i) {
        std::string name = "process" + std::to_string(nextProcessId);
        int totalInstructions = randomBetween(minIns, maxIns);
        ProcessScreen ps = createProcess(name, nextProcessId, totalInstructions);
        processScreens[name] = ps;
        nextProcessId++;
        scheduler.addProcess(ps);
    }
}

void checkCPUTicks(int batchSize, Scheduler& scheduler) {
    while (batch_scheduler_enabled) {
        cpuTick++;
        if (cpuTick % batchFreq == 0) {
            generateProcess(batchSize, scheduler);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
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
            std::cout << "CPU: " << numCpu << "\n"
                << "Scheduler: " << schedulerType << "\n"
                << "Quantum: " << quantumCycles << "\n"
                << "Batch Freq: " << batchFreq << "\n"
                << "Min Ins: " << minIns << "\n"
                << "Max Ins: " << maxIns << "\n"
                << "Delay: " << delay << "\n";

            initializeCoreTracking(numCpu);
            printf("INITIALIZE command recognized. Initializing System.\n");

            // Start command handler immediately after initialization
            handleCmdThread = std::thread([&]() {
                handleCommand(initialized);
                });

            break;
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

    // Wait for threads before exiting
    if (schedulerThread.joinable()) schedulerThread.join();
    if (handleCmdThread.joinable()) handleCmdThread.join();
    if (cpuTickThread.joinable()) cpuTickThread.join();

    delete schedulerPtr;

    return 0;
}
