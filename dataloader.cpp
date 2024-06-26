#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "headers/dataloader.h"

static int nodeId = 1;

void parseSegmentLine(const std::string& line, std::vector<Node*>& graph, std::unordered_map<int, std::pair<Node*, Node*>> &sequenceMap) {
    std::stringstream ss{line};
    std::string type; // always 'S', don't need it
    int sequenceId;
    std::string sequence;

    ss >> type >> sequenceId >> sequence;
    bool first_iteration = true;
    Node* firstNode = new Node();
    for (auto it = sequence.begin(); it != sequence.end(); ++it) {
        char letter = *it;
        Node* node = new Node(nodeId, letter);
        graph.push_back(node);

        // first iteration
        if (first_iteration) {
            firstNode = node;
            first_iteration = false;
            if (sequence.size() == 1) {
                sequenceMap[sequenceId] = std::make_pair(firstNode, firstNode);
            }
        } else {
            node->addPredecessor(nodeId - 1);
            // last iteration
            if (std::next(it) == sequence.end()) {
                Node* lastNode = node;
                sequenceMap[sequenceId] = std::make_pair(firstNode, lastNode);
            }
        }
        
        nodeId++;
    }
}


void parseLinkLine(const std::string& line, std::vector<Node*>& graph, std::unordered_map<int, std::pair<Node*, Node*>> &sequenceMap) {
    std::stringstream ss{line};
    std::string type; // always 'L', don't need it
    int fromSequenceId;
    int toSequenceId;
    char fromOrient; // always + in our case
    char toOrient; // always + in our case
    int overlap; // always 0M in our case

    ss >> type >> fromSequenceId >> fromOrient >> toSequenceId >> toOrient >> overlap;
    
    Node* toNode = sequenceMap[toSequenceId].first;
    Node* fromNode = sequenceMap[fromSequenceId].second;
    toNode->addPredecessor(fromNode->getId());
}

std::vector<Node*> loadGfa(const std::string& fileName) {
    std::vector<Node*> graph = {};
    std::unordered_map<int, std::pair<Node*, Node*>> sequenceMap; // <sequenceId, pair<firstNode, lastNode>>
    
    std::ifstream file{fileName};
    std::string line;
    while (std::getline(file, line)) {
        if (line[0] == 'S') {
            parseSegmentLine(line, graph, sequenceMap);
        }
    }

    file.clear();
    file.seekg(0, std::ios::beg);
    while (std::getline(file, line)) {
        if (line[0] == 'L') {
            parseLinkLine(line, graph, sequenceMap);
        }
    }

    return graph;
}

// Function to load sequence lines from a .fastq file
std::vector<std::string> loadFastq(const std::string& fileName) {
    std::ifstream file{fileName};
    std::vector<std::string> sequences;
    std::string line;
    int lineCount = 0;

    while (std::getline(file, line)) {
        if (lineCount % 4 == 1) {
            sequences.push_back(line);
        }
        lineCount++;
    }

    return sequences;
}