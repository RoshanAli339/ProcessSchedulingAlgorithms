#include "parser.h"

#define all(v) v.begin(), v.end()

const string TRACE = "trace";
const string SHOW_STATISTICS = "stats";
const vector<string> ALGORITHMS{"",     "FCS", "RR",  "SPN",  "SRT",
                                "HRRN", "FB1", "FB2", "AGING"};

/*
 * Algorithms:
 * FCS: First Come First Serve
 * RR: Round Robin (pre-emptive)
 * SPN: Shortest Process Next
 * SRT: Shortest Remaining Time (pre-emptive)
 * HRNN: Highest Response Ration Next
 * FB-1: Feedback or Priority based scheduling with time quantum 1
 * FB-2i: Feedback with time quantim 2i
 * AGING: Aging algorithm
 * */

bool sortByServiceTime(const tuple<string, int, int> &a,
                       const tuple<string, int, int> &b) {
    return (get<2>(a) < get<2>(b));
}

bool sortByArrivalTime(const tuple<string, int, int> &a,
                       const tuple<string, int, int> &b) {
    return (get<1>(a) < get<1>(b));
}

bool descendingByResponseRation(const tuple<string, double, int> &a,
                                const tuple<string, double, int> &b) {
    return get<1>(a) > get<1>(b);
}

bool sortByPriorityLevel(const tuple<int, int, int> &a,
                         const tuple<int, int, int> &b) {
    if (get<0>(a) == get<0>(b)) {
        return get<2>(a) > get<2>(b);
    }
    return get<0>(a) > get<0>(b);
}

void clearTimeline() {
    for (int i = 0; i < last_instant; i++) {
        for (int j = 0; j < process_count; j++) timeline[i][j] = ' ';
    }
}

string getProcessName(tuple<string, int, int> &p) { return get<0>(p); }

int getArrivalTime(tuple<string, int, int> &p) { return get<1>(p); }

int getServiceTime(tuple<string, int, int> &p) { return get<2>(p); }

int getPriorityLevel(tuple<string, int, int> &p) { return get<2>(p); }

double calculateReponseRation(int wait_time, int service_time) {
    return (wait_time + service_time) * 1.0 / service_time;
}

void fillInWaitTime() {
    for (int i = 0; i < process_count; i++) {
        int arrivalTime = getArrivalTime(processes[i]);
        for (int j = arrivalTime; j < finishTime[i]; j++) {
            if (timeline[j][i] != '*') timeline[j][i] = '.';
        }
    }
}

void firstComeFirstServe() {
    int time = getArrivalTime(processes[0]);
    for (int i = 0; i < process_count; i++) {
        int processIndex = i;
        int arrivalTime = getArrivalTime(processes[i]);
        int serviceTime = getServiceTime(processes[i]);

        finishTime[processIndex] = time + serviceTime;
        turnAroundTime[processIndex] = finishTime[processIndex] - arrivalTime;
        normTurn[processIndex] =
            turnAroundTime[processIndex] * 1.0 / serviceTime;

        for (int j = time; j < finishTime[processIndex]; j++) {
            timeline[j][processIndex] = '*';
        }
        for (int j = arrivalTime; j < time; j++) {
            timeline[j][processIndex] = '.';
        }
        time += serviceTime;
    }
}

void roundRobin(int originalQuantum) {
    queue<pair<int, int>> q;
    int j = 0;
    if (getArrivalTime(processes[0]) == 0) {
        q.push(make_pair(j, getServiceTime(processes[0])));
        j++;
    }

    int currentQuantum = originalQuantum;
    for (int time = 0; time < last_instant; time++) {
        if (!q.empty()) {
            int processIndex = q.front().first;
            q.front().second -= 1;
            int remainingServiceTime = q.front().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            currentQuantum--;
            timeline[time][processIndex] = '*';
            while (j < process_count &&
                   getArrivalTime(processes[j]) == time + 1) {
                q.push(make_pair(j, getServiceTime(processes[j])));
                j++;
            }
            if (currentQuantum == 0 && remainingServiceTime != 0) {
                q.pop();
                q.push(make_pair(processIndex, remainingServiceTime));
                currentQuantum = originalQuantum;
            } else if (remainingServiceTime == 0) {
                finishTime[processIndex] = time + 1;
                turnAroundTime[processIndex] =
                    finishTime[processIndex] - arrivalTime;
                normTurn[processIndex] =
                    turnAroundTime[processIndex] * 1.0 / serviceTime;
                q.pop();
                currentQuantum = originalQuantum;
            }
        }

        while (j < process_count && getArrivalTime(processes[j]) == time + 1) {
            q.push(make_pair(j, getServiceTime(processes[j])));
            j++;
        }
    }
    fillInWaitTime();
}

