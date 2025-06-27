#include "utils.h"
#include <thread>
#include <chrono>

std::mutex coreAssignMutex; // Needed for FCFS core assignment
std::map<std::string, int> processToCore; // For FCFS + legacy RR
int nextCoreId = 0; // Shared core round-robin index (for FCFS)

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

            // Only the thread whose turn it is will take the next process
            if (coreId != rrIndex % numCores) {
                continue;
            }

            current = readyQueue.front();
            readyQueue.pop();
            rrIndex++;  // Advance RR index after popping
        }

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
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            ++instructionsRun;
        }

        {
            std::lock_guard<std::mutex> lock(processMutex);
            runningProcesses.erase(current.name);
        }

        if (!current.hasFinished()) {
            std::lock_guard<std::mutex> lock(queueMutex);
            readyQueue.push(current); // rotate back into queue
        } else {
            current.endTime = getCurrentTimestamp();
            std::lock_guard<std::mutex> lock(processMutex);
            finishedProcesses[current.name] = current;
        }
    }
}
