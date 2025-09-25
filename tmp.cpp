#include <iostream>
#include <vector>
#include <algorithm>

#include <fstream>

int main() {
    std::ifstream fin("inp2.txt");
    int n, m, k;
    fin >> n >> m >> k;

    std::vector<std::vector<bool>> arr(n, std::vector<bool>(m, false));

    std::vector<std::pair<int, int>> marked;

    for (int t = 0; t < k; ++t) {
        int x1, x2, y1, y2;
        fin >> x1 >> x2 >> y1 >> y2;
        for (int x = x1; x <= x2; ++x) {
            for (int y = y1; y <= y2; ++y) {
                if (!arr[x][y]) {
                    arr[x][y] = true;
                    marked.push_back({x, y});
                }
            }
        }
    }
    std::sort(marked.begin(), marked.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a < b;
    });

    std::cout << marked.size() << '\n';

    std::ofstream fout("test.txt");
    fout << n << ' ' << m << ' ' << marked.size() << '\n';
    for (const auto& p : marked) {
        fout << p.first << ' ' << p.second << '\n';
    }

    return 0;
}