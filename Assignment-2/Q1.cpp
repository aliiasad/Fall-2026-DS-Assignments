#include <iostream>
#include <string>

// siding capacity, defined in spec
const int SIDING_CAPACITY = 3;

// design note: Coach is not nested (private) inside one container class like Node is in List
// because a coach travels between the entry line, the siding, the platform and track 2, so
// every one of them has to know about it
class Coach {
    public:
        // required data members
        std::string coachID;
        int weightTons;
        int slot;   // position this coach must take in the outgoing train
        int loops;  // how many times this coach has gone around the yard

        // address of next pointer
        Coach* next;

        // constructors (def. and para.)
        Coach() : coachID(), weightTons(0), slot(0), loops(0) {
            next = nullptr;
        }

        Coach(std::string coachID, int weightTons, int slot) {
            this->coachID = coachID;
            this->weightTons = weightTons;
            this->slot = slot;

            // def values (no input)
            this->loops = 0;
            this->next = nullptr;
        }

        // prints a single coach, detailed also shows the weight
        void printCoach(bool detailed) {
            if (detailed)
                std::cout << coachID << "(" << weightTons << "t, slot " << slot << ")";
            else
                std::cout << coachID << "(" << slot << ")";
        }
};

// one general queue class, it is used for the entry line, the platform and track 2
class CoachQueue {
    private:
        Coach* head;    // front of the queue (only this one can be seen or removed)
        Coach* tail;    // rear of the queue
        int count;      // current length of queue
        bool enforceOrder;  // slot rule, switched on only for the platform
        int expectedSlot;   // next slot the platform accepts

        // helper for turnAround, the recursion itself is the extra storage so only
        // enqueue and dequeue are used
        void reverseRec() {
            if (isEmpty())
                return;
            Coach* c = dequeue();
            reverseRec();
            enqueue(c);
        }

    public:
        // constructor, the rule is off by default (entry line and track 2 are plain queues)
        CoachQueue(bool enforceOrder = false) {
            head = nullptr;
            tail = nullptr;
            count = 0;
            this->enforceOrder = enforceOrder;
            expectedSlot = 1;
        }

        // a queue owns raw nodes so copying it would free the same coaches twice
        CoachQueue(const CoachQueue&) = delete;
        CoachQueue& operator=(const CoachQueue&) = delete;

        // destructor to free every remaining coach
        ~CoachQueue() {
            while (!isEmpty())
                delete dequeue();
        }

        bool isEmpty() const {
            return head == nullptr;
        }

        // only the front coach can be seen
        Coach* front() const {
            return head;
        }

        // the slot the platform is waiting for, shunt() continues from here
        int getExpectedSlot() const {
            return expectedSlot;
        }

        void enqueue(Coach* c) {
            c->next = nullptr;
            if (tail != nullptr)
                tail->next = c;
            else    // first coach of the queue
                head = c;
            tail = c;
            count++;
        }

        // used to put a coach back on the front when it was taken out but could not be placed
        void enqueueFront(Coach* c) {
            c->next = head;
            head = c;
            if (tail == nullptr)
                tail = c;
            count++;
        }

        Coach* dequeue() {
            if (head == nullptr)
                return nullptr;
            Coach* c = head;
            head = head->next;
            if (head == nullptr)
                tail = nullptr;
            c->next = nullptr;
            count--;
            return c;
        }

        // Part A Solution Function
        bool joinPlatform(Coach* c) {
            if (enforceOrder && c->slot != expectedSlot) {
                std::cout << "ERROR [WrongSlot]: Coach " << c->coachID << " has slot " << c->slot
                          << " but the platform expects slot " << expectedSlot << "." << std::endl;
                return false;
            }
            enqueue(c);
            // expected slot goes up after every coach (kept even after the rule is off so
            // that shunt() never loses track of it)
            expectedSlot++;
            return true;
        }

        // the middle of the queue cannot be reached, so every coach is taken out from the
        // front and put back on the rear, after n rounds the order is the same as before
        Coach* findByID(const std::string& coachID) {
            Coach* found = nullptr;
            int n = count;
            for (int i = 0; i < n; i++) {
                Coach* c = dequeue();
                if (c->coachID == coachID)
                    found = c;
                enqueue(c);
            }
            return found;
        }

