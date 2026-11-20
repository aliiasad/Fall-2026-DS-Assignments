#include <iostream>


/* The ControlRoom class contains every node and heap class inside it so a
single incident/unit/heap can't be accessed outside its vicinity. That's why
Incident, Unit, IncidentHeap and UnitHeap remain private entities of the
control room. The outside world only talks to the control room using plain ints
(ids, times etc.) and never sees a pointer.*/
class ControlRoom   {
    private:
        // incident heap handles both dispatch heap and shed heap using flags
        // so it removes the need of making different classes for almost similar
        // tasks
        class Incident  {
            public:
                int incidentID;
                int severity;
                int arrivalTime;
                int dueTime;
                int travel;
                int escalations;

                // since the incident node tracks itself inisde both Unit Heap
                // and Incident Heap (both dispatch and shed), it as its locating
                // pointers / index specifiers
                int heapIndex;
                int shedIndex;

                // def. and para. constructors (all inits are based on spec's directions)
                Incident() : incidentID(0), severity(1), arrivalTime(0), dueTime(1),
                travel(0), escalations(0), heapIndex(-1), shedIndex(-1) {}

                Incident(int id, int sev, int arrival, int due, int trv) :
                incidentID(id), severity(sev), arrivalTime(arrival), dueTime(due),
                travel(trv), escalations(0), heapIndex(-1), shedIndex(-1) {}

                /*design note: a single integer key will not work here because
                the order depends on four fields, so this is our own comparison.
                x is more urgent than y if (in this order) it has higher effective
                severity, earlier due time, earlier arrival time, smaller id...
                since ids are unique, two different incidents are never equal,
                so this is a strict total order (no ties to worry about)*/

                // implement effective severity first

                // since we call it using an object, no parameters required...
                // the object has its own sev and escalation
                int effectiveSeverity ()   {
                    int effSev = severity + escalations;
                    return (effSev > 5) ? 5 : effSev;
                }

                // did it static since it has to be called outside this private class
                static bool moreUrgent (Incident* x, Incident* y)  {
                    if (x->effectiveSeverity() != y->effectiveSeverity())
                        return x->effectiveSeverity() > y->effectiveSeverity();

                    if (x->dueTime != y->dueTime)
                        return x->dueTime < y->dueTime;

                    if (x->arrivalTime != y->arrivalTime)
                        return x->arrivalTime < y->arrivalTime;
                        
                    return x->incidentID < y->incidentID;   // cannot be equal (is unique)
                }
        };

        /*design note: the dispatch heap is a MAX heap (most urgent incident on
        top) and the shed heap is a MIN heap (least urgent incident on top),
        both measured by urgency. Apart from that, their logic (sift up, sift
        down, insert, remove) is exactly the same. So instead of writing a max
        heap class and a min heap class, one class does both, and a boolean
        (isMinHeap) decides which one an object is:
        isMinHeap = false -> max heap (dispatch), writes heapIndex of the incident
        isMinHeap = true  -> min heap (shed), writes shedIndex of the incident
        Like a normal priority queue there are four sift functions, incUp and
        incDown for the min heap, decUp and decDown for the max heap. The only
        difference from a plain priority queue is that "smaller" and "bigger"
        are not < and > on numbers, they are isLess() and isGreater() which use
        the four field urgency rule, and every swap also updates the positions
        remembered inside the incidents.
        The heap stores raw pointers in a raw array (Incident** arr, made with
        new[]). Swapping moves only these pointers, never the incident objects.
        It does NOT own the incident nodes (both heaps point to the same nodes,
        so deleting from here would double delete), the control room deletes
        the nodes.*/
        class IncidentHeap  {
            public:
                Incident** arr;
                int size;
                int capacity;
                bool isMinHeap; // true for Min Heap (least urgent on top),
                                // false for Max Heap (most urgent on top)

                // "smaller" means LESS urgent and "bigger" means MORE urgent
                bool isLess(Incident* a, Incident* b) {
                    return Incident::moreUrgent(b, a);
                }

                bool isGreater(Incident* a, Incident* b) {
                    return Incident::moreUrgent(a, b);
                }

                // reads/writes the position field that belongs to this heap
                int getIndex(Incident* node) {
                    return (isMinHeap ? node->shedIndex : node->heapIndex);
                }

                void setIndex(Incident* node, int index)  {
                    if (isMinHeap)
                        node->shedIndex = index;
                    else
                        node->heapIndex = index;
                }

