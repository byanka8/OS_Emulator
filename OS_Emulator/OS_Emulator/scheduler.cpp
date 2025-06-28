#include "utils.h"
#include <thread>
#include <chrono>
#include <sstream>

std::mutex coreAssignMutex;
std::map<std::string, int> processToCore;
int nextCoreId = 0;

Scheduler::Scheduler(int cpuCount,
    std::map<std::string, ProcessScreen>& running,
    std::map<std::string, ProcessScreen>& finished,
    std::mutex& mutex)
    : numCores(cpuCount),
    runningProcesses(running),
    finishedProcesses(finished),
    processMutex(mutex),
    noMoreProcesses(false),
    rrIndex(0) {
}

void Scheduler::addProcess(ProcessScreen& process) {
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
    while (true) {
        ProcessScreen current;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (readyQueue.empty()) {
                setCoreActive(coreId, false);
                return;
            }
            current = readyQueue.front();
            readyQueue.pop();
        }

        setCoreActive(coreId, true);

        {
            std::lock_guard<std::mutex> lock(coreAssignMutex);
            current.coreAssigned = nextCoreId;
            nextCoreId = (nextCoreId + 1) % numCores;
        }

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
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        current.endTime = getCurrentTimestamp();

        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses.erase(current.name);
            finishedProcesses[current.name] = current;
        }

        setCoreActive(coreId, false);
    }
}

void Scheduler::coreWorkerRR(int coreId, int quantum) {
    while (true) {
        ProcessScreen current;

        {
            std::unique_lock<std::mutex> lock(queueMutex);
            queueCV.wait(lock, [&] {
                return !readyQueue.empty() || noMoreProcesses.load();
                });

            if (readyQueue.empty() && noMoreProcesses.load()) {
                setCoreActive(coreId, false);
                return;
            }

            if (coreId != rrIndex % numCores) {
                lock.unlock();
                //std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            current = readyQueue.front();
            readyQueue.pop();
            rrIndex++;
        }

        setCoreActive(coreId, true);
        current.coreAssigned = coreId;

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
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            ++instructionsRun;

            {
                std::lock_guard<std::mutex> lock(processMutex);
                runningProcesses[current.name] = current;
            }
        }

        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses.erase(current.name);
        }

        if (!current.hasFinished()) {
            std::lock_guard<std::mutex> lock(queueMutex);
            readyQueue.push(current);
            queueCV.notify_one();
        }
        else {
            current.endTime = getCurrentTimestamp();
            std::lock_guard<std::mutex> lock(processMutex);
            finishedProcesses[current.name] = current;
        }

        setCoreActive(coreId, false);
    }
}

std::vector<bool> coreActiveGlobal;

void initializeCoreTracking(int numCores) {
    coreActiveGlobal.assign(numCores, false);
}

void setCoreActive(int coreId, bool active) {
    if (coreId >= 0 && coreId < static_cast<int>(coreActiveGlobal.size())) {
        coreActiveGlobal[coreId] = active;
    }
}

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
