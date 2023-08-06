#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>
#include <stack>
#include <random>

#include "graph.h"

// PARSER FUNCTION //
vector<PCPart> Graph::parseCSV(const string& filename) {
    vector<PCPart> parts;
    ifstream file(filename);

    if (!file) {
        cerr << "Unable to open file: " << filename << endl;
        return parts; // Returns an empty vector
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

    while (getline(file, line)) {
        istringstream ss(line);
        string field;
        PCPart part;

        // Grab first four common attributes for each part
        for (int i = 0; i < 4; i++) {
            getline(ss, field, ',');
            switch (i) {
                case 0: part.type = field; break;
                case 1: part.name = field; break;
                case 2: part.image = field; break;
                case 3: part.url = field; break;
            }
        }

        // Parse the rest of the columns as unique attributes
        int headerIndex = 4; // Start from the fifth column (0-based index)
        while (getline(ss, field, ',')) {
            if (headerIndex < headers.size()) {
                part.attributes[headers[headerIndex]] = field;
                headerIndex++;
            }
        }

        parts.push_back(part);
    }

    return parts;
}

// DATA SETTER(s) //
void Graph::loadData(const string& filename) {
    vector<PCPart> loadedParts = parseCSV(filename);

    for (const PCPart& part : loadedParts) {
        addPart(part);

        if (part.type == "case") {
            caseVector.push_back(part);
        }

        else if (part.type == "cooler") {
            coolerVector.push_back(part);
        }

        else if (part.type == "cpu") {
            cpuVector.push_back(part);
        }

        else if (part.type == "gpu") {
            gpuVector.push_back(part);
        }

        else if (part.type == "memory") {
            memoryVector.push_back(part);
        }

        else if (part.type == "motherboard") {
            motherboardVector.push_back(part);
        }

        else if (part.type == "psu") {
            psuVector.push_back(part);
        }

        else if (part.type == "storage") {
            storageVector.push_back(part);
        }

    }
}

void Graph::addPart(const PCPart &part) {
    // Add part to map of part indices
    partIndex[part.name] = parts.size();

    // Add part to list of parts
    parts.push_back(part);

    // Resize the adjacency matrix
    adjacencyMatrix.resize(parts.size());
    for (auto& row : adjacencyMatrix) {
        row.resize(parts.size(), false);
    }
}

void Graph::addEdge(const PCPart& part1, const PCPart& part2) {
    int index1 = partIndex[part1.name];
    int index2 = partIndex[part2.name];

    // Add edge to adjacency matrix
    adjacencyMatrix[index1][index2] = true;
    adjacencyMatrix[index2][index1] = true;
}

void Graph::constructGraph() {
//     Initialize a map of important relationships between each part
    unordered_map<string, vector<string>> relationships = {
            {"cpu",         {"motherboard", "memory"}},
            {"memory",      {"cpu", "motherboard"}},
            {"motherboard", {"cpu", "memory", "case"}},
            {"storage",     {"case"}},
            {"case",        {"motherboard", "storage", "cooler", "psu"}},
            {"cooler",      {"case"}},
            {"psu",         {"gpu", "case"}},
            {"gpu",         {"psu"}}
    };

    // Initialize a hash table to hold pointers to vectors of PCParts for each type
    unordered_map<string, vector<PCPart>*> partMap;
    partMap["cpu"] = &cpuVector;
    partMap["gpu"] = &gpuVector;
    partMap["memory"] = &memoryVector;
    partMap["motherboard"] = &motherboardVector;
    partMap["storage"] = &storageVector;
    partMap["case"] = &caseVector;
    partMap["cooler"] = &coolerVector;
    partMap["psu"] = &psuVector;

    // Construct the graph by iterating over each partType in the part map O(1)
    for (const auto& currentType: partMap) {

        // iterate over each part within associated part type Vector O(V) where V is the vertex
        for (const auto& currentPart : *currentType.second) {

            // Iterate over the types of parts that are connected to the current part type O(1)
            for (const auto& relatedType: relationships[currentType.first]) {

                // Iterate over the parts of the related type O(E) where E is the # of Edges (worst case E = V * (V-1) because no self loops)
                for (PCPart &currentRelatedPart: *(partMap[relatedType])) {

                    if (queryCompatible(currentPart) && queryCompatible(currentRelatedPart)) {
                        // Check compatibility for gpu and psu
                        if ((currentType.first == "gpu" && relatedType == "psu")
                            || (currentType.first == "psu" && relatedType == "gpu")) {
                            if (!arePartsCompatible(currentPart, currentRelatedPart)) {
                                break;
                            }
                        }
                        // Check compatibility for cpu and motherboard
                        if ((currentType.first == "cpu" && relatedType == "motherboard")
                            || (currentType.first == "motherboard" && relatedType == "cpu")) {
                            if (!arePartsCompatible(currentPart, currentRelatedPart)) {
                                break;
                            }
                        }
                        //if everything passes, add an edge between the parts
                        addEdge(currentPart, currentRelatedPart);
                    }
                }
            }
        }
    }
}


// DATA ANALYSIS //
bool Graph::queryCompatible(const PCPart& part) {

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
        }
    }

    // GPU Part
    else if (part.type == "gpu") {
        if (!query["VRAM"].empty()) {
            string query_VRAM = query["VRAM"];
            if (stoi(part.attributes.at("VRAM")) < stoi(query_VRAM))
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
        }
        if (!query["coreCount"].empty()) {
            string query_coreCount = query["coreCount"];
            if (stoi(part.attributes.at("coreCount")) < stoi(query_coreCount))
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
        }
    }
    return true;
}

