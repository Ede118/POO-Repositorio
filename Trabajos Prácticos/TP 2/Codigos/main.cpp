// main.cpp
#include "App.h"
#include <cstdlib>


int main(int argc, char** argv) {
    
    int ret = system("stty -F /dev/ttyACM0 9600 raw -echo -ixon -ixoff");
    if (ret != 0) {
        return 1;
    }
    
    App app;
    return app.run(argc, argv);
}
    