                /*every time a pointer is put in the array, its position field
                is updated right there, so positions are correct after every
                swap, not just at some places*/
                void place(int index, Incident* node)   {
                    arr[index] = node;
                    setIndex(node, index);
                }

                void swapNodes(int i, int j)    {
                    Incident* temp = arr[i];
                    place(i, arr[j]);
                    place(j, temp);
                }

                // min heap: push the node down while a child is smaller (less urgent)
                void incDown(int currParent) {
                    int smallest = currParent;
                    int leftChild = 2 * currParent + 1;
                    int rightChild = 2 * currParent + 2;

                    if (leftChild < size && isLess(arr[leftChild], arr[smallest]))
                        smallest = leftChild;

                    if (rightChild < size && isLess(arr[rightChild], arr[smallest]))
                        smallest = rightChild;

                    if (smallest != currParent) {
                        swapNodes(smallest, currParent);
                        incDown(smallest);
                    }
                }

                // max heap: push the node down while a child is larger (more urgent)
                void decDown(int currParent) {
                    int largest = currParent;
                    int leftChild = 2 * currParent + 1;
                    int rightChild = 2 * currParent + 2;

                    if (leftChild < size && isGreater(arr[leftChild], arr[largest]))
                        largest = leftChild;

                    if (rightChild < size && isGreater(arr[rightChild], arr[largest]))
                        largest = rightChild;

                    if (largest != currParent) {
                        swapNodes(largest, currParent);
                        decDown(largest);
                    }
                }

                // min heap: pull the node up while it is smaller than its parent
                void incUp(int idx) {
                    if (idx <= 0)
                        return;

                    int parent = (idx - 1) / 2;
                    if (isLess(arr[idx], arr[parent])) {
                        swapNodes(idx, parent);
                        incUp(parent);
                    }
                }

                // max heap: pull the node up while it is larger than its parent
                void decUp(int idx) {
                    if (idx <= 0)
                        return;

                    int parent = (idx - 1) / 2;
                    if (isGreater(arr[idx], arr[parent])) {
                        swapNodes(idx, parent);
                        decUp(parent);
                    }
                }

                // para constructor, raw array of pointers made here
                IncidentHeap(int maxSize, bool minHeap = true) {
                    capacity = maxSize;
                    size = 0;
                    isMinHeap = minHeap;
                    arr = new Incident*[capacity];
                    for (int i = 0; i < capacity; i++)
                        arr[i] = nullptr;
                }

                // heaps hold raw memory, so copying is blocked (shallow copy = double delete)
                IncidentHeap(const IncidentHeap&) = delete;
                IncidentHeap& operator=(const IncidentHeap&) = delete;

                ~IncidentHeap() {
                    delete[] arr;   // only the pointer array, not the nodes
                }

                bool isEmpty() const {
                    return size == 0;
                }

                bool isFull() const {
                    return size == capacity;
                }

                int getSize() const {
                    return size;
                }

                void makeEmpty() {
                    for (int i = 0; i < size; i++)  {
                        setIndex(arr[i], -1);
                        arr[i] = nullptr;
                    }
                    size = 0;
                }

                Incident* top() {
                    return (size == 0 ? nullptr : arr[0]);
                }

                // put at the end, then sift up (min or max version) -> O(log n)
                void enqueue(Incident* value) {
                    if (value == nullptr)
                        return;
                    if (isFull()) {
                        std::cout << "Queue overflow\n";
                        return;
                    }
                    place(size, value);
                    if (isMinHeap) {
                        incUp(size);
                    } else {
                        decUp(size);
                    }
                    size++;
                }

                // takes out the top (most urgent for max heap, least urgent for min heap)
                void dequeue(Incident*& result) {
                    if (isEmpty()) {
                        std::cout << "Queue underflow\n";
                        return;
                    }
                    result = arr[0];
                    removeAt(0);
                }

                /*design note: removal of ANY position (not only the top).
                last element comes and sits on the hole. Now that element is a
                stranger in that place, so it may need to go up OR down:
                up   -> if hole was in a different branch and the last one is
                        better than the hole's parent
                down -> if it is worse than the hole's children
                exactly one of the two will actually move, never both.*/
                void removeAt(int index)    {
                    if (index < 0 || index >= size)
                        return;

                    Incident* removed = arr[index];
                    setIndex(removed, -1);      // not in this heap anymore
                    size--;

                    // if we removed the last element, nothing to fill
                    if (index == size)  {
                        arr[size] = nullptr;
                        return;
                    }

                    place(index, arr[size]);
                    arr[size] = nullptr;
                    fix(index);
                }

