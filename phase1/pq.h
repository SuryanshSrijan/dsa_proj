#include<iostream>

class Heap{
private:
    struct Node{
        node* lchild;
        node* rchild;
        node* parent;
        int val1;
        int val2;

        Node(int p,int t): lchild(nullptr),rchild(nullptr),parent(nullptr),val1(p),val2(t){} 
    };
    
    Node* root;
    
    Node* insertion(Node* root,int p,int t){
        if(root == nullptr) {
            return new Node(p,t);
        }
        else{
            
        }
    }

public:
    void insert(int p,int t){
        if(root==nullptr) {
            root = new Node(p,t);
        }
        else{
            root=insertion(root,p,t);
        }
    }
}