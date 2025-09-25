#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <sstream>
#include "BM_old.cpp"
using namespace std;

struct Node {
	int treeId, nodeId;
};
bool operator==(const Node& lhs, const Node& rhs)
{
	return lhs.treeId == rhs.treeId && lhs.nodeId == rhs.nodeId;
}
bool operator<(const Node& lhs, const Node& rhs)
{
	return lhs.treeId < rhs.treeId || (lhs.treeId == rhs.treeId && lhs.nodeId < rhs.nodeId);
}
struct Leaf
{
	int node[2];
};

struct TreeNode
{
	vector<int> leaves, rleaves;
	vector<int> sons;
	set<Node> rsons;
	int size, depth, father;        // size: leaf count in subtree
	bool sel;				   		// selected or not
};

int n;
vector<TreeNode> trees[2];
vector<Leaf> leaves;
string treeStr;


struct Traid {
	int x[2], y;
};
bool compTraid(Traid a, Traid b) {
	return (a.x[0] < b.x[0] || (a.x[0] == b.x[0] && a.x[1] < b.x[1]));
};
vector<Traid> order;


int buildTree(int treeId, int fatherId, int index)
{
    trees[treeId].push_back(TreeNode());
    int nodeId = trees[treeId].size() - 1;
    trees[treeId][nodeId].father = fatherId;
    trees[treeId][nodeId].depth = (fatherId >= 0 ? trees[treeId][fatherId].depth + 1 : 0);

    while (treeStr[index] != ')') {
        if (treeStr[index] == '|') {
            index++;
        }
        else if (isdigit(treeStr[index])) {
            int digits = 0;
            while (isdigit(treeStr[index])) {
                digits = digits * 10 + treeStr[index] - '0';
                index++;
            }

            trees[treeId].push_back(TreeNode());
            int leafNodeId = trees[treeId].size() - 1;
            trees[treeId][leafNodeId].father = nodeId;
            trees[treeId][leafNodeId].depth = trees[treeId][nodeId].depth + 1;
            trees[treeId][leafNodeId].leaves.push_back(digits);
            trees[treeId][leafNodeId].size = 1;

            trees[treeId][nodeId].sons.push_back(leafNodeId);
            trees[treeId][nodeId].size += 1;

            leaves[digits].node[treeId] = leafNodeId;
        }
        else if (treeStr[index] == '(') {
            int sid = trees[treeId].size();
            index = buildTree(treeId, nodeId, index + 1);
            trees[treeId][nodeId].sons.push_back(sid);
            trees[treeId][nodeId].size += trees[treeId][sid].size;
            index++;
        }
        else throw "input format error";
    }

    return index;
}

vector<vector<int>> overlapSize;				// overlapSize[i][j]: overlap leaf size of trees[0][i] and trees[1][j]
vector<pair<int, int>> bipartiteEdges;			// edges for bipartite graph

void getBipartiteEdges(int u, int v) {
	if (overlapSize[u][v] >= 0) return;	// already computed

	for (int sonU: trees[0][u].sons)
		getBipartiteEdges(sonU, v);
	for (int sonV : trees[1][v].sons)
		getBipartiteEdges(u, sonV);

	int hasSonU = trees[0][u].sons.size() > 0;
	int hasSonV = trees[1][v].sons.size() > 0;

	overlapSize[u][v] = 0;
	if (hasSonV) {
		for (int sonV : trees[1][v].sons)
			overlapSize[u][v] += overlapSize[u][sonV];
	}
	else if (hasSonU) {
		for (int sonU : trees[0][u].sons)
			overlapSize[u][v] += overlapSize[sonU][v];
	}
	else overlapSize[u][v] = (trees[0][u].leaves[0] == trees[1][v].leaves[0] ? 1 : 0);

	if (overlapSize[u][v] > 0 && overlapSize[u][v] < min(trees[0][u].size, trees[1][v].size)) {
		bipartiteEdges.push_back(make_pair(u, v));	// u->v
	}
}