                /*called when the priority of the node at index has changed
                (escalate / aging). we dont know the direction, so we check
                the parent: if it beats its parent -> up, else try down*/
                void fix(int index) {
                    if (index < 0 || index >= size)
                        return;

                    bool goUp = false;
                    if (index > 0)  {
                        int parent = (index - 1) / 2;
                        if (isMinHeap)
                            goUp = isLess(arr[index], arr[parent]);
                        else
                            goUp = isGreater(arr[index], arr[parent]);
                    }

                    if (goUp)   {
                        if (isMinHeap)
                            incUp(index);
                        else
                            decUp(index);
                    }
                    else    {
                        if (isMinHeap)
                            incDown(index);
                        else
                            decDown(index);
                    }
                }

                // prints in array order and then level by level
                void print() {
                    if (size == 0)  {
                        std::cout << "Heap array: empty" << std::endl;
                        return;
                    }

                    std::cout << "Heap array:";
                    for (int i = 0; i < size; i++)
                        std::cout << " " << arr[i]->incidentID << "(S" << arr[i]->effectiveSeverity() << ")";
                    std::cout << std::endl;

                    // level 0 has 1 node, level 1 has 2, level 2 has 4 ... (doubles)
                    int level = 0;
                    int first = 0;
                    int levelCount = 1;
                    while (first < size)    {
                        std::cout << "Level " << level << ": ";
                        for (int i = first; i < first + levelCount && i < size; i++)    {
                            if (i > first)
                                std::cout << ", ";
                            std::cout << arr[i]->incidentID << "(S" << arr[i]->effectiveSeverity() << ")";
                        }
                        std::cout << std::endl;
                        first += levelCount;
                        levelCount *= 2;
                        level++;
                    }
                }

                // only the board line of showStatus() uses this (array order)
                void printArrayOrder() {
                    if (size == 0)  {
                        std::cout << "empty";
                        return;
                    }
                    for (int i = 0; i < size; i++)  {
                        if (i > 0)
                            std::cout << " ";
                        std::cout << arr[i]->incidentID << "(S" << arr[i]->effectiveSeverity() << ")";
                    }
                }

        };

        // kinda node for ambulance unit
        class Unit  {
            public:
                int unitID;
                int freeAt;

                // similar as above.. tracker / pointer for it sown location on heap
                int heapIndex;

                // def. and para. constructors
                Unit() : unitID(0), freeAt(0), heapIndex(-1) {}

                Unit(int id, int free) : unitID(id), freeAt(free), heapIndex(-1) {}

                // earliest free unit wins, tie -> smaller unit id wins
                // did static so it can be used outside this class (same as above)
                static bool freesEarlier(Unit* a, Unit* b) {
                    if (a->freeAt != b->freeAt)
                        return a->freeAt < b->freeAt;

                    return a->unitID < b->unitID;
                }
        };

        /*design note: the unit heap is a MIN heap, where "smaller" means "becomes
        free earlier" (tie -> smaller unitID), so the unit that can leave first
        is always on top. It uses the same priority queue logic as the incident
        heap, enqueue/dequeue on top of incUp/incDown, but only the min heap
        versions are needed (there is no max heap of units), so there is no
        isMinHeap flag and no decUp/decDown here, they would be dead code.
        Two differences from the incident heap:
        1) a unit lives in only ONE heap, so there is one position (heapIndex)
           and this heap OWNS its units (it deletes them in the destructor and
           in makeEmpty), unlike the incident heap which only borrows pointers.
        2) after a dispatch the top unit's freeAt grows, so its place has to be
           fixed (fix()). It can only sink, but fix() checks both directions
           the same way as the incident heap.
        showStatus needs the units in chosen order WITHOUT changing the real
        heap, so printSorted() works on a temporary copy of the pointer array
        and uses incDownCopy(), a version of incDown that never touches
        heapIndex (swapNodes/place would overwrite the real positions).*/
        class UnitHeap  {
            private:
                Unit** arr;
                int size;
                int capacity;

                // "smaller" unit = the one that becomes free earlier
                bool isLess(Unit* a, Unit* b) {
                    return Unit::freesEarlier(a, b);
                }

                // every time a pointer is put in the array, 
                // its heapIndex is updated right there
                void place(int index, Unit* node)   {
                    arr[index] = node;
                    node->heapIndex = index;
                }

                void swapNodes(int i, int j)    {
                    Unit* temp = arr[i];
                    place(i, arr[j]);
                    place(j, temp);
                }

