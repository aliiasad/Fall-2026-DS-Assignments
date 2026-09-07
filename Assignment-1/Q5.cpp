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

        // For Part C
        Node* cursor;

        public:
            // def constructor
            List()  {
                head = nullptr;
                tail = nullptr;
                length = 0;

                // Part C Extension
                cursor = nullptr;
            }

            // para constructor
            List(int startTime, int duration, int targetTemp, bool isLocked)  {
                Node* newNode = new Node(startTime, duration, targetTemp, isLocked);
                head = newNode;
                tail = newNode;
                length = 1;

                // Part C Extension
                cursor = head;
            }

            // Part A Solution Function
            void insertSchedule(int startTime, int duration, int targetTemp, bool isLocked) {
                Node* newNode = new Node(startTime, duration, targetTemp, isLocked);
                if (head == nullptr)    {
                    head = newNode; // forward and backward null from constructor
                    tail = newNode;

                    // Part C Extension
                    cursor = newNode;
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

            // Part B Solution Function Helper
            void mergeNodes(Node* node, Node* neighbor) {
                int mergedStartTime = (node->startTime >= neighbor->startTime) ? neighbor->startTime : node->startTime; // min(node, neighbor)
                int mergedEndTime = (node->startTime + node->duration >= neighbor->startTime + neighbor->duration) 
                     ? node->startTime + node->duration 
                     : neighbor->startTime + neighbor->duration;
                int mergedDuration = mergedEndTime - mergedStartTime; // max(nodes, neighbors)
                int mergedTargetTemp = (node->targetTemp + neighbor->targetTemp) / 2; // floor average
                bool mergedIsLocked = node->isLocked || neighbor->isLocked;

                // we are going to put merged data in node and delete neighbor
                node->startTime = mergedStartTime;
                node->duration = mergedDuration;
                node->targetTemp = mergedTargetTemp;
                node->isLocked = mergedIsLocked;

                // now we are going to simply delete neighbor and relink
                if (neighbor == head)   {
                    if (head == tail)   {
                        head = nullptr;
                        tail = nullptr;
                    }
                    else {
                        head->forward->backward = nullptr;
                        head = head->forward;
                    }
                }
                else if (neighbor == tail)  {
                    tail = tail->backward;
                    tail->forward = nullptr;

                }
                else {
                    neighbor->backward->forward = neighbor->forward;
                    neighbor->forward->backward = neighbor->backward;
                }

                delete neighbor;
                length--;
            }

            // Part B Solution Function
            void checkAndMerge(Node* node)  {
                int T = 10;
                bool merged = true; // to re-check after merge

                while (merged) {
                    merged = false;

                    if (node->forward != nullptr)   {
                        int overlap = (node->startTime + node->duration) - node->forward->startTime;
                        if (overlap > T)    {
                            mergeNodes(node, node->forward);
                            merged = true;
                            continue;
                        }
                        else if (overlap > 0) {
                            std::cout << "Minor Overlap Detected with Next Event!" << std::endl;
                        }
                    }

                    if (node->backward != nullptr)  {
                        int overlap = (node->backward->startTime + node->backward->duration) - node->startTime;
                        if (overlap > T)    {
                            mergeNodes(node, node->backward);
                            merged = true;
                            continue;
                        }
                        else if (overlap > 0) {
                            std::cout << "Minor Overlap Detected with Previous Event!" << std::endl;
                        }
                    }
                }
            }

            // Part C Solution Helpers (Kinda)
            void next() {
                if (cursor->forward != nullptr)
                    cursor = cursor->forward;
            }

            void prev() {
                if (cursor->backward != nullptr)
                    cursor = cursor->backward;
            }

            // Part C Solution Function
            void undoLast(int n)    {
                Node* current = cursor->backward;

                // required counter variables
                int deletedNodes = 0;
                int skippedNodes = 0;

                /*we have to delete the nodes before cursor upto n till nullptr
                and also skip locked*/
                while (current != nullptr && deletedNodes < n)  {
                    if (current->isLocked)  {
                        skippedNodes++;
                        current = current->backward;
                    }
                    else {
                        // now we are going to do the linking process for head and body
                        // not for tail because tail can max be cursor
                        Node* toDelete = current;
                        current = current->backward;
                        if (toDelete == head)   {
                            head = toDelete->forward;
                            head->backward = nullptr;
                        }
                        else {
                            toDelete->backward->forward = toDelete->forward;
                            toDelete->forward->backward = toDelete->backward;
                        }
                        // deleteion 
                        delete toDelete;
                        length--;
                        deletedNodes++;
                    }
                }

                std::cout << "Undid" << deletedNodes << " out of " << n << " entries." << std::endl;

                if (current == nullptr)
                    std::cout << "Reached start of timeline!" << std::endl;

                std::cout << skippedNodes << "skipped in the process." << std::endl;
            }
};