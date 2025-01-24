#include "interpreter.cpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int readFile(const string inputFileLoc) {
    string inputFileName = inputFileLoc;
    string outputFileName = "fileOutput1.txt"; // Default output file name

    ifstream inputFile(inputFileName);

    if (!inputFile.is_open()) {
        cout << "Error: Could not open file: " << inputFileName << endl;
        return 1;
    }

    createDatabase("default_db"); // Create the default database
    readFileInput(inputFileName, outputFileName); // Read and execute commands from the input file
    exportTableToCSV("default_db", "customer", "customer.csv"); // Export the "customer" table to a CSV file

    return 0; // End of the program
}

// Function to read input commands from a file
void readFileInput(const string &inputFileName, const string &outputFileName) {
    ifstream inputFile(inputFileName);
    ofstream outputFile(outputFileName);

    if (!inputFile.is_open() || !outputFile.is_open()) return;

    string commandBuffer;
    string line;
    while (getline(inputFile, line)) {
        line = trim(line);
        if (!line.empty()) {
            commandBuffer += line;
            if (line.back() == ';') {
                outputFile << ">" << commandBuffer << endl;
                processCommand(commandBuffer, outputFile);
                commandBuffer.clear();
            }
        }
    }
    inputFile.close();
    outputFile.close();
}

