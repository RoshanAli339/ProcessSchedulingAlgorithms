#include "parser.h"

string operation;
int last_instant, process_count;
vector<pair<string, int>> algorithms;
vector<tuple<string, int, int>> processes;
vector<vector<char>> timeline;
unordered_map<string, int> processToIndex;

vector<int> finishTime;
vector<int> turnAroundTime;
vector<float> normTurn;

void parse_algorithms(string algorithm_chunk) {
    stringstream stream(algorithm_chunk);
    while (stream.good()) {
        string temp_str;
        getline(stream, temp_str, ',');
        stringstream ss(temp_str);
        if (temp_str.find("-") == string::npos) {
            algorithms.push_back(make_pair(temp_str, 1));
        } else {
            getline(ss, temp_str, '-');
            string algorithm_id = temp_str;
            getline(ss, temp_str, '-');
            int quantum = stoi(temp_str);
            algorithms.push_back(make_pair(algorithm_id, quantum));
        }
    }
}

void parse_processes() {
    string process_chunk, process_name;
    int process_arrival_time, process_service_time;
    cout << "Processes : " << endl;
    for (int i = 0; i < process_count; i++) {
        cout << "Process " << i + 1 << " : ";
        cin >> process_chunk;

        stringstream stream(process_chunk);
        string temp_str;
        getline(stream, temp_str, ',');
        process_name = temp_str;
        getline(stream, temp_str, ',');
        process_arrival_time = stoi(temp_str);
        getline(stream, temp_str, ',');
        process_service_time = stoi(temp_str);

        processes.push_back(make_tuple(process_name, process_arrival_time,
                                       process_service_time));
        processToIndex[process_name] = i;
    }
    cout << "\n\n";
}

void parse() {
    string algorithm_chunk;

    cout << "Operation : ";
    cin >> operation;
    cout << "Algorithms : ";
    cin >> algorithm_chunk;
    cout << "Last instant : ";
    cin >> last_instant;
    cout << "Process count : ";
    cin >> process_count;

    parse_algorithms(algorithm_chunk);
    parse_processes();

    finishTime.resize(process_count);
    turnAroundTime.resize(process_count);
    normTurn.resize(process_count);
    timeline.resize(last_instant);

    for (int i = 0; i < last_instant; i++) {
        for (int j = 0; j < process_count; j++) {
            timeline[i].push_back(' ');
        }
    }
}
