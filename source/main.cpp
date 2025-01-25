// *********************************************************
// Program: T10L_G9_A1.cpp
// Course: CCP6114 Programming Fundamentals
// Lecture Class: TC5L
// Tutorial Class: T10L
// Trimester: 2430
// Member_1: 242UC241RV | Chetan Rana 
// Member_2: 243UC245XA | Fuad Mohtar 
// Member_3: 243UC247CQ | Eric Chin Yan Hong  
// Member_4: 243UC246P7 | Heng Yong An Damian  
// *********************************************************
// Task Distribution
// Member_1: Creating and Viewing databases and tables, CSV file implementation, Error handling and Processing the input file
// Member_2: File reading and writing functionality, outputs to the screen and Processing the input file
// Member_3: Table update, Desired Output Integration and Processing the input file
// Member_4: Created design documentation, including flowcharts, pseudocodes, comments and Processing the input file
// *********************************************************

#include <iostream> // For input and output
#include <fstream> // For file handling
#include <sstream> // For string stream operations
#include <vector> // For using dynamic arrays
#include <unordered_map> // For mapping databases and tables
#include <string> // For string manipulation
#include <algorithm> // For algorithms like remove_if
#include <cctype> // For character handling functions
#include <cstring>  // For handling platform-specific functions
#include <cstdlib>  // For realpath()


using namespace std;


#ifdef _WIN32
#include <direct.h>  // For _fullpath on Windows
#else
#include <unistd.h>  // For realpath on Unix-based systems
#endif

// Function to get the absolute path of a file
string getFullPath(const string &inputFileName) {
    char fullPath[512]; // Buffer to hold the full path

#ifdef _WIN32
    // Windows-specific code using _fullpath
    if (_fullpath(fullPath, inputFileName.c_str(), sizeof(fullPath)) == nullptr) {
        return "";
    }
#else
    // Unix/Linux-specific code using realpath
    if (realpath(inputFileName.c_str(), fullPath) == nullptr) {
        return "";
    }
#endif

    return string(fullPath);
}

// Function to strip quotes from a string and check if it represents an integer.
// This helps standardize values for storage and comparison.
string stripQuotes(const string &val, bool &isInteger) {
    string result = val;
    isInteger = false;

    // Remove surrounding single quotes if present
    if (!result.empty() && result.front() == '\'' && result.back() == '\'') {
        result = result.substr(1, result.size() - 2);
    }

    // Attempt to parse the value as an integer
    try {
        stoi(result);
        isInteger = true;
    } catch (...) {
        isInteger = false; // Not an integer if parsing fails
    }

    // Trim leading and trailing whitespace
    int start = 0;
    while (start < result.size() && isspace(result[start])) {
        start++;
    }

    int end = result.size() - 1;
    while (end >= 0 && isspace(result[end])) {
        end--;
    }

    // Return trimmed result or empty string if no valid characters
    return (start <= end) ? result.substr(start, end - start + 1) : "";
}

// Structure to define a column with its name and type.
struct Column {
    string name; // Column name.
    string type; // Data type of the column (e.g., INT, STRING).
};

// Structure to define a table containing columns and rows.
struct Table {
    string name; // Table name.
    vector<Column> columns; // List of columns in the table.
    vector<vector<string>> rows; // Data rows in the table.
};

// Structure to define a database containing multiple tables.
struct Database {
    unordered_map<string, Table> tables; // Map of table names to Table objects.
};

// Global map to store all databases.
unordered_map<string, Database> databases;

// Function declarations for database operations.
void createDatabase(const string &dbName);
void createTable(const string &dbName, const string &tableName, const vector<Column> &columns);
void insertIntoTable(const string &dbName, const string &tableName, const vector<string> &values);
void displayTable(const string &dbName, const string &tableName, ofstream &outputFile);
void deleteFromTable(const string &dbName, const string &tableName, const string &conditionCol, const string &conditionVal);
void exportTableToCSV(const string &dbName, const string &tableName, const string &csvFileName);
void processCommand(const string &command, ofstream &outputFile);
void readFileInput(const string &inputFileName, string &outputFileName);
int countRowsInTable(const string &dbName, const string &tableName);


string trim(const string &str);


