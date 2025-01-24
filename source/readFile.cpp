#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int choice;

int main()
{
    // Define the path for the input file and output file
    string inputFilename = "/Users/erichong/Documents/Assignment/MariaDBwithCPP/source/inputOutput/fileInput1.mdb"; // Path to the input file
    string outputFilename = "output.txt"; // Path to the output file

    cin>>choice;

    if(choice==1){
        // Open the input file in read mode
        ifstream infile(inputFilename);
        if (!infile)
        {
            // If the file cannot be opened, display an error message
            cout << "Error: Could not open " << inputFilename << " for reading." << endl;
            return 1;
        }

        // Read and display the contents of the file line by line
        cout << "Contents of " << inputFilename << ":" << endl;
        string line;
        while (getline(infile, line)) // Read each line from the file
        {
            cout << line << endl; // Print each line to the console
        }

        // Close the file after reading
        infile.close();
    }
    // If the user chooses to add new data to the file
    else if (choice == 2)
    {
        // Open the output file in append mode to keep old data and add new data
        ofstream outfile(outputFilename, ios::app); // ios::app opens the file in append mode
        if (!outfile)
        {
            // If the file cannot be opened, display an error message
            cout << "Error: Could not open " << outputFilename << " for writing." << endl;
            return 1;
        }

        // Prompt the user to enter data to append to the file
        cout << "Enter data to append to the file (" << outputFilename << ") (type 'exit' to stop):" << endl;
        string input;
        while (true)
        {
            // Get input from the user line by line
            getline(cin, input);
            if (input == "exit") // If the user types 'exit', stop adding data
            {
                break;
            }
            // Append the entered data to the output file
            outfile << input << endl;
        }

        // Close the output file after writing
        outfile.close();
        cout << "Data has been appended to " << outputFilename << " successfully." << endl;
    }
    else
    {
        // If the user entered an invalid choice, display an error message
        cout << "Invalid choice. Please run the program again and choose 1 or 2." << endl;
    }

    return 0;
}
