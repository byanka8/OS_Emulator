#include "utils.h"
#include <thread>
#include <chrono>
#include <sstream>

std::mutex coreAssignMutex;
std::map<std::string, int> processToCore;
int nextCoreId = 0;

Scheduler::Scheduler(int cores,
    std::map<std::string, ProcessScreen>& running,
    std::map<std::string, ProcessScreen>& finished,
    std::mutex& pm)
    : numCores(cores), runningProcesses(running), finishedProcesses(finished), processMutex(pm) {
};

void Scheduler::addProcess(const ProcessScreen& process) {
    std::lock_guard<std::mutex> lock(queueMutex);
    readyQueue.push(process);
    queueCV.notify_one();
}

void Scheduler::runSchedulerFCFS() {
    std::vector<std::thread> threads;
    for (int i = 0; i < numCores; ++i) {
        threads.emplace_back(&Scheduler::coreWorkerFCFS, this, i);
    }
    for (auto& t : threads) {
        t.join();
    }
}

void Scheduler::runSchedulerRR(int quantum) {
    std::vector<std::thread> threads;
    for (int i = 0; i < numCores; ++i) {
        threads.emplace_back(&Scheduler::coreWorkerRR, this, i, quantum);
    }
    for (auto& t : threads) {
        t.join();
    }
}

void Scheduler::coreWorkerFCFS(int coreId) {

    setCoreActive(coreId, true);

    while (true) {
        ProcessScreen current;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (readyQueue.empty()) return;
            current = readyQueue.front();
            readyQueue.pop();
        }

        {
            std::lock_guard<std::mutex> lock(coreAssignMutex);
            current.coreAssigned = nextCoreId;
            nextCoreId = (nextCoreId + 1) % numCores;
        }

        coreActiveGlobal[coreId] = true;
        current.startTime = getCurrentTimestamp();

        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses[current.name] = current;
        }

        while (!current.hasFinished()) {
            current.executeInstruction();
            {
                std::lock_guard<std::mutex> lock(processMutex);
                runningProcesses[current.name] = current;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        current.endTime = getCurrentTimestamp();

        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses.erase(current.name);
            finishedProcesses[current.name] = current;
        }

        coreActiveGlobal[coreId] = false;
    }

    setCoreActive(coreId, false);
}

void Scheduler::coreWorkerRR(int coreId, int quantum) {

    setCoreActive(coreId, true);

    while (true) {
        ProcessScreen current;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (readyQueue.empty()) return;

            if (coreId != rrIndex % numCores) {
                continue;
            }

            current = readyQueue.front();
            readyQueue.pop();
            rrIndex++;
        }

        current.coreAssigned = coreId;
        coreActiveGlobal[coreId] = true;

        {
            std::lock_guard<std::mutex> lock(processMutex);
            if (current.startTime.empty()) {
                current.startTime = getCurrentTimestamp();
            }
            runningProcesses[current.name] = current;
        }

        int instructionsRun = 0;
        while (!current.hasFinished() && instructionsRun < quantum) {
            current.executeInstruction();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++instructionsRun;
        }

        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses.erase(current.name);
        }

        if (!current.hasFinished()) {
            std::lock_guard<std::mutex> lock(queueMutex);
            readyQueue.push(current);
        }
        else {
            current.endTime = getCurrentTimestamp();
            std::lock_guard<std::mutex> lock(processMutex);
            finishedProcesses[current.name] = current;
        }

        coreActiveGlobal[coreId] = false;
    }

    setCoreActive(coreId, false);
}

std::vector<bool> coreActiveGlobal;

void initializeCoreTracking(int numCores) {
    coreActiveGlobal.assign(numCores, false); // Start with all cores idle
}

void setCoreActive(int coreId, bool active) {
    if (coreId >= 0 && coreId < static_cast<int>(coreActiveGlobal.size())) {
        coreActiveGlobal[coreId] = active;
    }
}


// original
//void reportCPUUtilization() {
//    int used = 0;
//    for (bool active : coreActiveGlobal) {
//        if (active) ++used;
//    }
//
//    int total = static_cast<int>(coreActiveGlobal.size());
//    int percent = total == 0 ? 0 : (used * 100) / total;
//    int available = total - used;
//
//    std::cout << "CPU Utilization: " << percent << "%" << std::endl;
//    std::cout << "Cores used: " << used << std::endl;
//    std::cout << "Cores available: " << available << std::endl;
//}

std::string getCPUUtilization() {
    std::ostringstream oss;

    int used = 0;
    for (bool active : coreActiveGlobal) {
        if (active) ++used;
    }

    int total = static_cast<int>(coreActiveGlobal.size());
    int percent = total == 0 ? 0 : (used * 100) / total;
    int available = total - used;

    oss << "CPU Utilization: " << percent << "%" << std::endl;
    oss << "Cores used: " << used << std::endl;
    oss << "Cores available: " << available << std::endl;

    return oss.str();
}