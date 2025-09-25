#include <iostream>
#include <vector>
#include <random>
#include <ctime>
#include <string>
using namespace std;

int N = 100;

// Binary tree node structure
struct TreeNode {
    int label; // label of leaf node, -1 for internal nodes
    TreeNode* left;
    TreeNode* right;
    TreeNode(int l) : label(l), left(nullptr), right(nullptr) {}
    TreeNode(TreeNode* l, TreeNode* r) : label(-1), left(l), right(r) {}
    ~TreeNode() {
        delete left;
        delete right;
    }
};

mt19937 rng(static_cast<unsigned int>(time(nullptr)));

// Generate a random binary tree
TreeNode* randomBinaryTree(int N) {
    if (N <= 0) return nullptr;
    // Initialize N leaf nodes
    vector<TreeNode*> nodes;
    for (int i = 0; i < N; ++i) {
        nodes.push_back(new TreeNode(i));
    }
    // Random number generator
    for (int i = 0; i < N - 1; ++i) {
        // Randomly select two different nodes to merge
        uniform_int_distribution<int> dist(0, nodes.size() - 1);
        int idx1 = dist(rng);
        int idx2 = dist(rng);
        while (idx2 == idx1) idx2 = dist(rng);

        TreeNode* left = nodes[idx1];
        TreeNode* right = nodes[idx2];

        // Create new node
        TreeNode* newNode = new TreeNode(left, right);

        // Add new node to the list and remove the merged nodes
        if (idx1 > idx2) swap(idx1, idx2);
        nodes.erase(nodes.begin() + idx2);
        nodes.erase(nodes.begin() + idx1);
        nodes.push_back(newNode);
    }
    return nodes[0];
}

// Generate string representation in the specified format
string treeToString(TreeNode* root) {
    if (!root) return "";
    if (root->label != -1) {
        return to_string(root->label);
    } else {
        string leftStr = treeToString(root->left);
        string rightStr = treeToString(root->right);
        return "(" + leftStr + "|" + rightStr + ")";
    }
}

int main() {
    freopen("C:\\Users\\tswdd\\Desktop\\AMCT\\inp.txt", "w", stdout);
    int n = N;
    TreeNode* root1 = randomBinaryTree(n);
    TreeNode* root2 = randomBinaryTree(n);
    cout << n << '\n';
    cout << treeToString(root1) << '\n';
    cout << treeToString(root2) << '\n';
    return 0;
}