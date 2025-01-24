#include <iostream>
using namespace std;

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
