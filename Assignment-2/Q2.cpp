#include <iostream>
#include <string>

class Tournament {
    // node must remain private since no user should know it exists
    private:
        class Node {
            // public since it should be able to be used inside Tournament
            public:
                // required data members
                int nodeID;
                std::string teamName;   // used only by a team slot, empty until a team is assigned
                int rating;             // used only by a team slot
                std::string winner;     // used only by a match, empty until a result is recorded
                int winnerRating;       // rating of the winner of a match

                // the two feeder nodes (both null for a team slot)
                Node* left;
                Node* right;

                // constructors (def. and para.)
                Node() : nodeID(0), teamName(), rating(0), winner(), winnerRating(0) {
                    left = nullptr;
                    right = nullptr;
                }

                // every node is born as an empty team slot
                Node(int nodeID) {
                    this->nodeID = nodeID;

                    // def values (no input)
                    this->teamName = "";
                    this->rating = 0;
                    this->winner = "";
                    this->winnerRating = 0;
                    this->left = nullptr;
                    this->right = nullptr;
                }
        };

    // members of tournament other than Node
    Node* root; // the final

    // ---------- helpers (all recursive ones work on a node, public ones start from root) ----------

    // frees the whole tree (post order, children first)
    void freeTree(Node* current) {
        if (current == nullptr)
            return;
        freeTree(current->left);
        freeTree(current->right);
        delete current;
    }

    Node* findNode(Node* current, int nodeID) {
        if (current == nullptr)
            return nullptr;
        if (current->nodeID == nodeID)
            return current;
        Node* found = findNode(current->left, nodeID);
        if (found != nullptr)
            return found;
        return findNode(current->right, nodeID);
    }

    // only team slots (leaves) hold teams, an empty name never matches
    Node* findTeam(Node* current, const std::string& teamName) {
        if (current == nullptr)
            return nullptr;
        if (current->left == nullptr) {
            if (!current->teamName.empty() && current->teamName == teamName)
                return current;
            return nullptr;
        }
        Node* found = findTeam(current->left, teamName);
        if (found != nullptr)
            return found;
        return findTeam(current->right, teamName);
    }

    // node does not store a parent pointer so the parent is searched from the top
    Node* findParent(Node* current, Node* target) {
        if (current == nullptr || current->left == nullptr)
            return nullptr;
        if (current->left == target || current->right == target)
            return current;
        Node* found = findParent(current->left, target);
        if (found != nullptr)
            return found;
        return findParent(current->right, target);
    }

    // distance of target from the final (-1 if it is not in this subtree)
    int depthOf(Node* current, Node* target, int depth) {
        if (current == nullptr)
            return -1;
        if (current == target)
            return depth;
        int d = depthOf(current->left, target, depth + 1);
        if (d != -1)
            return d;
        return depthOf(current->right, target, depth + 1);
    }

    // names are worked out when needed since the tree can change shape
    std::string roundName(int depth) {
        if (depth == 0)
            return "Final";
        if (depth == 1)
            return "Semi-final";
        if (depth == 2)
            return "Quarter-final";
        return "Early round";
    }

    // a team slot is decided when it holds a team, a match when it has a winner
    bool isDecided(Node* current) {
        if (current->left == nullptr)
            return !current->teamName.empty();
        return !current->winner.empty();
    }

    // the competitor of a decided node is its team (slot) or its winner (match)
    std::string getCompetitor(Node* current) {
        if (current->left == nullptr)
            return current->teamName;
        return current->winner;
    }

    int getCompetitorRating(Node* current) {
        if (current->left == nullptr)
            return current->rating;
        return current->winnerRating;
    }

    // short name of a node used in the messages and in printLevels
    std::string getLabel(Node* current) {
        if (current->left != nullptr)
            return "Match " + std::to_string(current->nodeID);
        if (!current->teamName.empty())
            return current->teamName;
        return "Slot " + std::to_string(current->nodeID);
    }

    int countEmpty(Node* current) {
        if (current == nullptr)
            return 0;
        if (current->left == nullptr) {
            if (current->teamName.empty())
                return 1;
            return 0;
        }
        return countEmpty(current->left) + countEmpty(current->right);
    }

