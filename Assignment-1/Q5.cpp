#include <iostream>

class List {
    private:
        class Node {
            public:
                // reuired data members
                int startTime; // type not specified
                int duration;   // type not specified
                int targetTemp; // type not specified
                bool isLocked;
                
                // Node controllers
                Node* forward;  // same as next
                Node* backward; // same as prev

                // def constructor
                Node()  : startTime(), duration(), targetTemp(), isLocked(false) {
                    forward = nullptr;
                    backward = nullptr;
                }

                // para constructor
                Node(int startTime, int duration, int targetTemp, bool isLocked)  {
                    this->startTime = startTime;
                    this->duration = duration;
                    this->targetTemp = targetTemp;
                    this->isLocked = isLocked;

                    forward = nullptr;
                    backward = nullptr;
                }
        };

        Node* head; // first node of the list
        Node* tail; // last node of the list
        int length; // lenth of the list

        public:
            // def constructor
            List()  {
                head = nullptr;
                tail = nullptr;
                length = 0;
            }

            // para constructor
            List(int startTime, int duration, int targetTemp, bool isLocked)  {
                Node* newNode = new Node(startTime, duration, targetTemp, isLocked);
                head = newNode;
                tail = newNode;
                length = 1;
            }

            // Part A Solution Function
            void insertSchedule(int startTime, int duration, int targetTemp, bool isLocked) {
                Node* newNode = new Node(startTime, duration, targetTemp, isLocked);
                if (head == nullptr)    {
                    head = newNode; // forward and backward null from constructor
                    tail = newNode;
                }
                else if (newNode->startTime <= head->startTime)   {
                    head->backward = newNode;
                    newNode->forward = head;
                    head = newNode;
                }
                else if (newNode->startTime >= tail->startTime)   {
                    newNode->backward = tail;
                    tail->forward = newNode;
                    tail = newNode;
                }
                else {
                    Node* temp = head;
                    while (newNode->startTime >= temp->startTime)   {
                        temp = temp->forward;
                    }
                    newNode->forward = temp;
                    temp->backward->forward = newNode;
                    newNode->backward = temp->backward;
                    temp->backward = newNode;
                }
                length++;
            }
};