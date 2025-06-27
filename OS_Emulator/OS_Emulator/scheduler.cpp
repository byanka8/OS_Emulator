#include "utils.h"
#include <thread>
#include <chrono>
#include <fstream>
#include <unordered_set>
#include <set>

Scheduler::Scheduler(int cores,
    std::map<std::string, ProcessScreen>& running,
    std::map<std::string, ProcessScreen>& finished,
    std::mutex& pm)
    : numCores(cores), runningProcesses(running), finishedProcesses(finished), processMutex(pm) {
}

void Scheduler::addProcess(const ProcessScreen& process) {
    std::lock_guard<std::mutex> lock(queueMutex);
    readyQueue.push(process);
    queueCV.notify_all();
}

void Scheduler::runSchedulerFCFS() {
    std::vector<std::thread> threads;
    for (int i = 0; i < numCores; ++i) {
        threads.emplace_back(&Scheduler::sharedWorkerFCFS, this, i);
    }
    for (auto& t : threads) {
        t.detach();
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

void Scheduler::sharedWorkerFCFS(int coreId) {
    while (true) {
        ProcessScreen current;
        bool found = false;

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!readyQueue.empty()) {
                current = readyQueue.front();
                readyQueue.pop();
                found = true;
            }
        }

        if (!found) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        current.coreAssigned = coreId;
        current.startTime = getCurrentTimestamp();
        current.timestamp = current.startTime;

        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses[current.name] = current;
        }

        while (!current.hasFinished()) {
            current.executeInstruction();
            current.timestamp = getCurrentTimestamp();

            {
                std::ofstream out(current.name + ".txt", std::ios::app);
                out << "(" << current.timestamp << ") Core:" << coreId
                    << " \"Hello world from " << current.name << "! [Line: "
                    << current.currentInstructions << "]\"\n";
            }

            {
                std::lock_guard<std::mutex> lock(processMutex);
                runningProcesses[current.name] = current;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        {
            std::lock_guard<std::mutex> lock(processMutex);
            current.endTime = getCurrentTimestamp();
            finishedProcesses[current.name] = current;
            runningProcesses.erase(current.name);
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

        {
            std::lock_guard<std::mutex> lock(processMutex);
            if (runningProcesses.find(current.name) != runningProcesses.end()) {
                current = runningProcesses[current.name];
            }
            else {
                current.startTime = getCurrentTimestamp();
                runningProcesses[current.name] = current;
            }
        }

        int instructionsRun = 0;
        while (!current.hasFinished() && instructionsRun < quantum) {
            current.coreAssigned = coreId;
            current.executeInstruction();
            current.timestamp = getCurrentTimestamp();

            {
                std::ofstream out(current.name + ".txt", std::ios::app);
                out << "(" << current.timestamp << ") Core:" << coreId
                    << " \"Hello world from " << current.name << "! [Line: "
                    << current.currentInstructions << "]\"\n";
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++instructionsRun;

            {
                std::lock_guard<std::mutex> lock(processMutex);
                runningProcesses[current.name] = current;
            }
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
