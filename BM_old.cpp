#include<bits/stdc++.h>
using namespace std;
#define NIL 0
#define INF INT_MAX

class BipGraph {
    int m, n;
    list<int> *adj;
    int *pairU, *pairV, *dist;
    vector<bool> visU, visV;

public:
    vector<int> MIS_U, MIS_V;
    BipGraph(int m, int n);
    void addEdge(int u, int v);
    bool bfs();
    bool dfs(int u);
    int hopcroftKarp();
    void calcMIS();
    void dfs_mark(int u);
};

int BipGraph::hopcroftKarp() {
    pairU = new int[m + 1];
    pairV = new int[n + 1];
    dist = new int[m + 1];
    for (int u = 0; u <= m; u++) pairU[u] = NIL;
    for (int v = 0; v <= n; v++) pairV[v] = NIL;
    int result = 0;
    while (bfs()) {
        for (int u = 1; u <= m; u++)
            if (pairU[u] == NIL && dfs(u))
                result++;
    }
    return result;
}

bool BipGraph::bfs() {
    queue<int> Q;
    for (int u = 1; u <= m; u++) {
        if (pairU[u] == NIL) {
            dist[u] = 0;
            Q.push(u);
        } else dist[u] = INF;
    }
    dist[NIL] = INF;
    while (!Q.empty()) {
        int u = Q.front(); Q.pop();
        if (dist[u] < dist[NIL]) {
            for (int v : adj[u]) {
                if (dist[pairV[v]] == INF) {
                    dist[pairV[v]] = dist[u] + 1;
                    Q.push(pairV[v]);
                }
            }
        }
    }
    return (dist[NIL] != INF);
}

bool BipGraph::dfs(int u) {
    if (u != NIL) {
        for (int v : adj[u]) {
            if (dist[pairV[v]] == dist[u] + 1) {
                if (dfs(pairV[v])) {
                    pairV[v] = u;
                    pairU[u] = v;
                    return true;
                }
            }
        }
        dist[u] = INF;
        return false;
    }
    return true;
}

BipGraph::BipGraph(int m, int n) {
    this->m = m;
    this->n = n;
    adj = new list<int>[m + 1];
}

void BipGraph::addEdge(int u, int v) {
    adj[u].push_back(v);
}

// Mark reachable vertices from free vertices in U
void BipGraph::dfs_mark(int u) {
    visU[u] = true;
    for (int v : adj[u]) {
        if (!visV[v] && pairU[u] != v) { // Not visited and not a matching edge
            visV[v] = true;
            if (pairV[v] && !visU[pairV[v]])
                dfs_mark(pairV[v]);
        }
    }
}

// Find maximum independent set
void BipGraph::calcMIS() {
    visU.assign(m + 1, false);
    visV.assign(n + 1, false);
    // Start from all unmatched vertices in U
    for (int u = 1; u <= m; u++) {
        if (pairU[u] == NIL && !visU[u]) {
            dfs_mark(u);
        }
    }
    for (int u = 1; u <= m; u++) {
        if (visU[u]) MIS_U.push_back(u);
    }
    for (int v = 1; v <= n; v++) {
        if (!visV[v]) MIS_V.push_back(v); 
    }
}
