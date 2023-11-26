#include <iostream>
#include <cstring>
#include <vector>

using namespace std;


class RBT{
private:
    struct node{
        node* lchild;
        node* rchild;
        node* parent;
        string label;
        int value;
        char color;

        node(string key,int val): lchild(nullptr),rchild(nullptr),parent(nullptr),label(key),value(val),color('R'){}  
    };

    node* root;
    bool ll; // Left-Left Rotation flag
    bool rr; // Right-Right Rotation flag
    bool lr; // Left-Right Rotation flag
    bool rl; // Right-Left Rotation flag

    // Function to perform Left Rotation
    node* rotateLeft(node* vertex) {
        node* x = vertex->rchild;
        node* y = x->lchild;
        x->lchild = vertex;
        vertex->rchild = y;
        vertex->parent = x;
        if (y != nullptr)
            y->parent = vertex;
        return x;
    }
 
    // Function to perform Right Rotation
    node* rotateRight(node* vertex) {
        node* x = vertex->lchild;
        node* y = x->rchild;
        x->rchild = vertex;
        vertex->lchild = y;
        vertex->parent = x;
        if (y != nullptr)
            y->parent = vertex;
        return x;
    }

    node* insertion(node* root, string key, int val){
        bool f = false; //flag to check red-red conflict
        if(root == nullptr) {
            return new node(key,val);
        }
        else if(key < root->label){
            root->lchild = insertion(root->lchild,key, val);
            root->lchild->parent = root;
            if (root != this->root) {
                if (root->color == 'R' && root->lchild->color == 'R')
                    f = true;
            }
        }
        else {
            root->rchild = insertion(root->rchild, key,val);
            root->rchild->parent = root;
            if (root != this->root) {
                if (root->color == 'R' && root->rchild->color == 'R')
                    f = true;
            }
        }
        // Perform rotations
        if (ll) {
            root = rotateLeft(root);
            root->color = 'B';
            root->lchild->color = 'R';
            ll = false;
        } else if (rr) {
            root = rotateRight(root);
            root->color = 'B';
            root->rchild->color = 'R';
            rr = false;
        } else if (rl) {
            root->rchild = rotateRight(root->rchild);
            root->rchild->parent = root;
            root = rotateLeft(root);
            root->color = 'B';
            root->lchild->color = 'R';
            rl = false;
        } else if (lr) {
            root->lchild = rotateLeft(root->lchild);
            root->lchild->parent = root;
            root = rotateRight(root);
            root->color = 'B';
            root->rchild->color = 'R';
            lr = false;
        }

        // Handle RED-RED conflicts
        if (f) {
            if (root->parent->rchild == root) {
                if (root->parent->lchild == nullptr || root->parent->lchild->color == 'B') {
                    if (root->lchild != nullptr && root->lchild->color == 'R')
                        rl = true;
                    else if (root->rchild != nullptr && root->rchild->color == 'R')
                        ll = true;
                } else {
                    root->parent->lchild->color = 'B';
                    root->color = 'B';
                    if (root->parent != this->root)
                        root->parent->color = 'R';
                }
            } else {
                if (root->parent->rchild == nullptr || root->parent->rchild->color == 'B') {
                    if (root->lchild != nullptr && root->lchild->color == 'R')
                        rr = true;
                    else if (root->rchild != nullptr && root->rchild->color == 'R')
                        lr = true;
                } else {
                    root->parent->rchild->color = 'B';
                    root->color = 'B';
                    if (root->parent != this->root)
                        root->parent->color = 'R';
                }
            }
            f = false;
        }
        return root;
    }

    node* search(node* start, string key){
        if(start==nullptr) return NULL;
        node *ans;
        if(start->label==key) return start;
        else if(key < start->label) {
            ans=search(start->lchild,key);
        }
        else{
            ans=search(start->rchild,key);
        }
        return ans;
    }

     // Helper function to perform Inorder Traversal
    void inorderTraversalHelper(node* vertex) {
        if (vertex != nullptr) {
            inorderTraversalHelper(vertex->lchild);
            std::cout << vertex->label << " ";
            inorderTraversalHelper(vertex->rchild);
        }
    }
 
    // Helper function to print the tree
    void printTreeHelper(node* root, int space) {
        if (root != nullptr) {
            space += 10;
            printTreeHelper(root->rchild, space);
            std::cout << std::endl;
            for (int i = 10; i < space; i++)
                std::cout << " ";
            std::cout << root->label << std::endl;
            printTreeHelper(root->lchild, space);
        }
    }

public:
    RBT(): root(nullptr),ll(false),rr(false),lr(false),rl(false){}

    void insert(string key, int val){
        if(root==nullptr) {
            root = new node(key,val);
            root->color='B';
        }
        else{
            root=insertion(root,key,val);
        }
    }

    node* find(string key){
        node* ans;
        ans = search(root,key);
        return ans;
    }

    void inorderTraversal() {
        inorderTraversalHelper(root);
    }
 
    // Function to print the tree
    void printTree() {
        printTreeHelper(root, 0);
    }
};

int main(){
    RBT t;
    vector<string> v={"ASML","AAP","x","AKG","KarGo","Bull","CHA","akg"};
    int i=3;
    for(auto it:v){
        t.insert(it,i);
        i+=2;
        cout<<endl;
        t.inorderTraversal();
    }
    t.printTree();
    return 0;
}