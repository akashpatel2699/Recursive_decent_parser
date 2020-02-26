//=================================================================================
// Name        : Part2Test.cpp
// Description : Complete Parse Tree 
//=================================================================================

#include <iostream>
#include <fstream>
#include <string>

#include "parsetreeSKEL.h"
#include "parse.h"
using namespace std;

int main (int argc, char *argv[])
{
    int line = 0;
    istream *in = &cin;
    ifstream file;
    if (argc > 2)
    {
        cout << "TOO MANY FILENAMES" << endl;
        return 0;
    }
    if (argc == 2)
    {
        file.open(argv[1]);
        if (file.is_open() == false)
        {
            cout << "COULD NOT OPEN " << argv[1] << endl;
            return 0;
        }
        in = &file;
        
    }
    ParseTree *tree = Prog(*in,line);
    if (tree == 0) //empty file 
    {
        return 0;
    }
    cout << "BANG COUNT: " << tree->BangCount() << endl;
    cout << "MAX DEPTH: " << tree->MaxDepth() << endl;
    return 0;
}