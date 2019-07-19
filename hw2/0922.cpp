#include <iostream>
#include <cstring>
using namespace std;

int main()
{
    char _readBuf[65536] = "I love you";
    char A[65536] = {0};
    int actual_legnth = 0;
    for (int i = 0; _readBuf[i] != '\0'; i++)
    {
        actual_legnth++;
    }
    // memecpy(A,_readBuf,sizeof(_readBuf));
    memmove(A, _readBuf, actual_legnth);
    cout<<"this is A   "<<A<<endl;
cout<<"address   "<<_readBuf<<endl;
    cout<<&_readBuf[3]<<endl;

    char *s = _readBuf;
    s+=2;
    printf("%p\n", _readBuf);
    printf("%p\n", s);
    cout<<s-_readBuf<<endl;
    int XD = s-_readBuf;
    cout<<XD<<endl;

    int B[4][2]={1,2,3,4,5,6,7,8};
    int x=4;
    int y=2;
    int **p = new int* [x];
    for(int i=0;i<x;i++)
    {
        *p = new int [y];
        *p++;
    }
    for(int i=0;i<x;i++)
    {
        for(int j=0;j<y;j++)
        {
            p[i][j]=B[i][j];
            cout<<p[i][j]<<endl;
            cout<<"B[i][j]"<<B[i][j]<<endl;
        }
    }
    cout<<B[0][0]<<endl;

}