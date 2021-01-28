#include <iostream>
#include <vector>
#include <ctime>

using namespace std;

double randDouble()
{
    return (double)rand() / RAND_MAX;
}

int main()
{
    srand(time(NULL));
    randDouble();

    int done;
    cout << "Enter Any Key To Exit: " << endl;
    cin >> done;
}
