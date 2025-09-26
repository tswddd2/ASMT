#include <iostream>
#include <cstdlib>
#include <ctime>
#include "BM_old.cpp"
using namespace std;

const int Trials = 10;

int main() {
    for (int trial = 0; trial < Trials; trial++) {

        cout << "Trial " << trial + 1 << ":\n";

        // databuilder
        system("databuilder.exe");

        // test ASMT old
        clock_t start = clock();
        system("ASMT_old.exe");
        clock_t end = clock();
        double elapsed = double(end - start) / CLOCKS_PER_SEC;
        cout << elapsed << " seconds\n";

        // test ASMT new
        clock_t start2 = clock();
        system("ASMT_new.exe");
        clock_t end2 = clock();
        double elapsed2 = double(end2 - start2) / CLOCKS_PER_SEC;
        cout << elapsed2 << " seconds\n";
    }

    return 0;
}