                // min heap: push the node down while a child is smaller
                void incDown(int currParent) {
                    int smallest = currParent;
                    int leftChild = 2 * currParent + 1;
                    int rightChild = 2 * currParent + 2;

                    if (leftChild < size && isLess(arr[leftChild], arr[smallest]))
                        smallest = leftChild;

                    if (rightChild < size && isLess(arr[rightChild], arr[smallest]))
                        smallest = rightChild;

                    if (smallest != currParent) {
                        swapNodes(smallest, currParent);
                        incDown(smallest);
                    }
                }

                // min heap: pull the node up while it is smaller than its parent
                void incUp(int idx) {
                    if (idx <= 0)
                        return;

                    int parent = (idx - 1) / 2;
                    if (isLess(arr[idx], arr[parent])) {
                        swapNodes(idx, parent);
                        incUp(parent);
                    }
                }

                // same as incDown but on a temporary copy, so NO heapIndex is touched
                void incDownCopy(Unit** copy, int n, int currParent) {
                    int smallest = currParent;
                    int leftChild = 2 * currParent + 1;
                    int rightChild = 2 * currParent + 2;

                    if (leftChild < n && isLess(copy[leftChild], copy[smallest]))
                        smallest = leftChild;

                    if (rightChild < n && isLess(copy[rightChild], copy[smallest]))
                        smallest = rightChild;

                    if (smallest != currParent) {
                        Unit* temp = copy[smallest];
                        copy[smallest] = copy[currParent];
                        copy[currParent] = temp;
                        incDownCopy(copy, n, smallest);
                    }
                }

            public:
                // para constructor, raw array of pointers made here
                UnitHeap(int maxSize) {
                    capacity = maxSize;
                    size = 0;
                    arr = new Unit*[capacity];
                    for (int i = 0; i < capacity; i++)
                        arr[i] = nullptr;
                }

                // heaps hold raw memory, so copying is blocked (shallow copy = double delete)
                UnitHeap(const UnitHeap&) = delete;
                UnitHeap& operator=(const UnitHeap&) = delete;

                // this heap owns its units, so it frees them here
                ~UnitHeap() {
                    for (int i = 0; i < size; i++)
                        delete arr[i];
                    delete[] arr;
                }

                bool isEmpty() const {
                    return size == 0;
                }

                bool isFull() const {
                    return size == capacity;
                }

                int getSize() const {
                    return size;
                }

                // owner heap, so emptying it must free the units too (else they leak)
                void makeEmpty() {
                    for (int i = 0; i < size; i++)  {
                        delete arr[i];
                        arr[i] = nullptr;
                    }
                    size = 0;
                }

                Unit* top() {
                    return (size == 0 ? nullptr : arr[0]);
                }

                /*the unit pool is tiny and addUnit has no time requirement,
                so a plain scan is fine for checking duplicates*/
                bool exists(int unitID) {
                    for (int i = 0; i < size; i++)  {
                        if (arr[i]->unitID == unitID)
                            return true;
                    }
                    return false;
                }

                // put at the end, then sift up -> O(log n)
                void enqueue(Unit* value) {
                    if (value == nullptr)
                        return;
                    if (isFull()) {
                        std::cout << "Queue overflow\n";
                        return;
                    }
                    place(size, value);
                    incUp(size);
                    size++;
                }

                /*takes out the earliest free unit. the caller gets the pointer
                and becomes the owner of that unit (it is no longer in the heap)*/
                void dequeue(Unit*& result) {
                    if (isEmpty()) {
                        std::cout << "Queue underflow\n";
                        return;
                    }
                    result = arr[0];
                    result->heapIndex = -1;
                    size--;

                    if (size > 0)   {
                        place(0, arr[size]);
                        arr[size] = nullptr;
                        incDown(0);
                    }
                    else
                        arr[0] = nullptr;
                }

                /*called when the freeAt of the unit at index has changed
                (after a dispatch). we dont know the direction, so we check
                the parent: if it is smaller than its parent -> up, else down*/
                void fix(int index) {
                    if (index < 0 || index >= size)
                        return;

                    if (index > 0 && isLess(arr[index], arr[(index - 1) / 2]))
                        incUp(index);
                    else
                        incDown(index);
                }

                // prints earliest free first by extracting from a temporary copy
                void printSorted() {
                    if (size == 0)  {
                        std::cout << "none";
                        return;
                    }

                    Unit** copy = new Unit*[size];
                    for (int i = 0; i < size; i++)
                        copy[i] = arr[i];       // the copy of a valid heap is a valid heap

                    int n = size;
                    bool firstPrinted = true;
                    while (n > 0)   {
                        if (!firstPrinted)
                            std::cout << ", ";
                        firstPrinted = false;

                        std::cout << "Unit " << copy[0]->unitID << " (free at " << copy[0]->freeAt << ")";

                        // extract: last goes to top, then sift down (on the copy only)
                        copy[0] = copy[n - 1];
                        n--;
                        incDownCopy(copy, n, 0);
                    }
                    delete[] copy;
                }

