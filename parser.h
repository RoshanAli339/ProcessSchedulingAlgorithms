#ifndef PARSER_INCLUDED
#define PARSER_INCLUDED

#include <bits/stdc++.h>

using namespace std;

// This file handles the parsing of the input data we are going to handle with.
// It also holds all the global variables we pare

extern string operation;
extern int last_instant, process_count;
extern vector<pair<string, int>> algorithms;
extern vector<tuple<string, int, int>> processes;
extern vector<vector<char>> timeline;
extern unordered_map<string, int> processToIndex;

// Results to be stored in the following

extern vector<int> finishTime;
extern vector<int> turnAroundTime;
extern vector<float> normTurn;

void parse_algorithms(string);
void parse_processes();
void parse();

#endif
