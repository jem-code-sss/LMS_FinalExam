#include <iostream>
#include "LMS.h"
using namespace std;

int main() {
    LibrarySystem sys;
    sys.LoadFromFile();
    sys.Run();
    sys.SaveToFile();
    return 0;
}