        Coach* findBySlot(int slot) {
            Coach* found = nullptr;
            int n = count;
            for (int i = 0; i < n; i++) {
                Coach* c = dequeue();
                if (c->slot == slot)
                    found = c;
                enqueue(c);
            }
            return found;
        }

        // Print function (required for every class)
        void print(bool detailed = false) {
            if (isEmpty()) {
                std::cout << "(empty)";
                return;
            }
            // same rotate technique as the find functions
            int n = count;
            for (int i = 0; i < n; i++) {
                Coach* c = dequeue();
                c->printCoach(detailed);
                if (i < n - 1)
                    std::cout << " -> ";
                enqueue(c);
            }
        }

        // Part C Solution Function
        // design note: the slot rule is just a flag of the platform queue, turning the flag
        // off here is how the reversed order is not rejected after assembly
        void turnAround() {
            enforceOrder = false;
            reverseRec();
        }

        // Part C Solution Function
        // keeps every coach up to and including coachID, moves the rest (same order) to track2
        bool splitAfter(const std::string& coachID, CoachQueue& track2) {
            if (findByID(coachID) == nullptr) {
                std::cout << "ERROR [CoachNotFound]: " << coachID
                          << " is not on the platform. Train unchanged: ";
                print();
                std::cout << std::endl;
                return false;
            }

            bool passed = false;    // true once the given coach has been kept
            int n = count;
            for (int i = 0; i < n; i++) {
                Coach* c = dequeue();
                if (!passed) {
                    enqueue(c);
                    if (c->coachID == coachID)
                        passed = true;
                }
                else
                    track2.enqueue(c);
            }
            return true;
        }
};

// the dead end siding (stack), only the top coach can be seen or removed
class Siding {
    private:
        Coach* top;
        int count;

        // pushes without any rule, only used to put coaches back after a search/print
        void forcePush(Coach* c) {
            c->next = top;
            top = c;
            count++;
        }

        // coaches are popped into a temporary siding (which reverses them) and popped back
        // again, so the siding looks unchanged afterwards
        Coach* findCoach(bool byID, const std::string& coachID, int slot) {
            Siding temp;
            Coach* found = nullptr;
            while (!isEmpty()) {
                Coach* c = pop();
                if (byID) {
                    if (c->coachID == coachID)
                        found = c;
                }
                else {
                    if (c->slot == slot)
                        found = c;
                }
                temp.forcePush(c);
            }
            while (!temp.isEmpty())
                forcePush(temp.pop());
            return found;
        }

    public:
        // constructor
        Siding() {
            top = nullptr;
            count = 0;
        }

        // same reason as CoachQueue
        Siding(const Siding&) = delete;
        Siding& operator=(const Siding&) = delete;

        // destructor to free every remaining coach
        ~Siding() {
            while (!isEmpty())
                delete pop();
        }

        bool isEmpty() const {
            return top == nullptr;
        }

        // Part A Solution Function
        Coach* peek() const {
            return top;
        }

        // silent version of the siding rule, shunt() uses it because a go-around is normal flow
        // and must not print an error, push() below prints the reason for the same rule
        bool canPark(Coach* c) const {
            if (count >= SIDING_CAPACITY)
                return false;
            // empty siding takes anything, otherwise the top must be at least as heavy
            return top == nullptr || top->weightTons >= c->weightTons;
        }

        // Part A Solution Function
        bool push(Coach* c) {
            if (count >= SIDING_CAPACITY) {
                std::cout << "ERROR [SidingFull]: Siding is full (" << SIDING_CAPACITY
                          << " coaches); cannot park " << c->coachID << "." << std::endl;
                return false;
            }
            if (top != nullptr && top->weightTons < c->weightTons) {
                std::cout << "ERROR [CoachTooHeavy]: Coach " << c->coachID << " (" << c->weightTons
                          << "t) is heavier than siding top " << top->coachID << " ("
                          << top->weightTons << "t)." << std::endl;
                return false;
            }
            forcePush(c);
            return true;
        }

