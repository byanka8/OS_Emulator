#include <string>
#include <ctime>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>


std::string getCurrentTimestamp() {
    time_t now = time(0);
    char buf[80];
    struct tm tstruct;
#if defined(_WIN32)
    localtime_s(&tstruct, &now);
#else
    tstruct = *localtime(&now);
#endif
    strftime(buf, sizeof(buf), "%m/%d/%Y, %I:%M:%S %p", &tstruct);
    return std::string(buf);
}

static std::string processReport;

void setProcessReport(const std::string& report) {
    processReport = report;
}

std::string getProcessReport() {
    return processReport;
}

void saveToFile(const std::string& content) {
    std::ofstream outFile("csopesy-log.txt");
    if (outFile.is_open()) {
        outFile << content << "\n";
        outFile.close();
    }
}

void readConfig(const std::string& filename,
    int& numCpu,
    std::string& scheduler,
    int& quantumCycles,
    int& batchProcessFreq,
    int& minIns,
    int& maxIns,
    int& delayPerExec)
{
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open config file: " << filename << std::endl;
        return;
    }

    std::string line, key;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        if (!(iss >> key)) continue;

        if (key == "num-cpu") {
            iss >> numCpu;
        }
        else if (key == "scheduler") {
            std::string rawValue;
            iss >> rawValue;

            // Strip quotes manually if present
            if (!rawValue.empty() && rawValue.front() == '"' && rawValue.back() == '"') {
                scheduler = rawValue.substr(1, rawValue.length() - 2);
            }
            else {
                scheduler = rawValue;
            }
        }
        else if (key == "quantum-cycles") {
            iss >> quantumCycles;
        }
        else if (key == "batch-process-freq") {
            iss >> batchProcessFreq;
        }
        else if (key == "min-ins") {
            iss >> minIns;
        }
        else if (key == "max-ins") {
            iss >> maxIns;
        }
        else if (key == "delay-per-exec") {
            iss >> delayPerExec;
        }
    }
}