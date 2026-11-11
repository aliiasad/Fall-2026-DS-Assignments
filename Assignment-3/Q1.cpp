#include <iostream>

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

            // required and needed functions (they may require helpers)

            void insert(const T& value);

            // bool so we know if we removed or not, and it tells whether to 
            // decrement count or not
            bool remove(const T& value);    


            // (required helpers which access private data that cannot be 
            // sent by the main())
            void insertHelper(Node* root, const T& value, Node* parent);
            Node* removeHelper(Node* root, const T& value, bool isRemoved);


};

int main()  {
    return 0;
}