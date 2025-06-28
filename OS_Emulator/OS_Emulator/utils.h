#pragma once
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>
#include <sstream>
#include <memory> // for shared_ptr

extern std::atomic<int> cpuTick;
extern std::atomic<bool> batch_scheduler_enabled;
extern std::atomic<bool> stop_all_threads;

// Global vector for tracking active status of each core
extern std::vector<bool> coreActiveGlobal;

// Initialization function (called once in Scheduler constructor)
void initializeCoreTracking(int numCores);

// Set core active (true = busy, false = free)
void setCoreActive(int coreId, bool active);

// Print CPU utilization report to stdout
std::string getCPUUtilization();

extern int delay;

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

    std::shared_ptr<std::ostringstream> msg = std::make_shared<std::ostringstream>();

    int getRemainingInstructions() const {
        return totalInstructions - currentInstructions;
    }

    bool hasFinished() const {
        return getRemainingInstructions() == 0;
    }

    void executeInstruction() {
        if (getRemainingInstructions() > 0) {
            print("Hello world from " + name + " !");
            currentInstructions++;
        }
    }

    void print(const std::string& text, int delay = delay) {
        if (delay > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        }
        *msg << text << "\n";
    }
};

class Scheduler {
public:
    Scheduler(int cpuCount,
        std::map<std::string, ProcessScreen>& running,
        std::map<std::string, ProcessScreen>& finished,
        std::mutex& mutex);

    void addProcess(ProcessScreen& process);
    void runSchedulerFCFS();
    void runSchedulerRR(int quantum);

    std::condition_variable queueCV;
    std::mutex queueMutex;
    std::atomic<bool> noMoreProcesses = false;

private:
    int numCores;
    int rrIndex = 0;

    std::queue<ProcessScreen> readyQueue;
    std::map<std::string, ProcessScreen>& runningProcesses;
    std::map<std::string, ProcessScreen>& finishedProcesses;
    std::mutex& processMutex;

    void coreWorkerFCFS(int coreId);
    void coreWorkerRR(int coreId, int quantum);
};

// Other utils
std::string getCurrentTimestamp();
void setProcessReport(const std::string& processReport);
std::string getProcessReport();
void saveToFile(const std::string& content);
void readConfig(const std::string& filename,
    int& numCpu,
    std::string& scheduler,
    int& quantumCycles,
    int& batchProcessFreq,
    int& minIns,
    int& maxIns,
    int& delayPerExec);
int randomBetween(int min, int max);

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
void processScreenLoop(const std::string& name, std::map<std::string, ProcessScreen>& processScreens);
std::string getProcessStatus(
    const std::map<std::string, ProcessScreen>& runningProcesses,
    const std::map<std::string, ProcessScreen>& finishedProcesses,
    const std::map<std::string, ProcessScreen>& processScreens,
    std::mutex& processMutex
);
ProcessScreen createProcess(std::string name, int id, int totalInstructions);

void generateProcess(int batchSize, Scheduler& scheduler);
void checkCPUTicks(int batchSize, Scheduler& scheduler);