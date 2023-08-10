#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>
#include <cmath>
#include <unordered_set>

#include "graph.h"

// PARSER FUNCTION //
void Graph::parseCSV(const string& filename) {
    ifstream file(filename);

    if (!file) {
        cerr << "Unable to open file: " << filename << endl;
        return; // Returns void
    }

    string line;

    // Get header row
    getline(file, line);
    istringstream headerStream(line);
    vector<string> headers;
    string header;

    while (getline(headerStream, header, ',')) {
        headers.push_back(header);
    }

//    int count = 0;
    while (getline(file, line)) {
//        count++;
//        if (count >= 3)
//            break;

        istringstream ss(line);
        string field;
        auto part = std::make_shared<PCPart>();

        // Grab first four common attributes for each part
        for (int i = 0; i < 4; i++) {
            getline(ss, field, ',');
            switch (i) {
                case 0: part->type = field; break;
                case 1: part->name = field; break;
                case 2: part->image = field; break;
                case 3: part->url = field; break;
                default:
                    // You can either do nothing here
                    break;
                    // Or perhaps throw an exception or add a logging statement to report an unexpected value
                     throw std::runtime_error("Unexpected value in switch");
            }
        }

        // Parse the rest of the columns as unique attributes
        int headerIndex = 4; // Start from the fifth column (0-based index)
        while (getline(ss, field, ',')) {
            if (headerIndex < headers.size()) {
                part->attributes[headers[headerIndex]] = field;
                headerIndex++;
            }
        }

        parts.push_back(part);
        partIndex[part->name] = parts.size() - 1; // since index is 0-based and we just added the part
    }

        // Resize the adjacency matrix
        adjacencyMatrix.resize(parts.size());
        for (auto& row : adjacencyMatrix) {
            row.resize(parts.size(), false);
        }

//    cout << "Total parts added from " << filename << ": " << parts.size() << endl;

}

// DATA SETTER(s) //
void Graph::loadData(const string& filename) {
    parseCSV(filename);

    unordered_map<string, vector<shared_ptr<PCPart>>*> typeToVectorMap = {
            {"case", &caseCompatibleVector},
            {"cooler", &coolerCompatibleVector},
            {"cpu", &cpuCompatibleVector},
            {"gpu", &gpuCompatibleVector},
            {"memory", &memoryCompatibleVector},
            {"motherboard", &motherboardCompatibleVector},
            {"psu", &psuCompatibleVector},
            {"storage", &storageCompatibleVector},
    };

    for (const shared_ptr<PCPart>& part : parts) {
        if (typeToVectorMap.find(part->type) != typeToVectorMap.end()) {
            if (queryCompatible(part)) {
                typeToVectorMap[part->type]->push_back(part);
            }
        }
    }
}


void Graph::addEdge(const shared_ptr<PCPart>& part1Ptr, const shared_ptr<PCPart>& part2Ptr) {
    const PCPart& part1 = *part1Ptr;
    const PCPart& part2 = *part2Ptr;

    int index1 = partIndex[part1.name];
    int index2 = partIndex[part2.name];

    adjacencyMatrix[index1][index2] = true;
    adjacencyMatrix[index2][index1] = true;
    numEdgesAdded++;
}