void shortestProcessNext() {
    priority_queue<pair<int, int>, vector<pair<int, int>>,
                   greater<pair<int, int>>>
        pq;
    int j = 0;
    for (int time = 0; time < last_instant; time++) {
        while (j < process_count && getArrivalTime(processes[j]) <= time) {
            pq.push(make_pair(getServiceTime(processes[j]), j));
            j++;
        }

        if (!pq.empty()) {
            int processIndex = pq.top().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            pq.pop();

            for (int temp = arrivalTime; temp < time; temp++) {
                timeline[temp][processIndex] = '.';
            }

            int temp = time;
            for (; temp < time + serviceTime; temp++) {
                timeline[temp][processIndex] = '*';
            }

            finishTime[processIndex] = time + serviceTime;
            turnAroundTime[processIndex] =
                finishTime[processIndex] - arrivalTime;
            normTurn[processIndex] =
                turnAroundTime[processIndex] * 1.0 / serviceTime;
            time = temp - 1;
        }
    }
}

void shortestRemainingTime() {
    priority_queue<pair<int, int>, vector<pair<int, int>>,
                   greater<pair<int, int>>>
        pq;
    int j = 0;
    for (int time = 0; time < last_instant; time++) {
        while (j < process_count && getArrivalTime(processes[j]) <= time) {
            pq.push(make_pair(getServiceTime(processes[j]), j));
            j++;
        }

        if (!pq.empty()) {
            int processIndex = pq.top().second;
            int remainingTime = pq.top().first;
            pq.pop();
            int serviceTime = getServiceTime(processes[processIndex]);
            int arrivalTime = getArrivalTime(processes[processIndex]);
            timeline[time][processIndex] = '*';

            if (remainingTime == 1) {
                // process completed
                finishTime[processIndex] = time + 1;
                turnAroundTime[processIndex] =
                    finishTime[processIndex] - arrivalTime;
                normTurn[processIndex] =
                    turnAroundTime[processIndex] * 1.0 / serviceTime;
            } else {
                pq.push(make_pair(remainingTime - 1, processIndex));
            }
        }
    }
    fillInWaitTime();
}

void highestResponseRatio() {
    // tuple is <process_name, process_response_ration, time_in_service> for
    // processes that are in the ready queue
    vector<tuple<string, double, int>> presentProcesses;
    int j = 0;
    for (int instant = 0; instant < last_instant; instant++) {
        while (j < process_count && getArrivalTime(processes[j]) <= instant) {
            presentProcesses.push_back(
                make_tuple(getProcessName(processes[j]), 1.0, 0));
            j++;
        }

        for (auto &proc : presentProcesses) {
            string processName = get<0>(proc);
            int processIndex = processToIndex[processName];
            int waitTime = instant - getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            get<1>(proc) = calculateReponseRation(waitTime, serviceTime);
        }

        sort(all(presentProcesses), descendingByResponseRation);

        if (!presentProcesses.empty()) {
            int processIndex = processToIndex[get<0>(presentProcesses[0])];
            while (instant < last_instant &&
                   get<2>(presentProcesses[0]) !=
                       getServiceTime(processes[processIndex])) {
                timeline[instant][processIndex] = '*';
                instant++;
                get<2>(presentProcesses[0])++;
            }
            instant--;
            presentProcesses.erase(presentProcesses.begin());
            finishTime[processIndex] = instant + 1;
            turnAroundTime[processIndex] =
                finishTime[processIndex] -
                getArrivalTime(processes[processIndex]);
            normTurn[processIndex] = turnAroundTime[processIndex] * 1.0 /
                                     getServiceTime(processes[processIndex]);
        }
    }
    fillInWaitTime();
}

