#include <iostream>
#include <cstdio>
#include <random>
#include <ctime>
using namespace std;

const int N = 1000;
int n = N;

void dfs(int l, int r) {
    // cout << l << ' ' << r << ":\n";
    int sp, len = r-l+1, each;
    cout << '(';
    if (len < 5) {
        // cout << "end\n";
        for (int i = l; i < r; i++) {
            cout << i << '|';
        }
        cout << r << ')';
        return ;
    }
    sp = rand() % 4 + 2;
    // cout << "split: "<< sp << ' ' << each << '\n';
    int j = l;
    for (int i = 0; i < sp && j < r; i++) {

        each = (r-j+1) / (sp-i) * 2;
        if (each == 0) break;

        if (i > 0) cout << "|";        
        int slen = min(max(rand()%each, 2), r-j+1);
        dfs(j, j+slen-1);
        j += slen;
    }
    if (j <= r)
        if (r-j+1 >= 5)
            dfs(j, r);
        else
            for (int i = j; i <= r; i++)
                cout << '|' << i;
    cout << ')';
}

int main() {
    srand((unsigned int)time(0));
    freopen("C:\\Users\\tswdd\\Desktop\\AMCT\\inp.txt", "w", stdout);
    cout << n << '\n';
    dfs(0, n-1); cout << '\n';
    dfs(0, n-1); cout << '\n';
    return 0;
}
