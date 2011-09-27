#include <Windows.h>
#include <iostream>
using namespace std;
 
class myClass
{
public:
        void test();
        int lol;
};
 
void myClass::test()
{
        this->lol = 42;
}
 
int main()
{
        void* testFunc = NULL;
        void (myClass::*addr)() = (void (myClass::*)())&myClass::test;
 
        myClass* meh = new myClass();
        meh->lol = 1337;
 
        __asm
        {
                push eax; // backup eax
                mov eax,addr; // copy the address of myClass::test into eax
                mov testFunc,eax; // copy the value of eax into testFunc
                pop eax; // restore eax
 
                mov ecx,meh; // assign our instance meh as "this"
                call testFunc; // call myClass::test
        }
 
        // meh->lol is now 42
 
        return 0;
}