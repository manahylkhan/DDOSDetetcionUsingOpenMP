#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <omp.h>
#include <vector>
#include <ctime>

using namespace std;

int main() {
    string filename = "traffic.log";
    ifstream file(filename);
    vector<string> lines;
    string line;

    // Load all lines
    while (getline(file, line)) {
        lines.push_back(line);
    }

    unordered_map<string, int> ipCount;
    omp_lock_t lock;
    omp_init_lock(&lock);

    // Parallel processing
    #pragma omp parallel for
    for (int i = 0; i < lines.size(); i++) {
        istringstream iss(lines[i]);
        string timestamp, ip;
        iss >> timestamp >> ip;

        // Clean source IP (remove port if exists)
        size_t pos = ip.find(".");
        if (ip.find(":") != string::npos) {
            ip = ip.substr(0, ip.find(":"));
        }

        // Lock to update shared map
        omp_set_lock(&lock);
        ipCount[ip]++;
        omp_unset_lock(&lock);
    }

    omp_destroy_lock(&lock);

    // Output
    cout << "\n[+] Suspicious IPs with >100 requests:\n";
    for (auto& entry : ipCount) {
        if (entry.second > 100) {
            cout << entry.first << " -> " << entry.second << " requests\n";
        }
    }

    return 0;
}