void Graph::constructGraph() {
    // Define the relationship map here

    // Initialize a hash table to hold pointers to vectors of PCParts for each type
    unordered_map<string, vector<shared_ptr<PCPart>> *> partMap;
    partMap["case"] = &caseCompatibleVector;
    partMap["motherboard"] = &motherboardCompatibleVector;
    partMap["cpu"] = &cpuCompatibleVector;
    partMap["gpu"] = &gpuCompatibleVector;
    partMap["memory"] = &memoryCompatibleVector;
    partMap["storage"] = &storageCompatibleVector;
    partMap["cooler"] = &coolerCompatibleVector;
    partMap["psu"] = &psuCompatibleVector;

    std::vector<std::string> orderOfPartType = {
            "case",
            "motherboard",
            "cpu",
            "memory",
            "gpu",
            "psu",
            "cooler",
            "storage"
    };

    // Construct the graph by iterating over each partType in the ordered list
    for (const string& currentType: orderOfPartType) {
        // Get the vector of parts associated with the current type
        auto currentTypeVector = partMap[currentType];

        // Iterate over each part within associated part type Vector O(V) where V is the vertex
        for (const auto &currentPart: *currentTypeVector) {
            // Iterate over the types of parts that are connected to the current part type O(1)
            for (const auto &relatedType: relationships[currentType]) {
                // Iterate over the parts of the related type O(E) where E is the # of Edges (worst case E = V * (V-1) because no self loops)
                for (const auto &currentRelatedPart: *(partMap[relatedType])) {

                    // Skip if already visited
                    if (adjacencyMatrix[partIndex[currentPart->name]][partIndex[currentRelatedPart->name]])
                        continue;

                    // Check compatibility
                    if (arePartsCompatible(currentPart, currentRelatedPart))
                        // If everything passes, add an edge between the parts
                        addEdge(currentPart, currentRelatedPart);
                }
            }
        }
    }
}