// Main function where the program starts running.
int main() {
    string inputFileName;
    string outputFileName = "default.txt"; // Default output file name if no other is given.

    // Ask the user for the path of the input file.
    cout << "Enter the path of the input file: ";
    getline(cin, inputFileName); // Get the input file path from the user.

    // Try to open the input file.
    ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        // If the file couldn't be opened, show an error message and stop the program.
        cout << "Error: Could not open file: " << inputFileName << endl;
        return 1; // Exit the program with an error code.
    }

    // Print the full path of the file
    cout<<endl<<endl;
    filesystem::path fuPath = filesystem::absolute(inputFileName);
    cout<<"The path for the file is :"<<fuPath.string()<<endl<<endl;

    // Create a default database called "default_db".
    createDatabase("default_db");

    // Read the input file and update the output file name.
    readFileInput(inputFileName, outputFileName);

    //Extract element name
    Table t;

    // Export data from the "customer" table in "default_db" to a CSV file called "customer.csv".
    exportTableToCSV("default_db", t.name, "customer.csv");

    return 0; // The program finishes without errors.
}


// This function removes extra spaces at the beginning and end of a string.
string trim(const string &str) {
    // Find the first and last non-space characters.
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");

    // If there are no non-space characters, return an empty string.
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}


// This function prints out a row of data, adding quotes around strings if they have special characters or spaces.
void printRow(const vector<string>& row) {
    // Loop through each value in the row.
    for (int i = 0; i < static_cast<int>(row.size()); ++i) {
        // If it's not the first value, add a comma before printing the next value.
        if (i != 0) cout << ",";

        // Flag to check if special characters are found.
        bool hasSpecialChar = false;

        // Check each character in the string for special characters or spaces.
        for (char c : row[i]) {
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == ',') {
                hasSpecialChar = true;
                break;
            }
        }

        // If special characters are found, put quotes around the value.
        if (hasSpecialChar) {
            cout << "'" << row[i] << "'";
        } else {
            // Otherwise, just print the value without quotes.
            cout << row[i];
        }
    }
    cout << endl; // Print a newline after the whole row.
}

void updateTable(const string &databaseName, const string &tblName, const string &condColumn, const string &condValue, const string &updColumn, const string &updValue) {
    // Check if the database exists
    if (databases.find(databaseName) == databases.end()) {
        cout << "Database not found\n";
        return;
    }
	
    // Check if the table exists
    if (databases[databaseName].tables.find(tblName) == databases[databaseName].tables.end()) {
        cout << "Table missing\n";
        return;
    }
	
    // Get the table
    Table &myTable = databases[databaseName].tables[tblName];
	
    // Find condition column index
    int condColIndex = -1;
    for (int x = 0; x < myTable.columns.size(); x++) {
        if (myTable.columns[x].name == condColumn) {
            condColIndex = x;
            break;
        }
    }
	
    // Find update column index
    int updColIndex = -1;
    for (int x = 0; x < myTable.columns.size(); x++) {
        if (myTable.columns[x].name == updColumn) {
            updColIndex = x;
            break;
        }
    }
	
    // If columns not found, stop
    if (condColIndex == -1 || updColIndex == -1) {
        cout << "Columns not found\n";
        return;
    }
	
    // Start updating rows
    bool didUpdate = false;
    bool condIsNum = false;
    string condValueStr = stripQuotes(condValue, condIsNum);

    for (int row = 0; row < myTable.rows.size(); row++) {
        string rowCondVal = stripQuotes(myTable.rows[row][condColIndex], condIsNum);

        if (condIsNum) {
            if (stoi(rowCondVal) == stoi(condValueStr)) {
                myTable.rows[row][updColIndex] = stripQuotes(updValue, condIsNum);
                didUpdate = true;
            }
        } else {
            if (rowCondVal == condValueStr) {
                myTable.rows[row][updColIndex] = stripQuotes(updValue, condIsNum);
                didUpdate = true;
            }
        }
    }

    // Show results
    if (didUpdate) {
        cout << "Table updated\n";
        for (int r = 0; r < myTable.rows.size(); r++) {
            for (int c = 0; c < myTable.rows[r].size(); c++) {
                cout << myTable.rows[r][c];
                if (c != myTable.rows[r].size() - 1) cout << ", ";
            }
            cout << endl;
        }
    } else {
        cout << "Nothing to update\n";
    }
}

