#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <limits>
#include <cctype>
#include <algorithm>
#include <cstdlib>
#include <queue>
#include <stack>
#include <iomanip>
#include <cmath>

using namespace std;

const int MAX_QUEUE_SIZE = 50;
const int MAX_STACK_SIZE = 50;
const int INITIAL_HASH_SIZE = 11;
const string VERSION = "2.0.0 Ultimate";

// ==========================================
//           UTILITY & HELPER FUNCTIONS
// ==========================================

/**
 * @brief Clears the console screen in a cross-platform way.
 */
void ClearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/**
 * @brief  Gets the current system time as a formatted string.
 * @return string YYYY-MM-DD HH:MM:SS
 */
string CurrentTimestamp() {
    time_t t = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buf);
}

/**
 * @brief Prints a decorative horizontal line.
 */
void PrintLine(char ch = '-', int length = 70) {
    for (int i = 0; i < length; i++) cout << ch;
    cout << endl;
}

/**
 * @brief Prints a centered header.
 */
void PrintHeader(string title) {
    PrintLine('=');
    int padding = (70 - title.length()) / 2;
    for(int i=0; i<padding; i++) cout << " ";
    cout << title << endl;
    PrintLine('=');
}

/**
 * @brief  Robust integer input handler.
 * Loops until the user enters a valid integer within the range.
 */
int InputInt(const string& prompt, int min = INT_MIN, int max = INT_MAX) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x && x >= min && x <= max) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer
            return x;
        }
        cout << " [ERROR] Invalid input. Please enter a number between " << min << " and " << max << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

/**
 * @brief  Robust string input handler.
 * Prevents empty inputs and buffer skipping issues.
 */
string InputString(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    while (s.empty()) {
        getline(cin, s);
    }
    return s;
}const int MAX_QUEUE_SIZE = 50;
const int MAX_STACK_SIZE = 50;
const int INITIAL_HASH_SIZE = 11;
const string VERSION = "2.0.0 Ultimate";

// ==========================================
//           UTILITY & HELPER FUNCTIONS
// ==========================================

/**
 * @brief Clears the console screen in a cross-platform way.
 */
void ClearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

/**
 * @brief  Gets the current system time as a formatted string.
 * @return string YYYY-MM-DD HH:MM:SS
 */
string CurrentTimestamp() {
    time_t t = time(nullptr);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buf);
}

/**
 * @brief Prints a decorative horizontal line.
 */
void PrintLine(char ch = '-', int length = 70) {
    for (int i = 0; i < length; i++) cout << ch;
    cout << endl;
}

/**
 * @brief Prints a centered header.
 */
void PrintHeader(string title) {
    PrintLine('=');
    int padding = (70 - title.length()) / 2;
    for(int i=0; i<padding; i++) cout << " ";
    cout << title << endl;
    PrintLine('=');
}

/**
 * @brief  Robust integer input handler.
 * Loops until the user enters a valid integer within the range.
 */
