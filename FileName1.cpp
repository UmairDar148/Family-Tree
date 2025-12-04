#include <iostream>
using namespace std;

/*
 Advanced Family Tree (centered, dual-parent families, top-down generations)
 - Only <iostream>
 - Classes only (no struct)
 - Name max 15 chars (for layout)
 - Shows parent pairs on one line, children centered below them on next line
 - Families displayed generation by generation (top -> down)
 - Robust input handling
*/

const int MAX_NAME = 15;   // max characters to display for a name
const int HSPACE = 6;      // horizontal spacing between families when printing

// -------------------------
// Class: FamilyMember
// -------------------------
class FamilyMember {
private:
    char name[64];
    char gender; // 'M' or 'F'
    bool alive;

    FamilyMember* father;
    FamilyMember* mother;
    FamilyMember* firstChild;
    FamilyMember* nextSibling;

    void copyName(const char* src) {
        int i = 0;
        while (src[i] != '\0' && i < 63) { name[i] = src[i]; ++i; }
        name[i] = '\0';
    }

public:
    FamilyMember(const char* n = "Unknown", char g = 'M', bool isAlive = true) {
        copyName(n);
        gender = (g == 'M' || g == 'm') ? 'M' : 'F';
        alive = isAlive;
        father = mother = firstChild = nextSibling = NULL;
    }

    const char* getName() const { return name; }
    char getGender() const { return gender; }
    bool isAlive() const { return alive; }

    void setAlive(bool a) { alive = a; }

    FamilyMember* getFather() const { return father; }
    FamilyMember* getMother() const { return mother; }
    FamilyMember* getFirstChild() const { return firstChild; }
    FamilyMember* getNextSibling() const { return nextSibling; }

    void setFather(FamilyMember* f) { father = f; }
    void setMother(FamilyMember* m) { mother = m; }
    void setNextSibling(FamilyMember* s) { nextSibling = s; }

    void addChild(FamilyMember* child) {
        if (!child) return;
        if (firstChild == NULL) firstChild = child;
        else {
            FamilyMember* t = firstChild;
            while (t->nextSibling) t = t->nextSibling;
            t->nextSibling = child;
        }
    }
};

// -------------------------
// Class: FamilyPair (represents parents pair and children list)
// We'll use a linked list of FamilyPair nodes for dynamic lists
// -------------------------
class FamilyPair {
private:
    FamilyMember* father;
    FamilyMember* mother;

    // children as an array of pointers (simple dynamic array)
    FamilyMember** children;
    int childCount;
    int childCap;

    FamilyPair* next;

    // helper to ensure capacity
    void ensureCap() {
        if (childCap == 0) {
            childCap = 4;
            children = new FamilyMember * [childCap];
        }
        else if (childCount >= childCap) {
            int nc = childCap * 2;
            FamilyMember** tmp = new FamilyMember * [nc];
            for (int i = 0; i < childCount; ++i) tmp[i] = children[i];
            delete[] children;
            children = tmp;
            childCap = nc;
        }
    }

public:
    FamilyPair(FamilyMember* f = NULL, FamilyMember* m = NULL) {
        father = f; mother = m;
        children = NULL; childCount = 0; childCap = 0;
        next = NULL;
    }
    ~FamilyPair() { if (children) delete[] children; }

    void addChild(FamilyMember* c) {
        if (!c) return;
        ensureCap();
        children[childCount++] = c;
    }

    int getChildCount() const { return childCount; }
    FamilyMember* getChild(int i) const { return (i >= 0 && i < childCount) ? children[i] : NULL; }

    FamilyMember* getFather() const { return father; }
    FamilyMember* getMother() const { return mother; }

    void setNext(FamilyPair* n) { next = n; }
    FamilyPair* getNext() const { return next; }

