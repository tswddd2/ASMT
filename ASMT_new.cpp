#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <algorithm>
#include <string>
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

struct TreeNode
{
	vector<int> leaves, rleaves;
	vector<int> sons;
	set<Node> rsons;
	int size, depth, father;
	int vertexId, pathId;			// vertex id is the order for bipartite graph
	int sel;
};

struct Path
{
	int top, bottom;	// id of tree nodes, not "pnum"
	int father;			// id of previous path
};

struct PathPair
{
	int pathId[2];
	vector<int> leaves;
};
struct PathNode {
	int vertexId, nodeId, neated, overlapSize;
	vector<int> leaves;
};

struct Leaf
{
	int node[2];
};

struct Traid {
	int x[2], y;
};
bool compTraid(Traid a, Traid b) {
	return (a.x[0] < b.x[0] || (a.x[0] == b.x[0] && a.x[1] < b.x[1]));
};
struct Quad {
	int x[2], y[2];
};

vector<TreeNode> trees[2];
vector<Path> paths[2];
vector<PathPair> pathPairs;
vector<Leaf> leaves;
vector<Traid> traids;

int n;

/*
	initialize trees[2]
*/
string treeStr;
int buildTree(int treeId, int fatherId, int index)
{
	trees[treeId].push_back(TreeNode());
	int nodeId = trees[treeId].size() - 1;
	trees[treeId][nodeId].father = fatherId;
	trees[treeId][nodeId].depth = (fatherId >= 0 ? trees[treeId][fatherId].depth + 1 : 0);

	while (treeStr[index] != ')') {
		if (treeStr[index] == '|')
			index++;
		else if (isdigit(treeStr[index])) {
			int digits = 0;
			trees[treeId][nodeId].size++;
			while (isdigit(treeStr[index])) {
				digits = digits * 10 + treeStr[index] - '0';
				index++;
			}
			trees[treeId][nodeId].leaves.push_back(digits);

			leaves[digits].node[treeId] = nodeId;
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

int pnum;
void buildCentroidPaths(int treeId, int nodeId, int pathId, int fatherPath = -1)
{
	if (nodeId == 0)
		pnum = 0;

	trees[treeId][nodeId].vertexId = pnum++;
	trees[treeId][nodeId].pathId = pathId;

	// init newly created path
	if (pathId == paths[treeId].size()) {
		paths[treeId].push_back(Path());
		paths[treeId][pathId].top = nodeId;
		paths[treeId][pathId].father = fatherPath;
	}

	// extend current path
	if (trees[treeId][nodeId].sons.size() == 0)
		paths[treeId][pathId].bottom = nodeId;
	else {
		// determine next son with largest subtree
		int nextSonId = -1;
		for (auto sonId : trees[treeId][nodeId].sons)
			if (nextSonId == -1 || trees[treeId][sonId].size > trees[treeId][nextSonId].size)
				nextSonId = sonId;
		buildCentroidPaths(treeId, nextSonId, pathId);

		// create paths for other subtrees
		for (auto sonId : trees[treeId][nodeId].sons)
			if (sonId != nextSonId)
				buildCentroidPaths(treeId, sonId, paths[treeId].size(), pathId);
	}

	//cout << nodeId << ' ' << trees[treeId][nodeId].pnum << ' ' << trees[treeId][nodeId].pathId << '\n';
}

void init()
{
	ifstream fin("inp.txt");
	fin >> n;
	leaves.resize(n, Leaf());
	fin >> treeStr;
	buildTree(0, -1, 1);
	buildCentroidPaths(0, 0, 0);

	fin >> treeStr;
	buildTree(1, -1, 1);
	buildCentroidPaths(1, 0, 0);

	// for each leaf: find overlap path pairs
	vector<Traid> pathPairsByLeaf;
	for (int leafId = 0; leafId < leaves.size(); leafId++) {
		Leaf leaf = leaves[leafId];
		int path1 = trees[0][leaf.node[0]].pathId;
		while (path1 >= 0) {
			int path2 = trees[1][leaf.node[1]].pathId;
			while (path2 >= 0) {
				Traid pathPairByLeaf = Traid();
				pathPairByLeaf.x[0] = path1; pathPairByLeaf.x[1] = path2;
				pathPairByLeaf.y = leafId;
				pathPairsByLeaf.push_back(pathPairByLeaf);
				path2 = paths[1][path2].father;
			}
			path1 = paths[0][path1].father;
		}
	}
	sort(pathPairsByLeaf.begin(), pathPairsByLeaf.end(), compTraid);

	// aggregate overlap leaf set to path pairs
	for (int index = 0; index < pathPairsByLeaf.size(); ) {
		PathPair pathPair;
		pathPair.pathId[0] = pathPairsByLeaf[index].x[0];
		pathPair.pathId[1] = pathPairsByLeaf[index].x[1];
		// Collect all leaves for this path pair
		while (index < pathPairsByLeaf.size() &&
			   pathPairsByLeaf[index].x[0] == pathPair.pathId[0] &&
			   pathPairsByLeaf[index].x[1] == pathPair.pathId[1]) {
			pathPair.leaves.push_back(pathPairsByLeaf[index].y);
			index++;
		}
		pathPairs.push_back(pathPair);
	}
}

vector<PathNode> pathNodes[2];
void getPathNodes(int treeId, PathPair pathPair)
{
	traids.clear();
	pathNodes[treeId].clear();
	int pathId = pathPair.pathId[treeId];
	int top = trees[treeId][paths[treeId][pathId].top].vertexId;
	int bottom = trees[treeId][paths[treeId][pathId].bottom].vertexId;

	// for each overlap leaf, find the first ancestor contained in path
	for (auto leafId : pathPair.leaves) {
		Traid traid = Traid();
		int nodeId = leaves[leafId].node[treeId];
		while (true) {
			int vertexId = trees[treeId][nodeId].vertexId;
			if (top <= vertexId && vertexId <= bottom)
				break;
			nodeId = paths[treeId][trees[treeId][nodeId].pathId].top;
			nodeId = trees[treeId][nodeId].father;
		}
		traid.x[0] = nodeId;
		traid.y = leafId;
		traids.push_back(traid);
	}

	// group leaves by nodes (from top to bottom)
	sort(traids.begin(), traids.end(), compTraid);
	// get nodeId, vertexId, leaves
	for (int traidId = 0; traidId < traids.size(); traidId++) {
		Traid traid = traids[traidId];
		if (traidId == 0 || traid.x[0] != traids[traidId - 1].x[0]) {
			PathNode pathNode = PathNode();
			pathNode.nodeId = traid.x[0];
			pathNode.vertexId = trees[treeId][traid.x[0]].vertexId;
			pathNodes[treeId].push_back(pathNode);
		}
		if (traid.y >= 0)
			pathNodes[treeId].back().leaves.push_back(traid.y);
	}

	// get overlapSize and neated (if overlap size equals to subtree size)
	int overlapSize = 0;
	for (int i = pathNodes[treeId].size() - 1; i >= 0; i--) {
		PathNode &pathNode = pathNodes[treeId][i];
		overlapSize += pathNode.leaves.size();
		pathNode.overlapSize = overlapSize;
		if (trees[treeId][pathNode.nodeId].size == overlapSize)
			pathNode.neated = 1;
	}
}

vector<Quad> tres, result;
vector<int> va, vb;
void calculate(int ppid) {
    //cout << "calculate:\n";
    int pni, previous, ai, bi, a2, b2, rel;
    int r0 = trees[0][paths[0][pathPairs[ppid].pathId[0]].top].vertexId;
    int r1 = trees[1][paths[1][pathPairs[ppid].pathId[1]].top].vertexId;

    // pni: current node to get interval pair, start from top to bottom
    pni = 0;

    // ai: first node in path 1 that can't contain pni
    // bi: last node in path 1 can be contained by pni (when rel is 1)
    //     or disjoint with pni (when rel is 0)
    ai = 0; bi = pathNodes[1].size();
    rel = 1;

    // va: frequency of leaves within subtree pni and ai
    // vb: frequency of leaves within subtree pni and bi
    for (auto leaf : pathPairs[ppid].leaves)
        va[leaf] = 2, vb[leaf] = 1;
    // a2, b2: number of leaves with frequency 2 in va, vb
    a2 = pathPairs[ppid].leaves.size();
    b2 = 0;

    // bi located to deepest neated node in pathNodes[1]
    // add leaves frequency under bi in vb
    while (bi > 0 && pathNodes[1][bi - 1].neated) {
        bi--;
        for (auto leaf : pathNodes[1][bi].leaves) {
            vb[leaf]++; if (vb[leaf] == 2) b2++;
        }
    }

    tres.clear();
    previous = r0;
    for (pni = 0; pni < pathNodes[0].size(); pni++) {
        // step 1: update ai and bi for current pni
        // lower down ai if current ai can contain pni
        while (ai < pathNodes[1].size() && pathNodes[0][pni].neated && pathNodes[0][pni].overlapSize == a2) {
            for (auto leaf : pathNodes[1][ai].leaves) {
                va[leaf]--; if (va[leaf] == 1) a2--;
            }
            ai++;
        }
        // rel is 1: lower down bi until bi is contained by pni
        if (rel) {
            while (bi < pathNodes[1].size() && pathNodes[1][bi].overlapSize > b2) {
                for (auto leaf : pathNodes[1][bi].leaves) {
                    vb[leaf]--; if (vb[leaf] == 1) b2--;
                }
                bi++;
            }
        }
        // when bi and pni no longer overlap, set rel to 0
        if (b2 == 0) {  
            rel = 0;
		}
        // rel is 0: move up bi if bi is disjoint to pni
        if (!rel) {
            while (bi >= 0 && b2 == 0) {
                bi--;
                if (bi < 0) break;
                for (auto leaf : pathNodes[1][bi].leaves) {
                    vb[leaf]++; if (vb[leaf] == 2) b2++;
                }
            }
        }

        // step 2: get interval pairs for current pni
        // last & now: vertexId of left interval
        // ta & tb: vertexId of right interval
        int now = pathNodes[0][pni].vertexId;
        int ta = ai > 0 ? pathNodes[1][ai - 1].vertexId + 1 : r1;
        int tb = pathNodes[1][bi].vertexId - rel;
        if (previous < now && tb >= r1) {           //bi - rel >= 0 before change
            Quad tmp = { {previous, now - 1} , {r1, tb} };
            tres.push_back(tmp);
        }
        if (ta <= tb) {                             //ai <= bi - rel before change
            Quad tmp = { {now, now} , {ta, tb} };
            tres.push_back(tmp);
        }

        // pni go down: reduce frequency of leaves
        for (auto leaf : pathNodes[0][pni].leaves) {
            va[leaf]--; vb[leaf]--;
            if (va[leaf] == 1) a2--;
            if (vb[leaf] == 1) b2--;
        }
        previous = now + 1;
    }

    for (auto res : tres) {
        if (res.y[0] > res.y[1])
            continue;
        if (result.size() > 0 && result.back().y[0] == res.y[0] && result.back().y[1] == res.y[1]
            && result.back().x[1] + 1 == res.x[0])
            result.back().x[1] = res.x[1];
        else
            result.push_back(res);
    }

    for (auto leaf : pathPairs[ppid].leaves)
        va[leaf] = vb[leaf] = 0;
}

void work() {
	va.resize(n, 0);
	vb.resize(n, 0);
	for (int i = 0; i < pathPairs.size(); i++) {
		PathPair pathPair = pathPairs[i];

		getPathNodes(0, pathPair);
		getPathNodes(1, pathPair);

		calculate(i);
	}
}

void print() {
	ofstream fout("inp2.txt");
	fout << trees[0].size() << ' ' << trees[1].size() << ' ' << result.size() << '\n';
	for (auto res : result) {
		fout << res.x[0] << ' ' << res.x[1] << ' ' << res.y[0] << ' ' << res.y[1] << '\n';
	}
}

void bipartiteMap() {
	init();
	work();
	print();
}

int k;
vector<int> toid[2];
vector<Node> ton, color[2];
vector<Traid> order;

#define Color(c) color[c.treeId][c.nodeId]
#define Trees(c) trees[c.treeId][c.nodeId]

Node find(Node c) {
	if (Color(c) == c)
		return c;
	else
		return Color(c) = find(Color(c));
}

void cal() {

	system("BM_new.exe");

	toid[0].resize(trees[0].size(), 0);
	toid[1].resize(trees[1].size(), 0);
	color[0].resize(trees[0].size(), { 0, 0 });
	color[1].resize(trees[1].size(), { 0, 0 });
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < trees[i].size(); j++) {
			toid[i][trees[i][j].vertexId] = j;
			color[i][j] = { i, j };
		}

	ifstream fin("inp3.txt");
	fin >> k;
	for (int i = 0; i < k; i++) {
		int treeId, nodeId, pnum;
		fin >> treeId >> pnum;
		nodeId = toid[treeId][pnum];
		if (trees[treeId][nodeId].leaves.size() > 0 || trees[treeId][nodeId].sons.size() > 1) {
			order.push_back({ {trees[treeId][nodeId].size, treeId}, nodeId });
			trees[treeId][nodeId].sel = true;
		}
	}

	ton.resize(n, { -1, -1 });
	sort(order.begin(), order.end(), compTraid);
	cout << k + 2*n << '\n';
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
	output += '(';
	for (auto rson : Trees(now).rsons) {
		dfs2(rson);
		output += '|';
	}
	for (auto leaf : Trees(now).rleaves)
		output += tree_to_string(leaf) + '|';
	output[output.length() - 1] = ')';
}

void stage2() {
	for (auto oc : order)
		dfs1(oc.x[1], oc.y, oc.y);

	dfs2({ 0, 0 });
	ofstream fout("oup.txt");
	fout << output << '\n';
}

int main() {
	bipartiteMap();
	cal();
	stage2();
	return 0;
}
