#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

// Global Structures
unordered_map<string, vector<string>> dbTables; // Database -> List of Tables
unordered_map<string, vector<vector<string>>> tableRows; // Table -> Rows

// Function to create a database
void createDatabase(const string& dbName) {
    if (dbTables.find(dbName) != dbTables.end()) {
        cout << "Database already exists.\n";
        return;
    }
    dbTables[dbName] = vector<string>();
    cout << "Database '" << dbName << "' created successfully.\n";
}

// Function to view all databases
void viewDatabases() {
    cout << "Databases:\n";
    for (const auto& db : dbTables) {
        cout << "- " << db.first << endl;
    }
}

// Function to create a table
void createTable(const string& dbName, const string& tableName, const vector<string>& columns) {
    if (dbTables.find(dbName) == dbTables.end()) {
        cout << "Database does not exist.\n";
        return;
    }
    if (find(dbTables[dbName].begin(), dbTables[dbName].end(), tableName) != dbTables[dbName].end()) {
        cout << "Table already exists in database.\n";
        return;
    }
    dbTables[dbName].push_back(tableName);
    tableRows[tableName] = vector<vector<string>>();
    tableRows[tableName].push_back(columns); // Store schema as first row
    cout << "Table '" << tableName << "' created successfully in database '" << dbName << "'.\n";
}

// Function to view all tables in a database
void viewTables(const string& dbName) {
    if (dbTables.find(dbName) == dbTables.end()) {
        cout << "Database does not exist.\n";
        return;
    }
    cout << "Tables in database '" << dbName << "':\n";
    for (const auto& table : dbTables[dbName]) {
        cout << "- " << table << endl;
    }
}

// Function to insert rows into a table
void insertIntoTable(const string& tableName, const vector<string>& rowData) {
    if (tableRows.find(tableName) == tableRows.end()) {
        cout << "Table does not exist.\n";
        return;
    }
    if (rowData.size() != tableRows[tableName][0].size()) {
        cout << "Row data does not match table schema.\n";
        return;
    }
    tableRows[tableName].push_back(rowData);
    cout << "Row inserted into table '" << tableName << "'.\n";
}

// Function to view table in CSV format
void viewTableCSV(const string& tableName) {
    if (tableRows.find(tableName) == tableRows.end()) {
        cout << "Table does not exist.\n";
        return;
    }
    for (const auto& row : tableRows[tableName]) {
        for (size_t i = 0; i < row.size(); ++i) {
            cout << row[i];
            if (i != row.size() - 1) cout << ",";
        }
        cout << endl;
    }
}

// Function to update rows in a table
void updateTable(const string& tableName, const string& conditionColumn, const string& conditionValue, const string& updateColumn, const string& newValue) {
    if (tableRows.find(tableName) == tableRows.end()) {
        cout << "Table does not exist.\n";
        return;
    }

    vector<string>& schema = tableRows[tableName][0];
    size_t conditionIndex = find(schema.begin(), schema.end(), conditionColumn) - schema.begin();
    size_t updateIndex = find(schema.begin(), schema.end(), updateColumn) - schema.begin();

    if (conditionIndex >= schema.size() || updateIndex >= schema.size()) {
        cout << "Invalid column name.\n";
        return;
    }

    bool updated = false;
    for (size_t i = 1; i < tableRows[tableName].size(); ++i) {
        if (tableRows[tableName][i][conditionIndex] == conditionValue) {
            tableRows[tableName][i][updateIndex] = newValue;
            updated = true;
        }
    }

    if (updated) {
        cout << "Table '" << tableName << "' updated successfully.\n";
    } else {
        cout << "No matching rows found to update.\n";
    }
}

// Function to delete rows from a table
void deleteFromTable(const string& tableName, const string& conditionColumn, const string& conditionValue) {
    if (tableRows.find(tableName) == tableRows.end()) {
        cout << "Table does not exist.\n";
        return;
    }

    vector<string>& schema = tableRows[tableName][0];
    size_t conditionIndex = find(schema.begin(), schema.end(), conditionColumn) - schema.begin();

    if (conditionIndex >= schema.size()) {
        cout << "Invalid column name.\n";
        return;
    }

    size_t initialSize = tableRows[tableName].size();
    tableRows[tableName].erase(remove_if(tableRows[tableName].begin() + 1, tableRows[tableName].end(), [&](const vector<string>& row) {
        return row[conditionIndex] == conditionValue;
    }), tableRows[tableName].end());

    if (tableRows[tableName].size() < initialSize) {
        cout << "Row(s) deleted successfully from table '" << tableName << "'.\n";
    } else {
        cout << "No matching rows found to delete.\n";
    }
}

// Main Function
int main() {
    // Sample 1: fileInput1.mdb
    createDatabase("fileInput1.mdb");
    vector<string> columns1 = {"customer_id", "customer_name", "customer_city", "customer_state", "customer_country", "customer_phone", "customer_email"};
    createTable("fileInput1.mdb", "customer", columns1);

    insertIntoTable("customer", {"1", "name1", "city1", "state1", "country1", "phone1", "email1"});
    insertIntoTable("customer", {"2", "name2", "city2", "state2", "country2", "phone2", "email2"});
    insertIntoTable("customer", {"3", "name3", "city3", "state3", "country3", "phone3", "email3"});
    insertIntoTable("customer", {"4", "name4", "city4", "state4", "country4", "phone4", "email4"});

    cout << "\nTable 'customer' in CSV format before updates:\n";
    viewTableCSV("customer");

    // Update sample
    updateTable("customer", "customer_id", "3", "customer_email", "email333");
    cout << "\nTable 'customer' in CSV format after update:\n";
    viewTableCSV("customer");

    // Delete sample
    deleteFromTable("customer", "customer_id", "4");
    cout << "\nTable 'customer' in CSV format after delete:\n";
    viewTableCSV("customer");

    return 0;
}
