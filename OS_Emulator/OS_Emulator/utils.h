#pragma once
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>

// Global vector for tracking active status of each core
extern std::vector<bool> coreActiveGlobal;

// Initialization function (called once in Scheduler constructor)
void initializeCoreTracking(int numCores);

// Set core active (true = busy, false = free)
void setCoreActive(int coreId, bool active);

// Print CPU utilization report to stdout
void reportCPUUtilization();

//Struct for the Process to show up will probably add more later on to show completion of instructions
struct ProcessScreen {
    std::string name;
    int id = 0;
    int totalInstructions = 0;
    int currentInstructions = 0;
    std::string timestamp;
    std::string startTime;
    std::string endTime;
    int coreAssigned = 0;

    int getRemainingInstructions() const {
        return totalInstructions - currentInstructions;
    }

    bool hasFinished() const {
        return getRemainingInstructions() == 0;
    }

    void executeInstruction() {
        int remainingInstructions = totalInstructions - currentInstructions;
        if (remainingInstructions > 0) {
            //std::cout << "Executing instruction for Process " << id << ": " << name << "\n";
            currentInstructions++;
        }
        else {
            //std::cout << "Process " << id << ": " << name << " has already finished.\n";
        }
    }
};

class Scheduler {
public:
    Scheduler(int cores,
        std::map<std::string, ProcessScreen>& running,
        std::map<std::string, ProcessScreen>& finished,
        std::mutex& pm);

    void addProcess(const ProcessScreen& process);

    void runSchedulerFCFS();
    void runSchedulerRR(int quantum);

private:
    void coreWorkerFCFS(int coreId);
    void coreWorkerRR(int coreId, int quantum);

    int numCores;
    std::queue<ProcessScreen> readyQueue;

    std::map<std::string, ProcessScreen>& runningProcesses;
    std::map<std::string, ProcessScreen>& finishedProcesses;

    std::mutex& processMutex;

    std::mutex queueMutex;
    std::condition_variable queueCV;

    int rrIndex = 0;  // For round-robin scheduling
};


// Other utils
std::string getCurrentTimestamp();
void readConfig(const std::string& filename,
    int& numCpu,
    std::string& scheduler,
    int& quantumCycles,
    int& batchProcessFreq,
    int& minIns,
    int& maxIns,
    int& delayPerExec);

// Console start page functions
void printMenuTopBorder();

void rowOne();
void rowTwo();
void rowThree();
void rowFour();
void rowFive();
void rowSix();
void rowSeven();
void rowEight();
void rowNine();

void printMenuMiddleBorder();
void printMenuBottomBorder();
void printCenteredGreeting(const char* message);

void startUp();		//Full opening page :D

void clearScreen();
void handleScreenCommand(const char* input, std::map<std::string, ProcessScreen>& processScreens, int maxIns, std::map<std::string, ProcessScreen>& runningProcesses,
     std::map<std::string, ProcessScreen>& finishedProcesses, std::mutex& processMutex);

// Process functions

void drawProcessScreen(const ProcessScreen& ps);
void processScreenLoop(const std::string& name);
void printProcessStatus(
    const std::map<std::string, ProcessScreen>& runningProcesses,
    const std::map<std::string, ProcessScreen>& finishedProcesses,
    std::mutex& processMutex
);
ProcessScreen createProcess(std::string name, int id, int totalInstructions);