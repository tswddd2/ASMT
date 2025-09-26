#include <iostream>
#include <fstream>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <set>
#define seti set<int>::iterator
using namespace std;

struct gNode {
	int gi, nid;
};
struct Segment {
	int l, r;
};

int gn[2], m;
vector<Segment> seg[2];

set<int> unv;
vector<set<int>> B;
void bAdd(int nid, set<int> &b) {
	b.insert(nid);
}
void bFill(int gi, set<int> &b) {
	unv.clear();
	for (int i = 0; i < gn[gi]; i++)
		unv.insert(i);
}
int bGetOne(int l, int r, set<int> &b) {
	seti ar = b.lower_bound(l);
	if (ar == b.end()) return -1;
	int res = *ar;
	if (res <= r) {
		b.erase(res);
		return res;
	}
	return -1;
}
void bGetList(int l, int r, set<int> &b, vector<int> &res) {
	res.clear();
	seti lb = b.lower_bound(l), ub = b.upper_bound(r);
	for (seti i = lb; i != ub; i++) {
		res.push_back(*i);
	}
	b.erase(lb, ub);
}

struct stNode {
	int l, r, lson, rson;
	vector<int> sid;
};
int tLastX;
vector<stNode> intervalTree;
vector<int> remlable;

int buildIntervalTree(int l, int r) {
	int x = intervalTree.size();
	stNode now = stNode();
	now.l = l, now.r = r;
	intervalTree.push_back(now);
	if (l == r)
		now.lson = now.rson = -1;
	else {
		int mid = (l + r) / 2;
		now.lson = buildIntervalTree(l, mid);
		now.rson = buildIntervalTree(mid + 1, r);
	}
	intervalTree[x] = now;
	return x;
}
void addInterval(int x, int l, int r, int sid) {
	int sl = intervalTree[x].l, sr = intervalTree[x].r;
	int mid = (sl + sr) / 2;
	if (l <= sl && sr <= r) {
		intervalTree[x].sid.push_back(sid);
	}
	else {
		if (l <= mid)
			addInterval(intervalTree[x].lson, l, r, sid);
		if (mid + 1 <= r)
			addInterval(intervalTree[x].rson, l, r, sid);
	}
}

void tRemove(int sid) { remlable[sid] = 1; }
void tBuild(int gi) {
	intervalTree.clear();
	buildIntervalTree(0, gn[gi] - 1);
	remlable.clear();
	remlable.resize(m, 0);
	for (int i = 0; i < seg[gi].size(); i++) {
		Segment s = seg[gi][i];
		addInterval(0, s.l, s.r, i);
	}
}
int tGetOne(int y, int x) {
	tLastX = x;
	int sl = intervalTree[x].l, sr = intervalTree[x].r;
	while (intervalTree[x].sid.size() > 0) {
		int sid = *intervalTree[x].sid.begin();
		if (!remlable[sid])
			return sid;
		else
			intervalTree[x].sid.erase(intervalTree[x].sid.begin());
	}
	if (sl < sr)
	{
		int mid = (sl + sr) / 2;
		if (y <= mid)
			return tGetOne(y, intervalTree[x].lson);
		else
			return tGetOne(y, intervalTree[x].rson);
	}
	else
		return -1;
}
void tGetList(int y, vector<int> &lip) {
	lip.clear();
	tLastX = 0;
	while (true) {
		int sid = tGetOne(y, tLastX);
		if (sid >= 0) {
			lip.push_back(sid);
			tRemove(sid);
		}
		else
			return;
	}
}

#define Level(c) level[c.gi][c.nid]
#define Match(c) match[c.gi][c.nid]
#define Gsel(c) gsel[c.gi][c.nid]
#define Null(c) (c.gi == -1)
vector<int> level[2], gsel[2], gres, cover;
vector<gNode> nodeQueue, match[2], gpath;
int term, depth, gcnt = 0;