        // Part A Solution Function
        Coach* pop() {
            if (top == nullptr)
                return nullptr;
            Coach* c = top;
            top = top->next;
            c->next = nullptr;
            count--;
            return c;
        }

        Coach* findByID(const std::string& coachID) {
            return findCoach(true, coachID, 0);
        }

        Coach* findBySlot(int slot) {
            return findCoach(false, "", slot);
        }

        // Print function (required for every class), top first
        void print() {
            if (isEmpty()) {
                std::cout << "(empty)";
                return;
            }
            Siding temp;
            bool first = true;
            while (!isEmpty()) {
                Coach* c = pop();
                if (!first)
                    std::cout << " -> ";
                c->printCoach(true);
                first = false;
                temp.forcePush(c);
            }
            while (!temp.isEmpty())
                forcePush(temp.pop());
        }
};

class Yard {
    private:
        CoachQueue entry;       // plain first come first served
        Siding siding;
        CoachQueue platform;    // slot rule is on
        CoachQueue track2;      // plain queue, filled by splitAfter

        // a coach can be in any of the four places so all of them are searched
        bool isIDUsed(const std::string& coachID) {
            return entry.findByID(coachID) != nullptr || siding.findByID(coachID) != nullptr ||
                   platform.findByID(coachID) != nullptr || track2.findByID(coachID) != nullptr;
        }

        // returns the coach already holding this slot (nullptr if the slot is free)
        Coach* findSlotOwner(int slot) {
            Coach* owner = entry.findBySlot(slot);
            if (owner == nullptr)
                owner = siding.findBySlot(slot);
            if (owner == nullptr)
                owner = platform.findBySlot(slot);
            if (owner == nullptr)
                owner = track2.findBySlot(slot);
            return owner;
        }

    public:
        // constructor, entry and track 2 are plain, only the platform has the slot rule
        Yard() : entry(false), platform(true), track2(false) {
        }

        // Part A Solution Function
        bool arriveCoach(std::string coachID, int weightTons, int slot) {
            std::cout << "arriveCoach(" << coachID << ", " << weightTons << ", " << slot << ") -> ";

            if (coachID.empty()) {
                std::cout << "ERROR [InvalidCoach]: Coach id is empty." << std::endl;
                return false;
            }
            if (isIDUsed(coachID)) {
                std::cout << "ERROR [InvalidCoach]: Coach id " << coachID << " is already used." << std::endl;
                return false;
            }
            if (weightTons <= 0) {
                std::cout << "ERROR [InvalidCoach]: Coach " << coachID << " has weight " << weightTons
                          << " (must be positive)." << std::endl;
                return false;
            }
            if (slot <= 0) {
                std::cout << "ERROR [InvalidCoach]: Coach " << coachID << " has slot " << slot
                          << " (must be positive)." << std::endl;
                return false;
            }
            Coach* owner = findSlotOwner(slot);
            if (owner != nullptr) {
                std::cout << "ERROR [InvalidCoach]: Slot " << slot << " is already taken by coach "
                          << owner->coachID << "." << std::endl;
                return false;
            }

            entry.enqueue(new Coach(coachID, weightTons, slot));
            std::cout << "accepted" << std::endl;
            return true;
        }

