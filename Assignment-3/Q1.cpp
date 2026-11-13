#include <iostream>
#include <queue>

template <typename T>
/* The tree class contains node class inside it so a single
node can't be accessed outside its vicinity. That's why node 
remains a private entity of tree.*/
class Tree {
    private:
        class Node  {
            public:
                // data container for node itself
                T data;

                // access pointers to move in tree
                Node* left;
                Node* right;
                Node* parent;

                // def. and para constructors for node
                Node() : data(), left(nullptr), 
                right(nullptr), parent(nullptr) {}

                Node(const T& value) : data(value), 
                left(nullptr), right(nullptr), 
                parent(nullptr) {}
        };

        public:
            Node* root;
            int count;

            // def. and para constructors
            Tree() : root(nullptr), count(0) {}

            Tree(const T& value) : root(nullptr), count(0)  {
                insert(value);
            }

            // required and needed functions (they may require helpers
            // implementation of base functions to be used in main()

            void insert(const T& value) {
                bool isInserted = false;
                insertHelper(root, value, nullptr, isInserted);
                if (isInserted)
                    count++;
                return;
            }

            void remove(const T& value) {
                bool isRemoved = false;
                root = removeHelper(root, value, isRemoved);
                if (isRemoved)
                    count--;
            }

            void levelOrder()   {
                levelOrderHelper(root);
                return;
            }

            ~Tree() { 
                destroyHelper(root);
            }


            // (required helpers which access private data that cannot be 
            // sent by the main())
            // void insertHelper(Node*& root, const T& value, Node* parent, bool& isInserted);
            // Node* removeHelper(Node* root, const T& value, bool& isRemoved);
            // Node* minNode(Node* root);
            // void levelOrderHelper(Node* root);
            // Node* rotateLL(Node* node);
            // Node* rotateRR(Node* node);
            // Node* rotateRL(Node* node);
            // Node* rotateLR(Node* node);
            // keep the prototypes commented out.. cpp doesnt allow prototyes in classes
            // if the actual func is also implemented inside class


        private:
            // implementation
            // to be used in remove node logic
            Node* minNode(Node* root)   {
                // the minimum node is the left most node of the tree
                if (root == nullptr || root->left == nullptr)
                    return root;    

                return minNode(root->left);
            }

            
            /*design note: the root travels with reference so that the new assigned
            root in base case is actually linked.. it can also be simply done like
            as done in removeHelper instead of pass by reference
            i have exercised both methods..  use whatever you wana use*/
            void insertHelper(Node*& root, const T& value, Node* parent, bool& isInserted) {
                /* when the traversal crosses last valid node, that's the position where
                the ext node should be.*/
                if (root == nullptr)    {
                    Node* newNode = new Node(value);
                    root = newNode;
                    root->parent = parent;
                    return;
                }
                
                /* if the given value is lesser than current node's value,
                we move to left, else we move to right... if at any point 
                given data is equal to node's data, its duplicate, cannot insert*/
                if (value < root->data)
                    insertHelper(root->left, value, root, isInserted);
                else if (root->data < value)
                    insertHelper(root->right, value, root, isInserted);
                else {
                    std::cout << "Did not perform Insertion! Duplicate Data." << std::endl;
                    return;
                }
            }

            

            /*design note: the functin maintains a boolean value (isRemoved) so 
            that it can track how if the nde is deleted to decrease count (to be 
            implemeted later)*/
            Node* removeHelper(Node* node, const T& value, bool& isRemoved)  {
                // crossed last possible node, then return null (base case)
                if (node == nullptr)    {
                    return nullptr;
                }

                /* the below logic locates the node to be deleted, it doesnt have to 
                do anything with deletion*/
                if (value < node->data) {
                    /*if any node at the left of root node is deleted, it means
                    the child(ren) of that deleted node have no parent, so
                    a new parent (parent of that delete node) must be assigned
                    to them*/
                    node->left = removeHelper(node->left, value, isRemoved);
                    /*now if the returned node is nullptr, then tou its fine, but
                    if a node is returned, we ave to link it to the parrnt, as 
                    explained right above the last line*/
                    if (node->left != nullptr)  {
                        node->left->parent = node;
                    }

                }
                else if (node->data < value)    {
                    // same explanation as above.. just for the right half of bst/avlt
                    node->right = removeHelper(node->right, value, isRemoved);
                    if (node->right != nullptr) {
                        node->right->parent = node;
                    }
                }
                /*if the value was neither small, nor large, then it must be the value*/
                else {
                    isRemoved = true;
                    /*now we have found the value, so its time to delete the node
                    There must be one of the three cases-> the node has no children,
                    the node has a right or a left child or the node has two children*/

                    // considering the case where no children, we simply delete
                    if (node->left == nullptr && node->right == nullptr)    {
                        delete node;
                        return nullptr; // we must return null to the parent side that sent call;
                    }

                    /*considering the case where there is either left child or right child
                    but not both... in this case, if left exists, we simply make it the root 
                    and if right exists, we make it the root*/
                    else if (node->left == nullptr)  {
                        Node* child = node->right;
                        delete node;
                        return child;
                    }
                    else if (node->right == nullptr) {
                        Node* child = node->left;
                        delete node;
                        return child;
                    }

                    /*remains the case where the node to be deleted has two children
                    in this case, the node that replaces to be deleted node is the node
                    having smallest value on the right side of that node*/

                    /*the replacement logic is simply putiing the smallest node value in it and 
                    then deleting that node itself*/
                    Node* successor = minNode(node->right);
                    node->data = successor->data;
                    bool dummy = false; // using a dummy false to delete the node, so that isRemoved is not overwritten
                    node->right = removeHelper(node->right, successor->data, dummy);
                    if (node->right != nullptr)
                        node->right->parent = node;
                }
                return node;
            }