void feedbackQ() {
    priority_queue<pair<int, int>, vector<pair<int, int>>,
                   greater<pair<int, int>>>
        pq;
    unordered_map<int, int> remainingServiceTime;
    int j = 0;

    if (getArrivalTime(processes[0]) == 0) {
        pq.push(make_pair(0, j));
        remainingServiceTime[j] = getServiceTime(processes[j]);
        j++;
    }

    for (int time = 0; time < last_instant; time++) {
        if (!pq.empty()) {
            int priorityLevel = pq.top().first;
            int processIndex = pq.top().second;

            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);

            pq.pop();

            while (j < process_count &&
                   getArrivalTime(processes[j]) == time + 1) {
                pq.push(make_pair(0, j));
                remainingServiceTime[j] = getServiceTime(processes[j]);
                j++;
            }

            remainingServiceTime[processIndex]--;
            timeline[time][processIndex] = '*';

            if (remainingServiceTime[processIndex] == 0) {
                finishTime[processIndex] = time + 1;
                turnAroundTime[processIndex] =
                    finishTime[processIndex] - arrivalTime;
                normTurn[processIndex] =
                    turnAroundTime[processIndex] * 1.0 / serviceTime;
            } else {
                if (pq.size() >= 1) {
                    pq.push(make_pair(priorityLevel + 1, processIndex));
                } else {
                    pq.push(make_pair(priorityLevel, processIndex));
                }
            }
        }
        while (j < process_count && getArrivalTime(processes[j]) == time + 1) {
            pq.push(make_pair(0, j));
            remainingServiceTime[j] = getServiceTime(processes[j]);
            j++;
        }
    }
    fillInWaitTime();
}

void feedbackQ2() {
    priority_queue<pair<int, int>, vector<pair<int, int>>,
                   greater<pair<int, int>>>
        pq;
    unordered_map<int, int> remainingServiceTime;

    int j = 0;
    if (getArrivalTime(processes[0]) == 0) {
        pq.push(make_pair(0, j));
        remainingServiceTime[j] = getServiceTime(processes[0]);
        j++;
    }

    for (int time = 0; time < last_instant; time++) {
        if (!pq.empty()) {
            int priorityLevel = pq.top().first;
            int processIndex = pq.top().second;
            pq.pop();
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);

            while (j < process_count &&
                   getArrivalTime(processes[j]) <= time + 1) {
                pq.push(make_pair(0, j));
                remainingServiceTime[j] = getServiceTime(processes[j]);
                j++;
            }

            int currentQuantum = pow(2, priorityLevel);
            int temp = time;
            while (currentQuantum && remainingServiceTime[processIndex]) {
                currentQuantum--;
                remainingServiceTime[processIndex]--;
                timeline[temp][processIndex] = '*';
                temp++;
            }

            if (remainingServiceTime[processIndex] == 0) {
                finishTime[processIndex] = temp;
                turnAroundTime[processIndex] =
                    finishTime[processIndex] - arrivalTime;
                normTurn[processIndex] =
                    turnAroundTime[processIndex] * 1.0 / serviceTime;
            } else {
                if (pq.size() >= 1) {
                    pq.push(make_pair(priorityLevel + 1, processIndex));
                } else {
                    pq.push(make_pair(priorityLevel, processIndex));
                }
            }
            time = temp - 1;
        }
        while (j < process_count && getArrivalTime(processes[j]) <= time + 1) {
            pq.push(make_pair(0, j));
            remainingServiceTime[j] = getServiceTime(processes[j]);
            j++;
        }
    }

    fillInWaitTime();
}

void aging(int originalQuantum) {
    // priorty level, process index, total waiting time
    vector<tuple<int, int, int>> v;
    int j = 0, currentProcessIndex = -1;

    for (int time = 0; time < last_instant; time++) {
        while (j < process_count && getArrivalTime(processes[j]) <= time) {
            v.push_back(make_tuple(getPriorityLevel(processes[j]), j, 0));
            j++;
        }

        for (int i = 0; i < (int)v.size(); i++) {
            if (get<1>(v[i]) == currentProcessIndex) {
                get<2>(v[i]) = 0;
                get<0>(v[i]) = getPriorityLevel(processes[currentProcessIndex]);
            } else {
                get<0>(v[i])++;
                get<2>(v[i])++;
            }
        }

        sort(all(v), sortByPriorityLevel);
        currentProcessIndex = get<1>(v[0]);
        int currentQuantum = originalQuantum;
        while (currentQuantum-- && time < last_instant) {
            timeline[time][currentProcessIndex] = '*';
            time++;
        }
        time--;
    }
}

