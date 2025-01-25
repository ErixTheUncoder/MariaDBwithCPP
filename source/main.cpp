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

using namespace std;

// Function to strip quotes from a string and check if it represents an integer.
// This helps standardize values for storage and comparison.
string stripQuotes(const string &val, bool &isInteger) {
    string result = val;
    isInteger = false;

    // Remove surrounding single quotes if present.
    if (result.front() == '\'' && result.back() == '\'') {
        result = result.substr(1, result.length() - 2);
    }

    // Attempt to parse the value as an integer.
    try {
        stoi(result); // Convert to integer; if successful, mark as integer.
        isInteger = true;
    } catch (const std::invalid_argument&) {
        isInteger = false; // If parsing fails, it is not an integer.
    }

    // Trim leading and trailing whitespace.
    size_t first = result.find_first_not_of(" \t\r\n");
    size_t last = result.find_last_not_of(" \t\r\n");
    if (first == string::npos || last == string::npos) {
        return ""; // Return an empty string if only whitespace remains.
    }

    return result.substr(first, last - first + 1);
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

    // Create a default database called "default_db".
    createDatabase("default_db");

    // Read the input file and update the output file name.
    readFileInput(inputFileName, outputFileName);

    // Export data from the "customer" table in "default_db" to a CSV file called "customer.csv".
    exportTableToCSV("default_db", "customer", "customer.csv");

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
    for (size_t i = 0; i < row.size(); ++i) {
        // If it's not the first value, add a comma before printing the next value.
        if (i != 0) cout << ","; 

        // Check if the value has spaces, tabs, newlines, or commas.
        if (row[i].find_first_of(" \t\n\r") != string::npos || row[i].find_first_of(",") != string::npos) {
            // If so, put quotes around the value.
            cout << "'" << row[i] << "'";
        } else {
            // Otherwise, just print the value without quotes.
            cout << row[i];
        }
    }
    cout << endl; // Print a newline after the whole row.
}


