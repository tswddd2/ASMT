#include <iostream>
#include <cstdlib>
#include <ctime>
#include "BM_old.cpp"
using namespace std;

const int N = 10000, Trials = 10;

void init_rand() {
    srand(static_cast<unsigned int>(time(0)));
}
int rand_n(int n) {
    return rand() % n;
}

vector<pair<int, int>> edges, interval1, interval2;

void generateIntervalPairs(int n1, int n2) {
    edges.clear();
    interval1.clear();
    interval2.clear();

    set<pair<int, int>> used;
    int sqrtn1 = (int)sqrt(n1);
    int sqrtn2 = (int)sqrt(n2);

    for (int i = 0; i < sqrtn1+sqrtn2; i++) {
        int l1 = rand_n(n1), r1 = l1 + rand_n(sqrtn1);
        if (r1 >= n1) r1 = n1 - 1;
        int l2 = rand_n(n2), r2 = l2 + rand_n(sqrtn2);
        if (r2 >= n2) r2 = n2 - 1;
        interval1.push_back({l1, r1});
        interval2.push_back({l2, r2});
        for (int u = l1; u <= r1; u++) {
            for (int v = l2; v <= r2; v++) {
                if (used.find({u, v}) == used.end()) {
                    edges.push_back({u, v});
                    used.insert({u, v});
                }
            }
        }
    }
}


int main() {
    int n1, n2;
    cout << "Enter n (number of left and right nodes): ";
    cin >> n1; n2 = n1;
    init_rand(); 

    for (int trial = 0; trial < Trials; trial++) {

        cout << "Trial " << trial + 1 << ":\n";

        int sqrtn = (int)sqrt(n2);
        int m = 0;

        // generateEdges(n1, n2);
        generateIntervalPairs(n1, n2);

        // test BM old
        BipGraph g(n1, n2);
        for (auto e : edges)
            g.addEdge(e.first + 1, e.second + 1);

        clock_t start = clock();
        g.hopcroftKarp();
        clock_t end = clock();
        double elapsed = double(end - start) / CLOCKS_PER_SEC;
        g.calcMIS();
        int mis1 = g.MIS_U.size() + g.MIS_V.size();
        cout << "BM old MIS size: " << mis1 << ", " << elapsed << " seconds\n";

        // test BM new
        ofstream fout = ofstream("inp2.txt");
        fout << n1 << ' ' << n2 << ' ' << interval1.size() << '\n';
        for (int i = 0; i < interval1.size(); i++)
            fout << interval1[i].first << ' ' << interval1[i].second << ' ' << interval2[i].first << ' ' << interval2[i].second << '\n';
        fout.close();
        clock_t start2 = clock();
        system("BM_new.exe");
        clock_t end2 = clock();
        double elapsed2 = double(end2 - start2) / CLOCKS_PER_SEC;
        ifstream fin = ifstream("inp3.txt");
        int mis2;
        fin >> mis2;
        fin.close();
        cout << "BM new MIS size: " << mis2 << ", " << elapsed2 << " seconds\n";


        if (mis1 != mis2) {
            cout << "Mismatch!\n";
            return -1;
        } else {
            cout << "Match.\n";
        }
    }

    return 0;
}