    // convenience: produce display string for parents (truncated to MAX_NAME)
    void parentLine(char* out, int outSize) const {
        // format: FatherName (M) - MotherName (F)
        const char* fn = father ? father->getName() : "Unknown";
        const char* mn = mother ? mother->getName() : "Unknown";

        // Build truncated names
        char tf[MAX_NAME + 1]; char tm[MAX_NAME + 1];
        int i = 0;
        for (; i < MAX_NAME && fn[i] != '\0'; ++i) tf[i] = fn[i];
        tf[i] = '\0';
        i = 0;
        for (; i < MAX_NAME && mn[i] != '\0'; ++i) tm[i] = mn[i];
        tm[i] = '\0';

        // gender display
        char fg = father ? father->getGender() : 'M';
        char mg = mother ? mother->getGender() : 'F';

        // compose (safe)
        int p = 0;
        int k = 0;
        // father
        while (tf[k] != '\0' && p < outSize - 1) out[p++] = tf[k++]; out[p] = '\0';
        if (p < outSize - 1) { out[p++] = ' '; out[p++] = '('; out[p++] = fg; out[p++] = ')'; }
        // separator
        if (p < outSize - 1) { out[p++] = ' '; out[p++] = '-'; out[p++] = ' '; }
        // mother
        k = 0;
        while (tm[k] != '\0' && p < outSize - 1) out[p++] = tm[k++]; out[p] = '\0';
        if (p < outSize - 1) { out[p++] = ' '; out[p++] = '('; out[p++] = mg; out[p++] = ')'; }
        out[(p < outSize) ? p : outSize - 1] = '\0';
    }

    // children line (concatenate child names separated by spaces, truncated)
    void childrenLine(char* out, int outSize) const {
        int p = 0;
        for (int i = 0; i < childCount; ++i) {
            const char* cn = children[i]->getName();
            // truncated name
            int c = 0;
            while (c < MAX_NAME && cn[c] != '\0' && p < outSize - 1) { out[p++] = cn[c++]; }
            if (i != childCount - 1 && p < outSize - 1) out[p++] = ' ';
        }
        out[p] = '\0';
    }
};

// -------------------------
// Class: FamilyTree
// -------------------------
class FamilyTree {
private:
    FamilyMember* root;

    // helpers for dynamic arrays of members
    FamilyMember** memberPool;
    int memCount;
    int memCap;

    void ensureMemberCap() {
        if (memCap == 0) {
            memCap = 8;
            memberPool = new FamilyMember * [memCap];
        }
        else if (memCount >= memCap) {
            int nc = memCap * 2;
            FamilyMember** tmp = new FamilyMember * [nc];
            for (int i = 0; i < memCount; ++i) tmp[i] = memberPool[i];
            delete[] memberPool;
            memberPool = tmp;
            memCap = nc;
        }
    }

    // Add to pool for later cleanup / traversal
    void poolAdd(FamilyMember* m) {
        ensureMemberCap();
        memberPool[memCount++] = m;
    }

    // simple name equality
    bool eq(const char* a, const char* b) const {
        int i = 0;
        while (a[i] != '\0' || b[i] != '\0') {
            if (a[i] != b[i]) return false;
            ++i;
        }
        return true;
    }

    // find member by name (linear search in pool)
    FamilyMember* findByName(const char* name) {
        for (int i = 0; i < memCount; ++i) {
            if (eq(memberPool[i]->getName(), name)) return memberPool[i];
        }
        return NULL;
    }

    // Recursively collect all members into pool (if we didn't maintain pool)
    void collectAllRecursive(FamilyMember* node) {
        if (!node) return;
        poolAdd(node);
        FamilyMember* c = node->getFirstChild();
        while (c) { collectAllRecursive(c); c = c->getNextSibling(); }
        // siblings are included when their parents traverse, root's siblings none.
    }

