//
//  main.cpp
//  MariaDBwithCPP
//
//  Created by Eric Hong on 23/12/2024.
//

#include <iostream>
#include "functions/readFile.cpp" //link the readFile.cpp to main.cpp

using namespace std;

int main() {
    string inputFile = "../source/inputOutput/fileInput1.mdb";
    readFile(inputFile);
}
