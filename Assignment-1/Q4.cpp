#include <iostream>
#include <string>

// For Part C
const int R = 5; // chat threshold, defined in spec

class List {
    private:
        class Node  {
            public:
                // Required data members
                std::string botId;  // type not mentioned so deduced from example
                int currentLoad; // type not mentioned
                int maxLoad; // capacity should be int
                bool isResting;
                int chatsHandledSinceRest;
                int lapsPassedWhileResting;

                Node* next; // controller

                // def constructor
                Node() : botId(), currentLoad(), maxLoad(), isResting(false),
                chatsHandledSinceRest(0), lapsPassedWhileResting(0) {
                    this->next = nullptr;
                }

                // para constructor
                Node(std::string botId, int maxLoad)   {
                    this-> botId = botId;                
                    this->maxLoad = maxLoad;

                    // predefined values
                    this->currentLoad = 0;
                    this->isResting = false;
                    this->chatsHandledSinceRest = 0;
                    this->lapsPassedWhileResting = 0;

                    // controller to null
                    this->next = nullptr;
                }
        };
    public: 

        int length = 0;

        static Node* current; // pointer to node being served

        List()  {}

        // para constructor
        List(std::string botId, int maxLoad)    {
            Node* newNode = new Node(botId, maxLoad);
            newNode->next = newNode;
            current = newNode;
            length++;
        }

        // required functions
        void addBot(std::string botId, int maxLoad) {
            Node* newNode = new Node(botId, maxLoad);   // parameterized constructor handles point 1 of the spec

            // if currently empty
            if (current == nullptr) {
                current = newNode;
                current->next = current;
            }
            else {
                newNode->next = current->next;
                current->next = newNode;
            }
            length++;
        }

        void assignQuery()  {
            Node* temp = current;
            do {
                if (temp->isResting == true || temp->currentLoad == temp->maxLoad)  {
                    temp = temp->next;
                }
                else {
                    temp->chatsHandledSinceRest++;
                    temp->currentLoad++;
                    current = temp->next;
                    return;
                }
            }
            while (temp != current);

            std::cout << "System Overload — query queued" << std::endl;
        }   

        void rotateCycle()  {
            Node* temp = current;

            do {
                if (!temp->isResting)   {
                    if (temp->chatsHandledSinceRest == R)   {
                        temp->isResting = true;
                        temp = temp->next;
                    }
                    else {
                        temp = temp->next;
                    }
                }
                else {
                    if (temp->lapsPassedWhileResting < 2)   {
                        temp->lapsPassedWhileResting++;
                        temp = temp->next;
                    }
                    else {
                        temp->isResting = false;
                        temp->chatsHandledSinceRest = 0;
                        temp->lapsPassedWhileResting = 0;
                        temp = temp->next;
                    }
                }                
            }
            while (temp != current);
        }
        void crashBot(std::string botId);
        void printRing();
};

List::Node* List::current = nullptr;