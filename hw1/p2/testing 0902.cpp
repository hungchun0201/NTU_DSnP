#include <iostream>
#include <regex>
using namespace std;

int main()
{

    regex reg(".*\"(.*)(\")(.*)([0-9]*)");
    regex reg2("((-)?[0-9]+)");
    string input,input2,want1,want2;
    getline(cin, input);
    input2=input;
    smatch sm;
    smatch sm2;
    int count = 0;
    while (regex_search(input, sm, reg))
    {
        ++count;
        cout << "Match " << count << ":\n";
        cout << "The matches are:\n";
        for (int i = 0; i < sm.size(); ++i)
        {
            cout << i << ": [" << sm[i] << ']' << endl;
        }
    want1=sm[1].str();
    cout<<want1<<endl;
    //input = sm.suffix().str();
    }
            cout << "input2" << input2 << endl;        
    while (regex_search(input2, sm2, reg2))
    {
        cout << "in while2" << input2 << endl;
        ++count;
        cout << "Match " << count << ":\n";
        cout << "The matches are:\n";
        for (int i = 0; i < sm2.size(); ++i)
        {
            cout << i << ": [" << sm2[i] << ']' << endl;
        }
        want2=sm2[1].str();
        cout<<want2<<endl;
        input2 = sm2.suffix().str();
    }

    return 0;
}