void deleteFromTable(const string &databaseName, const string &tblName, const string &condColumn, const string &condValue) {
    // Check if the database exists
    if (databases.find(databaseName) == databases.end()) {
        cout << "Database not found\n";
        return;
    }

    // Check if the table exists
    if (databases[databaseName].tables.find(tblName) == databases[databaseName].tables.end()) {
        cout << "Table missing\n";
        return;
    }

    // Get the table
    Table &myTable = databases[databaseName].tables[tblName];

    // Find condition column index
    int condColIndex = -1;
    for (int x = 0; x < myTable.columns.size(); x++) {
        if (myTable.columns[x].name == condColumn) {
            condColIndex = x;
            break;
        }
    }

    // If the condition column is not found, stop
    if (condColIndex == -1) {
        cout << "Condition column not found\n";
        return;
    }

    // Start deleting rows
    bool didDelete = false;
    bool condIsNum = false;
    string condValueStr = stripQuotes(condValue, condIsNum);

    for (int r = 0; r < myTable.rows.size(); r++) {
        string rowCondVal = stripQuotes(myTable.rows[r][condColIndex], condIsNum);

        if (condIsNum) {
            if (stoi(rowCondVal) == stoi(condValueStr)) {
                myTable.rows.erase(myTable.rows.begin() + r);
                r--; // Adjust index after removing a row
                didDelete = true;
            }
        } else {
            if (rowCondVal == condValueStr) {
                myTable.rows.erase(myTable.rows.begin() + r);
                r--; // Adjust index after removing a row
                didDelete = true;
            }
        }
    }

    // Show results
    if (didDelete) {
        cout << "Rows deleted. Remaining table:\n";
        for (int i = 0; i < myTable.rows.size(); i++) {
            for (int j = 0; j < myTable.rows[i].size(); j++) {
                cout << myTable.rows[i][j];
                if (j != myTable.rows[i].size() - 1) cout << ", ";
            }
            cout << endl;
        }
    } else {
        cout << "Nothing to delete\n";
    }
}

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

// Function to create a new database if it does not already exist.
void createDatabase(const string &dbName) {
    if (databases.find(dbName) != databases.end()) return; // Database already exists.
    databases[dbName] = {}; // Add a new empty database to the map.
}

// This function creates a new table in a database with the columns we give it.
void createTable(const string &dbName, const string &tableName, const vector<Column> &columns) {
    // First, check if the table already exists in the database
    if (databases[dbName].tables.find(tableName) != databases[dbName].tables.end()) return; // If it exists, don't do anything.
    
    // If the table doesn't exist, we create it by adding the table name, columns, and an empty list of rows.
    databases[dbName].tables[tableName] = {tableName, columns, {}}; // Now the table is ready to be used.
}

// This function adds a new row (set of data) to a table in a specific database.
void insertIntoTable(const string &dbName, const string &tableName, const vector<string> &values) {
    // Step 1: Check if the database exists.
    if (databases.find(dbName) == databases.end()) return; // If the database doesn't exist, stop the function.

    // Step 2: Check if the table exists in the database.
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return; // If the table doesn't exist, stop the function.

    // Step 3: Get the reference to the table where the data will be added.
    Table &table = databases[dbName].tables[tableName];

    // Step 4: Check if the number of values matches the number of columns in the table.
    if (values.size() == table.columns.size()) {
        table.rows.push_back(values); // If yes, add the row (values) to the table.
    }
    // If the value count doesn't match the column count, nothing happens (we quietly ignore the row).
}

// This function shows the contents of a table on the screen and saves it to a file.
void displayTable(const string &dbName, const string &tableName, ofstream &outputFile) {
    // Step 1: Check if the database exists.
    if (databases.find(dbName) == databases.end()) return; // If not, stop the function.

    // Step 2: Check if the table exists in the database.
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return; // If not, stop the function.

    // Step 3: Get the reference to the table we want to display.
    Table &table = databases[dbName].tables[tableName];

    // Step 4: Print the column headers (the names of the columns at the top of the table).
    for (const auto &col : table.columns) { 
        cout << col.name << ",";         // Print each column name on the screen, separated by commas.
        outputFile << col.name << ",";  // Also write each column name to the output file, separated by commas.
    }
    cout << endl; // Move to a new line on the screen after printing all column headers.
    outputFile << endl; // Do the same in the file.

    // Step 5: Print the rows of the table (the actual data).
    for (const auto &row : table.rows) { // Go through each row in the table.
        for (const auto &val : row) {   // Go through each value in the row.
            cout << val << ",";        // Print the value on the screen, separated by commas.
            outputFile << val << ",";  // Also write the value to the file, separated by commas.
        }
        cout << endl; // After printing all values in a row, move to a new line on the screen.
        outputFile << endl; // Do the same in the file.
    }
}

// This function saves the data from a table into a CSV file (a format that looks like a spreadsheet).
void exportTableToCSV(const string &dbName, const string &tableName, const string &csvFileName) {
    // First, check if the database exists.
    // If it doesn't, we stop here because we can't export a table that doesn't belong to any database.
    if (databases.find(dbName) == databases.end()) return;

    // Next, check if the table exists in the given database.
    // If the table isn't found, we stop because there's no data to export.
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return;

    // Get a reference to the table we want to export.
    Table &table = databases[dbName].tables[tableName];

    // Open the CSV file where the data will be saved.
    ofstream csvFile(csvFileName);

    // Check if the file opened correctly. If not, we stop because we can't write to it.
    if (!csvFile.is_open()) return;

    // First, write the column headers (the names of the columns in the table).
    for (const auto &col : table.columns) {
        csvFile << col.name << ","; // Write each column name followed by a comma.
    }
    csvFile << endl; // End the header row.

    // Now, write all the rows of data in the table.
    for (const auto &row : table.rows) {
        for (const auto &val : row) {
            csvFile << val << ","; // Write each value in the row followed by a comma.
        }
        csvFile << endl; // End the current row and move to the next one.
    }

    // Finally, close the CSV file because we're done writing.
    csvFile.close(); 
}