    // Part C helpers (the public functions just start them from the root)
    int countTeamsHelper(Node* current) {
        if (current == nullptr)
            return 0;
        if (current->left == nullptr) {
            if (!current->teamName.empty())
                return 1;
            return 0;
        }
        return countTeamsHelper(current->left) + countTeamsHelper(current->right);
    }

    int countMatchesHelper(Node* current) {
        if (current == nullptr || current->left == nullptr)
            return 0;
        return 1 + countMatchesHelper(current->left) + countMatchesHelper(current->right);
    }

    // matches on the longest path from a team slot up to the final, this is also the
    // depth of the deepest slot
    int roundsHelper(Node* current) {
        if (current == nullptr || current->left == nullptr)
            return 0;
        int l = roundsHelper(current->left);
        int r = roundsHelper(current->right);
        if (l > r)
            return l + 1;
        return r + 1;
    }

    // a bye is a team whose slot is shallower than the deepest slot
    int countByesHelper(Node* current, int depth, int deepest) {
        if (current == nullptr)
            return 0;
        if (current->left == nullptr) {
            if (!current->teamName.empty() && depth < deepest)
                return 1;
            return 0;
        }
        return countByesHelper(current->left, depth + 1, deepest) +
               countByesHelper(current->right, depth + 1, deepest);
    }

    // returns the slot of the champion if the higher rating always wins (recorded results
    // are ignored since only the slots are looked at), on equal ratings the left one wins
    Node* predictHelper(Node* current) {
        if (current == nullptr || current->left == nullptr)
            return current;
        Node* l = predictHelper(current->left);
        Node* r = predictHelper(current->right);
        if (r->rating > l->rating)
            return r;
        return l;
    }

    // Part B helpers
    void printBracketHelper(Node* current, int depth, int deepest) {
        if (current == nullptr)
            return;

        // two spaces per level
        std::string indent = "";
        for (int i = 0; i < depth; i++)
            indent = indent + "  ";

        if (current->left != nullptr) {
            std::cout << indent << roundName(depth) << " [node " << current->nodeID << "]";
            if (!current->winner.empty())
                std::cout << " -> " << current->winner;
            std::cout << std::endl;
            // left feeder first, then the right feeder
            printBracketHelper(current->left, depth + 1, deepest);
            printBracketHelper(current->right, depth + 1, deepest);
        }
        else {
            if (!current->teamName.empty())
                std::cout << indent << current->teamName << " (" << current->rating
                          << ") [node " << current->nodeID << "]";
            else
                std::cout << indent << "Empty slot [node " << current->nodeID << "]";
            if (depth < deepest)
                std::cout << " (bye)";
            std::cout << std::endl;
        }
    }

    // prints only the nodes that sit on the given level, left to right
    void printLevelHelper(Node* current, int level, bool& first) {
        if (current == nullptr)
            return;
        if (level == 0) {
            if (!first)
                std::cout << ", ";
            first = false;
            std::cout << getLabel(current);
            return;
        }
        printLevelHelper(current->left, level - 1, first);
        printLevelHelper(current->right, level - 1, first);
    }

    // post order, a match is printed only after both of its feeders so no match comes before
    // the matches that feed it
    void printFixturesHelper(Node* current, bool& first) {
        if (current == nullptr || current->left == nullptr)
            return;
        printFixturesHelper(current->left, first);
        printFixturesHelper(current->right, first);
        if (!first)
            std::cout << " -> ";
        first = false;
        std::cout << "Match " << current->nodeID;
    }

    // lowest node that has both a and b below it (the match where they first meet)
    Node* findLCA(Node* current, Node* a, Node* b) {
        if (current == nullptr || current == a || current == b)
            return current;
        Node* l = findLCA(current->left, a, b);
        Node* r = findLCA(current->right, a, b);
        if (l != nullptr && r != nullptr)
            return current;
        if (l != nullptr)
            return l;
        return r;
    }

    public:
        // constructor
        Tournament() {
            root = nullptr;
        }

        // a tournament owns raw nodes so copying it would free the same nodes twice
        Tournament(const Tournament&) = delete;
        Tournament& operator=(const Tournament&) = delete;

