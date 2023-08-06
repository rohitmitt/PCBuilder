#ifndef PROJECT3_GRAPH_H
#define PROJECT3_GRAPH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
using namespace std;

#include "constants.h"

/* Possible Attributes Based on Part Type
 * These will exist as PCPartObject.attributes["attributeName"]

 * Part Type : Attribute_Type 1 (list of potential attributes), Attribute_Type 2, ...

 * Case : size (ATX, MicroATX, ITX)
 * Cooler : type (liquid, air)
 * CPU : brand (intel, amd), socket (LGA 1151, LGA 1200, LGA 1700, AM3, AM4, AM5, X399, TRX40), speed, coreCount, threadCount, power
 * GPU : brand (NVIDIA, AMD), VRAM, resolution (1080p, 1440p, 2160p), power
 * Memory : type (DDR3, DDR4, DDR5), size
 * Motherboard : brand (Intel, AMD), socket (LGA 1151, LGA 1200, LGA 1700, AM3, AM4, AM5, X399, TRX40), size (ATX, MicroATX, ITX)
 * PSU : power, size (SFX, ATX)
 * Storage : driveType (NVME, SATA, HDD), space
 */

struct PCPart {
    PCPart() = default;
    string type;
    string name;
    string image;
    string url;
    unordered_map<string, string> attributes;
};

class Graph {
private:
    //Adjacency Matrix Variables
    vector<vector<bool>> adjacencyMatrix; // adjacencyMatrix[i][j] returns true if compatible parts
    unordered_map<string, int> partIndex; // generates part index map to keep track of part in adjacencyMatrix
    vector<PCPart> parts; // gives access to part object at adjacencyMatrix index

    // Sorts part by type, used for hash mapping when doing compatibility checks to reduce total checks occurred
    vector<PCPart> caseVector;
    vector<PCPart> coolerVector;
    vector<PCPart> cpuVector;
    vector<PCPart> gpuVector;
    vector<PCPart> memoryVector;
    vector<PCPart> motherboardVector;
    vector<PCPart> psuVector;
    vector<PCPart> storageVector;

    // stores a single, fully compatible random build
    unordered_map<string, PCPart> randomBuild;
public:
    // Stores user query to be used with remaining functions
    unordered_map<string, string> query;

    // CSV -> Part Object Parser
    vector<PCPart> parseCSV(const string& filename);

    //Loading data from parseCSV into respective graph private members
    void loadData(const string& filename);

    void addPart(const PCPart& part);

    void addEdge(const PCPart& part1, const PCPart& part2);

    void constructGraph();

    //Checking compatibility
    bool queryCompatible(const PCPart& part);

    bool arePartsCompatible(const PCPart& part1, const PCPart& part2);

    //BFS and DFS Traversals
    unordered_map<string, vector<PCPart>> traverseBFS();

    unordered_map<string, vector<PCPart>> traverseDFS();

    //Helper functions
    unordered_map<string, PCPart> generateRandomBuild
    (const unordered_map<string, vector<PCPart>>& traversalResult);

    PCPart selectRandomPart(vector<PCPart> compatiblePartVector);

    void initializeQuery(string& size,
                         string& gpuRAM,
                         string& resolution,
                         string& brand,
                         string& speed,
                         string& coreCount,
                         string& generation,
                         string& driveType,
                         string& driveSpace,
                         string& memoryRAM);

    string queryPrintHelper(const string& attribute);

    void printQuery();
};

#endif //PROJECT3_GRAPH_H
