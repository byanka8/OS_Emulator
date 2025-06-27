//// FCFSScheduler.cpp : This file contains the 'main' function. Program execution begins and ends there.
#include <iostream>
#include <fstream>
#include <thread>
#include <queue>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct Process {
    int id;
    int totalPrints;
    int remainingPrints;
    chrono::system_clock::time_point startTime;
    int coreAssigned;
};

queue<Process> readyQueue;
mutex queueMutex;
condition_variable queueCV;
atomic<bool> running(true);

unordered_map<int, Process> runningProcesses; // id -> process info
unordered_set<int> finishedProcesses;
mutex procMutex;

const int NUM_CORES = 4;
const int NUM_PROCESSES = 10;
const int PRINTS_PER_PROCESS = 100;

string currentTime() {
    auto now = chrono::system_clock::now();
    time_t in_time_t = chrono::system_clock::to_time_t(now);

    struct tm timeinfo;
    localtime_s(&timeinfo, &in_time_t);

    stringstream ss;
    ss << put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p");
    return ss.str();
}

string timeToString(const chrono::system_clock::time_point& tp) {
    time_t in_time_t = chrono::system_clock::to_time_t(tp);
    struct tm timeinfo;
    localtime_s(&timeinfo, &in_time_t);
    stringstream ss;
    ss << put_time(&timeinfo, "%m/%d/%Y %I:%M:%S%p");
    return ss.str();
}

void cpuWorker(int coreId) {
    while (running) {
        Process proc;
        {
            unique_lock<mutex> lock(queueMutex);
            queueCV.wait(lock, [] { return !readyQueue.empty() || !running; });
            if (!running && readyQueue.empty()) break;

            proc = readyQueue.front();
            readyQueue.pop();
        }

        proc.startTime = chrono::system_clock::now();
        proc.coreAssigned = coreId;

        {
            lock_guard<mutex> lock(procMutex);
            runningProcesses[proc.id] = proc;
        }

        string filename = "screen_01.txt";  // Changed to match the reference
        ofstream outfile(filename, ios::app);

        for (int i = 0; i < proc.totalPrints; ++i) {
            outfile << "(" << currentTime() << ") Core:" << coreId << " \"Hello world from screen_01!\"" << endl;
            this_thread::sleep_for(chrono::milliseconds(5));
        }
        outfile.close();

        {
            lock_guard<mutex> lock(procMutex);
            runningProcesses.erase(proc.id);
            finishedProcesses.insert(proc.id);
        }
    }
}

void scheduler() {
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        Process proc = { i + 1, PRINTS_PER_PROCESS, PRINTS_PER_PROCESS };
        {
            lock_guard<mutex> lock(queueMutex);
            readyQueue.push(proc);
        }
        queueCV.notify_one();
        this_thread::sleep_for(chrono::milliseconds(50));  // Simulate staggered arrival
    }
}

void commandHandler() {
    string cmd;
    while (true) {
        cout << "> ";
        getline(cin, cmd);

        if (cmd == "screen -ls") {
            lock_guard<mutex> lock(procMutex);

            cout << "-------------------------------\n";
            cout << "Running processes:\n";
            for (unordered_map<int, Process>::iterator it = runningProcesses.begin(); it != runningProcesses.end(); ++it) {
                const Process& proc = it->second;
                auto time = chrono::system_clock::now();
                cout << "process" << setw(2) << setfill('0') << proc.id
                    << " (" << timeToString(time) << ")" // original: timeToString(proc.startTime)
                    << "  Core: " << proc.coreAssigned
                    << "  [ " << proc.completedPrints << " / " << proc.totalPrints << " ]"
                    << endl;
            }

            cout << "\nFinished processes:\n";
            /*for (unordered_set<int>::iterator it = finishedProcesses.begin(); it != finishedProcesses.end(); ++it) {
                cout << "process" << setw(2) << setfill('0') << *it
                    << " (" << it. << ")"
                    << "  Finished  "
                    << "  [ " << proc.totalPrints << " / " << proc.totalPrints << " ]"
                    << endl;

            }*/
            for (unordered_map<int, Process>::iterator it = finishedProcesses.begin(); it != finishedProcesses.end(); ++it) {
                const Process& proc = it->second; // Access the finished process
                cout << "process" << setw(2) << setfill('0') << proc.id
                    << " (" << timeToString(proc.endTime) << ")"
                    << "  Finished  "
                    << "  [ " << proc.totalPrints << " / " << proc.totalPrints << " ]"
                    << endl;
            }
            cout << "-------------------------------\n";
        }
        else if (runningProcesses.empty()) {
            running = false;
            queueCV.notify_all();
            break;
        }
        else {
            cout << "Unrecognized command. Try again.\n" << endl;
        }
    }
}

int main() {
    thread schedulerThread(scheduler);

    vector<thread> workers;
    for (int i = 0; i < NUM_CORES; ++i)
        workers.emplace_back(cpuWorker, i);

    thread cmdThread(commandHandler);

    schedulerThread.join();
    cmdThread.join();
    for (auto& worker : workers)
        worker.join();

    cout << "Emulator exited successfully." << endl;
    return 0;
}





//#include <iostream>
//#include <fstream>
//#include <string>
//#include <direct.h>  // For _mkdir() on Windows
//
//void print(std::string pNum) {
//    const char* folderName = "print";  // Relative to current directory
//
//    _mkdir(folderName);  // Creates "print" in the current directory
//
//    std::string file_name = "/myfile" + pNum + ".txt";
//
//    std::string filePath = std::string(folderName) + file_name;
//
//    std::ofstream outFile(filePath);
//
//    if (outFile.is_open()) {
//        outFile << "File saved in a folder relative to current directory.\n";
//        outFile.close();
//        //std::cout << "File saved at: " << filePath << std::endl;
//    }
//    else {
//        std::cout << "Failed to open file at: " << filePath << std::endl;
//    }
//}
//
//int main() {
//    
//    print("1");
//
//    return 0;
//}
//
