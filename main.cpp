#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "graph.h"

using namespace std;

int main() {
    //initialize variables needed for query
    string monitorResolution;
    string pcSize;
    string cpuBrand;
    string cpuGeneration;
    string driveType;
    string driveSpace;
    string gpuRAM;
    string resolution;
    string cpuSpeed;
    string cpuCores;
    string memoryRAM;


    // initialize user input variable
    string option;

    //terminal-based "start" button
    cout << "Press Enter to Begin";
    cin.ignore();

    // questions prompted towards user to fill appropriate data for the query variables

    cout << "What is the resolution of your monitor?" << endl <<
         "[1] 1080p (1920 x 1080)" << endl <<
         "[2] 1440p (2560 x 1440)" << endl <<
         "[3] 4K (3840 x 2160)" << endl;

    do {
        cout << "Please choose a valid option" << endl;
        cin >> option;
    } while (!cin.fail() && option != "1" && option != "2" && option != "3");

    switch (stoi(option)) {
        case 1:
            monitorResolution = "1080p";
            resolution = "1080p";
            break;
        case 2:
            monitorResolution = "1440p";
            resolution = "1440p";

            break;
        case 3:
            monitorResolution = "4K";
            resolution = "4K";

            break;
    }

    cout << "What type of games will you play?" << endl <<
         "[1] Indie Games (Minecraft, Terraria, etc.)" << endl <<
         "[2] Esports (CS:GO, Valorant, etc)" << endl <<
         "[3] AAA (CyberPunk, Elden Rings, etc.)" << endl <<
         "[4] None, just basic office work." << endl;

    do {
        cout << "Please choose a valid option" << endl;
        cin >> option;
    } while (!cin.fail() && option != "1" && option != "2" && option != "3" && option != "4");

    switch (stoi(option)) {
        case 1:
            gpuRAM = "4";
            cpuSpeed = "2.5";
            cpuCores = "4";
            cpuGeneration = "current";
            driveType = "SATA";
            driveSpace = "500";
            memoryRAM = "16";
            break;
        case 2:
            gpuRAM = "8";
            cpuSpeed = "3.0";
            cpuCores = "6";
            cpuGeneration = "current";
            driveType = "SATA";
            driveSpace = "1000";
            memoryRAM = "16";
            break;
        case 3:
            gpuRAM = "16";
            cpuSpeed = "3.5";
            cpuCores = "10";
            cpuGeneration = "latest";
            driveType = "NVME";
            driveSpace = "2000";
            memoryRAM = "32";
            break;
        case 4:
            gpuRAM = "2";
            resolution = "";
            cpuSpeed = "2";
            cpuCores = "2";
            cpuGeneration = "past";
            driveType = "HDD";
            driveSpace = "1000";
            memoryRAM = "8";
            break;
    }

    if (stoi(option) != 4) {
        //Generates initial GPU recommendation (i.e. AAA game @ low settings = lower gpuResolution, basically resolution attribute correlates to GPU capability)
        cout << "What settings do you wish to play these games at?" << endl <<
             "[1] Low" << endl <<
             "[2] Medium" << endl <<
             "[3] High" << endl;

        do {
            cout << "Please choose a valid option" << endl;
            cin >> option;
        } while (!cin.fail() && option != "1" && option != "2" && option != "3");

        if (monitorResolution == "1080p") {
            // Indie games can still run on a "1080p" monitor regardless of setting

            //Esports title on 1080p at medium/high setting, upgrade GPU base power
            if (gpuRAM == "8") {
                switch (stoi(option)) {
                    case 1: // low settings
                        resolution = "1080p";
                        break;
                    case 2: // Medium settings
                        resolution = "1080p";
                        break;
                    case 3: // High settings
                        resolution = "1080p/1440p";
                        break;
                }
            }

                //if AAA game on 1080p monitor is being played at medium/high settings, upgrade GPU base power
            else if (gpuRAM == "16") { // For AAA games
                switch (stoi(option)) {
                    case 2: // Medium settings
                        resolution = "1080p/1440p";
                        break;
                    case 3: // High settings
                        resolution = "1440p/4K";
                        break;
                }
            }
        } else if (monitorResolution == "1440p") {
            // if Indie game on 1440p Monitor at low/medium settings, then downgrade GPU resolution
            if (gpuRAM == "4") {
                switch (stoi(option)) {
                    case 1: // low settings
                        resolution = "1080p";
                        break;
                    case 2: // medium settings
                        resolution = "1080p";
                        break;
                    case 3: // high settings
                        resolution = "1080p/1440p";
                        break;
                }
            } else if (gpuRAM ==
                       "8") { // if Esports game on 1440p Monitor at low/medium settings, then downgrade GPU resolution
                switch (stoi(option)) {
                    case 1: // low settings
                        resolution = "1080p/1440p";
                        break;
                    case 2: // medium settings
                        resolution = "1080p/1440p";
                        break;
                    case 3: // high settings
                        resolution = "1440p";
                        break;
                }
            } else if (gpuRAM == "16") { // AAA titles played on 1440p monitor
                switch (stoi(option)) {
                    case 1: // low settings
                        gpuRAM = "8";
                        resolution = "1080p/1440p";
                        break;
                    case 2: // medium settings
                        gpuRAM = "8";
                        resolution = "1440p";
                        break;
                    case 3: // High settings
                        resolution = "4K";
                        break;
                }
            }
        } else if (monitorResolution == "4K") {
            // if Indie game on 4K Monitor at low/medium settings, then downgrade GPU resolution
            if (gpuRAM == "4") {
                switch (stoi(option)) {
                    case 1: // low settings
                        gpuRAM = "8";
                        resolution = "1080p";
                        break;
                    case 2: // medium settings
                        gpuRAM = "8";
                        resolution = "1080p";
                        break;
                    case 3: // high settings
                        gpuRAM = "8";
                        resolution = "1440p";
                        break;
                }
            } else if (gpuRAM == "8") { // if Esports game on 4K Monitor at low/medium settings
                switch (stoi(option)) {
                    case 1: // low settings
                        resolution = "1080p/1440p";
                        break;
                    case 2: // medium settings
                        resolution = "1440p";
                        break;
                    case 3: // high settings
                        resolution = "1440p/4K";
                        break;
                }
            } else if (gpuRAM == "16") { // AAA titles played on 4K monitor
                switch (stoi(option)) {
                    case 1: // low settings
                        gpuRAM = "8";
                        resolution = "1080p/1440p";
                        break;
                    case 2: // medium settings
                        gpuRAM = "8";
                        resolution = "1440p/4K";
                        break;
                    case 3: // High settings
                        resolution = "4K";
                        break;
                }
            }
        }
    }

    //pcSize gets updated to iterate through appropriate case, motherboard, and cooler parts
    cout << "What is your preferred PC Size?" << endl <<
         "[1] Small" << endl <<
         "[2] Medium" << endl <<
         "[3] Large" << endl <<
         "[4] No preference" << endl;

    do {
        cout << "Please choose a valid option" << endl;
        cin >> option;
    } while (!cin.fail() && option != "1" && option != "2" && option != "3" && option != "4");

    switch (stoi(option)) {
        case 1:
            pcSize = ITX;
            break;
        case 2:
            pcSize = MICRO_ATX;
            break;
        case 3:
            pcSize = ATX;
            break;
    }

    cout << "What is your preferred CPU brand?" << endl <<
         "[1] Intel" << endl <<
         "[2] AMD" << endl <<
         "[3] No preference" << endl;

    do {
        cout << "Please choose a valid option" << endl;
        cin >> option;
    }
    while (!cin.fail() && option != "1" && option != "2" && option != "3");

    switch (stoi(option)) {
        case 1:
            cpuBrand = "Intel";
            break;
        case 2:
            cpuBrand = "AMD";
            break;
        default:
            cpuBrand = "";
            break;
    }

    cout << "\n";
    cout << "Loading Program ..." << "\n" << endl;

    // Initialize graph structure - this gets handled in QT front end
    Graph newGraph;

    // I initialize the variables in the newGraph. This I will do in QT as well.
    newGraph.initializeQuery(pcSize,
                             gpuRAM,
                             resolution,
                             cpuBrand,
                             cpuSpeed,
                             cpuCores,
                             cpuGeneration,
                             driveType,
                             driveSpace,
                             memoryRAM);

    // debugging to see what variables were initialized
    newGraph.printQuery();

    //this is loading data from the CSV files. Most likely why QT crashed whne I clicked "generatePC" button
    newGraph.loadData("data/Case.csv");
    newGraph.loadData("data/Cooler.csv");
    newGraph.loadData("data/CPU.csv");
    newGraph.loadData("data/GPU.csv");
    newGraph.loadData("data/Memory.csv");
    newGraph.loadData( "data/Motherboard.csv");
    newGraph.loadData("data/PSU.csv");
    newGraph.loadData("data/Storage.csv");

    //constructs graph, this takes the longest time.
    newGraph.constructGraph();

    // We have started implementing this inside generatePC button clicked ()
    unordered_map<string, PCPart> currentBuild;

    currentBuild = newGraph.generateRandomBuild(newGraph.traverseBFS());

    for (auto& part : currentBuild) {
        cout << part.first << " : " << part.second.name << endl;
    }

    return 0;
}