        // destructor to free every node
        ~Tournament() {
            freeTree(root);
            root = nullptr;
        }

        // Part A Solution Function
        void createFinal(int nodeID) {
            // creating the final again would lose the old tree
            if (root != nullptr) {
                std::cout << "ERROR [FinalExists]: The final has already been created (node "
                          << root->nodeID << ")." << std::endl;
                return;
            }
            if (nodeID <= 0) {
                std::cout << "ERROR [BadNodeID]: Node id must be positive." << std::endl;
                return;
            }
            root = new Node(nodeID);
            std::cout << "Final created (node " << nodeID << ")" << std::endl;
        }

        // Part A Solution Function
        void splitMatch(int nodeID, int leftID, int rightID) {
            Node* current = findNode(root, nodeID);
            if (current == nullptr) {
                std::cout << "ERROR [NodeNotFound]: No node with ID " << nodeID << "." << std::endl;
                return;
            }
            if (current->left != nullptr) {
                std::cout << "ERROR [AlreadyMatch]: Node " << nodeID << " is already a match." << std::endl;
                return;
            }
            if (!current->teamName.empty()) {
                std::cout << "ERROR [SlotTaken]: Node " << nodeID << " already holds "
                          << current->teamName << "." << std::endl;
                return;
            }
            if (leftID <= 0 || rightID <= 0) {
                std::cout << "ERROR [BadFeederID]: Feeder ids must be positive." << std::endl;
                return;
            }
            if (leftID == rightID) {
                std::cout << "ERROR [BadFeederID]: Feeder ids must be different." << std::endl;
                return;
            }
            if (findNode(root, leftID) != nullptr) {
                std::cout << "ERROR [BadFeederID]: Node id " << leftID << " is already used." << std::endl;
                return;
            }
            if (findNode(root, rightID) != nullptr) {
                std::cout << "ERROR [BadFeederID]: Node id " << rightID << " is already used." << std::endl;
                return;
            }

            // both feeders are born as empty team slots
            current->left = new Node(leftID);
            current->right = new Node(rightID);
            std::cout << "Node " << nodeID << " is now fed by nodes " << leftID << " and " << rightID << std::endl;
        }

        // Part A Solution Function
        void assignTeam(int nodeID, std::string teamName, int rating) {
            Node* current = findNode(root, nodeID);
            if (current == nullptr) {
                std::cout << "ERROR [NodeNotFound]: No node with ID " << nodeID << "." << std::endl;
                return;
            }
            if (current->left != nullptr) {
                std::cout << "ERROR [NotATeamSlot]: Node " << nodeID << " is a match, not a team slot." << std::endl;
                return;
            }
            if (!current->teamName.empty()) {
                std::cout << "ERROR [SlotTaken]: Node " << nodeID << " already holds "
                          << current->teamName << "." << std::endl;
                return;
            }
            if (teamName.empty()) {
                std::cout << "ERROR [IncompleteInfo]: Team name must not be empty." << std::endl;
                return;
            }
            if (rating <= 0) {
                std::cout << "ERROR [IncompleteInfo]: Rating must be positive." << std::endl;
                return;
            }
            if (findTeam(root, teamName) != nullptr) {
                std::cout << "ERROR [DuplicateTeam]: " << teamName << " is already in the tournament." << std::endl;
                return;
            }

            current->teamName = teamName;
            current->rating = rating;
            std::cout << teamName << " (" << rating << ") placed in slot " << nodeID << std::endl;
        }

        // Part A Solution Function
        void checkReady() {
            // no tree at all is not the same as a tree with 0 empty slots
            if (root == nullptr) {
                std::cout << "Bracket incomplete: no final created yet" << std::endl;
                return;
            }
            int empty = countEmpty(root);
            if (empty == 0)
                std::cout << "Bracket ready" << std::endl;
            else
                std::cout << "Bracket incomplete: " << empty << " empty slot(s)" << std::endl;
        }

        // Part B Solution Function
        void printBracket() {
            if (root == nullptr) {
                std::cout << "(empty)" << std::endl;
                return;
            }
            printBracketHelper(root, 0, roundsHelper(root));
        }