void printAlgorithm(int algorithmIndex) {
    string algorithmId = algorithms[algorithmIndex].first;
    if (algorithmId.compare("RR") || algorithmId.compare("AGING")) {
        cout << algorithmId
             << ", Time Quantum : " << algorithms[algorithmIndex].second
             << endl;
    } else {
        cout << algorithmId << endl;
    }
}

void printProcesses() {
    cout << "Process    ";
    for (int i = 0; i < process_count; i++) {
        cout << "|  " << getProcessName(processes[i]) << "  ";
    }
    cout << "| \n";
}

void printArrivalTime() {
    cout << "Arrival    ";
    for (int i = 0; i < process_count; i++) {
        printf("|%3d  ", getArrivalTime(processes[i]));
    }
    cout << "| \n";
}

void printServiceTime() {
    cout << "Service    |";
    for (int i = 0; i < process_count; i++) {
        printf("%3d  |", getServiceTime(processes[i]));
    }
    cout << " Mean|\n";
}

void printFinishTime() {
    cout << "Finish     ";
    for (int i = 0; i < process_count; i++) {
        printf("|%3d  ", finishTime[i]);
    }
    cout << "|-----|\n";
}

void printTurnAroundTime() {
    cout << "Turnaround |";
    int sum = 0;
    for (int i = 0; i < process_count; i++) {
        printf("%3d  |", turnAroundTime[i]);
        sum += turnAroundTime[i];
    }
    double avgTurnAround = (1.0 * sum) / turnAroundTime.size();
    cout << (avgTurnAround >= 10 ? "" : " ");
    printf("%2.2f|\n", avgTurnAround);
}

void printNormTurn() {
    cout << "NormTurn   |";
    float sum = 0;
    for (int i = 0; i < process_count; i++) {
        cout << (normTurn[i] >= 10 ? "" : " ");
        printf("%2.2f|", normTurn[i]);
        sum += normTurn[i];
    }

    float avgNormTurn = (1.0 * sum) / normTurn.size();

    cout << (avgNormTurn >= 10 ? "" : " ");
    printf("%2.2f|\n", avgNormTurn);
}

void printStats(int algorithmIndex) {
    printAlgorithm(algorithmIndex);
    printProcesses();
    printArrivalTime();
    printServiceTime();
    printFinishTime();
    printTurnAroundTime();
    printNormTurn();
}

void printTimeline() {
    for (int i = 0; i <= last_instant; i++) cout << i % 10 << " ";
    cout << "\n";
    cout << "------------------------------------------------\n";
    for (int i = 0; i < process_count; i++) {
        cout << getProcessName(processes[i]) << "     |";
        for (int j = 0; j < last_instant; j++) {
            cout << timeline[j][i] << "|";
        }
        cout << " \n";
    }
    cout << "------------------------------------------------\n";
}

void execute_algorithm(string algorithmId, int quantum, string operation) {
    switch (find(all(ALGORITHMS), algorithmId) - ALGORITHMS.begin()) {
        case 1:
            if (operation == TRACE) cout << "FCFS  ";
            firstComeFirstServe();
            break;
        case 2:
            if (operation == TRACE) cout << "RR-" << quantum << "  ";
            roundRobin(quantum);
            break;
        case 3:
            if (operation == TRACE) cout << "SPN   ";
            shortestProcessNext();
            break;
        case 4:
            if (operation == TRACE) cout << "SRT   ";
            shortestRemainingTime();
            break;
        case 5:
            if (operation == TRACE) cout << "HRRN  ";
            highestResponseRatio();
            break;
        case 6:
            if (operation == TRACE) cout << "FB-1  ";
            feedbackQ();
            break;
        case 7:
            if (operation == TRACE) cout << "FB-2i ";
            feedbackQ2();
            break;
        case 8:
            if (operation == TRACE) cout << "Aging ";
            aging(quantum);
            break;
        default:
            break;
    }
}

int main(int argc, char *argv[]) {
    parse();
    for (int idx = 0; idx < (int)algorithms.size(); idx++) {
        clearTimeline();
        execute_algorithm(algorithms[idx].first, algorithms[idx].second,
                          operation);
        if (operation == TRACE)
            printTimeline();
        else if (operation == SHOW_STATISTICS)
            printStats(idx);
        cout << endl;
    }
    return 0;
}