        // Part B Solution Function
        bool shunt() {
            // design note: next is not started from 1 but taken from the platform itself so
            // both counters can never disagree (e.g. when shunt is called again after a jam)
            int next = platform.getExpectedSlot();
            int goArounds = 0;
            std::string goList = "";

            while (!entry.isEmpty()) {
                Coach* f = entry.dequeue();

                if (f->slot == next) {
                    platform.joinPlatform(f);
                    next++;
                    // drain the siding
                    while (!siding.isEmpty() && siding.peek()->slot == next) {
                        platform.joinPlatform(siding.pop());
                        next++;
                    }
                }
                else if (siding.canPark(f)) {
                    siding.push(f);
                }
                // the siding refused f, first time it gets its one go-around
                else if (f->loops == 0) {
                    f->loops = 1;
                    goArounds++;
                    if (!goList.empty())
                        goList = goList + ", ";
                    goList = goList + f->coachID;
                    entry.enqueue(f);
                }
                // refused for the second time, yard jam
                else {
                    Coach* t = siding.peek();   // cant be null since the siding refused f
                    std::cout << "Platform so far: ";
                    platform.print();
                    std::cout << std::endl;
                    std::cout << "ERROR [YardJam]: Coach " << f->coachID << " (" << f->weightTons
                              << "t, slot " << f->slot << ") was rejected twice; siding top is "
                              << t->coachID << " (" << t->weightTons << "t, slot " << t->slot
                              << ")." << std::endl;
                    // f was only taken out to look at it, so it goes back and nothing is lost
                    entry.enqueueFront(f);
                    return false;
                }
            }

            // entry line is empty but the siding still holds coaches
            if (!siding.isEmpty()) {
                Coach* t = siding.peek();
                std::cout << "Platform so far: ";
                platform.print();
                std::cout << std::endl;
                std::cout << "ERROR [Deadlock]: Entry line empty, expecting slot " << next
                          << " but siding top is " << t->coachID << " (" << t->weightTons
                          << "t, slot " << t->slot << ")." << std::endl;
                return false;
            }

            std::cout << "Train formed: ";
            platform.print();
            std::cout << std::endl;
            std::cout << "Go-arounds: " << goArounds;
            if (goArounds > 0)
                std::cout << " (" << goList << ")";
            std::cout << std::endl;
            return true;
        }

        // Part C Solution Function
        void turnAround() {
            platform.turnAround();
            std::cout << "turnAround() -> Platform: ";
            platform.print();
            std::cout << std::endl;
        }

        // Part C Solution Function
        void splitAfter(std::string coachID) {
            // the error message (if any) is printed by the platform itself
            if (platform.splitAfter(coachID, track2)) {
                std::cout << "splitAfter(" << coachID << ") -> Platform: ";
                platform.print();
                std::cout << "  Track 2: ";
                track2.print();
                std::cout << std::endl;
            }
        }