int InputInt(const string& prompt, int min = INT_MIN, int max = INT_MAX) {
    int x;
    while (true) {
        cout << prompt;
        if (cin >> x && x >= min && x <= max) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear buffer
            return x;
        }
        cout << " [ERROR] Invalid input. Please enter a number between " << min << " and " << max << ".\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

/**
 * @brief  Robust string input handler.
 * Prevents empty inputs and buffer skipping issues.
 */
string InputString(const string& prompt) {
    string s;
    cout << prompt;
    getline(cin, s);
    while (s.empty()) {
        getline(cin, s);
    }
    return s;
}
class SystemLogger {
private:
    struct LogEntry {
        string timestamp;
        string action;
        string details;
    };
    vector<LogEntry> logs;

public:
    void Log(string action, string details) {
        logs.push_back({CurrentTimestamp(), action, details});
    }

    void DisplayLogs() {
        PrintHeader("SYSTEM LOGS");
        if (logs.empty()) {
            cout << " No activity recorded.\n";
            return;
        }
        for (auto& log : logs) {
            cout << " [" << log.timestamp << "] " << left << setw(15) << log.action << " : " << log.details << endl;
        }
        PrintLine();
    }
};

// Global logger instance
SystemLogger sysLog;

// ==========================================
//           CORE DATA CLASSES
// ==========================================

class FileVersion {
public:
    string content; 
    string timestamp;
    int versionNumber;

    FileVersion(const string &c, int v) : content(c), versionNumber(v) {
        timestamp = CurrentTimestamp();
    }
};
// ==========================================
//       DATA STRUCTURE: LINKED LIST (VERSIONS)
// ==========================================

/**
 * @class VersionNode
 * @brief Node for Singly Linked List storing file versions
 */
class VersionNode {
public:
    FileVersion data;
    VersionNode* next;
    
    VersionNode(const FileVersion& v) : data(v), next(nullptr) {}
};

/**
 * @class VersionLinkedList
 * @brief Singly Linked List for file version history
 */
class VersionLinkedList {
private:
    VersionNode* head;
    int count;
    
    // Helper function to copy nodes
    VersionNode* CopyNodes(VersionNode* source) {
        if (!source) return nullptr;
        VersionNode* newHead = new VersionNode(source->data);
        VersionNode* current = newHead;
        VersionNode* sourceCurrent = source->next;
        
        while (sourceCurrent) {
            current->next = new VersionNode(sourceCurrent->data);
            current = current->next;
            sourceCurrent = sourceCurrent->next;
        }
        return newHead;
    }
    
    // Helper function to clear nodes
    void Clear() {
        while (head) {
            VersionNode* temp = head;
            head = head->next;
            delete temp;
        }
        count = 0;
    }
    
public:
    VersionLinkedList() : head(nullptr), count(0) {}
    
    // Copy constructor
    VersionLinkedList(const VersionLinkedList& other) : head(nullptr), count(0) {
        head = CopyNodes(other.head);
        count = other.count;
    }
    
    // Copy assignment operator
    VersionLinkedList& operator=(const VersionLinkedList& other) {
        if (this != &other) {
            Clear();
            head = CopyNodes(other.head);
            count = other.count;
        }
        return *this;
    }
    
    ~VersionLinkedList() {
        Clear();
    }
    
    void AddVersion(const FileVersion& v) {
        VersionNode* newNode = new VersionNode(v);
        newNode->next = head;  // Insert at front (LIFO - latest first)
        head = newNode;
        count++;
    }
    
    FileVersion GetLatest() const {
        return head ? head->data : FileVersion("", 0);
    }
    
    int GetCount() const { return count; }
    
    void DisplayAll() const {
        if (!head) {
            cout << " No versions available.\n";
            return;
        }
        VersionNode* current = head;
        int verNum = count;
        cout << " --- VERSION HISTORY (Linked List) ---\n";
        while (current) {
            cout << " Version " << verNum << " [" << current->data.timestamp << "]\n";
            current = current->next;
            verNum--;
        }
    }
    
    FileVersion GetVersion(int versionNum) const {
        if (versionNum < 1 || versionNum > count) return FileVersion("", 0);
        
        VersionNode* current = head;
        int pos = count;
        while (current && pos != versionNum) {
            current = current->next;
            pos--;
        }
        return current ? current->data : FileVersion("", 0);
    }
};

/**
 * @class File
 * @brief Represents a single file entity with version control and metadata.
 */
class File {
private:
    int id;
    string name;
    string type;
    string owner;
    int sizeBytes;
    int priority; // 1-10, for Heap
    VersionLinkedList versions;  // Changed from vector to Linked List
    vector<string> tags;
    
    // --- RLE Compression Algorithms ---
    
    string RLECompress(const string &s) {
        if (s.empty()) return "";
        string out;
        for (size_t i = 0; i < s.size();) {
            char c = s[i];
            size_t j = i + 1;
            while (j < s.size() && s[j] == c) ++j;
            out += to_string(j - i) + string(1, c);
            i = j;
        }
        return out;
    }

    string RLEDecompress(const string &s) const {
        string out;
        size_t i = 0;
        while (i < s.size()) {
            int count = 0;
            while (i < s.size() && isdigit(s[i])) {
                count = count * 10 + (s[i] - '0');
                ++i;
            }
            if (i < s.size()) {
                char c = s[i++];
                out.append(count, c);
            }
        }
        return out;
    }

public:
    File() : id(0), sizeBytes(0), priority(0) {} 

    void SetValues(int id_, const string &name_, const string &type_, const string &owner_, const string &content_, int prio = 1) {
        id = id_;
        name = name_;
        type = type_;
        owner = owner_;
        priority = prio;
        sizeBytes = content_.size();
        AddVersion(content_);
    }

    void AddVersion(const string &rawContent) {
        string comp = RLECompress(rawContent);
        int nextVer = versions.GetCount() + 1;
        versions.AddVersion(FileVersion(comp, nextVer));
        sizeBytes = (int)rawContent.size();
    }

    string GetContent() const {
        FileVersion latest = versions.GetLatest();
        if (latest.versionNumber == 0) return "";
        return RLEDecompress(latest.content);
    }
    
    void DisplayVersionHistory() const {
        versions.DisplayAll();
    }
    
    int GetVersionCount() const {
        return versions.GetCount();
    }

    void DisplayDetailed() const {
        PrintLine('.');
        cout << " FILE DETAILS\n";
        cout << " ID:       " << id << "\n";
        cout << " Name:     " << name << "." << type << "\n";
        cout << " Owner:    " << owner << "\n";
        cout << " Priority: " << priority << "/10\n";
        cout << " Size:     " << sizeBytes << " bytes\n";
        cout << " Versions: " << versions.GetCount() << "\n";
        cout << " Content:  " << GetContent() << "\n";
        PrintLine('.');
    }

    void DisplayRow() const {
        cout << " " << setw(5) << id << " | " << setw(15) << name << " | " << setw(5) << type << " | " << setw(5) << sizeBytes << "B | Prio: " << priority << endl;
    }

    int GetID() const { return id; }
    void SetID(int newID) { id = newID; } 
    string GetName() const { return name; }
    int GetSize() const { return sizeBytes; }
    int GetPriority() const { return priority; }
};
// ==========================================
//       DATA STRUCTURE: DOUBLE LINKED LIST (FILE NAVIGATION)
// ==========================================

/**
 * @class FileNode
 * @brief Node for Double Linked List for file navigation
 */
class FileNode {
public:
    File data;
    FileNode* prev;
    FileNode* next;
    
    FileNode(const File& f) : data(f), prev(nullptr), next(nullptr) {}
};

/**
 * @class FileDoubleLinkedList
 * @brief Double Linked List for sequential file browsing
 */
class FileDoubleLinkedList {
private:
    FileNode* head;
    FileNode* tail;
    FileNode* current;  // Current position for navigation
    
    // Helper to copy nodes
    void CopyNodes(const FileDoubleLinkedList& other) {
        if (!other.head) {
            head = tail = current = nullptr;
            return;
        }
        
        // Copy first node
        head = new FileNode(other.head->data);
        FileNode* otherCurrent = other.head->next;
        FileNode* thisCurrent = head;
        current = head;  // Set current to head initially
        
        // Copy remaining nodes
        while (otherCurrent) {
            thisCurrent->next = new FileNode(otherCurrent->data);
            thisCurrent->next->prev = thisCurrent;
            thisCurrent = thisCurrent->next;
            otherCurrent = otherCurrent->next;
        }
        
        tail = thisCurrent;
    }
    
public:
    FileDoubleLinkedList() : head(nullptr), tail(nullptr), current(nullptr) {}
    
    // Copy constructor
    FileDoubleLinkedList(const FileDoubleLinkedList& other) : head(nullptr), tail(nullptr), current(nullptr) {
        CopyNodes(other);
    }
    
    // Copy assignment operator
    FileDoubleLinkedList& operator=(const FileDoubleLinkedList& other) {
        if (this != &other) {
            Clear();
            CopyNodes(other);
        }
        return *this;
    }
    
    ~FileDoubleLinkedList() {
        Clear();
    }
    
    void AddFile(const File& f) {
        FileNode* newNode = new FileNode(f);
        if (!head) {
            head = tail = current = newNode;
        } else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }
    
    void Clear() {
        FileNode* temp = head;
        while (temp) {
            FileNode* next = temp->next;
            delete temp;
            temp = next;
        }
        head = tail = current = nullptr;
    }
    
    void BuildFromVector(const vector<File>& files) {
        Clear();
        for (const auto& f : files) {
            AddFile(f);
        }
        current = head;  // Reset to beginning
    }
    
    File* GetCurrent() { 
        return current ? &current->data : nullptr; 
    }
    
    File* GetNext() { 
        if (current && current->next) {
            current = current->next;
            return &current->data;
        }
        return nullptr;
    }
    
    File* GetPrev() {
        if (current && current->prev) {
            current = current->prev;
            return &current->data;
        }
        return nullptr;
    }
    
    void Reset() { current = head; }
    
    bool HasNext() const { return current && current->next != nullptr; }
    bool HasPrev() const { return current && current->prev != nullptr; }
};

// ==========================================
//       DATA STRUCTURE: CIRCULAR LINKED LIST (NOTIFICATIONS)
// ==========================================

/**
 * @class NotificationNode
 * @brief Node for Circular Linked List storing notifications
 */
class NotificationNode {
public:
    string message;
    NotificationNode* next;
    
    NotificationNode(const string& msg) : message(msg), next(nullptr) {}
};

/**
 * @class CircularNotificationQueue
 * @brief Circular Linked List for notification management
 */
class CircularNotificationQueue {
private:
    NotificationNode* tail;  // Points to last node (tail->next = head)
    int count;
    
public:
    CircularNotificationQueue() : tail(nullptr), count(0) {}
    
    ~CircularNotificationQueue() {
        if (!tail) return;
        
        NotificationNode* current = tail->next;  // Start from head
        NotificationNode* start = current;
        
        if (current) {
            do {
                NotificationNode* temp = current;
                current = current->next;
                delete temp;
            } while (current != start);
        }
    }
    
    void AddNotification(const string& msg) {
        NotificationNode* newNode = new NotificationNode(msg);
        if (!tail) {
            tail = newNode;
            tail->next = tail;  // Points to itself (circular)
        } else {
            newNode->next = tail->next;  // New node points to head
            tail->next = newNode;        // Old tail points to new node
            tail = newNode;              // Update tail
        }
        count++;
    }
    
    void DisplayAll() {
        if (!tail) { 
            cout << " No notifications.\n"; 
            return; 
        }
        
        NotificationNode* current = tail->next;  // Start from head
        NotificationNode* start = current;
        int idx = 1;
        
        cout << " --- NOTIFICATIONS (Circular Linked List) ---\n";
        do {
            cout << " [" << idx++ << "] " << current->message << endl;
            current = current->next;
        } while (current != start);  // Until we circle back
    }
    
    int GetCount() const { return count; }
    
    void Clear() {
        if (!tail) return;
        
        NotificationNode* current = tail->next;
        NotificationNode* start = current;
        
        if (current) {
            do {
                NotificationNode* temp = current;
                current = current->next;
                delete temp;
            } while (current != start);
        }
        
        tail = nullptr;
        count = 0;
    }
};

// ==========================================
//       DATA STRUCTURE: STACK & QUEUE
// ==========================================

/**
 * @class FileStack
 * @brief Implements LIFO structure for Deleted Files (Trash Bin).
 */
class FileStack { 
    int top;
    File arr[MAX_STACK_SIZE];
public:
    FileStack() : top(0) {}
    
    void Push(File f) { 
        if(top < MAX_STACK_SIZE) {
            arr[top++] = f; 
        } else {
            cout << " [WARN] Trash bin full. Oldest deleted file overwritten.\n";
            // Shift left to make space
            for(int i=0; i<MAX_STACK_SIZE-1; i++) arr[i] = arr[i+1];
            arr[MAX_STACK_SIZE-1] = f;
        }
    }

    File Pop() { 
        if(!IsEmpty()) return arr[--top]; 
        return File(); 
    }

    bool IsEmpty() const { return top == 0; }
    
    void EmptyTrash() {
        top = 0;
        cout << " Trash bin emptied.\n";
    }

    void Display() const {
        if(IsEmpty()) { cout << " (Trash is empty)\n"; return; }
        for (int i = top-1; i >= 0; i--) {
            cout << " [" << i+1 << "] " << arr[i].GetName() << " (ID: " << arr[i].GetID() << ")\n";
        }
    }
};

/**
 * @class FileQueue
 * @brief Implements FIFO structure for Recent Files history.
 */
class FileQueue { 
    int front, rear;
    File arr[MAX_QUEUE_SIZE];
    int count;
public:
    FileQueue() : front(0), rear(0), count(0) {}
    
    void Enqueue(File f) {
        arr[rear] = f;
        rear = (rear + 1) % MAX_QUEUE_SIZE;
        if (count < MAX_QUEUE_SIZE) count++;
        else front = (front + 1) % MAX_QUEUE_SIZE; 
    }

    void Display() const {
        if (count == 0) { cout << " (No recent files)\n"; return; }
        int idx = front;
        for(int i=0; i<count; i++) {
            cout << "  " << i+1 << ". " << arr[idx].GetName() << " (Accessed: " << CurrentTimestamp() << ")\n";
            idx = (idx + 1) % MAX_QUEUE_SIZE;
        }
    }
};

// ==========================================
//       DATA STRUCTURE: MAX HEAP (PRIORITY)
// ==========================================

/**
 * @class FileMaxHeap
 * @brief Manages "Starred" or High Priority files.
 */
class FileMaxHeap {
private:
    vector<File> heap;

    int Parent(int i) { return (i - 1) / 2; }
    int Left(int i) { return (2 * i + 1); }
    int Right(int i) { return (2 * i + 2); }

    void HeapifyDown(int i) {
        int l = Left(i);
        int r = Right(i);
        int largest = i;

        if (l < heap.size() && heap[l].GetPriority() > heap[largest].GetPriority())
            largest = l;
        if (r < heap.size() && heap[r].GetPriority() > heap[largest].GetPriority())
            largest = r;

        if (largest != i) {
            swap(heap[i], heap[largest]);
            HeapifyDown(largest);
        }
    }

    void HeapifyUp(int i) {
        if (i && heap[Parent(i)].GetPriority() < heap[i].GetPriority()) {
            swap(heap[i], heap[Parent(i)]);
            HeapifyUp(Parent(i));
        }
    }

public:
    void Insert(File f) {
        heap.push_back(f);
        HeapifyUp(heap.size() - 1);
    }

    void DisplayTop() {
        if (heap.empty()) { cout << " No starred files.\n"; return; }
        
        // Sort a copy for display without ruining heap structure
        vector<File> temp = heap;
        sort(temp.begin(), temp.end(), [](File a, File b){
            return a.GetPriority() > b.GetPriority();
        });

        cout << " --- STARRED FILES (High Priority) ---\n";
        for (const auto& f : temp) {
            cout << " [Prio " << f.GetPriority() << "] " << f.GetName() << "\n";
        }
    }

    File ExtractMax() {
        if (heap.empty()) return File();
        File root = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        HeapifyDown(0);
        return root;
    }
};

// ==========================================
//       DATA STRUCTURE: HASH TABLE
// ==========================================

/**
 * @class HashTableFiles
 * @brief Stores files in a folder using Linear Probing for collision resolution.
 */
class HashTableFiles {
private:
    int capacity;
    int currentSize;
    File* arr;

    int HashFunction(int key) { return key % capacity; }

    void Resize() {
        int oldCapacity = capacity;
        File* oldArr = arr;
        
        capacity = capacity * 2; 
        arr = new File[capacity];
        currentSize = 0;

        for (int i = 0; i < oldCapacity; i++) {
            if (oldArr[i].GetID() > 0) { 
                Insert(oldArr[i]);
            }
        }
        delete[] oldArr;
        // sysLog.Log("System", "Hash Table Resized to " + to_string(capacity));
    }

public:
    HashTableFiles(int cap = INITIAL_HASH_SIZE) : capacity(cap), currentSize(0) {
        arr = new File[capacity];
    }

    // Copy constructor - CRITICAL: Prevents shallow copy crashes
    HashTableFiles(const HashTableFiles& other) : capacity(other.capacity), currentSize(other.currentSize) {
        arr = new File[capacity];
        for (int i = 0; i < capacity; i++) {
            arr[i] = other.arr[i];  // Deep copy each File
        }
    }

    // Copy assignment operator - CRITICAL: Prevents shallow copy crashes
    HashTableFiles& operator=(const HashTableFiles& other) {
        if (this != &other) {
            delete[] arr;  // Free existing memory
            capacity = other.capacity;
            currentSize = other.currentSize;
            arr = new File[capacity];
            for (int i = 0; i < capacity; i++) {
                arr[i] = other.arr[i];  // Deep copy each File
            }
        }
        return *this;
    }

    ~HashTableFiles() { 
        if (arr) {
            delete[] arr;
            arr = nullptr;
        }
    }

    void Insert(File f) {
        if (currentSize >= capacity * 0.7) Resize();

        int idx = HashFunction(f.GetID());
        int startIdx = idx;
        
        while (arr[idx].GetID() > 0) { 
            if (arr[idx].GetID() == f.GetID()) {
                cout << " [Error] Duplicate File ID.\n";
                return;
            }
            idx = (idx + 1) % capacity;
            if (idx == startIdx) return; // Should not happen due to resize
        }
        
        arr[idx] = f;
        currentSize++;
    }

    File* Search(int id) {
        int idx = HashFunction(id);
        int startIdx = idx;
        
        while (arr[idx].GetID() != 0) { 
            if (arr[idx].GetID() == id) return &arr[idx]; 
            idx = (idx + 1) % capacity;
            if (idx == startIdx) break; 
        }
        return nullptr;
    }

    File Delete(int id) {
        int idx = HashFunction(id);
        int startIdx = idx;

        while (arr[idx].GetID() != 0) {
            if (arr[idx].GetID() == id) {
                File temp = arr[idx];
                arr[idx].SetID(-1); // Tombstone
                currentSize--;
                return temp;
            }
            idx = (idx + 1) % capacity;
            if (idx == startIdx) break;
        }
        return File();
    }

    void DisplayAll() {
        if (currentSize == 0) { cout << " (Folder is empty)\n"; return; }
        
        cout << " ---------------------------------------------------------\n";
        cout << "   ID  |      NAME       | TYPE  | SIZE   | PRIORITY \n";
        cout << " ---------------------------------------------------------\n";
        for (int i = 0; i < capacity; i++) {
            if (arr[i].GetID() > 0) {
                arr[i].DisplayRow();
            }
        }
        cout << " ---------------------------------------------------------\n";
    }

int main(){

    return 0;
}