    // Build family pairs list for a given set of members.
    // We'll create a linked list of FamilyPair nodes in insertion order.
    FamilyPair* buildFamilyPairsForMembers(FamilyMember** members, int count) {
        FamilyPair* head = NULL;
        FamilyPair* tail = NULL;
        // For each member, check their parents and add to corresponding pair
        for (int i = 0; i < count; ++i) {
            FamilyMember* child = members[i];
            FamilyMember* f = child->getFather();
            FamilyMember* m = child->getMother();

            // If both parents NULL AND child != root, treat child as own family with Unknown parents attached to root?
            // We'll still create a pair (father,mother) possibly both NULL representing unattached
            // But avoid duplicates: check if pair exists already
            FamilyPair* cur = head;
            bool found = false;
            while (cur) {
                FamilyMember* cf = cur->getFather();
                FamilyMember* cm = cur->getMother();
                if (cf == f && cm == m) { cur->addChild(child); found = true; break; }
                cur = cur->getNext();
            }
            if (!found) {
                FamilyPair* np = new FamilyPair(f, m);
                np->addChild(child);
                if (tail == NULL) { head = tail = np; }
                else { tail->setNext(np); tail = np; }
            }
        }
        return head;
    }

    // helper to free family pairs linked list
    void freeFamilyPairs(FamilyPair* head) {
        while (head) {
            FamilyPair* nx = head->getNext();
            delete head;
            head = nx;
        }
    }

    // Helper to truncate and format a single name for display (max MAX_NAME chars)
    static void formatName(const char* src, char* out, int outSize) {
        int i = 0;
        while (i < MAX_NAME && src[i] != '\0' && i < outSize - 1) { out[i] = src[i]; ++i; }
        out[i] = '\0';
    }

public:
    FamilyTree() { root = NULL; memberPool = NULL; memCount = memCap = 0; }

    ~FamilyTree() {
        // delete all members stored in pool
        for (int i = 0; i < memCount; ++i) delete memberPool[i];
        if (memberPool) delete[] memberPool;
    }

    bool hasRoot() const { return root != NULL; }

    bool createRootInteractive() {
        if (root) { cout << "Root already exists.\n"; return false; }
        char name[64];
        readLine("Enter root ancestor full name: ", name, 64);
        if (name[0] == '\0') { cout << "Empty name. Aborted.\n"; return false; }
        char g = readGender("Enter gender (M/F): ");
        bool alive = readYesNoDefaultYes("Is ancestor alive? (y/n) [y]: ");
        FamilyMember* m = new FamilyMember(name, g, alive);
        root = m;
        poolAdd(m);
        cout << "Root '" << name << "' created.\n";
        return true;
    }

