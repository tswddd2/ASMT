#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <string>
#include <fstream>
using namespace std;

int N = 10;
double R = 0.3;

struct TreeNode {
    int label; // label of leaf node, -1 for internal nodes
    vector<TreeNode*> children;

    TreeNode(int l) : label(l) {}
    TreeNode(const vector<TreeNode*>& ch) : label(-1), children(ch) {}
};

mt19937 rng(static_cast<unsigned int>(time(nullptr)));

// Generate a random binary tree
TreeNode* randomBinaryTree(int N) {
    if (N <= 0) return nullptr;
    vector<TreeNode*> nodes;
    for (int i = 0; i < N; ++i) {
        nodes.push_back(new TreeNode(i));
    }

    for (int i = 0; i < N - 1; ++i) {
        // Randomly select two different nodes to merge
        uniform_int_distribution<int> dist(0, nodes.size() - 1);
        int idx1 = dist(rng);
        int idx2 = dist(rng);
        while (idx2 == idx1) idx2 = dist(rng);

        vector<TreeNode*> ch = { nodes[idx1], nodes[idx2] };
        TreeNode* newNode = new TreeNode(ch);

        // Remove merged nodes and add the new node
        if (idx1 > idx2) swap(idx1, idx2);
        nodes.erase(nodes.begin() + idx2);
        nodes.erase(nodes.begin() + idx1);
        nodes.push_back(newNode);
    }
    return nodes[0];
}

// Contract edges with probability r
void contractEdges(TreeNode* root, double r) {
    if (!root) return;
    vector<TreeNode*>& ch = root->children;
    for (size_t i = 0; i < ch.size(); ) {
        TreeNode* child = ch[i];
        uniform_real_distribution<double> dist(0.0, 1.0);
        bool do_contract = (dist(rng) < r);
        if (do_contract && child->label == -1) {
            // Contract this edge: remove child, merge its children into root
            ch.erase(ch.begin() + i);
            ch.insert(ch.begin() + i, child->children.begin(), child->children.end());
            // Detach pointers from child (so destructor doesn't double-free)
            child->children.clear();
            delete child;
            // Do not increment i, as new children are now at i
        } else {
            // Recurse into child
            contractEdges(child, r);
            ++i;
        }
    }
}

// Generate string representation (children separated by "|")
string treeToString(TreeNode* root) {
    if (!root) return "";
    if (root->label != -1) {
        return to_string(root->label);
    } else {
        string res = "(";
        for (size_t i = 0; i < root->children.size(); ++i) {
            if (i) res += "|";
            res += treeToString(root->children[i]);
        }
        res += ")";
        return res;
    }
}

int main() {
    ofstream fout("inp.txt");
    int n = N;
    TreeNode* root1 = randomBinaryTree(n);
    TreeNode* root2 = randomBinaryTree(n);

    double r = R;

    contractEdges(root1, r);
    contractEdges(root2, r);

    fout << n << '\n';
    fout << treeToString(root1) << '\n';
    fout << treeToString(root2) << '\n';
    delete root1;
    delete root2;
    return 0;
}