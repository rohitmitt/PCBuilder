#ifndef PROJECT3_GRAPH_H
#define PROJECT3_GRAPH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <map>
#include <memory>
#include <unordered_set>
#include <random>
#include <set>
#include <iomanip>
#include <queue>

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
    //Initialize random variables
    std::random_device rd;
    std::mt19937 rng;

    // Sorts part by type, used for hash mapping when doing compatibility checks to reduce total checks occurred
    vector<shared_ptr<PCPart>> caseCompatibleVector;
    vector<shared_ptr<PCPart>> coolerCompatibleVector;
    vector<shared_ptr<PCPart>> cpuCompatibleVector;
    vector<shared_ptr<PCPart>> gpuCompatibleVector;
    vector<shared_ptr<PCPart>> memoryCompatibleVector;
    vector<shared_ptr<PCPart>> motherboardCompatibleVector;
    vector<shared_ptr<PCPart>> psuCompatibleVector;
    vector<shared_ptr<PCPart>> storageCompatibleVector;


    // stores build from traversal
    unordered_map<string, set<shared_ptr<PCPart>>> potentialBuildPartsSet;
    unordered_map<string, vector<shared_ptr<PCPart>>> potentialBuildPartsVec;

    // stores a single, fully compatible random build
    unordered_map<string, shared_ptr<PCPart>> randomBuild;

    unordered_map<string, vector<string>> relationships = {
            {"cpu",         {"motherboard", "memory"}},
            {"memory",      {"cpu",         "motherboard"}},
            {"motherboard", {"cpu",         "memory",  "case"}},
            {"storage",     {"case"}},
            {"case",        {"motherboard", "storage", "cooler", "psu"}},
            {"cooler",      {"case"}},
            {"psu",         {"gpu",         "case"}},
            {"gpu",         {"psu"}}
    };
public:
    //Adjacency Matrix Variables
    vector<vector<bool>> adjacencyMatrix; // adjacencyMatrix[i][j] returns true if compatible parts
    unordered_map<string, int> partIndex; // generates part index map to keep track of part in adjacencyMatrix
    vector<shared_ptr<PCPart>> parts; // gives access to part object at adjacencyMatrix index

    //constructor to initialize rd random variable once
    Graph() : rng(rd()) {}

    int numEdgesAdded = 0;

    void getSize() {
        cout << "# parts" << parts.size() << endl;
        cout << "# rows: " << adjacencyMatrix.size() << endl;
        cout << "# columns: " << adjacencyMatrix.size() << endl;
        cout << "# edges added: " << numEdgesAdded << endl;
    }

    void addPotentialPart(const shared_ptr<PCPart> &part) {
        potentialBuildPartsVec[part->type].push_back(part);
        potentialBuildPartsSet[part->type].insert(part);
    }

    bool partExistsInPotentialBuild(const shared_ptr<PCPart> &part) {
        return potentialBuildPartsSet[part->type].find(part) != potentialBuildPartsSet[part->type].end();
    }

    //ACCESSOR
    unordered_map<string, vector<shared_ptr<PCPart>>> &getBuildVec() {
        return potentialBuildPartsVec;
    }

    unordered_map<string, shared_ptr<PCPart>> getRandomBuild() {
        return randomBuild;
    }

    // Stores user query to be used with remaining functions
    unordered_map<string, string> query;

    // CSV -> Part Object Parser
    void parseCSV(const string &filename);

    //Loading data from parseCSV into respective graph private members
    void loadData(const string &filename);

    void addEdge(const shared_ptr<PCPart> &part1Ptr, const shared_ptr<PCPart> &part2Ptr);

    void constructGraph();

    //Checking compatibility
    bool queryCompatible(const shared_ptr<PCPart> &partPtr);

    bool arePartsCompatible(const shared_ptr<PCPart> &part1Ptr, const shared_ptr<PCPart> &part2Ptr);

    //BFS and DFS Traversals
    void traverseBFS();

    void traverseDFS();

    //Helper functions
    void generateRandomBuild
            (const unordered_map<string, vector<shared_ptr<PCPart>>> &traversalResult);

    const shared_ptr<PCPart> &selectRandomPart(const vector<shared_ptr<PCPart>> &compatiblePartVector);

    void initializeQuery(string size,
                         string gpuRAM,
                         string resolution,
                         string brand,
                         string speed,
                         string coreCount,
                         string generation,
                         string driveType,
                         string driveSpace,
                         string memoryRAM);

    string queryPrintHelper(const string &attribute);

    void printQuery();

    bool areAllPartsCompatible(const unordered_map<string, shared_ptr<PCPart>>& build) {
        for (const auto& [partType, part] : build) {
            for (const auto& relatedType : relationships[partType]) {
                if (!arePartsCompatible(part, build.at(relatedType))) {
                    return false;
                }
            }
        }
        return true;
    }




};


#endif //PROJECT3_GRAPH_H