// DATA ANALYSIS //
// ADD MODULARITY
bool Graph::queryCompatible(const shared_ptr<PCPart>& partPtr) {
    const PCPart& part = *partPtr;

    // Motherboard Part
    if (part.type == "motherboard") {
        if (!query["size"].empty()) {
            string query_size = query["size"];
            if (part.attributes.at("size") != query_size)
                return false;
        }
        if (!query["brand"].empty()) {
            string query_brand = query["brand"];
            if (part.attributes.at("brand") != query_brand)
                return false;
        }
        if (!query["generation"].empty()) {
            string query_generation = query["generation"];
            if (query_generation == "past") {
                // Only allow specific sockets for "current" generation
                if (part.attributes.at("socket") != "LGA 1151" && part.attributes.at("socket") != "LGA 1200"
                    && part.attributes.at("socket") != "AM3")
                    return false;
            } else if (query_generation == "current") {
                // Only allow specific sockets for "current" generation
                if (part.attributes.at("socket") != "LGA 1200" && part.attributes.at("socket") != "LGA 1700"
                    && part.attributes.at("socket") != "AM4")
                    return false;
            } else if (query_generation == "latest") {
                // Only allow specific sockets for "latest" generation
                if (part.attributes.at("socket") != "LGA 1700"
                    && part.attributes.at("socket") != "AM5")
                    return false;
            }
        }
    }

        // Case Part
    else if (part.type == "case") {
        if (!query["size"].empty()) {
            string query_size = query["size"];
            if (part.attributes.at("size") != query_size)
                return false;
        }
    }

        // PSU Part
    else if (part.type == "psu") {
        if (!query["size"].empty()) {
            string query_size = query["size"];
            if ((query_size == "ITX" && part.attributes.at("size") == "ATX")
                || (query_size == "MicroATX" && part.attributes.at("size") == "ATX"))
                return false;
        }
    }

        // Cooler Part
    else if (part.type == "cooler") {
        if (!query["size"].empty()) {
            string query_size = query["size"];
            if (query_size == "ITX" && part.attributes.at("type") == "liquid")
                return false;
        }
        if (!query["generation"].empty()) {
            string query_generation = query["generation"];
            string part_type = part.attributes.at("type");
            if ((query_generation == "past" && part_type == "liquid") ||
                (query_generation == "current" && part_type == "liquid") ||
                (query_generation == "latest" && part_type != "liquid"))
                return false;
        }
    }

        // Storage Part
    else if (part.type == "storage") {
        if (!query["size"].empty()) {
            string query_size = query["size"];
            if (query_size == "ITX" && part.attributes.at("type") == "HDD")
                return false;
        }
        if (!query["driveType"].empty()) {
            string query_driveType = query["driveType"];
            string part_driveType = part.attributes.at("type");
            if ((query_driveType == "HDD" && part_driveType != "HDD") ||
                (query_driveType == "SATA" && part_driveType == "HDD") ||
                (query_driveType == "NVME" && part_driveType != "NVME"))
                return false;
        }
        if (!query["space"].empty()) {
            int query_space = stoi(query["space"]);
            int part_space = stoi(part.attributes.at("space"));
            if (part_space < query_space)
                return false;
            if (part_space > query_space*2)
                return false;
        }
    }

        // GPU Part
    else if (part.type == "gpu") {
        if (!query["VRAM"].empty()) {
            string query_VRAM = query["VRAM"];
            if (stoi(part.attributes.at("VRAM")) < stoi(query_VRAM))
                return false;
            if (stoi(part.attributes.at("VRAM")) > stoi(query_VRAM)*1.5)
                return false;
        }
        if (!query["resolution"].empty()) {
            string query_resolution = query["resolution"];
            if (part.attributes.at("resolution") != query_resolution)
                return false;
        }
        if (!query["generation"].empty()) {
            string query_generation = query["generation"];
            if (query_generation == "past" && part.attributes.at("resolution") != "1080p")
                return false;
        }
    }

        // CPU Part
    else if (part.type == "cpu") {
        if (!query["brand"].empty()) {
            string query_brand = query["brand"];
            if (part.attributes.at("brand") != query_brand)
                return false;
        }
        if (!query["generation"].empty()) {
            string query_generation = query["generation"];
            if (query_generation == "past") {
                // Only allow specific sockets for "current" generation
                if (part.attributes.at("socket") != "LGA 1151" && part.attributes.at("socket") != "LGA 1200"
                    && part.attributes.at("socket") != "AM3")
                    return false;
            } else if (query_generation == "current") {
                // Only allow specific sockets for "current" generation
                if (part.attributes.at("socket") != "LGA 1200" && part.attributes.at("socket") != "LGA 1700"
                    && part.attributes.at("socket") != "AM4")
                    return false;
            } else if (query_generation == "latest") {
                // Only allow specific sockets for "latest" generation
                if (part.attributes.at("socket") != "LGA 1700"
                    && part.attributes.at("socket") != "AM5")
                    return false;
            }
        }
        if (!query["speed"].empty()) {
            string query_speed = query["speed"];
            if (stof(part.attributes.at("speed")) < stof(query_speed))
                return false;
            if (stof(part.attributes.at("speed")) > stof(query_speed) * 1.2)
                return false;
        }
        if (!query["coreCount"].empty()) {
            string query_coreCount = query["coreCount"];
            if (stoi(part.attributes.at("coreCount")) < stoi(query_coreCount))
                return false;
            if (stoi(part.attributes.at("coreCount")) > stoi(query_coreCount)*1.5)
                return false;
        }

        return true;
    }

        // Memory Part
    else if (part.type == "memory") {
        if (!query["generation"].empty()) {
            string query_generation = query["generation"];
            string part_type = part.attributes.at("type");
            if (query_generation == "past") {
                // Only allow specific sockets for "current" generation
                if (part_type != "DDR3")
                    return false;
            } else if (query_generation == "current") {
                if (part_type != "DDR4")
                    return false;
            } else if (query_generation == "latest") {
                if (part_type != "DDR5")
                    return false;
            }
        }

        if (!query["RAM"].empty()) {
            int query_memory = stoi(query["RAM"]);
            int part_memory = stoi(part.attributes.at("size"));
            if (part_memory < query_memory)
                return false;
            if (part_memory >= query_memory * 2)
                return false;
        }
    }
    return true;
}


