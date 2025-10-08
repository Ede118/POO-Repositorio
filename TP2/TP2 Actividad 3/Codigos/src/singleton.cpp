//singleton.cpp
#include "singleton.hpp"
#include <iostream>
 
using namespace std;
 
Singleton::Singleton()
{
 cout<<"Singleton constructor"<<endl;
}
 
Singleton* Singleton::getInstance()
{
 if(!_instance)
   _instance = new Singleton;
  return _instance;
}
 
Singleton::~Singleton()
{
  cout<<"Singleton destructor"<<endl; 
}
 
int Singleton :: getVal()
{
  return value;  
}
 
void Singleton :: setVal(int val)
{
  value = val;
}
 
int main()
{
  Singleton::getInstance()->setVal(20);
  cout << Singleton::getInstance()->getVal();   
}