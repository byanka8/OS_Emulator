#pragma once
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>

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
private:
    int numCores;

    std::queue<ProcessScreen> readyQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;

    std::mutex ioMutex;
    std::mutex& processMutex;

    std::map<std::string, ProcessScreen>& runningProcesses;
    std::map<std::string, ProcessScreen>& finishedProcesses;

    std::atomic<int> rrIndex = 0;  // global round index for deterministic RR

    void coreWorkerFCFS(int coreId);
    void coreWorkerRR(int coreId, int quantum);

public:
    Scheduler(int cores,
              std::map<std::string, ProcessScreen>& running,
              std::map<std::string, ProcessScreen>& finished,
              std::mutex& pm);

    void addProcess(const ProcessScreen& process);
    void runSchedulerFCFS();
    void runSchedulerRR(int quantum);
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