        // Part B Solution Function
        void printLevels() {
            if (root == nullptr) {
                std::cout << "(empty)" << std::endl;
                return;
            }
            // deepest level is the same as the number of rounds
            int deepest = roundsHelper(root);
            for (int level = 0; level <= deepest; level++) {
                bool first = true;
                std::cout << "Level " << level << ": ";
                printLevelHelper(root, level, first);
                std::cout << std::endl;
            }
        }

        // Part B Solution Function
        void printFixtures() {
            std::cout << "Fixtures: ";
            // no matches to play in an empty tree or in a single slot
            if (root == nullptr || root->left == nullptr)
                std::cout << "none";
            else {
                bool first = true;
                printFixturesHelper(root, first);
            }
            std::cout << std::endl;
        }

        // Part C Solution Functions
        int countTeams() {
            return countTeamsHelper(root);
        }

        int countMatches() {
            return countMatchesHelper(root);
        }

        int rounds() {
            return roundsHelper(root);
        }

        int countByes() {
            return countByesHelper(root, 0, roundsHelper(root));
        }

        // returns empty string if there is no champion to predict
        std::string predictChampion() {
            Node* champion = predictHelper(root);
            if (champion == nullptr)
                return "";
            return champion->teamName;
        }

        // Part C Solution Function
        void printMeasures() {
            std::cout << "Teams: " << countTeams() << std::endl;
            std::cout << "Matches: " << countMatches() << std::endl;
            std::cout << "Rounds: " << rounds() << std::endl;
            std::cout << "Byes: " << countByes() << std::endl;
            std::string champion = predictChampion();
            if (champion.empty())
                std::cout << "Predicted champion: none" << std::endl;
            else
                std::cout << "Predicted champion: " << champion << std::endl;
        }

        // Part D Solution Function
        void recordResult(int nodeID, std::string winnerName) {
            Node* current = findNode(root, nodeID);
            if (current == nullptr) {
                std::cout << "ERROR [NodeNotFound]: No node with ID " << nodeID << "." << std::endl;
                return;
            }
            if (current->left == nullptr) {
                std::cout << "ERROR [NotAMatch]: Node " << nodeID << " is a team slot, not a match." << std::endl;
                return;
            }
            if (!current->winner.empty()) {
                std::cout << "ERROR [AlreadyRecorded]: Match " << nodeID << " already has a winner ("
                          << current->winner << ")." << std::endl;
                return;
            }
            // left feeder is named first
            if (!isDecided(current->left)) {
                std::cout << "ERROR [NotReady]: Match " << nodeID << " cannot be played yet because "
                          << getLabel(current->left) << " is undecided." << std::endl;
                return;
            }
            if (!isDecided(current->right)) {
                std::cout << "ERROR [NotReady]: Match " << nodeID << " cannot be played yet because "
                          << getLabel(current->right) << " is undecided." << std::endl;
                return;
            }

            std::string leftName = getCompetitor(current->left);
            std::string rightName = getCompetitor(current->right);
            if (winnerName != leftName && winnerName != rightName) {
                std::cout << "ERROR [InvalidWinner]: " << winnerName << " is not playing in Match "
                          << nodeID << "." << std::endl;
                return;
            }

            // work out who won and who lost, left is assumed first and swapped if right won
            std::string loserName = rightName;
            int winnerRating = getCompetitorRating(current->left);
            int loserRating = getCompetitorRating(current->right);
            if (winnerName == rightName) {
                loserName = leftName;
                winnerRating = getCompetitorRating(current->right);
                loserRating = getCompetitorRating(current->left);
            }

            current->winner = winnerName;
            current->winnerRating = winnerRating;

            std::cout << "Match " << nodeID << ": " << winnerName << " beat " << loserName;
            if (winnerRating < loserRating)
                std::cout << " (UPSET: rating " << winnerRating << " beat " << loserRating << ")";
            std::cout << std::endl;

            // the root is the final
            if (current == root)
                std::cout << "CHAMPION: " << winnerName << std::endl;
        }

