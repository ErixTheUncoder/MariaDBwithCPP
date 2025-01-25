//*********************************************************
// Program: main.cpp
// Course: CCP6114 Programming Fundamentals // Tutorial Class: TT5L
// Lecture Class: TC3L
// Trimester: 2430
// Member_1: 243UC247CQ | ERIC CHIN YAN HONG | eric.chin.yan@student.mmu.edu.my | +60168262342
// Member_2: ID | NAME | EMAIL | PHONE
// Member_3: ID | NAME | EMAIL | PHONE
// Member_4: ID | NAME | EMAIL | PHONE
// ********************************************************* // Member_1:
// Task Distribution
// Member_1:  Integration, Create table, Table name
// Member_2:
// Member_3:
// Member_4:
// *********************************************************
#include <iostream> // For input and output
#include <fstream> // For file handling
#include <sstream> // For string stream operations
#include <vector> // For using dynamic arrays
#include <unordered_map> // For mapping databases and tables
#include <string> // For string manipulation
#include <algorithm> // For algorithms like remove_if
#include <cctype> // For character handling functions

using namespace std;

//define a column with name and type
struct Column {
    string name;
    string type;
};

//define table with columns and rows
struct Table {
    string name;
    vector<Column> columns;
    vector<vector<string>> rows;
};

//define database
struct Database {
    unordered_map<string, Table> tables;
};

// Function declarations
void createDatabase(const string &dbName);
void createTable(const string &dbName, const string &tableName, const vector<Column> &columns);
void insertIntoTable(const string &dbName, const string &tableName, const vector<string> &values);
void displayTable(const string &dbName, const string &tableName, ofstream &outputFile);
void deleteFromTable(const string &dbName, const string &tableName, const string &conditionCol, const string &conditionVal);
void exportTableToCSV(const string &dbName, const string &tableName, const string &csvFileName);
void processCommand(const string &command, ofstream &outputFile);
void readFileInput(const string &inputFileName, const string &outputFileName);
string outputName();

