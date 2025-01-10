//
//  main.cpp
//  MariaDBwithCPP
//
//  Created by Eric Hong on 23/12/2024.
//

#include <iostream>
#include <fstream>

using namespace std;

bool checkDatabase();

int main() {
    bool database=checkDatabase();

    if(database==true){ //if database
        cout<<"Database Exist!\n";
    }
    else if(database==false){//if !database
        cout<<"Database DNE!\n";
    }


    return 0;
}

//check if DB exist
bool checkDatabase(){
    char condition;
    cout<<"Do the database exist?\nEnter Y for yes, F for no\n";
    cin>>condition;
    if(condition=='Y'){
        return true;
    }
    else if(condition=='F'){
        return false;
    }
    else{
        return false;
    }
}


//Create database function
void functionCreateDB(){
    
};
