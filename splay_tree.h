
#include <functional>
#include <iostream>
#include <list>
using namespace std;

#ifndef SPLAY_TREE
#define SPLAY_TREE



template <typename T>

class SplayTree {
    struct node {
        node *left, *right;
        node* parent;
        T key;
        int max_val;
        node(const T& init = T())
            : left(0)
            , right(0)
            , parent(0)
            , key(init)
        {
        }
    };
public:
    bool comp(T i, T j){
        return i < j;
    }

    unsigned long p_size;
    node* root;

    void left_rotate(node* x)
    {
        node* y = x->right;
        x->right = y->left;
        if (y->left)
            y->left->parent = x;
        y->parent = x->parent;
        if (!x->parent)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void right_rotate(node* x)
    {
        node* y = x->left;
        x->left = y->right;
        if (y->right)
            y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->right = x;
        x->parent = y;
    }

    void splay(node* x)
    {
        while (x->parent) {
            if (!x->parent->parent) {
                if (x->parent->left == x)
                    right_rotate(x->parent);
                else
                    left_rotate(x->parent);
            }
            else if (x->parent->left == x && x->parent->parent->left == x->parent) {
                right_rotate(x->parent->parent);
                right_rotate(x->parent);
            }
            else if (x->parent->right == x && x->parent->parent->right == x->parent) {
                left_rotate(x->parent->parent);
                left_rotate(x->parent);
            }
            else if (x->parent->left == x && x->parent->parent->right == x->parent) {
                right_rotate(x->parent);
                left_rotate(x->parent);
            }
            else {
                left_rotate(x->parent);
                right_rotate(x->parent);
            }
        }
    }

    void replace(node* u, node* v)
    {
        if (!u->parent)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        if (v)
            v->parent = u->parent;
    }



    node* subtree_minimum(node* u)
    {
        while (u->left)
            u = u->left;
        return u;
    }

    node* subtree_maximum(node* u)
    {
        while (u->right)
            u = u->right;
        return u;
    }

    SplayTree()
        : root(0)
        , p_size(0)
    {}

    SplayTree(node* z){root = z;}
    SplayTree(int s){
        root = 0;
        for(int i = 0; i < s; i++){
            insert(i);
        }
    }

    node* get_root()
    {
        return root;
    }

    node* find_small(const int& key)
    {
        return find_small_help(root, key);
    }

    node* find_small_help(node* z, const int& key)
    {
        if (!z) return 0;
        if (z -> key == key) return z;
        if(comp(z -> key, key))
        {
            node* k = find_small_help(z -> right, key);
            if (k == 0)
                return z;
            return k;
        }
        return find_small_help(z->left, key);
    }


    node* find_large(const int& key)
    {
        return find_large_help(root, key);
    }

    node* find_large_help(node* z, const int& key)
    {
        if (!z) return 0;
        if (z -> key == key) return z;
        if(comp(key, z -> key))
        {
            node* k = find_large_help(z -> left, key);
            if (k == 0)
                return z;
            return k;
        }
        return find_large_help(z->right, key);
    }


    void insert(const T& key)
    {
        node* z = root;
        node* p = 0;

        while (z) {
            p = z;
            if (comp(z->key, key))
                z = z->right;
            else
                z = z->left;
        }

        z = new node(key);
        z->parent = p;

        if (!p)
            root = z;
        else if (comp(p->key, z->key))
            p->right = z;
        else
            p->left = z;

        splay(z);
        p_size++;
    }

    node* find(const T& key)
    {
        node* z = root;
        while (z) {
            if (comp(z->key, key))
                z = z->right;
            else if (comp(key, z->key))
                z = z->left;
            else
                return z;
        }
        return 0;
    }

    void erase(const T& key)
    {
        node* z = find(key);
        if (!z)
            return;

        splay(z);

        if (!z->left)
            replace(z, z->right);
        else if (!z->right)
            replace(z, z->left);
        else {
            node* y = subtree_minimum(z->right);
            if (y->parent != z) {
                replace(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            replace(z, y);
            y->left = z->left;
            y->left->parent = y;
        }

        p_size--;
    }

    node* split_right(node* x){
        splay(x);
        node* t = root -> right;
        root -> right = 0;
        if(t)
            t -> parent = 0;
        return t;
    }

    node* split_left(node* x){
        splay(x);

        node* t = root -> left;
        root -> left = 0;
        if(t)
            t -> parent = 0;
        return t;
    }

    void show_element(node* z, int *dist, int d, int *w, int *p, int mode)
    {
        if(!z) return;
        show_element(z -> left, dist, d, w, p, mode);
        int k = z -> key;
        //cout << "!" << z -> key << " " << dist[k] << " " << d << endl;

        if(mode == 0 && dist[k] == 0){
            dist[k] = d;
            w[(*p)++] = k;
        }
        if (mode == 1 && dist[k] ==  d){
            w[(*p)++] = k;
        }
        show_element(z -> right, dist, d, w, p, mode);

    }

    void zero(int i, int j, int *dist, int d, int *w, int *p, int mode){ // mode 0 for bfs, mode 1 for dfs

        node* z = find_small(i - 1);

        node* tt;

        SplayTree<int> s2;

        if (z){
            tt = split_right(z);
            s2.root = tt;
        } else{
            s2.root = root;
            root = 0;
        }

        if (!s2.root) {
            return;
        }

        node* t = s2.find_large(j + 1);
        z = s2.root;
        if (t){
            s2.splay(t);
            z = s2.root ->left;
        }

        show_element(z, dist, d, w, p, mode);

        if(!root) {root = s2.root; return;}
        splay(subtree_maximum(root));

        root -> right = s2.root;

        s2.root -> parent = root;


    }

    const T& minimum() { return subtree_minimum(root)->key; }
    const T& maximum() { return subtree_maximum(root)->key; }

    bool empty() const { return root == 0; }
    unsigned long size() const { return p_size; }
};


#endif // SPLAY_TREE

