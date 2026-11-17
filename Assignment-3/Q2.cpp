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

                bool moreUrgent (Incident* x, Incident* y)  {
                    if (x->effectiveSeverity() != y->effectiveSeverity())
                        return x->effectiveSeverity() > y->effectiveSeverity();

                    if (x->dueTime != y->dueTime)
                        return x->dueTime < y->dueTime;

                    if (x->arrivalTime != y->arrivalTime)
                        return x->arrivalTime < y->arrivalTime;
                        
                    return x->incidentID < y->incidentID;   // cannot be equal (is unique)
                }
        };

        class IncidentHeap  {
            public:

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

                Unit(int id, int free) : unitID(id), freeAt(free) {}

                // earliest free unit wins, tie -> smaller unit id wins
                bool freesEarlier(Unit* a, Unit* b) {
                    if (a->freeAt != b->freeAt)
                        return a->freeAt < b->freeAt;
                        
                    return a->unitID < b->unitID;
                }
        };

        class UnitHeap  {
            public:
        };
};

int main()  {
    return 0;
}