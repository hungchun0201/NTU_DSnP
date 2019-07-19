#include <iostream>
#include <regex>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>

using namespace std;

#define PI 222
#define pl 223


class A
{
    public:
    static int mem1 ;
    
    ostream＆　operator<<(ostream& os,A a)
    {

    }
};

int main()
{
    vector<int> _container;
    _container.push_back(3);
    _container.push_back(5);
    cout<<_container.size()<<endl;
    vector<int>::iterator it = _container.begin();
    cout<<*it<<endl;
    // auto int a = 10;
    
    for(int i = 0 ;i<10;i++)
    {
        int b = 0;
        cout<<b++<<endl;;
    }

    const int i = 10;
    


}