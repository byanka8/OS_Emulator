#include "utils.h"
#include <thread>
#include <chrono>

Scheduler::Scheduler(int cores,
    std::map<std::string, ProcessScreen>& running,
    std::map<std::string, ProcessScreen>& finished,
    std::mutex& pm)
    : numCores(cores), runningProcesses(running), finishedProcesses(finished), processMutex(pm) {
}

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
    while (true) {
        ProcessScreen current;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (readyQueue.empty()) return;
            current = readyQueue.front();
            readyQueue.pop();
        }

        current.coreAssigned = coreId;
        current.startTime = getCurrentTimestamp();
        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses[current.name] = current;
        }

        while (!current.hasFinished()) {
            current.executeInstruction();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        current.endTime = getCurrentTimestamp();
        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses.erase(current.name);
            finishedProcesses[current.name] = current;
        }
    }
}

void Scheduler::coreWorkerRR(int coreId, int quantum) {
    while (true) {
        ProcessScreen current;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            if (readyQueue.empty()) return;
            current = readyQueue.front();
            readyQueue.pop();
        }

        current.coreAssigned = coreId;
        current.startTime = getCurrentTimestamp();
        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses[current.name] = current;
        }

        int instructionsRun = 0;
        while (!current.hasFinished() && instructionsRun < quantum) {
            current.executeInstruction();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++instructionsRun;
        }

        if (!current.hasFinished()) {
            std::lock_guard<std::mutex> lock(queueMutex);
            readyQueue.push(current);
        }
        else {
            current.endTime = getCurrentTimestamp();
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses.erase(current.name);
            finishedProcesses[current.name] = current;
        }
    }
}