                // all units in array order (for the print function of the class)
                void print() {
                    if (size == 0)  {
                        std::cout << "Unit heap array: empty" << std::endl;
                        return;
                    }
                    std::cout << "Unit heap array:";
                    for (int i = 0; i < size; i++)
                        std::cout << " " << arr[i]->unitID << "(free " << arr[i]->freeAt << ")";
                    std::cout << std::endl;
                }
        };

        int capacity;
        int maxUnits;

        IncidentHeap dispatchHeap;
        IncidentHeap shedHeap;
        UnitHeap unitHeap;

        // counters and showStatus
        int dispatched;
        int missed;
        int shed;
        int cancelled;
        int rejected;

        /*ids are from 1 to 999, so the incident is found from its id directly
        in O(1) with an array of node pointers indexed by id. nullptr means the
        incident is not on the board. We never search the heap array for an id*/
        Incident* byID[1000];

        /*used remembers that an id was accepted once (even if the incident was
        dispatched, shed, missed or cancelled later), because a reused id is an
        error. A BoardFull rejection never sets it, so that id stays free*/
        bool used[1000];

        // private helpers to add and remove

        void addToBoard(Incident* node) {
            dispatchHeap.enqueue(node);
            shedHeap.enqueue(node);
            byID[node->incidentID] = node;
        }
        
        /*the node leaves both heaps using the positions it remembers (O(log n)
        each). NOT dequeue, dequeue only takes out the top of a heap, but the
        incident to remove can be anywhere (cancel, shed). whatever has to be
        printed about the node must be printed BEFORE calling this*/
        void removeFromBoard(Incident* node)    {
            dispatchHeap.removeAt(node->heapIndex);
            shedHeap.removeAt(node->shedIndex);
            byID[node->incidentID] = nullptr;
            delete node;
        }

        Incident* findOnBoard(int incidentID)   {
            if (incidentID < 1 || incidentID > 999)
                return nullptr;
            return byID[incidentID];
        }

        void fixBothHeaps(Incident* node) {
            dispatchHeap.fix(node->heapIndex);
            shedHeap.fix(node->shedIndex);
        }

    public:
        // para. constructor
        ControlRoom(int capacity, int maxUnits) :
        capacity(capacity > 0 ? capacity : 1), maxUnits(maxUnits > 0 ? maxUnits : 0),
        dispatchHeap(capacity > 0 ? capacity : 1, false), 
        shedHeap(capacity > 0 ? capacity : 1, true), unitHeap(maxUnits > 0 ? maxUnits : 0),
        dispatched(0), missed(0), shed(0), cancelled(0), rejected(0) {
            for (int i = 0; i < 1000; i++)  {
                byID[i] = nullptr;
                used[i] = false;
            }
        }

        // a control room owns raw memory, so it must not be copied
        ControlRoom(const ControlRoom&) = delete;
        ControlRoom& operator=(const ControlRoom&) = delete;

        /*PART A + PART D: validates in the exact order of the spec, and
        then either inserts, sheds the least urgent one to make room, or
        rejects. A rejected report leaves everything unchanged, so the node is
        only created after all validation, and is deleted if rejected later.*/