void init()
{
	ifstream fin("inp.txt");
	fin >> n;
	leaves.resize(n, Leaf());
	fin >> treeStr;
	buildTree(0, -1, 1);

	fin >> treeStr;
	buildTree(1, -1, 1);

    overlapSize.resize(trees[0].size());
    for (int i = 0; i < trees[0].size(); i++) {
        overlapSize[i].resize(trees[1].size(), -1);
	}
}

void bipartiteMap() {

	ofstream fout("debug.txt");

	getBipartiteEdges(0, 0);
	BipGraph g(trees[0].size(), trees[1].size());
	fout << trees[0].size() << ' ' << trees[1].size() << ' ' << bipartiteEdges.size() << '\n';
	for (auto e : bipartiteEdges) {
		g.addEdge(e.first+1, e.second+1);
		fout << e.first+1 << " " << e.second+1 << '\n';
	}
	g.hopcroftKarp();
	g.calcMIS();

	for (auto u : g.MIS_U) {
		order.push_back({ {trees[0][u - 1].size, 0}, u - 1 });
		trees[0][u - 1].sel = true;
	}
	for (auto v : g.MIS_V) {
		order.push_back({ {trees[1][v - 1].size, 1}, v - 1 });
		trees[1][v - 1].sel = true;
	}
	sort(order.begin(), order.end(), compTraid);

	cout << g.MIS_U.size() + g.MIS_V.size() << '\n';
}

vector<Node> ton, color[2];
#define Color(c) color[c.treeId][c.nodeId]
#define Trees(c) trees[c.treeId][c.nodeId]
Node find(Node c) {
	if (Color(c) == c)
		return c;
	else
		return Color(c) = find(Color(c));
}

void dfs1(int treeId, int nodeId, int fatherId) {
	Node father = { treeId, fatherId }, tson;
	Node now = { treeId, nodeId };
	if (nodeId != fatherId && Trees(now).sel) {
		tson = find({ treeId, nodeId });
		if (!(tson == father))
			Trees(father).rsons.insert(tson);
		Color(tson) = father;
		return;
	}
	for (auto leaf : trees[treeId][nodeId].leaves) {
		if (ton[leaf].treeId == -1) {
			ton[leaf] = father;
			Trees(father).rleaves.push_back(leaf);
		}
		else {
			tson = find(ton[leaf]);
			if (!(tson == father))
				Trees(father).rsons.insert(tson);
			Color(tson) = father;
		}
	}
	for (auto sid : Trees(now).sons)
		dfs1(treeId, sid, fatherId);
}

string output;

string tree_to_string(int x) {
    string s;
    while (x > 0) {
        s = (char)(x % 10 + '0') + s;
        x /= 10;
    }
    if (s.length() == 0)
        s = "0";
    return s;
}

void dfs2(Node now) {
	if (Trees(now).rsons.size() == 1 && Trees(now).rleaves.size() == 0) {
		dfs2(*(Trees(now).rsons.begin()));
		return;
	}
	int elementCount = Trees(now).rsons.size() + Trees(now).rleaves.size();
	if (elementCount > 1)
		output += '(';
	for (auto rson : Trees(now).rsons) {
		dfs2(rson);
		output += '|';
	}
	for (auto leaf : Trees(now).rleaves)
		output += tree_to_string(leaf) + '|';
	if (elementCount > 1)
		output[output.length() - 1] = ')';
	else
		output.pop_back();
}


void print() {
	ton.resize(n, { -1, -1 });
	color[0].resize(trees[0].size(), { 0, 0 });
	color[1].resize(trees[1].size(), { 0, 0 });
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < trees[i].size(); j++) {
			color[i][j] = { i, j };
		}
	for (auto oc : order)
		dfs1(oc.x[1], oc.y, oc.y);

	dfs2({ 0, 0 });
	ofstream fout("oup.txt");
	fout << output << '\n';
}

int main() {
	init();
	bipartiteMap();
	print();
	return 0;
}
