#include <iostream> // For input and output
#include <fstream>  // For file handling
#include <sstream>  // For string stream operations
#include <vector>   // For using dynamic arrays
#include <unordered_map> // For mapping databases and tables
#include <string>   // For string manipulation
#include <algorithm>// For algorithms like remove_if
#include <cctype>   // For character handling functions

using namespace std;

// Updated structures (already present in your code)
struct Column {
    string name;
    string type;
};

struct Table {
    string name;
    vector<Column> columns;
    vector<vector<string>> rows;
};

struct Database {
    unordered_map<string, Table> tables;
};

// Global map to store all databases
unordered_map<string, Database> databases;

// Function declarations
void createDatabase(const string &dbName);
void createTable(const string &dbName, const string &tableName, const vector<Column> &columns);
void insertIntoTable(const string &dbName, const string &tableName, const vector<string> &values);
void displayTable(const string &dbName, const string &tableName, ofstream &outputFile);
void processCommand(const string &command, ofstream &outputFile);
void readFileInput(const string &inputFileName, const string &outputFileName);
int countRowsInTable(const string &dbName, const string &tableName);
string trim(const string &str);

// Function to count rows in a table
int countRowsInTable(const string &dbName, const string &tableName) {
    if (databases.find(dbName) == databases.end()) {
        cout << "Database not found: " << dbName << endl;
        return -1;
    }
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) {
        cout << "Table not found: " << tableName << endl;
        return -1;
    }

    return databases[dbName].tables[tableName].rows.size();
}

// Updated processCommand to handle COUNT ROWS
void processCommand(const string &command, ofstream &outputFile) {
    string trimmedCmd = trim(command);

    if (trimmedCmd.find("CREATE TABLE") == 0) {
        // Existing table creation logic
    } else if (trimmedCmd.find("INSERT INTO") == 0) {
        // Existing row insertion logic
    } else if (trimmedCmd.find("COUNT ROWS FROM") == 0) {
        // New logic for counting rows
        cout << "> " << trimmedCmd << endl; // Print the command itself
        string tableName = trimmedCmd.substr(16); // Extract table name
        tableName = trim(tableName);
        int rowCount = countRowsInTable("default_db", tableName);
        if (rowCount >= 0) {
            cout << "Number of rows in table " << tableName << ": " << rowCount << endl;
            outputFile << "Number of rows in table " << tableName << ": " << rowCount << endl;
        }
    } else {
        // Other commands remain unchanged
    }
}

// Existing function definitions for file handling, table operations, etc.
// ...

int main() {
    string inputFileName = "fileInput1.mdb";
    string outputFileName = "fileOutput1.txt"; // Default output file name

    ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        cout << "Error: Could not open file: " << inputFileName << endl;
        return 1;
    }

    createDatabase("default_db"); // Create the default database
    readFileInput(inputFileName, outputFileName); // Read and execute commands from the input file
    return 0; // End of the program
}