        void reportIncident(int incidentID, int severity, int arrivalTime, int dueTime,
        int travel)   {
            if (incidentID > 999 || incidentID < 1) {
                std::cout << "ERROR [InvalidIncident]: Incident ID " << incidentID <<
                " is outside 1 to 999." << std::endl;
                return;
            }

            if (used[incidentID])   {
                std::cout << "ERROR [InvalidIncident]: ID " << incidentID <<
                " is already used." << std::endl;
                return;
            }
            
            if (severity > 5 || severity < 1)   {
                std::cout << "ERROR [InvalidIncident]: Incident " << incidentID <<
                " has severity " << severity << " (must be 1 to 5)." << std::endl;
                return;
            }

            if (arrivalTime < 0)    {
                std::cout << "ERROR [InvalidIncident]: Incident " << incidentID <<
                " has arrival time " << arrivalTime << " (must be 0 or more)." << std::endl;
                return;
            }

            // due time must be AFTER arrival time, so equal is also an error
            if (dueTime <= arrivalTime)  {
                std::cout << "ERROR [InvalidIncident]: Incident " << incidentID << 
                " has due time " << dueTime << ", which is not after arrival time " << arrivalTime
                << "." << std::endl;
                return;
            }

            if (travel <= 0)    {
                std::cout << "ERROR [InvalidIncident]: Incident " << incidentID <<
                " has travel time " << travel << " (must be more than 0)." << std::endl;
                return; 
            }

            Incident* newIncident = new Incident (incidentID, severity, arrivalTime, dueTime,
            travel);

            // above is Part A logic handled
            // now according to the board rules, if the newest w has a priority more than the
            // least important one, we push it in heap and fix it then.. else it is simply rejected
            
            // if w has more priority than taht of the least prior incident, then the least prior
            // incident is shed (not completely removed)
            if (dispatchHeap.isFull())  {
                Incident* w = shedHeap.top();

                if (w != nullptr && Incident::moreUrgent(newIncident, w)) {
                    std::cout << "Board full: Incident " << w->incidentID
                    << " (S" << w->effectiveSeverity() << ") shed to make room. ";
                    removeFromBoard(w);
                    shed++;
                }
                else {
                    // never stored, id stays free... used not touched as spec says
                    if (w != nullptr) {
                        std::cout << "ERROR [BoardFull]: Incident " << incidentID
                        << " (S" << newIncident->effectiveSeverity() << ") is less urgent than the least urgent waiting incident "
                        << w->incidentID << " (S" << w->effectiveSeverity()
                        << "); not accepted." << std::endl;
                    }
                    else {
                        std::cout << "ERROR [BoardFull]: Incident " << incidentID
                        << " cannot be stored; the board has no room." << std::endl;
                    }
                    rejected++;
                    delete newIncident;
                    return;
                }
            }

            addToBoard(newIncident);
            used[incidentID] = true;
            std::cout << "Incident " << incidentID << " accepted." << std::endl;
        }

        /*PART B: validation order is id positive, already exists, negative
        free time, pool full*/
        void addUnit(int unitID, int freeAt)    {
            if (unitID <= 0)    {
                std::cout << "ERROR [InvalidUnit]: Unit ID must be positive." << std::endl;
                return;
            }

            if (unitHeap.exists(unitID))    {
                std::cout << "ERROR [InvalidUnit]: Unit " << unitID << " already exists." << std::endl;
                return;
            }

            if (freeAt < 0) {
                std::cout << "ERROR [InvalidUnit]: Unit " << unitID << " has free-at time "
                << freeAt << " (must be 0 or more)." << std::endl;
                return;
            }

            if (unitHeap.isFull())  {
                std::cout << "ERROR [InvalidUnit]: Unit pool is full (maximum " << maxUnits
                << " units)." << std::endl;
                return;
            }

            unitHeap.enqueue(new Unit(unitID, freeAt));
            std::cout << "Unit " << unitID << " added (free at " << freeAt << ")." << std::endl;
        }

        /*PART B: handles the single most urgent incident. board is checked
        first, then units. In both outcomes (sent or missed) the incident
        leaves the board, only the unit differs*/

        void dispatch(int now)  {
            if (dispatchHeap.isEmpty()) {
                std::cout << "ERROR [NoIncident]: The board is empty; nothing to dispatch."
                << std::endl;
                return;
            }
            
            if (unitHeap.isEmpty())   {
                std::cout << "ERROR [NoUnit]: No ambulance units available."
                << std::endl;
                return;
            }

            // if ambulance is available and some id also exists on board, then we
            // shall dispatch
            Incident* t = dispatchHeap.top();
            Unit* u = unitHeap.top();

            // unit cannot leave before it is free
            int start = (now > u->freeAt) ? now : u->freeAt;
            int eta = start + t->travel;

            if (eta <= t->dueTime)  {
                std::cout << "Dispatched: Unit " << u->unitID << " -> Incident " << t->incidentID
                << " (ETA " << eta << ", due " << t->dueTime << ")." << std::endl;

                // unit has to come back too, its freeAt only grows so it may sink in unit heap
                u->freeAt = start + 2 * t->travel;
                unitHeap.fix(u->heapIndex);
                dispatched++;
            }
            else {
                std::cout << "ERROR [DeadlineMissed]: Incident " << t->incidentID
                << " cannot be reached by due time " << t->dueTime << " (best ETA " << eta
                << " by Unit " << u->unitID << ")." << std::endl;
                missed++;      // unit stays unchanged
            }

            removeFromBoard(t);
        }

