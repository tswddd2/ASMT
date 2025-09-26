#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

const int Trials = 10;

int main() {
    int n; double r;
    cout << "Enter n (number of leaves) and r (edge contraction probability): ";
    cin >> n >> r;
    for (int trial = 0; trial < Trials; trial++) {

        cout << "Trial " << trial + 1 << ":\n";

        // databuilder
        system(("databuilder.exe " + to_string(n) + " " + to_string(r)).c_str());

        // Run ASMT_old.exe and capture output and time
        clock_t start = clock();
        system("ASMT_old.exe > old_output.txt");
        clock_t end = clock();
        double elapsed = double(end - start) / CLOCKS_PER_SEC;

        ifstream oldfile("old_output.txt");
        string old_output((istreambuf_iterator<char>(oldfile)), istreambuf_iterator<char>());
        oldfile.close();

        cout << "ASMT_old.exe runtime: " << elapsed << " seconds\n";

        // Run ASMT_new.exe and capture output and time
        clock_t start2 = clock();
        system("ASMT_new.exe > new_output.txt");
        clock_t end2 = clock();
        double elapsed2 = double(end2 - start2) / CLOCKS_PER_SEC;

        ifstream newfile("new_output.txt");
        string new_output((istreambuf_iterator<char>(newfile)), istreambuf_iterator<char>());
        newfile.close();

        cout << "ASMT_new.exe runtime: " << elapsed2 << " seconds\n";

        // Compare outputs
        if (old_output == new_output) {
            cout << "Output matches.\n";
        } else {
            cout << "Output differs!\n";
        }

        cout << endl;
    }

    return 0;
}