        // Part E Solution Function
        void withdrawTeam(std::string teamName) {
            Node* leaf = findTeam(root, teamName);
            if (leaf == nullptr) {
                std::cout << "ERROR [TeamNotFound]: No team named " << teamName << "." << std::endl;
                return;
            }

            // the match this team was due to play (null only if the team is the whole tree)
            Node* parent = findParent(root, leaf);
            if (parent == nullptr) {
                std::cout << "ERROR [NoMatch]: " << teamName
                          << " is the only team in the tournament, there is no match to leave." << std::endl;
                return;
            }
            if (!parent->winner.empty()) {
                std::cout << "ERROR [AlreadyPlayed]: " << teamName << " has already played in Match "
                          << parent->nodeID << " and cannot withdraw." << std::endl;
                return;
            }

            // the other feeder takes the place of the match with its whole subtree intact
            Node* sibling = parent->left;
            if (parent->left == leaf)
                sibling = parent->right;

            // saved before the nodes are deleted since they are needed for the message
            std::string siblingLabel = getLabel(sibling);
            int parentID = parent->nodeID;

            // relinking, the removed match is the root, a left child or a right child
            Node* grandParent = findParent(root, parent);
            if (grandParent == nullptr)
                root = sibling;
            else if (grandParent->left == parent)
                grandParent->left = sibling;
            else
                grandParent->right = sibling;

            // Node has no destructor that frees children, so only these two nodes are deleted
            delete leaf;
            delete parent;

            std::cout << teamName << " withdrew. Match " << parentID << " removed; "
                      << siblingLabel << " advances with a bye." << std::endl;
        }

        // Part F Solution Function
        void teamJourney(std::string teamName) {
            Node* leaf = findTeam(root, teamName);
            if (leaf == nullptr) {
                std::cout << "ERROR [TeamNotFound]: No team named " << teamName << "." << std::endl;
                return;
            }

            std::cout << teamName << " (" << leaf->rating << ") journey:" << std::endl;

            // a lone team has no match on the way
            if (leaf == root) {
                std::cout << teamName << " has no matches to play." << std::endl;
                return;
            }

            // going up from the slot, one match at a time, until the final (or a loss)
            Node* current = leaf;
            while (current != root) {
                Node* match = findParent(root, current);
                int depth = depthOf(root, match, 0);

                // the opponent is the competitor of the other feeder, TBD if it is undecided
                Node* other = match->left;
                if (match->left == current)
                    other = match->right;
                std::string opponent = "TBD";
                if (isDecided(other))
                    opponent = getCompetitor(other);

                std::string outcome = "pending";
                if (!match->winner.empty()) {
                    if (match->winner == teamName)
                        outcome = "won";
                    else
                        outcome = "lost";
                }

                std::cout << "Match " << match->nodeID << " (" << roundName(depth) << "): vs "
                          << opponent << " - " << outcome << std::endl;

                // journey stops after the first match the team lost
                if (outcome == "lost") {
                    std::cout << teamName << " was eliminated in the " << roundName(depth) << "." << std::endl;
                    return;
                }
                if (outcome == "won" && match == root)
                    std::cout << teamName << " is the CHAMPION." << std::endl;

                current = match;
            }
        }

        // Part F Solution Function
        void firstMeeting(std::string teamA, std::string teamB) {
            Node* a = findTeam(root, teamA);
            if (a == nullptr) {
                std::cout << "ERROR [TeamNotFound]: No team named " << teamA << "." << std::endl;
                return;
            }
            Node* b = findTeam(root, teamB);
            if (b == nullptr) {
                std::cout << "ERROR [TeamNotFound]: No team named " << teamB << "." << std::endl;
                return;
            }
            if (teamA == teamB) {
                std::cout << "ERROR [SameTeam]: Choose two different teams." << std::endl;
                return;
            }

            // they can first meet at the lowest match that has both of them below it
            Node* meeting = findLCA(root, a, b);
            int depth = depthOf(root, meeting, 0);
            std::cout << teamA << " and " << teamB << " can first meet in Match " << meeting->nodeID
                      << " (" << roundName(depth) << ")." << std::endl;
        }
};

