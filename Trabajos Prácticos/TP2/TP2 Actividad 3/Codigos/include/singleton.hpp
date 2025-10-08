//singleton.hpp
class Singleton
{
  private:
    int value;
    static Singleton *_instance;
    Singleton();
    Singleton(const Singleton&);
    Singleton & operator=(const Singleton &);
    ~Singleton();
     
  public:
    int getVal();
    void setVal(int);
    static Singleton* getInstance();
};
 
Singleton* Singleton::_instance;