    void addMemberInteractive() {
        if (!root) { cout << "Create root first (option 1).\n"; return; }

        char name[64];
        readLine("Enter new member full name: ", name, 64);
        if (name[0] == '\0') { cout << "Empty name. Aborted.\n"; return; }
        if (findByName(name)) { cout << "Member already exists. Aborted.\n"; return; }

        char gender = readGender("Enter gender (M/F): ");
        bool alive = readYesNoDefaultYes("Is person alive? (y/n) [y]: ");

        // Ask if user will provide parents
        cout << "Do you want to specify parents for this member? (y/n): ";
        bool specify = readYesNo(false);

        FamilyMember* father = NULL;
        FamilyMember* mother = NULL;

        if (specify) {
            char pname[64];
            readLine("Enter father's name (or blank if unknown): ", pname, 64);
            if (pname[0] != '\0') {
                father = findByName(pname);
                if (!father) {
                    cout << "Father not found. Create father now? (y/n): ";
                    if (readYesNo(false)) {
                        char fg = readGender("Enter father's gender (M/F): ");
                        bool falive = readYesNoDefaultYes("Is father alive? (y/n) [y]: ");
                        father = new FamilyMember(pname, fg, falive);
                        poolAdd(father);
                        // attach to root to ensure visibility (so it's part of tree)
                        root->addChild(father);
                        cout << "Father created and attached under root for visibility.\n";
                    }
                    else {
                        father = NULL;
                    }
                }
            }

            readLine("Enter mother's name (or blank if unknown): ", pname, 64);
            if (pname[0] != '\0') {
                mother = findByName(pname);
                if (!mother) {
                    cout << "Mother not found. Create mother now? (y/n): ";
                    if (readYesNo(false)) {
                        char mg = readGender("Enter mother's gender (M/F): ");
                        bool malive = readYesNoDefaultYes("Is mother alive? (y/n) [y]: ");
                        mother = new FamilyMember(pname, mg, malive);
                        poolAdd(mother);
                        root->addChild(mother);
                        cout << "Mother created and attached under root for visibility.\n";
                    }
                    else {
                        mother = NULL;
                    }
                }
            }

            // If one parent provided and other missing, ask to create spouse optionally
            if (father && !mother) {
                cout << "Mother missing. Create/set mother now? (y/n): ";
                if (readYesNo(false)) {
                    char pname2[64];
                    readLine("Enter mother's name: ", pname2, 64);
                    if (pname2[0] != '\0') {
                        FamilyMember* mnode = findByName(pname2);
                        if (!mnode) {
                            char mg = readGender("Enter mother's gender (M/F) [F suggested]: ");
                            bool malive = readYesNoDefaultYes("Is mother alive? (y/n) [y]: ");
                            mnode = new FamilyMember(pname2, mg, malive);
                            poolAdd(mnode);
                            root->addChild(mnode);
                        }
                        mother = mnode;
                    }
                }
            }
            else if (mother && !father) {
                cout << "Father missing. Create/set father now? (y/n): ";
                if (readYesNo(false)) {
                    char pname2[64];
                    readLine("Enter father's name: ", pname2, 64);
                    if (pname2[0] != '\0') {
                        FamilyMember* mnode = findByName(pname2);
                        if (!mnode) {
                            char mg = readGender("Enter father's gender (M/F) [M suggested]: ");
                            bool malive = readYesNoDefaultYes("Is father alive? (y/n) [y]: ");
                            mnode = new FamilyMember(pname2, mg, malive);
                            poolAdd(mnode);
                            root->addChild(mnode);
                        }
                        father = mnode;
                    }
                }
            }
        }

        FamilyMember* nm = new FamilyMember(name, gender, alive);
        poolAdd(nm);

        // link parents
        if (father) {
            nm->setFather(father);
            father->addChild(nm);
        }
        if (mother) {
            nm->setMother(mother);
            // if father not present, attach child to mother list for visibility
            if (!father) mother->addChild(nm);
        }
        if (!father && !mother) {
            // attach under root to keep tree connected
            root->addChild(nm);
            cout << "No parents specified; member attached under root for visibility.\n";
        }

        cout << "Member '" << name << "' added successfully.\n";
    }

    void markLateInteractive() {
        if (!root) { cout << "No tree exists.\n"; return; }
        char name[64];
        readLine("Enter member name to mark as Late: ", name, 64);
        if (name[0] == '\0') { cout << "Empty name.\n"; return; }
        FamilyMember* m = findByName(name);
        if (!m) { cout << "Member not found.\n"; return; }
        if (!m->isAlive()) { cout << "Already marked Late.\n"; return; }
        cout << "Confirm marking '" << m->getName() << "' as Late? (y/n): ";
        if (readYesNo(false)) {
            m->setAlive(false);
            cout << "Marked Late.\n";
        }
        else cout << "Cancelled.\n";
    }