void bfs() {
	int p = 0, ext = -1;
	nodeQueue.clear();
	for (int i = 0; i < 2; i++) {
		level[i].clear();
		level[i].resize(gn[i], -1);
	}
	for (int i = 0; i < gn[0]; i++) {
		gNode u = { 0, i };
		if (Null(Match(u))) {
			Level(u) = 0;
			nodeQueue.push_back(u);
		}
	}

	tBuild(0);
	bFill(1, unv);
	while (p < nodeQueue.size()) {
		gNode u = nodeQueue[p];
		if (ext >= 0 && Level(u) > ext)
			break;
		tGetList(u.nid, cover);
		for (auto sid : cover) {
			int l = seg[1][sid].l, r = seg[1][sid].r;
			bGetList(l, r, unv, gres);
			for (auto vid : gres) {
				gNode v = { 1, vid };
				Level(v) = Level(u) + 1;
				if (!Null(Match(v))) {
					gNode gnew = Match(v);
					Level(gnew) = Level(v) + 1;
					nodeQueue.push_back(gnew);
				}
				else
					ext = Level(v);
			}
		}
		p++;
	}
	if (ext == -1)
		term = true;
	depth = ext + 1;
}

bool dfs(gNode w) {
	int d = Level(w);
	if (d == 0)
		return true;
	else if (d % 2 == 0)
		if (dfs(Match(w)))
			return true;
		else return false;
	else {
		tLastX = 0;
		while (true) {
			int sid = tGetOne(w.nid, tLastX);
			if (sid == -1) break;
			int sl = seg[0][sid].l, sr = seg[0][sid].r;
			while (true) {
				int res = bGetOne(sl, sr, B[d - 1]);
				if (res == -1) break;
				gNode u = { 0, res };
				if (dfs(u)) {
					gpath.push_back(u);
					gpath.push_back(w);
					return true;
				}
			}
			tRemove(sid);
		}
	}
	return false;
}

void ginit() {
	ifstream fin("inp2.txt");
	int x0, y0, x1, y1;
	fin >> gn[0] >> gn[1] >> m;
	seg[0].clear(); seg[1].clear();
	for (int i = 0; i < m; i++) {
		fin >> x0 >> y0 >> x1 >> y1;
		seg[0].push_back({ x0, y0 });
		seg[1].push_back({ x1, y1 });
	}
}

void gwork() {
	for (int i = 0; i < 2; i++) {
		level[i].resize(gn[i]);
		match[i].resize(gn[i], { -1, -1 });
	}

	while (true) {
		if (term) return;
		tBuild(1);
		B.clear();
		B.resize(depth);
		for (int i = 0; i < gn[0]; i++) {
			gNode u = { 0, i };
			if (Level(u) >= 0 && Level(u) < depth)
				bAdd(u.nid, B[Level(u)]);
		}
		for (int vi = 0; vi < gn[1]; vi++) {
			gNode v = { 1, vi };
			if (Level(v) == depth - 1) {
				gpath.clear();
				if (Null(Match(v)) && dfs(v)) {
					gcnt++;
					for (int i = gpath.size() - 1; i > 0; i -= 2) {
						gNode nu = gpath[i], nv = gpath[i - 1];
						Match(nu) = nv;
						Match(nv) = nu;
					}
				}
			}
		}
	}
}

void travel(gNode v) {
	tGetList(v.nid, cover);
	for (auto sid : cover) {
		int sl = seg[0][sid].l, sr = seg[0][sid].r;
		bGetList(sl, sr, unv, gres);
		for (auto uid : gres) {
			gNode nu = { 0, uid };
			gNode nv = Match(nu);
			if (Null(nv)) continue;
			Gsel(nu) = 0;
			travel(nv);
		}
	}
}

void gprint() {
    ofstream fout("inp3.txt");
	fout << gn[0] + gn[1] - gcnt << '\n';
	tBuild(1);
	bFill(0, unv);
	for (int i = 0; i < 2; i++) {
		gsel[i].resize(gn[i], -1);
	}
	for (int i = 0; i < gn[1]; i++) {
		gNode v = { 1, i };
		if (Null(Match(v)))
			travel(v);
	}
	for (int i = 0; i < gn[1]; i++) {
		gNode v = { 1, i };
		if (!Null(Match(v)) && Gsel(Match(v)))
			Gsel(v) = 0;
	}
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < gn[i]; j++) {
			gNode w = { i, j };
			if (Gsel(w))
				fout << w.gi << ' ' << w.nid << '\n';
		}
    fout.close();
}

int main() {
	ginit();
	gwork();
	gprint();
	return 0;
}