bool Graph::arePartsCompatible(const shared_ptr<PCPart>& part1Ptr, const shared_ptr<PCPart>& part2Ptr) {
    const PCPart& part1 = *part1Ptr;
    const PCPart& part2 = *part2Ptr;

    // GPU and PSU COMPATIBILITY
    if ((part1.type == "gpu" && part2.type == "psu") || (part1.type == "psu" && part2.type == "gpu")) {
        const PCPart& gpuPart = part1.type == "gpu" ? part1 : part2;
        const PCPart& psuPart = part1.type == "psu" ? part1 : part2;

        //calculate minimum power required to run GPU
        int minPower = static_cast<int>(-536.9 + 522.65 * log10(static_cast<double>(stoi(gpuPart.attributes.at("power")))));
//        int maxPower = static_cast<int>(1.1 * minPower);  // 150% of minPower
        int psuPower = stoi(psuPart.attributes.at("power"));

        return psuPower >= minPower;
    }

    // CPU AND MOTHERBOARD
    if ((part1.type == "cpu" && part2.type == "motherboard") || (part1.type == "motherboard" && part2.type == "cpu")) {
        const PCPart& cpuPart = part1.type == "cpu" ? part1 : part2;
        const PCPart& moboPart = part1.type == "motherboard" ? part1 : part2;

        if (cpuPart.attributes.at("socket") == moboPart.attributes.at("socket")) {
            // Ensure Intel CPUs are only compatible with Intel motherboards and vice-versa for AMD
            if ((cpuPart.attributes.at("brand") == "Intel" && moboPart.attributes.at("brand") == "Intel") ||
                (cpuPart.attributes.at("brand") == "AMD" && moboPart.attributes.at("brand") == "AMD")) {
                return true;
            }
        }
        return false;
    }

    if (query["size"] == "") {
        // STORAGE AND CASE
        if ((part1.type == "storage" && part2.type == "case") || (part1.type == "case" && part2.type == "storage")) {
            const PCPart& storagePart = part1.type == "storage" ? part1 : part2;
            const PCPart& casePart = part1.type == "case" ? part1 : part2;

            if (storagePart.attributes.at("type") == "HDD" && casePart.attributes.at("size") == "ITX") {
                return false;
            }
        }

        // COOLER AND CASE
        if ((part1.type == "cooler" && part2.type == "case") || (part1.type == "case" && part2.type == "cooler")) {
            const PCPart& coolerPart = part1.type == "cooler" ? part1 : part2;
            const PCPart& casePart = part1.type == "case" ? part1 : part2;

            if (coolerPart.attributes.at("type") == "liquid" && casePart.attributes.at("size") == "ITX") {
                return false;
            }
        }

        // PSU AND CASE
        if ((part1.type == "psu" && part2.type == "case") || (part1.type == "case" && part2.type == "psu")) {
            const PCPart& psuPart = part1.type == "psu" ? part1 : part2;
            const PCPart& casePart = part1.type == "case" ? part1 : part2;

            if ((casePart.attributes.at("size") == "ITX" || casePart.attributes.at("size") == "MicroATX") && psuPart.attributes.at("size") == "ATX") {
                return false;
            }
        }
    }

    return true;
}


// TRAVERSAL ALGORITHMS //
void Graph::traverseBFS() {
    potentialBuildPartsSet.clear();
    potentialBuildPartsVec.clear();

    // select a random GPU as the source node, add it to the randomBuild mapping for final results.
    shared_ptr<PCPart> startingPart = selectRandomPart(motherboardCompatibleVector);

    // Insert starting part into both set and vector
    potentialBuildPartsSet[startingPart->type].insert(startingPart);
    potentialBuildPartsVec[startingPart->type].push_back(startingPart);

    randomBuild[startingPart->type] = startingPart;

    queue<shared_ptr<PCPart>> q;
    q.push(startingPart);

    while (!q.empty()) {
        auto currentPart = q.front();
        q.pop();

        // Visit all adjacent parts and not already in the build parts
        int index = partIndex[currentPart->name];
        for (int i = 0; i < adjacencyMatrix[index].size(); i++) {
            if (adjacencyMatrix[index][i]) {  // If there's a compatibility to the ith part
                const auto &adjacentPart = parts[i];

                // Check for existence in the set
                if (potentialBuildPartsSet[adjacentPart->type].find(adjacentPart) ==
                    potentialBuildPartsSet[adjacentPart->type].end()) {

                    potentialBuildPartsSet[adjacentPart->type].insert(adjacentPart);
                    potentialBuildPartsVec[adjacentPart->type].push_back(adjacentPart);

                    q.push(adjacentPart);
                }
            }
        }
    }
}