bool Graph::arePartsCompatible(const PCPart& part1, const PCPart& part2) {
    if ((part1.type == "gpu" && part2.type == "psu") || (part1.type == "psu" && part2.type == "gpu")) {
        const PCPart& gpuPart = part1.type == "gpu" ? part1 : part2;
        const PCPart& psuPart = part1.type == "psu" ? part1 : part2;

        //calculate minimum power required to run GPU
        int minPower = stoi(gpuPart.attributes.at("power")) * 2 + 100;

        return stoi(psuPart.attributes.at("power")) >= minPower;
    }

    if ((part1.type == "cpu" && part2.type == "motherboard") || (part1.type == "motherboard" && part2.type == "cpu")) {
        const PCPart& cpuPart = part1.type == "cpu" ? part1 : part2;
        const PCPart& moboPart = part1.type == "motherboard" ? part1 : part2;

        if (cpuPart.attributes.at("socket") == moboPart.attributes.at("socket"))
            return true;
    }

    return false;
}


// TRAVERSAL ALGORITHMS //
unordered_map<string, vector<PCPart>> Graph::traverseBFS() {
    unordered_map<string, vector<PCPart>> build;

    vector<PCPart> compatibleGPUs;

    // Gather all compatible GPUs
    for (const PCPart& gpu : gpuVector) {
        if (queryCompatible(gpu)) {
            compatibleGPUs.push_back(gpu);
        }
    }

    // select a random GPU as the source node, add it to the randomBuild mapping for final results.
    PCPart startingPart = selectRandomPart(compatibleGPUs);
    build[startingPart.type].push_back(startingPart);
    randomBuild[startingPart.type] = startingPart;

    queue<PCPart> q;
    q.push(startingPart);
    while (!q.empty()) {
        PCPart currentPart = q.front();
        q.pop();

        // Visit all adjacent parts and not already in the build parts
        int index = partIndex[currentPart.name];
        for (int i = 0; i < adjacencyMatrix[index].size(); i++) {
            if (adjacencyMatrix[index][i]) {  // If there's an compatibility to the ith part
                const PCPart& adjacentPart = parts[i];
                bool isFound = false;
                for (auto& part : build[adjacentPart.type]) {  // And it's not already in the build
                    if (part.name == adjacentPart.name) {
                        isFound = true;
                        break;
                    }
                }
                //if compatible part is found that doesn't exist in the vector of parts (hasn't been visited)
                //add it to the vector of parts, and push the part onto the queue
                if (!isFound) {
                    build[adjacentPart.type].push_back(adjacentPart);
                    q.push(adjacentPart);
                }
            }
        }
    }

    return build;
}

unordered_map<string, vector<PCPart>> Graph::traverseDFS() {
    unordered_map<string, vector<PCPart>> build;

    vector<PCPart> compatibleGPUs;

    // Gather all compatible GPUs
    for (const PCPart& gpu : gpuVector) {
        if (queryCompatible(gpu)) {
            compatibleGPUs.push_back(gpu);
        }
    }

    // select a random GPU as the source node, add it to the randomBuild mapping for final results.
    PCPart startingPart = selectRandomPart(compatibleGPUs);
    build[startingPart.type].push_back(startingPart);
    randomBuild[startingPart.type] = startingPart;

    stack<PCPart> s;
    s.push(startingPart);
    while (!s.empty()) {
        PCPart currentPart = s.top();
        s.pop();

        // Visit all adjacent and not already in the build parts
        int index = partIndex[currentPart.name];
        for (int i = 0; i < adjacencyMatrix[index].size(); i++) {
            if (adjacencyMatrix[index][i]) {  // If there's an edge to the ith part
                const PCPart& adjacentPart = parts[i];
                bool isFound = false;
                for (auto& part : build[adjacentPart.type]) {  // And it's not already in the build
                    if (part.name == adjacentPart.name) {
                        isFound = true;
                        break;
                    }
                }
                //if compatible part is found that doesn't exist in the vector of parts (hasn't been visited)
                //add it to the vector of parts, and push the part onto the stack
                if (!isFound) {
                    build[adjacentPart.type].push_back(adjacentPart);
                    s.push(adjacentPart);
                }
            }
        }
    }

    return build;
}


// HELPER FUNCTIONS //
PCPart Graph::selectRandomPart(vector<PCPart> compatiblePartVector) {
    // Check if any compatible CPUs were found
    if (compatiblePartVector.empty()) {
        throw runtime_error("No compatible parts found");
    }

    // Select a random compatible GPU
    std::random_device rd;  // Random number generator
    std::mt19937 rng(rd());  // Mersenne Twister random number generator
    std::uniform_int_distribution<int> uni(0, compatiblePartVector.size() - 1);  // Uniform distribution between 0 and size-1

    int randomIndex = uni(rng);

    return compatiblePartVector[randomIndex];
}

unordered_map<string, PCPart> Graph::generateRandomBuild
(const unordered_map<string, vector<PCPart>>& traversalResult) {
    // For each part type, select a random part
    for (const auto& partType : traversalResult) {
        randomBuild[partType.first] = selectRandomPart(partType.second);
    }
    return randomBuild;
}

void Graph::initializeQuery(string& size,
                            string& gpuRAM,
                            string& resolution,
                            string& brand,
                            string& speed,
                            string& coreCount,
                            string& generation,
                            string& driveType,
                            string& driveSpace,
                            string& memoryRAM) {
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
