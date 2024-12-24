#include <iostream> // For input and output
#include <fstream> // For file handling
#include <sstream> // For string stream operations
#include <vector> // For using dynamic arrays
#include <unordered_map> // For mapping databases and tables
#include <string> // For string manipulation
#include <algorithm> // For algorithms like remove_if
#include <cctype> // For character handling functions

using namespace std;

// Structure to define a column with its name and type
struct Column {
    string name; // Column name
    string type; // Column data type (e.g., INT, TEXT)
};

// Structure to define a table with columns and rows
struct Table {
    string name; // Table name
    vector<Column> columns; // List of columns in the table
    vector<vector<string>> rows; // Data rows in the table
};

// Structure to define a database containing multiple tables
struct Database {
    unordered_map<string, Table> tables; // Map of table names to Table objects
};

// Global map to store all databases
unordered_map<string, Database> databases;

// Function declarations
void createDatabase(const string &dbName); // Create a new database
void createTable(const string &dbName, const string &tableName, const vector<Column> &columns); // Create a new table
void insertIntoTable(const string &dbName, const string &tableName, const vector<string> &values); // Insert a row into a table
void displayTable(const string &dbName, const string &tableName, ofstream &outputFile); // Display table contents
void deleteFromTable(const string &dbName, const string &tableName, const string &conditionCol, const string &conditionVal); // Delete rows from a table
void exportTableToCSV(const string &dbName, const string &tableName, const string &csvFileName); // Export table data to a CSV file
void processCommand(const string &command, ofstream &outputFile); // Process a single SQL-like command
void readFileInput(const string &inputFileName, const string &outputFileName); // Read input commands from a file
string trim(const string &str); // Trim whitespace from a string

int main() {
    string inputFileName = "fileInput1.mdb"; // Input file name containing commands
    string outputFileName = "fileOutput1.txt"; // Output file name for results

    createDatabase("default_db"); // Create the default database
    readFileInput(inputFileName, outputFileName); // Read and execute commands from the input file
    exportTableToCSV("default_db", "customer", "customer.csv"); // Export the "customer" table to a CSV file

    return 0; // End of the program
}

// Function to trim leading and trailing whitespace from a string
string trim(const string &str) {
    size_t start = str.find_first_not_of(" \t\n\r"); // Find the first non-whitespace character
    size_t end = str.find_last_not_of(" \t\n\r"); // Find the last non-whitespace character
    return (start == string::npos) ? "" : str.substr(start, end - start + 1); // Return the trimmed string
}

// Function to create a new database
void createDatabase(const string &dbName) {
    if (databases.find(dbName) != databases.end()) return; // If database already exists, do nothing
    databases[dbName] = {}; // Add a new database with the given name
}

// Function to create a new table in a database
void createTable(const string &dbName, const string &tableName, const vector<Column> &columns) {
    if (databases[dbName].tables.find(tableName) != databases[dbName].tables.end()) return; // If table already exists, do nothing
    databases[dbName].tables[tableName] = {tableName, columns, {}}; // Add a new table with columns and no rows
}

// Function to insert a row into a table
void insertIntoTable(const string &dbName, const string &tableName, const vector<string> &values) {
    if (databases.find(dbName) == databases.end()) return; // If database does not exist, do nothing
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return; // If table does not exist, do nothing
    Table &table = databases[dbName].tables[tableName]; // Get the reference to the table
    if (values.size() == table.columns.size()) { // Check if the number of values matches the number of columns
        table.rows.push_back(values); // Add the row to the table
    }
}

// Function to display table contents to the console and output file
void displayTable(const string &dbName, const string &tableName, ofstream &outputFile) {
    if (databases.find(dbName) == databases.end()) return; // If database does not exist, do nothing
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return; // If table does not exist, do nothing

    Table &table = databases[dbName].tables[tableName]; // Get the reference to the table

    // Print column headers
    for (const auto &col : table.columns) {
        cout << col.name << ",";
        outputFile << col.name << ",";
    }
    cout << endl;
    outputFile << endl;

    // Print each row
    for (const auto &row : table.rows) {
        for (const auto &val : row) {
            cout << val << ",";
            outputFile << val << ",";
        }
        cout << endl;
        outputFile << endl;
    }
}

// Function to delete rows from a table based on a condition
void deleteFromTable(const string &dbName, const string &tableName, const string &conditionCol, const string &conditionVal) {
    if (databases.find(dbName) == databases.end()) return; // If database does not exist, do nothing
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return; // If table does not exist, do nothing

    Table &table = databases[dbName].tables[tableName]; // Get the reference to the table

    // Find the index of the condition column
    auto colIt = find_if(table.columns.begin(), table.columns.end(), [&](const Column &col) {
        return col.name == conditionCol;
    });
    if (colIt == table.columns.end()) return; // If column does not exist, do nothing

    size_t colIdx = distance(table.columns.begin(), colIt); // Get the index of the column

    // Remove rows that match the condition
    table.rows.erase(remove_if(table.rows.begin(), table.rows.end(), [&](const vector<string> &row) {
        return row[colIdx] == conditionVal;
    }), table.rows.end());
}

// Function to export table data to a CSV file
void exportTableToCSV(const string &dbName, const string &tableName, const string &csvFileName) {
    if (databases.find(dbName) == databases.end()) return; // If database does not exist, do nothing
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return; // If table does not exist, do nothing

    Table &table = databases[dbName].tables[tableName]; // Get the reference to the table

    ofstream csvFile(csvFileName); // Open the CSV file for writing
    if (!csvFile.is_open()) return; // If file cannot be opened, do nothing

    // Write column headers
    for (const auto &col : table.columns) {
        csvFile << col.name << ",";
    }
    csvFile << endl;

    // Write each row
    for (const auto &row : table.rows) {
        for (const auto &val : row) {
            csvFile << val << ",";
        }
        csvFile << endl;
    }
    csvFile.close(); // Close the file
}

// Function to process a single SQL-like command
void processCommand(const string &command, ofstream &outputFile) {
    string trimmedCmd = trim(command); // Trim the command to remove leading/trailing spaces

    // Handle CREATE TABLE command
    if (trimmedCmd.find("CREATE TABLE") == 0) {
        size_t start = trimmedCmd.find('(') + 1; // Find the start of column definitions
        size_t end = trimmedCmd.find(')'); // Find the end of column definitions

        string tableName = trimmedCmd.substr(13, start - 14); // Extract table name
        string columnsStr = trimmedCmd.substr(start, end - start); // Extract column definitions

        stringstream colStream(columnsStr); // Create a string stream for parsing columns
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
        displayTable("default_db", "customer", outputFile);
    } else if (trimmedCmd.find("DELETE FROM") == 0) {
        size_t whereIdx = trimmedCmd.find("WHERE");
        string condition = trimmedCmd.substr(whereIdx + 6);
        string colName = condition.substr(0, condition.find('='));
        string value = condition.substr(condition.find('=') + 1);
        deleteFromTable("default_db", "customer", trim(colName), trim(value));
    }
}

// Read input file and execute commands
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
                outputFile << "Executing: " << commandBuffer << endl;
                processCommand(commandBuffer, outputFile);
                commandBuffer.clear();
            }
        }
    }
    inputFile.close();
    outputFile.close();
}
