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
};

int main()  {
    return 0;
}