// builds the main bracket used in all later parts
void buildMain(Tournament& t) {
    t.createFinal(1);
    t.splitMatch(1, 2, 3);
    t.splitMatch(2, 4, 5);
    t.splitMatch(3, 6, 7);
    t.splitMatch(4, 8, 9);
    t.splitMatch(7, 10, 11);
    t.assignTeam(8, "Lahore", 88);
    t.assignTeam(9, "Karachi", 80);
    t.assignTeam(5, "Peshawar", 79);
    t.assignTeam(6, "Quetta", 85);
    t.assignTeam(10, "Multan", 90);
    t.assignTeam(11, "Islamabad", 72);
}

int main() {
    // Part A: building the bracket
    std::cout << "----- Part A: building the bracket -----" << std::endl;
    Tournament t;
    t.checkReady();         // no final yet
    t.createFinal(1);
    t.createFinal(2);       // final already exists
    t.splitMatch(1, 2, 3);
    t.splitMatch(2, 4, 5);
    t.splitMatch(3, 6, 7);
    t.splitMatch(4, 8, 9);
    t.splitMatch(7, 10, 11);
    t.assignTeam(8, "Lahore", 88);
    t.assignTeam(9, "Karachi", 80);
    t.assignTeam(5, "Peshawar", 79);
    t.assignTeam(6, "Quetta", 85);
    t.assignTeam(10, "Multan", 90);
    t.checkReady();                         // expected incomplete: 1 empty slot(s)
    t.assignTeam(11, "Lahore", 70);         // duplicate team
    t.assignTeam(11, "Islamabad", 0);       // bad rating
    t.assignTeam(11, "", 72);               // empty name
    t.assignTeam(11, "Islamabad", 72);
    t.checkReady();                         // expected ready

    std::cout << "----- rejected calls -----" << std::endl;
    t.splitMatch(99, 12, 13);   // node not found
    t.splitMatch(1, 12, 13);    // already a match
    t.splitMatch(8, 12, 13);    // slot taken
    t.assignTeam(2, "Hyderabad", 70);   // not a team slot
    t.assignTeam(8, "Hyderabad", 70);   // slot taken
    t.assignTeam(99, "Hyderabad", 70);  // node not found

    // feeder id checks on a throwaway tree (order: positive, equal, already used)
    std::cout << "----- feeder id validation -----" << std::endl;
    {
        Tournament v;
        v.createFinal(1);
        v.splitMatch(1, 0, 3);      // not positive
        v.splitMatch(1, 2, 2);      // equal
        v.splitMatch(1, 1, 3);      // already used
        v.splitMatch(1, 2, 1);      // already used
        v.splitMatch(1, 2, 3);      // fine
    }

    // Part B: reports
    std::cout << "----- Part B: reports -----" << std::endl;
    t.printBracket();
    t.printLevels();
    t.printFixtures();

    // Part C: measures
    std::cout << "----- Part C: measures -----" << std::endl;
    t.printMeasures();  // expected 6, 5, 3, 2, Multan

    // equal ratings, the left feeder must win the prediction
    std::cout << "----- Part C: equal ratings -----" << std::endl;
    {
        Tournament e;
        e.createFinal(1);
        e.splitMatch(1, 2, 3);
        e.assignTeam(2, "Left", 50);
        e.assignTeam(3, "Right", 50);
        e.printMeasures();  // expected champion Left
    }

    // empty / lone-slot trees
    std::cout << "----- Extra: empty and single slot trees -----" << std::endl;
    {
        Tournament empty;
        empty.printBracket();
        empty.printLevels();
        empty.printFixtures();
        empty.printMeasures();
        Tournament lone;
        lone.createFinal(1);
        lone.printBracket();
        lone.printLevels();
        lone.printFixtures();
        lone.printMeasures();
        lone.checkReady();
    }

    // Part D: recording results
    std::cout << "----- Part D: recording results -----" << std::endl;
    Tournament d;
    buildMain(d);
    d.recordResult(2, "Lahore");        // not ready (Match 4 undecided)
    d.recordResult(4, "Peshawar");      // invalid winner
    d.recordResult(4, "Lahore");
    d.recordResult(4, "Karachi");       // already recorded
    d.recordResult(7, "Islamabad");     // upset
    d.printBracket();
    d.recordResult(2, "Peshawar");      // upset
    d.recordResult(3, "Quetta");
    d.recordResult(1, "Quetta");        // champion
    d.recordResult(8, "Lahore");        // not a match
    d.recordResult(50, "Lahore");       // node not found

    // right feeder undecided is named too (left is decided here)
    std::cout << "----- Part D: right feeder undecided -----" << std::endl;
    {
        Tournament r;
        r.createFinal(1);
        r.splitMatch(1, 2, 3);
        r.assignTeam(2, "A", 50);
        r.recordResult(1, "A");     // right feeder (Slot 3) is undecided
        r.recordResult(2, "A");     // team slot
        r.assignTeam(3, "B", 60);
        r.recordResult(1, "B");     // fine, B wins the final
    }

    // Part F: journeys and meetings on a fresh bracket
    std::cout << "----- Part F: journeys and meetings (fresh bracket) -----" << std::endl;
    Tournament f;
    buildMain(f);
    f.teamJourney("Lahore");
    f.teamJourney("Zaman");
    f.firstMeeting("Lahore", "Karachi");
    f.firstMeeting("Lahore", "Peshawar");
    f.firstMeeting("Lahore", "Multan");
    f.firstMeeting("Quetta", "Islamabad");
    f.firstMeeting("Lahore", "Lahore");
    f.firstMeeting("Lahore", "Zaman");
    f.firstMeeting("Zaman", "Lahore");

    std::cout << "----- Part F: after all results -----" << std::endl;
    d.teamJourney("Lahore");
    d.teamJourney("Quetta");
    d.teamJourney("Islamabad");
    d.teamJourney("Peshawar");

    // Part E: withdrawals
    std::cout << "----- Part E: withdrawals -----" << std::endl;
    Tournament e;
    buildMain(e);
    e.withdrawTeam("Karachi");  // left child of match 4
    e.printBracket();
    e.printMeasures();          // expected 5, 4, 3, 3, Multan
    e.withdrawTeam("Multan");   // match 7 removed, Islamabad takes its place
    e.printMeasures();          // expected 4, 3, 2, 0, Lahore
    e.printBracket();

    std::cout << "----- root case -----" << std::endl;
    Tournament r;
    r.createFinal(1);
    r.splitMatch(1, 2, 3);
    r.assignTeam(2, "A", 50);
    r.assignTeam(3, "B", 60);
    r.withdrawTeam("A");
    r.printMeasures();          // expected 1, 0, 0, 0, B
    r.printBracket();
    r.teamJourney("B");
    r.withdrawTeam("B");        // only team left, refused

    // right child withdrawal (Peshawar is the right child of match 2, match 2 is a left child of the root)
    std::cout << "----- right child, sibling is a match -----" << std::endl;
    Tournament p;
    buildMain(p);
    p.withdrawTeam("Peshawar");
    p.printBracket();
    p.printMeasures();          // expected 5, 4, 3, 3, Multan

    // removed match is a right child of the root, sibling already has a result
    std::cout << "----- right child of root, sibling keeps its result -----" << std::endl;
    Tournament q;
    buildMain(q);
    q.recordResult(7, "Multan");
    q.withdrawTeam("Quetta");
    q.printBracket();
    q.printLevels();
    q.printFixtures();
    q.printMeasures();
    q.recordResult(3, "Multan");    // match 3 no longer exists (its id was removed)

    std::cout << "----- refused withdrawals -----" << std::endl;
    Tournament x;
    buildMain(x);
    x.recordResult(4, "Lahore");
    x.withdrawTeam("Karachi");  // already played
    x.withdrawTeam("Lahore");   // already played
    x.withdrawTeam("Zaman");    // not found
    x.printMeasures();          // unchanged: 6, 5, 3, 2, Multan
    x.withdrawTeam("Peshawar"); // match 2 has no winner yet, so this is allowed
    x.printBracket();

    return 0;
}

bool isValidBST(TreeNode* root) {
    if (root == nullptr)
        return true;
    if (root->left)
        return (isValidBST(root->left) && root->data > root->left data);

    if (root->right)
        return (isValidBST(root->right) && root->data < root->right->data);
}