// This function takes a command (like a sentence that says what to do with the database) 
// and decides what action to perform. It supports commands like:
// - CREATE TABLE
// - INSERT INTO
// - SELECT
// - UPDATE
// - DELETE
void processCommand(const string &command, ofstream &outputFile) {
    string cmd = trim(command); // Remove extra spaces
    cout << "> " << cmd << endl; // Show the command on the screen
    Table t;

    if (cmd.find("CREATE TABLE") == 0) {
        // Extract table name and columns
        int start = cmd.find('(');
        int end = cmd.find(')');
        string tableName = trim(cmd.substr(13, start - 13));
        string columnsStr = cmd.substr(start + 1, end - start - 1);

        // Parse columns
        vector<Column> columns;
        stringstream colStream(columnsStr);
        string col;
        while (getline(colStream, col, ',')) {
            stringstream colSS(trim(col));
            string name, type;
            colSS >> name >> type;
            columns.push_back({name, type});
        }

        createTable("default_db", tableName, columns);
    } 
    else if (cmd.find("INSERT INTO") == 0) {
        // Extract values
        int start = cmd.find("VALUES (") + 8;
        int end = cmd.find(')', start);
        string valuesStr = cmd.substr(start, end - start);

        vector<string> values;
        stringstream valStream(valuesStr);
        string val;
        while (getline(valStream, val, ',')) {
            values.push_back(trim(val));
        }

        insertIntoTable("default_db", "customer", values);
    } 
    else if (cmd.find("SELECT * FROM") == 0) {
        displayTable("default_db", "customer", outputFile);
    } 
    else if (cmd.find("UPDATE") == 0) {
        // Extract table name, set clause, and where clause
        int setPos = cmd.find("SET");
        int wherePos = cmd.find("WHERE");
        string tableName = trim(cmd.substr(7, setPos - 7));
        string setPart = cmd.substr(setPos + 4, wherePos - setPos - 4);
        string wherePart = cmd.substr(wherePos + 6);

        string updateCol = trim(setPart.substr(0, setPart.find('=')));
        string updateVal = trim(setPart.substr(setPart.find('=') + 1));
        string conditionCol = trim(wherePart.substr(0, wherePart.find('=')));
        string conditionVal = trim(wherePart.substr(wherePart.find('=') + 1));

        updateTable("default_db", tableName, conditionCol, conditionVal, updateCol, updateVal);
    } 
    else if (cmd.find("SELECT COUNT(*) FROM") == 0) {
        // Extract table name
        string tableName = trim(cmd.substr(cmd.find("FROM") + 5));

        int rowCount = countRowsInTable("default_db", "customer");
        if (rowCount >= 0) {
            cout << "Number of rows in table " << "customer" << ": " << rowCount << endl;
            outputFile << "Number of rows in table " << "customer" << ": " << rowCount << endl;
        } else {
            cout << "Error counting rows for table: " << tableName << endl;
        }
    } 
    else {
        cout << "Unknown command: " << cmd << endl;
    }
}

void readFileInput(const string &inputFileName, string &outputFileName) {
    // Get the full path of the input file
    // string fullPath = getFullPath(inputFileName);
    // if (fullPath.empty()) {
    //     cout << "Error: Could not resolve full path for input file: " << inputFileName << endl;
    //     return;
    // }
    // cout << "Input file full path: " << fullPath << endl;

    // Open the input file
    ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        cout << "Error: Could not open input file: " << inputFileName << endl;
        return;
    }

    // Open the output file
    ofstream outputFile(outputFileName, ios::out | ios::trunc);
    if (!outputFile.is_open()) {
        cout << "Error: Could not create output file: " << outputFileName << endl;
        return;
    }

    string commandBuffer;
    string line;

    while (getline(inputFile, line)) {
        line = trim(line);  // Assuming 'trim' is a defined function
        if (!line.empty()) {
            commandBuffer += line;
            if (line.back() == ';') {
                cout << "Executing: " << commandBuffer << endl;
                outputFile << "Executing: " << commandBuffer << endl;
                processCommand(commandBuffer, outputFile);  // Assuming 'processCommand' is defined
                commandBuffer.clear();
            }
        }
    }

    inputFile.close();
    outputFile.close();
}