        /*PART C--> raises the incident by one level using its id (O(1) lookup)
        and then its remembered positions to fix both heaps (O(log n)).
        in dispatch heap it floats up, in shed heap it sinks down (because it
        became more important), fix() decides the direction by itself*/
        void escalate(int incidentID)   {
            Incident* node = findOnBoard(incidentID);
            if (node == nullptr)    {
                std::cout << "ERROR [IncidentNotFound]: No incident with ID " << incidentID
                << " on the board." << std::endl;
                return;
            }
            if (node->effectiveSeverity() >= 5) {
                std::cout << "ERROR [AlreadyCritical]: Incident " << incidentID
                << " is already at severity 5." << std::endl;
                return;
            }

            node->escalations++;
            fixBothHeaps(node);
            std::cout << "Incident " << incidentID << " escalated to severity "
            << node->effectiveSeverity() << "." << std::endl;
        }

        // PART C-> removes any waiting incident, not only the top one
        void cancel(int incidentID) {
            Incident* node = findOnBoard(incidentID);
            if (node == nullptr)    {
                std::cout << "ERROR [IncidentNotFound]: No incident with ID " << incidentID
                << " on the board." << std::endl;
                return;
            }

            removeFromBoard(node);
            cancelled++;
            std::cout << "Incident " << incidentID << " cancelled." << std::endl;
        }

        /*PART C--> the one place where scanning every incident is allowed. ids are
        visited in increasing order (1 to 999), byID skips the ones not on board.
        each escalated incident is fixed in BOTH heaps immediately, before the next
        one is visited. aging never lowers anything (only escalate if target >
        escalations). the changed ones are remembered for the summary line*/
        void ageAll(int now)    {
            int changedID[1000];
            int changedSeverity[1000];
            int changed = 0;

            for (int id = 1; id < 1000; id++)   {
                Incident* node = byID[id];
                if (node == nullptr)
                    continue;

                int target = (now - node->arrivalTime) / 30;
                if (target > 5 - node->severity)
                    target = 5 - node->severity;

                if (target > node->escalations) {
                    node->escalations = target;
                    fixBothHeaps(node);

                    changedID[changed] = id;
                    changedSeverity[changed] = node->effectiveSeverity();
                    changed++;
                }
            }

            std::cout << "Aged to time " << now << ": " << changed << " incident(s) escalated";
            if (changed > 0)    {
                std::cout << " (";
                for (int i = 0; i < changed; i++)   {
                    if (i > 0)
                        std::cout << ", ";
                    std::cout << changedID[i] << " -> S" << changedSeverity[i];
                }
                std::cout << ")";
            }
            std::cout << "." << std::endl;
        }

        // prints the dispatch heap (array order, then level by level)
        void printHeap()    {
            dispatchHeap.print();
        }

        // extra print functions so every class shows all of its items
        void printShedHeap()    {
            std::cout << "Shed ";
            shedHeap.print();
        }

        void printUnits()   {
            unitHeap.print();
        }

        /*PART E: board, most/least urgent, units (in the order they would be
        chosen, from a copy) and the counters*/
        void showStatus()   {
            std::cout << "===== STATUS =====" << std::endl;

            std::cout << "Board (" << dispatchHeap.getSize() << "/" << capacity << "): ";
            dispatchHeap.printArrayOrder();
            std::cout << std::endl;

            std::cout << "Most urgent: ";
            printSummary(dispatchHeap.top());
            std::cout << " | Least urgent: ";
            printSummary(shedHeap.top());
            std::cout << std::endl;

            std::cout << "Units (earliest free first): ";
            unitHeap.printSorted();
            std::cout << std::endl;

            std::cout << "Dispatched: " << dispatched << " | Missed: " << missed
            << " | Shed: " << shed << " | Cancelled: " << cancelled
            << " | Rejected: " << rejected << std::endl;
        }

        /*every node in the heaps is freed here. the incident heaps only
        free their pointer arrays, so the incidents still on the board are
        deleted through byID (every live incident is in it exactly once). the
        unit heap frees its own units in its own destructor*/
        ~ControlRoom() {
            for (int id = 1; id < 1000; id++)   {
                if (byID[id] != nullptr)    {
                    delete byID[id];
                    byID[id] = nullptr;
                }
            }
        }

        // (required helpers which access private data that cannot be
        // sent by the main())
        // Incident* findOnBoard(int incidentID);
        // void printSummary(Incident* node);
        // keep the prototypes commented out.. cpp doesnt allow prototyes in classes
        // if the actual func is also implemented inside class