    // Build top-down generations as families:
    // Level 0: family pairs whose parents are both NULL (i.e., top ancestors or root and spouse)
    // Level 1: families formed by children of Level 0 families, etc.
    void showCenteredTree() {
        if (!root) { cout << "No tree. Create root first.\n"; return; }

        // gather all members into array (memberPool already maintained)
        // We'll create array of pointers pointing to all members
        FamilyMember** all = memberPool;
        int total = memCount;

        // Build mapping child -> parent pair is implicit. We'll find families for current level by checking parents = something
        // We'll use a simple iterative BFS by generation: start with families where both parents are NULL (top-level families)
        // But many real trees have root as an ancestor with no parents. We'll treat families by parent pair identity.

        // To avoid complexity of true spatial layout, we'll produce centered blocks per family per generation.
        // Step 1: For generation 0, find families where parents have no parents themselves (parents are top)
        // We'll instead pick families where at least one parent is root or has no parents.
        // Simpler: start generation 0 as families that include root as a parent OR members with no parents attached (root)
        // Then iteratively build next generations using the children of families in previous generation.

        // Build initial family list for generation: families that contain members who have father==root or mother==root or whose parents are both NULL and the member is root.
        FamilyPair* genHead = NULL;
        FamilyPair* genTail = NULL;

        // Helper to append a pair node to list
        auto appendPair = [&](FamilyPair* p) {
            if (!genHead) { genHead = genTail = p; }
            else { genTail->setNext(p); genTail = p; }
            };

        // For every member, if their parent pair includes root or both parents null and member is root, include that pair
        for (int i = 0; i < total; ++i) {
            FamilyMember* ch = all[i];
            FamilyMember* f = ch->getFather();
            FamilyMember* m = ch->getMother();
            bool include = false;
            if (f == root || m == root) include = true;
            if (!f && !m && ch == root) include = true;
            if (!include) continue;

            // check existing pair in genHead
            FamilyPair* cur = genHead; bool found = false;
            while (cur) {
                if (cur->getFather() == f && cur->getMother() == m) { cur->addChild(ch); found = true; break; }
                cur = cur->getNext();
            }
            if (!found) {
                FamilyPair* np = new FamilyPair(f, m);
                np->addChild(ch);
                appendPair(np);
            }
        }

        // Now we will print generation by generation using family pairs, and build next generation list from children who are parents themselves
        int generation = 0;
        FamilyPair* currentGen = genHead;

        if (!currentGen) {
            // Fallback: if nothing found (strange), create one family containing root
            FamilyPair* np = new FamilyPair(NULL, NULL);
            np->addChild(root);
            currentGen = np;
        }

        cout << "\n=== CENTERED FAMILY TREE ===\n\n";

        while (currentGen != NULL) {
            // For current generation, compute widths for each family block (parentLine and childrenLine)
            // We'll build arrays by traversing linked list to count nodes
            int count = 0;
            for (FamilyPair* t = currentGen; t; t = t->getNext()) ++count;

            // dynamic arrays
            char** parentLines = new char* [count];
            char** childLines = new char* [count];
            int* widths = new int[count];
            for (int i = 0; i < count; ++i) {
                parentLines[i] = new char[256];
                childLines[i] = new char[256];
                parentLines[i][0] = childLines[i][0] = '\0';
            }

            // fill arrays
            int idx = 0;
            int maxWidth = 0;
            for (FamilyPair* t = currentGen; t; t = t->getNext()) {
                t->parentLine(parentLines[idx], 256);
                t->childrenLine(childLines[idx], 256);
                // compute display width (string length)
                int w = 0; while (parentLines[idx][w] != '\0') ++w;
                int w2 = 0; while (childLines[idx][w2] != '\0') ++w2;
                int ww = w > w2 ? w : w2;
                if (ww < 6) ww = 6; // minimal width for neatness
                widths[idx] = ww;
                if (ww > maxWidth) maxWidth = ww;
                ++idx;
            }

            // Now print parent line for all families centered in their width, separated by HSPACE
            // We'll pad each parent's line to widths[i], centering text
            // totalLineWidth for this generation
            int totalLineWidth = 0;
            for (int i = 0; i < count; ++i) totalLineWidth += widths[i];
            totalLineWidth += (count - 1) * HSPACE;

            // we will print two rows: parents row and children row
            // Parents row
            for (int i = 0; i < count; ++i) {
                // center parentLines[i] in widths[i]
                int len = 0; while (parentLines[i][len] != '\0') ++len;
                int padLeft = (widths[i] - len) / 2;
                for (int sp = 0; sp < padLeft; ++sp) cout << ' ';
                cout << parentLines[i];
                for (int sp = 0; sp < widths[i] - padLeft - len; ++sp) cout << ' ';
                if (i != count - 1) for (int s = 0; s < HSPACE; ++s) cout << ' ';
            }
            cout << '\n';

            // Print connector line: draw a vertical connector from parents to center and a down connector to children
            for (int i = 0; i < count; ++i) {
                int len = 0; while (parentLines[i][len] != '\0') ++len;
                int padLeft = (widths[i] - len) / 2;
                int parentCenter = padLeft + len / 2; // approximate center pos inside block

                // print spaces up to parentCenter
                int pre = parentCenter;
                for (int s = 0; s < pre; ++s) cout << ' ';
                cout << "│";
                // pad rest of block
                for (int s = pre + 1; s < widths[i]; ++s) cout << ' ';
                if (i != count - 1) for (int s = 0; s < HSPACE; ++s) cout << ' ';
            }
            cout << '\n';

            // Children row
            for (int i = 0; i < count; ++i) {
                int len = 0; while (childLines[i][len] != '\0') ++len;
                int padLeft = (widths[i] - len) / 2;
                for (int sp = 0; sp < padLeft; ++sp) cout << ' ';
                cout << childLines[i];
                for (int sp = 0; sp < widths[i] - padLeft - len; ++sp) cout << ' ';
                if (i != count - 1) for (int s = 0; s < HSPACE; ++s) cout << ' ';
            }
            cout << "\n\n";

            // build next generation: families where parents are members listed in children of these families
            // Gather children pointers into an array
            FamilyPair* nextGenHead = NULL; FamilyPair* nextGenTail = NULL;
            // Collect children pointers
            // We'll create an array of children pointers (without duplicates)
            FamilyMember** childList = NULL; int childCount = 0; int childCap = 0;
            auto childEnsure = [&]() {
                if (childCap == 0) { childCap = 8; childList = new FamilyMember * [childCap]; }
                else if (childCount >= childCap) {
                    int nc = childCap * 2;
                    FamilyMember** tmp = new FamilyMember * [nc];
                    for (int ii = 0; ii < childCount; ++ii) tmp[ii] = childList[ii];
                    delete[] childList;
                    childList = tmp;
                    childCap = nc;
                }
                };

            // fill childList from currentGen FamilyPair children
            for (int j = 0; j < count; ++j) {
                for (int c = 0; c < 10000; ++c) {
                    // safe access via childLines length: but we used FamilyPair getter earlier; better to extract children via getChild
                    break;
                }
            }
            // Instead iterate currentGen again to collect children pointers
            int idx2 = 0;
            for (FamilyPair* t = currentGen; t; t = t->getNext()) {
                int cc = t->getChildCount();
                for (int k = 0; k < cc; ++k) {
                    FamilyMember* ch = t->getChild(k);
                    // avoid duplicates
                    bool dup = false;
                    for (int z = 0; z < childCount; ++z) if (childList[z] == ch) { dup = true; break; }
                    if (!dup) {
                        childEnsure();
                        childList[childCount++] = ch;
                    }
                }
            }

            // For each child in childList, if this child is a parent of someone (i.e., has firstChild), we create a FamilyPair for that child's children
            for (int i = 0; i < childCount; ++i) {
                FamilyMember* potentialParent = childList[i];
                if (!potentialParent->getFirstChild()) continue; // not a parent
                // For each of potentialParent's children, we need to find the partner (spouse) pointer: child's father/mother combined
                // For each child c = potentialParent->firstChild ... determine the pair (father,mother) and add family pair
                FamilyMember* cnode = potentialParent->getFirstChild();
                while (cnode) {
                    FamilyMember* f = cnode->getFather();
                    FamilyMember* m = cnode->getMother();
                    // find existing pair in nextGenHead
                    FamilyPair* cur = nextGenHead;
                    bool found = false;
                    while (cur) {
                        if (cur->getFather() == f && cur->getMother() == m) { cur->addChild(cnode); found = true; break; }
                        cur = cur->getNext();
                    }
                    if (!found) {
                        FamilyPair* np = new FamilyPair(f, m);
                        np->addChild(cnode);
                        if (!nextGenHead) nextGenHead = nextGenTail = np;
                        else { nextGenTail->setNext(np); nextGenTail = np; }
                    }
                    cnode = cnode->getNextSibling();
                }
            }

            // cleanup temp arrays
            for (int i = 0; i < count; ++i) {
                delete[] parentLines[i];
                delete[] childLines[i];
            }
            delete[] parentLines;
            delete[] childLines;
            delete[] widths;

            if (childList) delete[] childList;

            // free current generation family pairs and move to next
            FamilyPair* old = currentGen;
            currentGen = nextGenHead;
            // we free old generation list
            FamilyPair* it = old;
            while (it) { FamilyPair* nx = it->getNext(); delete it; it = nx; }
            generation++;
        }

        cout << "=== END OF TREE ===\n";
    }