            /*level order traversal prints each level from left to right
            idea: use a queue to hold the next level while printing the current one
            for example, start manually from root, and then push its children in queue
            so when you complete a level, you have next one in queue*/
            void levelOrderHelper(Node* root) {
                if (root == nullptr)
                    return;
                
                std::queue<Node*> q;
                // manually pushing root to start
                q.push(root);
                while (!q.empty())    {
                    int levelSize = q.size();
                    while (levelSize--) {
                        Node* current = q.front();
                        q.pop();
                        std::cout << current->data << " ";

                        /*now check if none, one or two children exist and then put
                        them in queue for next level's printing*/
                        if (current->left)
                            q.push(current->left);
                        if (current->right)
                            q.push(current->right);
                    }
                    std::cout << std::endl;
                }
                return;
            }

            /*Design Note: the left rotation's working is described as when the node 
            is rotated, the left node of the primary node becomes new top whereas the
            left node of primary node points to the right node of new top node. Then
            check if that new top had a right side, if yes then make pr node the parent of 
            its right side. Now make pr node's parent new top's parent
            Then check of te pr node had a parent, if no, assign new top to root
            , if it was left child of its own parent, then make new top left child of pr 
            node's parent and else right. Now, finally assign node to new top's right
            and make new top node's parent*/
            Node* rotateLL(Node* node)  {
                if (node == nullptr || node->left == nullptr)
                    return node;

                Node* newTop = node->left;
                node->left = newTop->right;

                if (newTop->right != nullptr)
                    newTop->right->parent = node;
                
                newTop->parent = node->parent;

                if (node->parent == nullptr)
                    root = newTop;
                else if (node->parent->left == node)    
                    node->parent->left = newTop;
                else
                    node->parent->right = newTop;

                newTop->right = node;
                node->parent = newTop;

                return newTop;
            }

            /*Design Note: the right rotation's working is described as when the node 
            is rotated, the right node of the primary node becomes new top whereas the
            right node of primary node points to the left node of new top node. Then
            check if that new top had a left side, if yes then make pr node the parent of 
            its left side. Now make pr node's parent new top's parent
            Then check if the pr node had a parent, if no, assign new top to root
            , if it was left child of its own parent, then make new top left child of pr 
            node's parent and else right. Now, finally assign node to new top's left
            and make new top node's parent*/
            Node* rotateRR(Node* node)  {
                if (node == nullptr || node->right == nullptr)
                    return node;
                
                Node* newTop = node->right;
                node->right = newTop->left;

                if (newTop->left != nullptr)
                    newTop->left->parent = node;

                newTop->parent = node->parent;

                if (node->parent == nullptr)
                    root = newTop;
                else if (node->parent->left == node)
                    node->parent->left = newTop;
                else
                    node->parent->right = newTop;

                newTop->left = node;
                node->parent = newTop;

                return newTop;
            }

            /*simply rotate pivot's right node LL and then pivot RR*/
            Node* rotateRL(Node* node)  {
                if (node == nullptr || node->right == nullptr)
                    return node;

                node->right = rotateLL(node->right);
                return rotateRR(node);
            }

            /*simply rotate pivot's left node RR and then pivot LL*/
            Node* rotateLR(Node* node)   {
                if (node == nullptr || node->left == nullptr)
                    return node;

                node->left = rotateRR(node->left);
                return rotateLL(node);
            }

            void destroyHelper(Node* node) {
                if (node == nullptr) return;
                destroyHelper(node->left);
                destroyHelper(node->right);
                delete node;
            }
};

int main()  {
    Tree<int> tree;
    tree.insert(30);
    tree.insert(20);
    tree.insert(40);
    tree.insert(10);
    tree.insert(25);
    tree.insert(5); 

    tree.levelOrder();
    return 0;
}