        // Part D Solution Function
        void showStatus() {
            std::cout << "Yard status:-" << std::endl;
            std::cout << "Entry   : ";
            entry.print(true);
            std::cout << std::endl;
            std::cout << "Siding  : ";
            siding.print();
            std::cout << "   (top first)" << std::endl;
            std::cout << "Platform: ";
            platform.print(true);
            std::cout << std::endl;
            // track 2 is shown only once a train has been split
            if (!track2.isEmpty()) {
                std::cout << "Track 2 : ";
                track2.print(true);
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
};

int main() {
    // Part A: arrival validation
    std::cout << "----- Part A: arrival validation -----" << std::endl;
    {
        Yard y;
        y.arriveCoach("A", 30, 2);  // accepted
        y.arriveCoach("B", 0, 3);   // weight error
        y.arriveCoach("C", 20, 2);  // slot taken by A
        y.arriveCoach("A", 10, 9);  // id used
        y.arriveCoach("", 10, 9);   // empty id
        y.arriveCoach("D", 10, 0);  // slot error
        y.arriveCoach("E", -5, 4);  // negative weight
        y.showStatus();
    }

    // Part A: siding rules checked directly (full, too heavy, unchanged after refusal)
    std::cout << "----- Part A: siding rules -----" << std::endl;
    {
        Siding s;
        Coach* c1 = new Coach("S1", 40, 1);
        Coach* c2 = new Coach("S2", 40, 2);     // equal weight is allowed
        Coach* c3 = new Coach("S3", 50, 3);     // heavier than top, refused
        Coach* c4 = new Coach("S4", 10, 4);
        Coach* c5 = new Coach("S5", 5, 5);      // siding is full by now, refused
        s.push(c1);
        s.push(c2);
        if (!s.push(c3))
            delete c3;
        s.push(c4);
        if (!s.push(c5))
            delete c5;
        std::cout << "Siding (expected S4 -> S2 -> S1): ";
        s.print();
        std::cout << std::endl;
        std::cout << "peek (expected S4): " << s.peek()->coachID << std::endl;
        std::cout << "pop (expected S4): " << s.pop()->coachID << std::endl;
        delete c4;
        std::cout << "Siding after pop (expected S2 -> S1): ";
        s.print();
        std::cout << std::endl;
    }

    // Part A: platform slot rule checked directly
    std::cout << "----- Part A: platform rule -----" << std::endl;
    {
        CoachQueue platform(true);
        Coach* w1 = new Coach("W1", 10, 1);
        Coach* w2 = new Coach("W2", 10, 3);     // expected slot is 2, refused
        Coach* w3 = new Coach("W3", 10, 2);
        platform.joinPlatform(w1);
        if (!platform.joinPlatform(w2))
            delete w2;
        platform.joinPlatform(w3);
        std::cout << "Platform (expected W1(1) -> W3(2)): ";
        platform.print();
        std::cout << std::endl;
    }

    // Part B: Example 1 (no conflicts)
    std::cout << "----- Example 1 (no conflicts) -----" << std::endl;
    {
        Yard y;
        y.arriveCoach("A", 50, 3);
        y.arriveCoach("B", 40, 1);
        y.arriveCoach("C", 30, 2);
        y.arriveCoach("D", 20, 5);
        y.arriveCoach("E", 10, 4);
        y.shunt();  // expected B(1) -> C(2) -> A(3) -> E(4) -> D(5), Go-arounds: 0
    }

    // Part B + C: Example 2 (one go-around), then turn-around and split
    std::cout << "----- Example 2 (one go-around) + Part C -----" << std::endl;
    {
        Yard y;
        y.arriveCoach("A", 30, 2);
        y.arriveCoach("B", 50, 3);
        y.arriveCoach("C", 40, 1);
        y.shunt();  // expected C(1) -> A(2) -> B(3), Go-arounds: 1 (B)
        y.showStatus();
        y.turnAround();     // expected B(3) -> A(2) -> C(1)
        y.splitAfter("A");  // expected Platform: B(3) -> A(2)  Track 2: C(1)
        y.splitAfter("Z");  // expected CoachNotFound, train unchanged
        y.showStatus();
        // coaches moved to track 2 must still count for the arrival checks
        y.arriveCoach("C", 10, 8);  // id used (C is on track 2)
        y.arriveCoach("Q", 10, 1);  // slot taken by C on track 2
    }

    // Part B: Example 3 (yard jam)
    std::cout << "----- Example 3 (yard jam) -----" << std::endl;
    {
        Yard y;
        y.arriveCoach("A", 30, 3);
        y.arriveCoach("B", 50, 4);
        y.arriveCoach("C", 40, 2);
        y.arriveCoach("D", 10, 1);
        y.shunt();
        y.showStatus();
        std::cout << "shunt again, same jam expected and nothing lost:" << std::endl;
        y.shunt();
        y.showStatus();
    }

    // Part B: Example 4 (deadlock)
    std::cout << "----- Example 4 (deadlock) -----" << std::endl;
    {
        Yard y;
        y.arriveCoach("A", 30, 3);
        y.arriveCoach("B", 50, 2);
        y.arriveCoach("C", 20, 4);
        y.arriveCoach("D", 10, 1);
        y.shunt();
        y.showStatus();
        std::cout << "shunt again, expecting slot 3 again:" << std::endl;
        y.shunt();
    }

    // Part B: siding full while shunting (go-around because of capacity, then jam)
    std::cout << "----- Extra: siding full during shunt -----" << std::endl;
    {
        Yard y;
        y.arriveCoach("A", 90, 9);
        y.arriveCoach("B", 80, 8);
        y.arriveCoach("C", 70, 7);
        y.arriveCoach("D", 60, 6);  // siding is full (A, B, C), refused
        y.arriveCoach("E", 50, 1);
        y.shunt();
        y.showStatus();
    }

    // Part B: empty yard
    std::cout << "----- Extra: empty yard -----" << std::endl;
    {
        Yard y;
        y.shunt();
        y.showStatus();
    }

    return 0;
}