// This function updates rows in a table where a specific condition is met.
// It looks for rows where a given column's value matches a condition, and if it finds one, it updates another column in that row.
void updateTable(const string &dbName, const string &tableName, const string &conditionCol, const string &conditionVal, const string &updateCol, const string &updateVal) {
    // First, check if the database exists. If not, exit the function.
    if (databases.find(dbName) == databases.end()) return; // Database not found.
    
    // Then, check if the table exists in that database. If not, exit the function.
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return; // Table not found.

    // Grab the table object to work with.
    Table &table = databases[dbName].tables[tableName];

    // Find the columns for the condition and the update by their names.
    auto conditionIt = find_if(table.columns.begin(), table.columns.end(), [&](const Column &col) {
        return col.name == conditionCol;
    });
    auto updateIt = find_if(table.columns.begin(), table.columns.end(), [&](const Column &col) {
        return col.name == updateCol;
    });

    // If either of the columns isn't found, exit the function.
    if (conditionIt == table.columns.end() || updateIt == table.columns.end()) return; // Columns not found.

    // Get the index of the columns to compare and update.
    size_t conditionIdx = distance(table.columns.begin(), conditionIt);
    size_t updateIdx = distance(table.columns.begin(), updateIt);

    bool updated = false; // Flag to track if any row was updated.
    bool isIntegerCondition = false; // To check if the condition involves integers.

    // Remove quotes around the condition value and figure out if it's an integer.
    string conditionValStripped = stripQuotes(conditionVal, isIntegerCondition);

    // Loop through each row in the table.
    for (auto &row : table.rows) {
        string rowConditionVal = stripQuotes(row[conditionIdx], isIntegerCondition);

        if (isIntegerCondition) {
            // If the condition is an integer, compare the values as integers.
            if (stoi(rowConditionVal) == stoi(conditionValStripped)) {
                row[updateIdx] = stripQuotes(updateVal, isIntegerCondition); // Update the row.
                updated = true;
            }
        } else {
            // If the condition is a string, compare the values as strings.
            if (rowConditionVal == conditionValStripped) {
                row[updateIdx] = stripQuotes(updateVal, isIntegerCondition); // Update the row.
                updated = true;
            }
        }
    }

    // Print the updated table if changes were made, or show a message if no rows were updated.
    if (updated) {
        cout << "> Updated table after changes:" << endl;
        for (const auto &row : table.rows) {
            printRow(row); // Display the updated rows.
        }
    } else {
        cout << "> No rows matched the condition for update.\n"; // No rows found for the update.
    }
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


// This function removes rows from a table that match a specific condition.
void deleteFromTable(const string &dbName, const string &tableName, const string &conditionCol, const string &conditionVal) {
    // Check if the database exists. If it doesn't, stop here.
    if (databases.find(dbName) == databases.end()) return;

    // Check if the table exists in the database. If it doesn't, stop here.
    if (databases[dbName].tables.find(tableName) == databases[dbName].tables.end()) return;

    // Get a reference to the table we want to delete rows from.
    Table &table = databases[dbName].tables[tableName];

    // Find the index of the column where we want to check the condition.
    auto conditionIt = find_if(table.columns.begin(), table.columns.end(), [&](const Column &col) {
        return col.name == conditionCol; // Look for the column with the same name as the condition.
    });

    // If the column isn't found, stop because we can't delete rows based on a missing column.
    if (conditionIt == table.columns.end()) return;

    // Get the position (index) of the condition column in the table.
    size_t conditionIdx = distance(table.columns.begin(), conditionIt);

    bool deleted = false; // Keep track of whether we deleted any rows.
    bool isIntegerCondition = false; // Check if the condition is an integer.

    // Remove quotes from the condition value and check if it's an integer.
    string conditionValStripped = stripQuotes(conditionVal, isIntegerCondition);

    // Go through each row in the table and remove rows that match the condition.
    table.rows.erase(remove_if(table.rows.begin(), table.rows.end(), [&](const vector<string> &row) {
        // Get the value in the row at the condition column's index.
        string rowConditionVal = stripQuotes(row[conditionIdx], isIntegerCondition);

        if (isIntegerCondition) {
            // Compare values as integers if the condition is a number.
            if (stoi(rowConditionVal) == stoi(conditionValStripped)) {
                deleted = true; // Mark that we deleted something.
                return true; // Remove this row.
            }
        } else {
            // Compare values as strings if the condition is not a number.
            if (rowConditionVal == conditionValStripped) {
                deleted = true; // Mark that we deleted something.
                return true; // Remove this row.
            }
        }
        return false; // Keep this row if it doesn't match the condition.
    }), table.rows.end());

    // After deleting rows, print the updated table or say no rows matched.
    if (deleted) {
        // Show the table after deletion.
        cout << "> Table after deletion:" << endl;
        for (const auto &row : table.rows) {
            printRow(row); // Print each row left in the table.
        }
    } else {
        // If no rows matched the condition, let the user know.
        cout << "> No rows matched the condition for deletion.\n";
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
    // First, clean up the command by removing extra spaces from the beginning and end.
    string trimmedCmd = trim(command); 

    // If the command starts with "CREATE " and mentions a ".txt" file, we just ignore it.
    // This program doesn't handle creating non-table files like this.
    if (trimmedCmd.find("CREATE ") == 0 && trimmedCmd.find(".txt;") != string::npos) {
        cout << "CREATE command is not implemented for non-table objects." << endl;
        return; // Stop here because we don’t do anything with it.
    }

    // If the command starts with "CREATE TABLE," it's asking us to create a new table.
    if (trimmedCmd.find("CREATE TABLE") == 0) {
        cout << "> " << trimmedCmd << endl; // Show the command on the screen.

        // Find the part where the columns of the table are listed inside parentheses.
        size_t start = trimmedCmd.find('(') + 1; // Start of the column definitions.
        size_t end = trimmedCmd.find(')'); // End of the column definitions.

        // Get the table name. It's between "CREATE TABLE" and the parentheses.
        string tableName = trimmedCmd.substr(13, start - 14); 

        // Get the list of columns as one long string.
        string columnsStr = trimmedCmd.substr(start, end - start);

        // Use a string stream to break the columns list into individual column definitions.
        stringstream colStream(columnsStr);
        vector<Column> columns; // This will store the column info.
        string col;

        // Go through each column definition (like "id INT" or "name TEXT").
        while (getline(colStream, col, ',')) {
            stringstream colSS(trim(col)); // Clean up each column definition.
            string name, type;
            colSS >> name >> type; // Split it into the column name and type.
            columns.push_back({name, type}); // Add it to the columns list.
        }

        // Now, create the table in the default database using these columns.
        createTable("default_db", trim(tableName), columns);
    } 
    // If the command starts with "INSERT INTO," it's adding a new row to a table.
    else if (trimmedCmd.find("INSERT INTO") == 0) {
        cout << "> " << trimmedCmd << endl; // Show the command on the screen.

        // Find the part where the values are listed (after "VALUES (").
        size_t start = trimmedCmd.find("VALUES (") + 8;
        size_t end = trimmedCmd.find(')', start);

        // Get the list of values as a string.
        string valuesStr = trimmedCmd.substr(start, end - start);

        // Use a string stream to break the values into individual pieces.
        stringstream valStream(valuesStr);
        vector<string> values; // This will store the values for the row.
        string val;

        // Go through each value, clean it up, and add it to the list.
        while (getline(valStream, val, ',')) {
            values.push_back(trim(val)); 
        }

        // Add the row to the "customer" table in the default database.
        insertIntoTable("default_db", "customer", values);
    } 
    // If the command starts with "SELECT * FROM," it wants to show all rows in a table.
    else if (trimmedCmd.find("SELECT * FROM") == 0) {
        cout << "> " << trimmedCmd << endl; // Show the command on the screen.
        // Display the table contents on the screen and write them to the output file.
        displayTable("default_db", "customer", outputFile);
    } 
    // If the command starts with "UPDATE," it wants to change some rows in a table.
    else if (trimmedCmd.find("UPDATE") == 0) {
        cout << "> " << trimmedCmd << endl; // Show the command on the screen.

        // Find where the "SET" and "WHERE" parts of the command are.
        size_t setIdx = trimmedCmd.find("SET");
        size_t whereIdx = trimmedCmd.find("WHERE");

        // Get the table name (it's between "UPDATE" and "SET").
        string tableName = trimmedCmd.substr(7, setIdx - 7);
        tableName = trim(tableName); 

        // Get the part of the command that says what to update (like "name='John'").
        string setPart = trimmedCmd.substr(setIdx + 4, whereIdx - setIdx - 4);

        // Get the condition part (like "id=5").
        string wherePart = trimmedCmd.substr(whereIdx + 6);

        // Break the update part into the column and the new value.
        string updateCol = trim(setPart.substr(0, setPart.find('=')));
        string updateVal = trim(setPart.substr(setPart.find('=') + 1));

        // Break the condition part into the column and the value to match.
        string conditionCol = trim(wherePart.substr(0, wherePart.find('=')));
        string conditionVal = trim(wherePart.substr(wherePart.find('=') + 1));

        // Use the updateTable function to make the changes.
        updateTable("default_db", tableName, conditionCol, conditionVal, updateCol, updateVal);
    } 
    // If the command starts with "DELETE FROM," it wants to remove rows from a table.
    else if (trimmedCmd.find("DELETE FROM") == 0) {
        cout << "> " << trimmedCmd << endl; // Show the command on the screen.

        // Find where the "WHERE" part starts.
        size_t whereIdx = trimmedCmd.find("WHERE");

        // Get the table name (it's after "DELETE FROM" and before "WHERE").
        string tableName = trimmedCmd.substr(12, whereIdx - 12);

        // Get the condition (like "id=5").
        string condition = trimmedCmd.substr(whereIdx + 6);

        // Break the condition into the column and value to match.
        string conditionCol = trim(condition.substr(0, condition.find('=')));
        string conditionVal = trim(condition.substr(condition.find('=') + 1));

        // Use the deleteFromTable function to remove the rows.
        deleteFromTable("default_db", trim(tableName), conditionCol, conditionVal);
    }
}

void readFileInput(const string &inputFileName, string &outputFileName) {
    // Open the input file containing commands
    ifstream inputFile(inputFileName);
    if (!inputFile.is_open()) {
        cout << "Error: Could not open input file: " << inputFileName << endl;
        return;
    }

    // Open the initial output file for writing results
    ofstream outputFile(outputFileName, ios::out | ios::trunc); // Open in truncate mode to clear existing data
    if (!outputFile.is_open()) {
        cout << "Error: Could not create output file: " << outputFileName << endl;
        return;
    }

    string commandBuffer; // Buffer to hold complete commands
    string line; // Current line being read from the input file

    while (getline(inputFile, line)) {
        line = trim(line); // Remove extra spaces
        if (!line.empty()) {
            commandBuffer += line; // Accumulate command lines

            // Check if the command ends with a semicolon (indicating end of the command)
            if (line.back() == ';') {
                // If the command specifies a new output file...
                if (commandBuffer.find("CREATE ") == 0 && commandBuffer.find(".txt;") != string::npos) {
                    size_t start = commandBuffer.find(' ') + 1;
                    size_t end = commandBuffer.find(';', start);
                    string newFileName = trim(commandBuffer.substr(start, end - start));

                    // If the new output file is different from the current one
                    if (newFileName != outputFileName) {
                        cout << "Switching output file to: " << newFileName << endl;

                        // Close the current output file before opening a new one
                        outputFile.close();

                        // Update the output file name
                        outputFileName = newFileName;

                        // Open the new output file in truncate mode to ensure it starts fresh
                        outputFile.open(outputFileName, ios::out | ios::trunc);
                        if (!outputFile.is_open()) {
                            cout << "Error: Could not create new output file: " << outputFileName << endl;
                            return;
                        }
                    }
                }

                // Log the command execution to both the console and output file
                cout << "Executing: " << commandBuffer << endl;
                outputFile << "Executing: " << commandBuffer << endl;

                // Process the command
                processCommand(commandBuffer, outputFile);

                // Clear the buffer for the next command
                commandBuffer.clear();
            }
        }
    }

    // Close both input and output files when done
    inputFile.close();
    outputFile.close();
}


