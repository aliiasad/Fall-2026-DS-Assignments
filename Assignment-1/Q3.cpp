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

        // Part B Solution Function
        void allocateSeat() {
            // temp nodes to perform traversal
            Node* current = head;
            Node* prev = nullptr;

            while (current != nullptr)    {
                // eligibility check simplified
                bool isEligible = current->creditHoursEnrolled + 3 <= 18;

                if (isEligible) {
                    if (prev == nullptr) // if head is eligibile
                        head = current->next;
                    else    // if random node is eligible
                        prev->next = current->next;
                    if (current == tail)    // if tail is eligible
                        tail = prev;
                    
                    
                    // print message and then delete the node
                    std::cout << "Seat Allocated to Student " << current->studentID << std::endl;
                    delete current;
                    length--;
                    return;
                }
                else {
                    current->skipCount++;
                    if (current->skipCount == 3)    {
                        // creating another to the pointer to be deleted since current
                        // would have to move before deletion
                        Node* toDelete = current;
                        // if head to be deleted
                        if (prev == nullptr)
                            head = current->next;
                        // if random node to be deleted
                        else
                            prev->next = current->next;
                        // if tail is to be deleted
                        if (tail == current)
                            tail = prev;
                        

                        current = current->next; // move current to next since current current is toDelete
                        delete toDelete;
                        length--;
                        std::cout << "Student Auto-Dropped" << std::endl;
                    }
                    else {
                        prev = current;
                        current = current->next;
                    }
                }
            }
            std::cout << "Seat Remains Unallocated" << std::endl;
        }

        // Part C SOlution Function
        // **important design note**
        /*the function signature provided in the problem statement uses a Node* data member
        i.e., the head of other list (dummy.list) but the design followed in this question
        keeps the Node* member of the List private from any outer access. Since, it is private
        , this design cannot specifically use the Node* from other list and hence the signature
        is altered according to current system design :)*/

        void mergeWaitLists(List& dummy)   {
            Node* i = this->head;   // head data is safe now
            Node* j = dummy.head;

            Node* mergeTail = nullptr;  // this tracks the last node of merged list

            this->head = nullptr;   // now we can push new data in head adn know atleast one element has come

            while (i != nullptr && j != nullptr)    {
                Node* winner = nullptr;

                if (i->cgpa > j->cgpa ||
                    (i->cgpa == j->cgpa && i->requestTime < j->requestTime))    {
                        winner = i;
                        i = i->next;
                    }
                else {
                    winner = j;
                    j = j->next;
                }
                // the winner node has not been merged yet.. it only sits as winner == i || j
                if (mergeTail == nullptr)
                    this->head = winner;
                else 
                    mergeTail->next = winner;

                // after this, move mergeTail to next Node since that would be now last and its
                // previous winner
                mergeTail = winner;
            }

            // at this point we have lost our tail pointer but we can locate it later, after
            // merging the remainder nodes

            // design node for next part is that the remainder node and others if any are
            // already linked since they belong to one of the two lists.. so linking one
            // to last winner would link the whole remaining list and its already in sortd
            // order so te result would also be sorted
            Node* remainder = (i == nullptr) ? j : i;
            
            // there must exist a case where the second list is empty (j == nullptr from 
            // beginning), that means the loop never runs and hence head (this->head) must 
            // be linked to remainder
            if (mergeTail == nullptr)
                this->head = remainder;
            else {
                mergeTail->next = remainder;
            }

            // now lets rellocate our tail since it has been lost during merging operation
            // this List contains a *tail, but if your design doesn't have tail, you are already 
            // done since no *tail to keep updated
            if (head == nullptr)
                tail = nullptr;
            else{
                Node* temp = head;
                while (temp->next != nullptr)   {
                    temp = temp->next;
                }
                tail = temp;
                temp = nullptr;
            }

            // other list is now empty
            dummy.head = nullptr;
            dummy.tail = nullptr;
            this->length += dummy.length;
            dummy.length = 0;
        }

        void printList() {
            Node* temp = head;
            while (temp != nullptr) {
                std::cout << "[ID:" << temp->studentID
                       << " cgpa:" << temp->cgpa
                       << " rt:" << temp->requestTime
                       << " credits:" << temp->creditHoursEnrolled
                    << " skip:" << temp->skipCount << "] -> ";
            temp = temp->next;
            }
    std::cout << "nullptr" << std::endl;
        }
};

int main()  {
    // test main
    std::cout << "Test 1: insertStudent (Part A)\n";
    List listA;
    listA.insertStudent(1, 3.5, 1, 15);  // S1
    listA.insertStudent(2, 3.8, 2, 16);  // S2
    listA.insertStudent(3, 3.8, 1, 12);  // S3
    listA.insertStudent(4, 3.2, 3, 9);   // S4

    std::cout << "Expected: S3 -> S2 -> S1 -> S4\n";
    listA.printList();

    std::cout << "\nTest 2: allocateSeat (Part B)\n";
    std::cout << "Allocating a seat...\n";
    listA.allocateSeat();   // S3 is eligible (12+3<=18) -> allocated
    std::cout << "Expected: S2 -> S1 -> S4\n";
    listA.printList();

    std::cout << "\nTest 3: allocateSeat skip/auto-drop\n";
    List listB;
    listB.insertStudent(7, 3.0, 1, 17);  // S7: 17+3=20 > 18, ineligible
    listB.insertStudent(8, 2.9, 2, 16);  // S8: 16+3=19 > 18, ineligible
    listB.insertStudent(9, 2.8, 3, 6);   // S9: 6+3=9  <= 18, eligible

    std::cout << "Before:\n";
    listB.printList();

    // call allocateSeat a couple of times to naturally build up skipCount,
    // then a final call to see eligible allocation happen
    listB.allocateSeat();  // S9 eligible immediately -> allocated
    std::cout << "After one allocateSeat call:\n";
    listB.printList();

    std::cout << "\nTest 4: mergeWaitLists (Part C)\n";
    List listC;
    listC.insertStudent(10, 3.9, 1, 5);  // S10
    listC.insertStudent(11, 3.1, 1, 5);  // S11

    List listD;
    listD.insertStudent(12, 3.6, 1, 5);  // S12
    listD.insertStudent(13, 3.0, 1, 5);  // S13

    std::cout << "listC before merge:\n";
    listC.printList();
    std::cout << "listD before merge:\n";
    listD.printList();

    listC.mergeWaitLists(listD);

    std::cout << "listC after merge (expected: S10 -> S12 -> S11 -> S13):\n";
    listC.printList();
    std::cout << "listD after merge (should be empty):\n";
    listD.printList();
    
    return 0;
}