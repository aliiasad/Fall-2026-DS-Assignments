#include <iostream>

class List {
    // node must remain private since no user should know it exists
    private:
        class Node {
            // public since it should be able to be used inside List
            public:
                // required data members
                int studentID;  // type assumed
                float cgpa;
                int requestTime;
                int creditHoursEnrolled;    // type assumed
                int skipCount;

                // address of next pointer
                Node* next;

                // constructors (def. and para.)
                Node()  {
                    studentID = 0;
                    cgpa = 0;
                    requestTime = 0;
                    creditHoursEnrolled = 0;
                    skipCount = 0;
                    next = nullptr;
                }

                Node(int studentID, float cgpa,int requestTime, int creditHoursEnrolled)  {
                    this->studentID = studentID;
                    this->cgpa = cgpa;
                    this->requestTime = requestTime;
                    this->creditHoursEnrolled = creditHoursEnrolled;

                    // def values (no input)
                    this->skipCount = 0;
                    this->next = nullptr;
                }
        };

    // members of list other than Node (Node is also a member like them)
    Node* head; // first node of the list
    Node* tail; // last node of the list
    int length; // current length of list

    public:
        // constructors (def. and para.)
        List()  {
            head = nullptr;
            tail = nullptr;
            length = 0;
        }

        List(int studentID, float cgpa,int requestTime, int creditHoursEnrolled)  {
                    Node* newNode = new Node(studentID, cgpa, requestTime, creditHoursEnrolled);
                    // receives null from constructor so no need to assign explicitly
                    head = newNode;
                    tail = newNode;
                    length = 1;
        }

        // Part A Solution Function
        void insertStudent(int studentID, float cgpa, int requestTime, int creditHoursEnrolled) {
            Node* newNode = new Node(studentID, cgpa, requestTime, creditHoursEnrolled);
            // if the coming node is the forst ever in the list
            if (head == nullptr)    {   // same as length == 0
                head = newNode;
                tail = newNode;
                head->next = nullptr;
                return;
            }
            // if it is not the first in the list
            else {
                Node* temp = head;
                // if coming node is better than head
                if (newNode->cgpa > temp->cgpa || 
                    (newNode->cgpa == temp->cgpa && newNode->requestTime < temp->requestTime))  {
                        newNode->next = head;
                        head = newNode;
                        return;
                    }
                
                else {
                    // if not better than head but not worst (tail)
                    while (temp->next != nullptr && 
                            !((newNode->cgpa > temp->next->cgpa) || 
                               newNode->cgpa == temp->next->cgpa && (newNode->requestTime < temp->next->requestTime))) {
                                temp = temp->next;
                    }
                    // the coming node is better than te node pointed by temp (temp->next)
                    // but not temp itself (checked in previous iteration of while)
                    newNode->next = temp->next;
                    temp->next = newNode;

                    // if it was indeed worst node of all then it  hsould be tail
                    if (newNode->next == nullptr)   {
                        tail = newNode;
                    }
                }
            }
            length++;
            return;
        }
};

int main()  {
    return 0;
}