#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <limits>

using namespace std;

// DATA STRUCTURES 

struct Word {
    string kurdish;
    string english;
    string category;
};

/* Simple hash table that maps Kurdish word text to index in the words list */

class WordHashTable {
private:
    vector<vector<pair<string, int>>> table;
    size_t capacity;

    size_t hashString(const string &s) const {
        unsigned long h = 0;
        for (char c : s) {
            h = h * 31 + static_cast<unsigned char>(c);
        }
        return h % capacity;
    }

public:
    WordHashTable(size_t cap = 101) : capacity(cap) {
        table.resize(capacity);
    }

    void insert(const string &key, int index) {
        size_t h = hashString(key);
        for (auto &p : table[h]) {
            if (p.first == key) {
                p.second = index;
                return;
            }
        }
        table[h].push_back({key, index});
    }

    int find(const string &key) const {
        size_t h = hashString(key);
        for (const auto &p : table[h]) {
            if (p.first == key) {
                return p.second;
            }
        }
        return -1;
    }
};

/* Tree for categories */

struct CategoryNode {
    string name;
    vector<int> wordIndices; // indices into main word list
    vector<CategoryNode*> children;
};

/* Graph for city connections using adjacency list */

class CityGraph {
private:
    vector<string> cityNames;
    vector<vector<int>> adjList;

public:
    CityGraph(const vector<string> &names)
        : cityNames(names), adjList(names.size()) {}

    void addUndirectedEdge(int a, int b) {
        if (a < 0 || b < 0 || a >= (int)cityNames.size() || b >= (int)cityNames.size()) return;
        adjList[a].push_back(b);
        adjList[b].push_back(a);
    }

    void printConnections() const {
        cout << "\nKurdish city connections:\n";
        for (size_t i = 0; i < cityNames.size(); ++i) {
            cout << "  " << cityNames[i] << ": ";
            for (size_t j = 0; j < adjList[i].size(); ++j) {
                cout << cityNames[adjList[i][j]];
                if (j + 1 < adjList[i].size()) cout << ", ";
            }
            cout << "\n";
        }
        cout << "\n";
    }
};

// HELPER FUNCTIONS

void clearInputLine() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void printAllWords(const vector<Word> &words) {
    cout << "\nAll Kurdish words:\n";
    for (size_t i = 0; i < words.size(); ++i) {
        cout << "  " << words[i].kurdish
             << "  =  " << words[i].english
             << "  [" << words[i].category << "]\n";
    }
    cout << "\n";
}

/* Practice mode uses a queue of indices */

void practiceWords(const vector<Word> &words) {
    if (words.empty()) {
        cout << "\nNo words to practice.\n\n";
        return;
    }

    queue<int> q;
    for (size_t i = 0; i < words.size(); ++i) {
        q.push(static_cast<int>(i));
    }

    cout << "\nPractice mode. Press Enter to flip each card. Type q and Enter to quit.\n\n";
    clearInputLine(); // clear leftover newline from menu

    while (!q.empty()) {
        int idx = q.front();
        q.pop();

        cout << "Kurdish: " << words[idx].kurdish << "\n";
        cout << "[Press Enter to see meaning, or q then Enter to quit]: ";
        string input;
        getline(cin, input);
        if (!input.empty() && (input == "q" || input == "Q")) {
            cout << "\nExiting practice mode.\n\n";
            return;
        }

        cout << "  English: " << words[idx].english
             << "  [" << words[idx].category << "]\n\n";
    }

    cout << "You reached the end of the practice list.\n\n";
}

// Lookup using hash table

void lookUpWord(const vector<Word> &words, const WordHashTable &table) {
    cout << "\nEnter a Kurdish word to look up: ";
    clearInputLine();
    string query;
    getline(cin, query);

    int idx = table.find(query);
    if (idx == -1) {
        cout << "Word not found: " << query << "\n\n";
    } else {
        const Word &w = words[idx];
        cout << "Found:\n";
        cout << "  Kurdish: " << w.kurdish << "\n";
        cout << "  English: " << w.english << "\n";
        cout << "  Category: " << w.category << "\n\n";
    }
}

// Print category tree

void printCategoryTree(CategoryNode *node, int depth = 0) {
    if (!node) return;
    for (int i = 0; i < depth; ++i) cout << "  ";
    cout << "- " << node->name << "\n";
    for (CategoryNode *child : node->children) {
        printCategoryTree(child, depth + 1);
    }
}

// MAIN SETUP 

int main() {
    // Hard coded words list
    vector<Word> words = {
        {"slaw",    "hello",          "greetings"},
        {"roj bash","good morning",   "greetings"},
        {"choni",   "how are you",    "greetings"},
        {"bawk",    "father",         "family"},
        {"dayik",   "mother",         "family"},
        {"brak",    "brother",        "family"},
        {"xoshawistim", "my love",    "family"},
        {"nan",     "bread",          "food"},
        {"aw",      "water",          "food"},
        {"mast",    "yogurt",         "food"}
    };

    // Build hash table
    WordHashTable table(101);
    for (size_t i = 0; i < words.size(); ++i) {
        table.insert(words[i].kurdish, static_cast<int>(i));
    }

    // Build category tree
    CategoryNode root{"Kurdish vocabulary", {}, {}};
    CategoryNode greetings{"greetings", {}, {}};
    CategoryNode family{"family", {}, {}};
    CategoryNode food{"food", {}, {}};

    for (size_t i = 0; i < words.size(); ++i) {
        if (words[i].category == "greetings") {
            greetings.wordIndices.push_back(static_cast<int>(i));
        } else if (words[i].category == "family") {
            family.wordIndices.push_back(static_cast<int>(i));
        } else if (words[i].category == "food") {
            food.wordIndices.push_back(static_cast<int>(i));
        }
    }

    root.children.push_back(&greetings);
    root.children.push_back(&family);
    root.children.push_back(&food);

    // Build city graph
    vector<string> cityNames = {"Zakho", "Duhok", "Erbil", "Silemani"};
    CityGraph graph(cityNames);

    int ZAKHO = 0;
    int DUHOK = 1;
    int ERBIL = 2;
    int SILEMANI = 3;

    graph.addUndirectedEdge(ZAKHO, DUHOK);
    graph.addUndirectedEdge(DUHOK, ERBIL);
    graph.addUndirectedEdge(ERBIL, SILEMANI);

    // Main menu loop
    while (true) {
        cout << "==============================\n";
        cout << "  Kurdish Vocab Helper\n";
        cout << "==============================\n";
        cout << "1. View all words\n";
        cout << "2. Practice words in order\n";
        cout << "3. Look up a word\n";
        cout << "4. Show category tree\n";
        cout << "5. Show city connections\n";
        cout << "0. Exit\n";
        cout << "Choose an option: ";

        int choice;
        if (!(cin >> choice)) {
            cin.clear();
            clearInputLine();
            cout << "\nInvalid input. Please enter a number.\n\n";
            continue;
        }

        switch (choice) {
        case 1:
            printAllWords(words);
            break;
        case 2:
            practiceWords(words);
            break;
        case 3:
            lookUpWord(words, table);
            break;
        case 4:
            cout << "\nCategory tree:\n";
            printCategoryTree(&root);
            cout << "\n";
            break;
        case 5:
            graph.printConnections();
            break;
        case 0:
            cout << "\nGoodbye, spas.\n";
            return 0;
        default:
            cout << "\nInvalid option. Try again.\n\n";
            break;
        }
    }

    return 0;
}
