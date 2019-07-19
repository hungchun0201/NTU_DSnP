#include <iostream>
#include <regex>
#include <string>

using namespace std;

int main()
{
    while (1)
    {
        string input;
        getline(cin, input);
        // cin>>input;
        regex reg("[ ]*\"([\\w]+)\"[ ]*:{1}[ ]*(-?[0-9]+)[ ]*");
        smatch sm;
        bool match = regex_search(input, sm, reg);
        cout << (match ? "Matched" : "NOT") << endl;
        cout<<input<<endl;
        
        for (int i = 0; i < sm.size() ; ++i)
        {
            cout << "sm["<<i<<"] = "<<sm[i]<<endl;
        }
    }
}