    // ---------- Utilities: safe input ----------
    static void readLine(const char* prompt, char* buffer, int size) {
        cout << prompt;
        // clear newline
        if (cin.peek() == '\n') cin.get();
        cin.getline(buffer, size);
    }

    static char readGender(const char* prompt) {
        char g;
        while (true) {
            cout << prompt;
            cin >> g;
            if (g == 'M' || g == 'm' || g == 'F' || g == 'f') {
                if (cin.peek() == '\n') cin.get();
                return (g == 'M' || g == 'm') ? 'M' : 'F';
            }
            else {
                cout << "Invalid gender. Enter M or F.\n";
                cin.clear();
                if (cin.peek() != '\n') cin.ignore(1000, '\n');
            }
        }
    }

    static bool readYesNo(bool defaultNo) {
        char ch;
        while (true) {
            cin >> ch;
            if (ch == 'y' || ch == 'Y') { if (cin.peek() == '\n') cin.get(); return true; }
            if (ch == 'n' || ch == 'N') { if (cin.peek() == '\n') cin.get(); return false; }
            cout << "Please enter y or n: ";
            if (cin.peek() != '\n') cin.ignore(1000, '\n');
        }
    }
    static bool readYesNoDefaultYes(const char* prompt) {
        cout << prompt;
        char ch;
        while (true) {
            cin >> ch;
            if (ch == 'y' || ch == 'Y') { if (cin.peek() == '\n') cin.get(); return true; }
            if (ch == 'n' || ch == 'N') { if (cin.peek() == '\n') cin.get(); return false; }
            cout << "Please enter y or n: ";
            if (cin.peek() != '\n') cin.ignore(1000, '\n');
        }
    }