    private:

        // prints like: 105 (S1, due 90), or none for an empty board
        void printSummary(Incident* node)   {
            if (node == nullptr)    {
                std::cout << "none";
                return;
            }
            std::cout << node->incidentID << " (S" << node->effectiveSeverity()
            << ", due " << node->dueTime << ")";
        }
};

void loadPartA(ControlRoom& room)   {
    room.reportIncident(101, 3, 0, 40, 8);
    room.reportIncident(102, 5, 5, 25, 8);
    room.reportIncident(103, 3, 10, 30, 6);
    room.reportIncident(104, 3, 12, 30, 9);
    room.reportIncident(105, 1, 15, 90, 20);
}

// THE MAIN IS DESIGNED TO REPLICATE THE SPEC'S EXAMPLE (TOOK ME 3 DAYS TO DO THIS Q ONLY)

int main()  {
    std::cout << "Part A: reporting" << std::endl;
    ControlRoom room(5, 2);
    loadPartA(room);
    room.reportIncident(106, 7, 20, 50, 5);
    room.reportIncident(102, 2, 20, 50, 5);
    room.reportIncident(107, 2, 20, 20, 5);
    room.reportIncident(1000, 2, 20, 50, 5);
    room.reportIncident(108, 2, -1, 50, 5);
    room.reportIncident(109, 2, 20, 50, 0);
    std::cout << "printHeap():" << std::endl;
    room.printHeap();

    std::cout << std::endl << "Part B: units and dispatch" << std::endl;
    room.addUnit(1, 0);
    room.addUnit(2, 18);
    room.addUnit(1, 5);
    room.addUnit(0, 5);
    room.addUnit(3, -4);
    room.addUnit(3, 5);
    room.dispatch(15);
    room.dispatch(20);
    room.dispatch(24);
    room.dispatch(24);
    std::cout << "showStatus():" << std::endl;
    room.showStatus();

    std::cout << std::endl << "Part C: escalate" << std::endl;
    ControlRoom roomEscalate(5, 2);
    loadPartA(roomEscalate);
    roomEscalate.escalate(101);
    std::cout << "printHeap():" << std::endl;
    roomEscalate.printHeap();
    roomEscalate.escalate(102);
    roomEscalate.escalate(999);

    std::cout << std::endl << "Part C: aging" << std::endl;
    ControlRoom roomAge(5, 2);
    loadPartA(roomAge);
    roomAge.ageAll(40);
    std::cout << "printHeap():" << std::endl;
    roomAge.printHeap();

    std::cout << std::endl << "Part C: cancel" << std::endl;
    ControlRoom roomCancel(10, 2);
    roomCancel.reportIncident(201, 3, 0, 10, 5);
    roomCancel.reportIncident(202, 3, 0, 30, 5);
    roomCancel.reportIncident(203, 3, 0, 20, 5);
    roomCancel.reportIncident(204, 3, 0, 40, 5);
    roomCancel.reportIncident(205, 3, 0, 50, 5);
    roomCancel.reportIncident(206, 3, 0, 25, 5);
    roomCancel.reportIncident(207, 3, 0, 22, 5);
    std::cout << "printHeap():" << std::endl;
    roomCancel.printHeap();
    roomCancel.cancel(204);
    roomCancel.printHeap();
    roomCancel.cancel(201);
    roomCancel.printHeap();

    std::cout << std::endl << "Part D: capacity and shedding" << std::endl;
    ControlRoom roomShed(3, 2);
    roomShed.reportIncident(301, 3, 0, 50, 5);
    roomShed.reportIncident(302, 2, 2, 60, 5);
    roomShed.reportIncident(303, 4, 4, 40, 5);
    roomShed.reportIncident(304, 5, 6, 30, 5);
    roomShed.reportIncident(305, 1, 8, 90, 5);
    roomShed.reportIncident(306, 3, 10, 45, 5);
    std::cout << "printHeap():" << std::endl;
    roomShed.printHeap();
    roomShed.printShedHeap();
    std::cout << "showStatus():" << std::endl;
    roomShed.showStatus();

    std::cout << std::endl << "Part E: errors and status" << std::endl;
    ControlRoom roomError(4, 1);
    roomError.cancel(555);
    roomError.dispatch(5);
    roomError.reportIncident(401, 3, 0, 50, 5);
    roomError.dispatch(5);
    roomError.cancel(401);
    roomError.cancel(401);
    roomError.escalate(401);
    std::cout << "showStatus():" << std::endl;
    roomError.showStatus();

    return 0;
}