int main() {
    string inputFileName = "inputOutput/fileInput2.mdb";
    string outputFileName = "fileOutput.txt";

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

// Function to trim leading and trailing whitespace from a string
string trim(const string &str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
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


//Function to handle integer and string values
string stripQuotes(const string &val, bool &isInteger) {
    string result = val;
    isInteger = false;
    
    if (result.front() == '\'' && result.back() == '\'') {
        result = result.substr(1, result.length() - 2);  // Remove quotes
    }

    // Check if the value is an integer
    try {
        stoi(result);  // Try to convert to integer
        isInteger = true;
    } catch (const std::invalid_argument&) {
        isInteger = false;  // Not an integer
    }

    // Trim any leading or trailing whitespace
    size_t first = result.find_first_not_of(" \t\r\n");
    size_t last = result.find_last_not_of(" \t\r\n");
    if (first == string::npos || last == string::npos) {
        return "";  // Empty string after trimming
    }

    return result.substr(first, last - first + 1);
}

// Global map to store all databases
unordered_map<string, Database> databases;

// Updated print function to handle quotes properly for strings
void printRow(const vector<string>& row) {
    for (size_t i = 0; i < row.size(); ++i) {
        // Check if the value is a string that needs quotes
        if (i != 0) cout << ",";
        if (row[i].find_first_of(" \t\n\r") != string::npos || row[i].find_first_of(",") != string::npos) {
            // Print the string with single quotes
            cout << "'" << row[i] << "'";
        } else {
            // Print the value without quotes
            cout << row[i];
        }
    }
    cout << endl;
}

// To handle different data types in conditions
void updateTable(const string &dbName, const string &tableName, const string &conditionCol, const string &conditionVal, const string &updateCol, const string &updateVal) {
    if (databases.find(dbName) == databases.end()) return;
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return;

    Table &table = databases[dbName].tables[tableName];

    // Find column indices
    auto conditionIt = find_if(table.columns.begin(), table.columns.end(), [&](const Column &col) {
        return col.name == conditionCol;
    });
    auto updateIt = find_if(table.columns.begin(), table.columns.end(), [&](const Column &col) {
        return col.name == updateCol;
    });

    if (conditionIt == table.columns.end() || updateIt == table.columns.end()) return;

    size_t conditionIdx = distance(table.columns.begin(), conditionIt);
    size_t updateIdx = distance(table.columns.begin(), updateIt);

    bool updated = false;
    bool isIntegerCondition = false;

    // Remove quotes and check if the condition is an integer
    string conditionValStripped = stripQuotes(conditionVal, isIntegerCondition);

    for (auto &row : table.rows) {
        string rowConditionVal = stripQuotes(row[conditionIdx], isIntegerCondition);

        if (isIntegerCondition) {
            // Compare as integers
            if (stoi(rowConditionVal) == stoi(conditionValStripped)) {
                row[updateIdx] = stripQuotes(updateVal, isIntegerCondition);
                updated = true;
            }
        } else {
            // Compare as strings
            if (rowConditionVal == conditionValStripped) {
                row[updateIdx] = stripQuotes(updateVal, isIntegerCondition);
                updated = true;
            }
        }
    }

    if (updated) {
        cout << "> Updated table after changes:" << endl;
        for (const auto &row : table.rows) {
            printRow(row);
        }
    } else {
        cout << "> No rows matched the condition for update.\n";
    }
}


// Function to create a new database
void createDatabase(const string &dbName) {
    if (databases.find(dbName) != databases.end()) return;
    databases[dbName] = {};
}

// Function to create a new table in a database
void createTable(const string &dbName, const string &tableName, const vector<Column> &columns) {
    if (databases[dbName].tables.find(tableName) != databases[dbName].tables.end()) return;
    databases[dbName].tables[tableName] = {tableName, columns, {}};
}

// Function to insert a row into a table
void insertIntoTable(const string &dbName, const string &tableName, const vector<string> &values) {
    if (databases.find(dbName) == databases.end()) return;
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return;

    Table &table = databases[dbName].tables[tableName];
    if (values.size() == table.columns.size()) {
        table.rows.push_back(values);
    }
}

// Function to display table contents to the console and output file
void displayTable(const string &dbName, const string &tableName, ofstream &outputFile) {
    if (databases.find(dbName) == databases.end()) return;
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return;

    Table &table = databases[dbName].tables[tableName];

    for (const auto &col : table.columns) {
        cout << col.name << ",";
        outputFile << col.name << ",";
    }
    cout << endl;
    outputFile << endl;

    for (const auto &row : table.rows) {
        for (const auto &val : row) {
            cout << val << ",";
            outputFile << val << ",";
        }
        cout << endl;
        outputFile << endl;
    }
}


void deleteFromTable(const string &dbName, const string &tableName, const string &conditionCol, const string &conditionVal) {
    if (databases.find(dbName) == databases.end()) return;
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return;

    Table &table = databases[dbName].tables[tableName];

    // Find column index for the condition
    auto conditionIt = find_if(table.columns.begin(), table.columns.end(), [&](const Column &col) {
        return col.name == conditionCol;
    });

    if (conditionIt == table.columns.end()) return;

    size_t conditionIdx = distance(table.columns.begin(), conditionIt);

    bool deleted = false;
    bool isIntegerCondition = false;

    // Remove quotes and check if the condition is an integer
    string conditionValStripped = stripQuotes(conditionVal, isIntegerCondition);

    table.rows.erase(remove_if(table.rows.begin(), table.rows.end(), [&](const vector<string> &row) {
        string rowConditionVal = stripQuotes(row[conditionIdx], isIntegerCondition);

        if (isIntegerCondition) {
            // Compare as integers
            if (stoi(rowConditionVal) == stoi(conditionValStripped)) {
                deleted = true;
                return true;
            }
        } else {
            // Compare as strings
            if (rowConditionVal == conditionValStripped) {
                deleted = true;
                return true;
            }
        }
        return false;
    }), table.rows.end());

    if (deleted) {
        cout << "> Table after deletion:" << endl;
        for (const auto &row : table.rows) {
            printRow(row);
        }
    } else {
        cout << "> No rows matched the condition for deletion.\n";
    }
}

// Function to export table data to a CSV file
void exportTableToCSV(const string &dbName, const string &tableName, const string &csvFileName) {
    if (databases.find(dbName) == databases.end()) return;
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return;

    Table &table = databases[dbName].tables[tableName];

    ofstream csvFile(csvFileName);
    if (!csvFile.is_open()) return;

    for (const auto &col : table.columns) {
        csvFile << col.name << ",";
    }
    csvFile << endl;

    for (const auto &row : table.rows) {
        for (const auto &val : row) {
            csvFile << val << ",";
        }
        csvFile << endl;
    }
    csvFile.close();
}

// Function to process a single SQL-like command
void processCommand(const string &command, ofstream &outputFile) {
    string trimmedCmd = trim(command);

    if (trimmedCmd.find("CREATE TABLE") == 0) {
        cout << "> " << trimmedCmd << endl;  // Print the command itself
        size_t start = trimmedCmd.find('(') + 1;
        size_t end = trimmedCmd.find(')');
        string tableName = trimmedCmd.substr(13, start - 14);
        string columnsStr = trimmedCmd.substr(start, end - start);
        stringstream colStream(columnsStr);
        vector<Column> columns;
        string col;
        while (getline(colStream, col, ',')) {
            stringstream colSS(trim(col));
            string name, type;
            colSS >> name >> type;
            columns.push_back({name, type});
        }
        createTable("default_db", trim(tableName), columns);
    } else if (trimmedCmd.find("INSERT INTO") == 0) {
        cout << "> " << trimmedCmd << endl;  // Print the command itself
        size_t start = trimmedCmd.find("VALUES (") + 8;
        size_t end = trimmedCmd.find(')', start);
        string valuesStr = trimmedCmd.substr(start, end - start);
        stringstream valStream(valuesStr);
        vector<string> values;
        string val;
        while (getline(valStream, val, ',')) {
            values.push_back(trim(val));
        }
        insertIntoTable("default_db", "customer", values);
    } else if (trimmedCmd.find("SELECT * FROM") == 0) {
        cout << "> " << trimmedCmd << endl;  
        displayTable("default_db", "customer", outputFile);
    } else if (trimmedCmd.find("UPDATE") == 0) {
        cout << "> " << trimmedCmd << endl; 
        size_t setIdx = trimmedCmd.find("SET");
        size_t whereIdx = trimmedCmd.find("WHERE");
        string tableName = trimmedCmd.substr(7, setIdx - 7);
        tableName = trim(tableName);

        string setPart = trimmedCmd.substr(setIdx + 4, whereIdx - setIdx - 4);
        string wherePart = trimmedCmd.substr(whereIdx + 6);

        string updateCol = trim(setPart.substr(0, setPart.find('=')));
        string updateVal = trim(setPart.substr(setPart.find('=') + 1));
        string conditionCol = trim(wherePart.substr(0, wherePart.find('=')));
        string conditionVal = trim(wherePart.substr(wherePart.find('=') + 1));

        updateTable("default_db", tableName, conditionCol, conditionVal, updateCol, updateVal);
    } else if (trimmedCmd.find("DELETE FROM") == 0) {
        cout << "> " << trimmedCmd << endl; 
        size_t whereIdx = trimmedCmd.find("WHERE");
        string tableName = trimmedCmd.substr(12, whereIdx - 12);
        string condition = trimmedCmd.substr(whereIdx + 6);

        string conditionCol = trim(condition.substr(0, condition.find('=')));
        string conditionVal = trim(condition.substr(condition.find('=') + 1));

        deleteFromTable("default_db", trim(tableName), conditionCol, conditionVal);
    }
}