    // ---------- For Menu & testing ----------
    void showAllNames() {
        cout << "\nAll members:\n";
        for (int i = 0; i < memCount; ++i) cout << "- " << memberPool[i]->getName() << "\n";
    }
};

// -------------------------
// Menu (short main logic)
// -------------------------
class Menu {
private:
    FamilyTree tree;
    int readChoice() {
        int c;
        while (!(cin >> c)) {
            cout << "Invalid number, enter again: ";
            cin.clear(); cin.ignore(1000, '\n');
        }
        if (cin.peek() == '\n') cin.get();
        return c;
    }
public:
    void run() {
        cout << "===== CENTERED FAMILY TREE SYSTEM =====\n";
        while (true) {
            cout << "\n1. Create Root Ancestor\n2. Add Member\n3. Mark Member as Late\n4. Show Centered Tree\n5. List All Members\n0. Exit\nEnter choice: ";
            int ch = readChoice();
            if (ch == 0) { cout << "Exiting...\n"; break; }
            if (ch == 1) tree.createRootInteractive();
            else if (ch == 2) tree.addMemberInteractive();
            else if (ch == 3) tree.markLateInteractive();
            else if (ch == 4) tree.showCenteredTree();
            else if (ch == 5) tree.showAllNames();
            else cout << "Invalid choice.\n";
        }
    }
};
int main() {
    Menu m;
    m.run();
    return 0;
}