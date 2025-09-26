#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <string>
using namespace std;

int N = 10;

// Generalized tree node structure
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
    freopen("inp.txt", "w", stdout);
    int n = N;
    TreeNode* root1 = randomBinaryTree(n);
    TreeNode* root2 = randomBinaryTree(n);
    cout << n << '\n';
    cout << treeToString(root1) << '\n';
    cout << treeToString(root2) << '\n';
    return 0;
}