void Graph::traverseDFS() {
    potentialBuildPartsSet.clear();
    potentialBuildPartsVec.clear();

    // select a random GPU as the source node, add it to the randomBuild mapping for final results.
    shared_ptr<PCPart> startingPart = selectRandomPart(gpuCompatibleVector);

    // Insert starting part into both set and vector=
    potentialBuildPartsSet[startingPart->type].insert(startingPart);
    potentialBuildPartsVec[startingPart->type].push_back(startingPart);

    randomBuild[startingPart->type] = startingPart;

    stack<PCPart> s;
    s.push(*startingPart);
    unordered_set<string> visitedParts;

    while (!s.empty()) {
        PCPart currentPart = s.top();
        s.pop();

        if (visitedParts.find(currentPart.name) != visitedParts.end()) {
            continue;  // Skip if already visited
        }

        visitedParts.insert(currentPart.name);  // Mark as visited

        // Visit all adjacent parts and not already in the build parts
        int index = partIndex[currentPart.name];
        for (int i = 0; i < adjacencyMatrix[index].size(); i++) {
            if (adjacencyMatrix[index][i]) {  // If there's a compatibility to the ith part
                const auto &adjacentPart = parts[i];

                // Check for existence in the set
                if (potentialBuildPartsSet[adjacentPart->type].find(adjacentPart) ==
                    potentialBuildPartsSet[adjacentPart->type].end()) {
                    // Insert into both set and vector
                    potentialBuildPartsSet[adjacentPart->type].insert(adjacentPart);
                    potentialBuildPartsVec[adjacentPart->type].push_back(adjacentPart);
                    s.push(*adjacentPart);
                }
            }
        }
    }
}

// HELPER FUNCTIONS //
const shared_ptr<PCPart>& Graph::selectRandomPart(const vector<shared_ptr<PCPart>>& compatiblePartVector) {
    // Check if any compatible CPUs were found
    if (compatiblePartVector.empty()) {
        throw runtime_error("No compatible parts found");
    }

    // Select a random compatible GPU
    std::uniform_int_distribution<int> uni(0, compatiblePartVector.size() - 1);
    int randomIndex = uni(rng);

    return compatiblePartVector[randomIndex];
}

void Graph::generateRandomBuild(const unordered_map<string, vector<shared_ptr<PCPart>>>& traversalResult) {
    randomBuild.clear();

    do {
        // For each part type, select a random part
        for (const auto& partType : traversalResult) {
            randomBuild[partType.first] = selectRandomPart(partType.second);
        }
    } while (!areAllPartsCompatible(randomBuild));  // Retry until a compatible build is found
}


void Graph::initializeQuery(string size,
                            string gpuRAM,
                            string resolution,
                            string brand,
                            string speed,
                            string coreCount,
                            string generation,
                            string driveType,
                            string driveSpace,
                            string memoryRAM) {
    // Motherboard, Case, PSU, Cooler, Storage
    query["size"] = size;

    // GPU
    query["VRAM"] = gpuRAM;
    query["resolution"] = resolution;

    // Motherboard, CPU
    query["brand"] = brand;
    query["generation"] = generation;

    // CPU
    query["speed"] = speed;
    query["coreCount"] = coreCount;

    //storage
    query["driveType"] = driveType;
    query["space"] = driveSpace;

    //memory
    query["RAM"] = memoryRAM;
}

void Graph::printQuery() {
    //format query to be accessible from an unordered map
    cout << "----List of Recommended Part Attributes----" << endl;
    cout << "Generation PC: " << queryPrintHelper("generation") << endl;
    cout << "GPU VRAM: " << queryPrintHelper("VRAM") << " GB" << endl;
    cout << "RAM Memory: " << queryPrintHelper("RAM") << " GB" << endl;
    cout << "CPU Brand: " << queryPrintHelper("brand") << endl;
    cout << "CPU Speed: " << queryPrintHelper("speed") << " Ghz" << endl;
    cout << "CPU Cores: " << queryPrintHelper("coreCount") << endl;
    cout << "Storage Type: " << queryPrintHelper("driveType") << endl;
    cout << "Storage Space: " << queryPrintHelper("space")  << " GB" << endl;
    cout << "PC Size: " << queryPrintHelper("size") << endl;
    cout << "Monitor Resolution: " << queryPrintHelper("resolution") << endl;
    cout << endl;

}

string Graph::queryPrintHelper(const string& attribute) {
    if (query[attribute].empty())
        return "N/A";
    else
        return query[attribute];
}
