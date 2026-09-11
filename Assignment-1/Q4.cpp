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
        void crashBot(std::string botId)    {
            if (current == nullptr) {
                return;
            }

            bool found = false;
            Node* prevToCrashed = current;

            do
            {
                if (prevToCrashed->next->botId == botId)    {
                    found = true;
                    break;
                }
                else {
                    prevToCrashed = prevToCrashed->next;
                }
            } while (prevToCrashed != current);
            
            if (!found)
                return;

            Node* crashed = prevToCrashed->next;
            int crashedQueries = crashed->currentLoad;

            if (crashed->next == crashed)   {
                if (crashedQueries > 0)   {
                    std::cout << crashedQueries << " query/queries unassigned!" << std::endl;
                }
                delete crashed;
                current = nullptr;
                length--;
                return;
            }
            Node* runner = crashed->next;
            prevToCrashed->next = runner;

            if (current == crashed) {
                current = runner;
            }
            delete crashed;
            length--;

            Node* temp = runner;

            int remaining = crashedQueries;

            do
            {
                if (remaining > 0)  {
                    int room = temp->maxLoad - temp->currentLoad;
                    int absorbed = (room < remaining) ? room : remaining;
                    temp->currentLoad += absorbed;
                    remaining -= absorbed;
                }
                temp = temp->next;
            } while (remaining > 0 && temp != runner);
            
            if (remaining > 0)  {
                std::cout << remaining << "query/queries unassigned" << std::endl;
            }
        }
        
        // this method is made accorsing to how spec shows example
        void printRing() {
            if (current == nullptr) {
                std::cout << "Ring is empty." << std::endl;
                return;
            }

            Node* temp = current;
            do {
                std::cout << temp->botId
                        << "(load=" << temp->currentLoad
                        << ",max=" << temp->maxLoad
                        << ",resting=" << (temp->isResting ? "T" : "F")
                        << ",handled=" << temp->chatsHandledSinceRest
                        << ") -> ";
                temp = temp->next;
            } while (temp != current);

            std::cout << "(back to " << current->botId << ") current = " << current->botId << std::endl;
        }
};

List::Node* List::current = nullptr;