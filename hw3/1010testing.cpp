#include <iostream>
#include <regex>
#include <string>
#include <iomanip>
#include <fstream>
using namespace std;

int main()
{
    ifstream myfile("test2.json");
    //myfile.open("test3.json", ios::in);
    string input;
    regex reg("[ \t]*\"([\\w]+)\"[ \t]*:{1}[ \t]*(-?[0-9]+)[ \t]*");
    smatch sm;
    if (myfile.is_open())
    {
        while (getline(myfile, input, '\n'))
        {

            bool match = regex_search(input, sm, reg);
            cout << (match ? "Matched" : "NOT") << endl;
            cout << input << endl;

            for (int i = 0; i < sm.size(); ++i)
            {
                cout << "sm[" << i << "] = " << sm[i] << endl;
            }
        }
    }
    else
    {
        cerr << "cannot read in" << endl;
    }
    myfile.close();
    if(myfile)
    cout<<"has thing"<<endl;   
    else
    {
        cout<<"empty"<<endl;
    }
    
}