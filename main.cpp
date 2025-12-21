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
string Trim(string s) {
    int start = s.find_first_not_of(" \t\n\r");
    int end = s.find_last_not_of(" \t\n\r");
    if(start == string::npos) return "";
    return s.substr(start, end - start + 1);
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

class FileVersion {
public:
    string content; 
    string timestamp;
    int versionNumber;

    FileVersion(const string &c, int v) : content(c), versionNumber(v) {
        timestamp = CurrentTimestamp();
    }
};

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

    // Helper to get vector for sorting
    vector<File> GetFilesVector() {
        vector<File> v;
        for(int i=0; i<capacity; i++) {
            if(arr[i].GetID() > 0) v.push_back(arr[i]);
        }
        return v;
    }

    // 1. Bubble Sort (Size)
    void SortBubbleSize() {
        vector<File> v = GetFilesVector();
        for (size_t i = 0; i < v.size() - 1; i++)
            for (size_t j = 0; j < v.size() - i - 1; j++)
                if (v[j].GetSize() > v[j+1].GetSize())
                    swap(v[j], v[j+1]);

        cout << "\n [SORTED BY SIZE (BUBBLE SORT)]\n";
        for(auto& f : v) f.DisplayRow();
    }

    // 2. Quick Sort (Name)
    int Partition(vector<File>& v, int low, int high) {
        string pivot = v[high].GetName();
        int i = (low - 1);
        for (int j = low; j <= high - 1; j++) {
            if (v[j].GetName() < pivot) {
                i++;
                swap(v[i], v[j]);
            }
        }
        swap(v[i + 1], v[high]);
        return (i + 1);
    }

    void QuickSort(vector<File>& v, int low, int high) {
        if (low < high) {
            int pi = Partition(v, low, high);
            QuickSort(v, low, pi - 1);
            QuickSort(v, pi + 1, high);
        }
    }

    void SortQuickName() {
        vector<File> v = GetFilesVector();
        if(!v.empty()) QuickSort(v, 0, v.size()-1);
        cout << "\n [SORTED BY NAME (QUICK SORT - O(n log n))]\n";
        for(auto& f : v) f.DisplayRow();
    }

    // 3. Insertion Sort (Size)
    void SortInsertionSize() {
        vector<File> v = GetFilesVector();
        
        for (size_t i = 1; i < v.size(); i++) {
            File key = v[i];
            int j = i - 1;
            while (j >= 0 && v[j].GetSize() > key.GetSize()) {
                v[j + 1] = v[j];
                j--;
            }
            v[j + 1] = key;
        }
        
        cout << "\n [SORTED BY SIZE (INSERTION SORT - O(n²))]\n";
        for(auto& f : v) f.DisplayRow();
    }

    // 4. Selection Sort (Size)
    void SortSelectionSize() {
        vector<File> v = GetFilesVector();
        
        for (size_t i = 0; i < v.size() - 1; i++) {
            int minIdx = i;
            for (size_t j = i + 1; j < v.size(); j++) {
                if (v[j].GetSize() < v[minIdx].GetSize()) {
                    minIdx = j;
                }
            }
            swap(v[i], v[minIdx]);
        }
        
        cout << "\n [SORTED BY SIZE (SELECTION SORT - O(n²))]\n";
        for(auto& f : v) f.DisplayRow();
    }

    // 5. Merge Sort (Size)
    void Merge(vector<File>& arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;
        
        vector<File> L(n1), R(n2);
        for (int i = 0; i < n1; i++) L[i] = arr[left + i];
        for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];
        
        int i = 0, j = 0, k = left;
        while (i < n1 && j < n2) {
            if (L[i].GetSize() <= R[j].GetSize()) {
                arr[k] = L[i++];
            } else {
                arr[k] = R[j++];
            }
            k++;
        }
        
        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    void MergeSort(vector<File>& arr, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            MergeSort(arr, left, mid);
            MergeSort(arr, mid + 1, right);
            Merge(arr, left, mid, right);
        }
    }

    void SortMergeSize() {
        vector<File> v = GetFilesVector();
        if (!v.empty()) MergeSort(v, 0, v.size() - 1);
        cout << "\n [SORTED BY SIZE (MERGE SORT - O(n log n))]\n";
        for(auto& f : v) f.DisplayRow();
    }

    // 6. Heap Sort (Size)
    void Heapify(vector<File>& arr, int n, int i) {
        int largest = i;
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        
        if (left < n && arr[left].GetSize() > arr[largest].GetSize())
            largest = left;
        if (right < n && arr[right].GetSize() > arr[largest].GetSize())
            largest = right;
        
        if (largest != i) {
            swap(arr[i], arr[largest]);
            Heapify(arr, n, largest);
        }
    }

    void SortHeapSize() {
        vector<File> v = GetFilesVector();
        int n = v.size();
        
        // Build max heap
        for (int i = n / 2 - 1; i >= 0; i--)
            Heapify(v, n, i);
        
        // Extract elements one by one
        for (int i = n - 1; i > 0; i--) {
            swap(v[0], v[i]);
            Heapify(v, i, 0);
        }
        
        cout << "\n [SORTED BY SIZE (HEAP SORT - O(n log n))]\n";
        for(auto& f : v) f.DisplayRow();
    }

    // 7. Counting Sort (Size) - O(n+k)
    void SortCountingSize() {
        vector<File> v = GetFilesVector();
        if (v.empty()) return;
        
        // Find max size
        int maxSize = v[0].GetSize();
        for (auto& f : v) {
            if (f.GetSize() > maxSize) maxSize = f.GetSize();
        }
        
        // Count array
        vector<int> count(maxSize + 1, 0);
        for (auto& f : v) count[f.GetSize()]++;
        
        // Cumulative count
        for (int i = 1; i <= maxSize; i++) count[i] += count[i - 1];
        
        // Output array
        vector<File> output(v.size());
        for (int i = v.size() - 1; i >= 0; i--) {
            output[count[v[i].GetSize()] - 1] = v[i];
            count[v[i].GetSize()]--;
        }
        
        cout << "\n [SORTED BY SIZE (COUNTING SORT - O(n+k))]\n";
        for(auto& f : output) f.DisplayRow();
    }

    // 8. Radix Sort (Name) - O(d*n)
    void RadixSortNames(vector<File>& arr) {
        if (arr.empty()) return;
        
        // Get max length
        int maxLen = 0;
        for (auto& f : arr) {
            int len = f.GetName().length();
            if (len > maxLen) maxLen = len;
        }
        
        // Sort by each character position (right to left)
        for (int pos = maxLen - 1; pos >= 0; pos--) {
            vector<File> output(arr.size());
            vector<int> count(256, 0);  // ASCII characters
            
            // Count occurrences
            for (auto& f : arr) {
                char ch = (pos < f.GetName().length()) ? tolower(f.GetName()[pos]) : 0;
                count[ch]++;
            }
            
            // Cumulative count
            for (int i = 1; i < 256; i++) count[i] += count[i - 1];
            
            // Build output
            for (int i = arr.size() - 1; i >= 0; i--) {
                char ch = (pos < arr[i].GetName().length()) ? tolower(arr[i].GetName()[pos]) : 0;
                output[count[ch] - 1] = arr[i];
                count[ch]--;
            }
            
            arr = output;
        }
    }

    void SortRadixName() {
        vector<File> v = GetFilesVector();
        if (!v.empty()) RadixSortNames(v);
        cout << "\n [SORTED BY NAME (RADIX SORT - O(d*n))]\n";
        for(auto& f : v) f.DisplayRow();
    }
};

class Folder {
private:
    string name;
    string owner;
    int id;
    int fileIDCounter;
    
    HashTableFiles files;
    FileStack deletedFiles;
    FileQueue recentFiles;
    FileMaxHeap starredFiles;
    FileDoubleLinkedList fileNavigator;  // For file browsing

public:
    Folder() : id(0), fileIDCounter(1) {}
    
    // Copy constructor - CRITICAL: Ensures all members are properly copied
    Folder(const Folder& other) 
        : name(other.name), owner(other.owner), id(other.id), 
          fileIDCounter(other.fileIDCounter),
          files(other.files),  // HashTableFiles now has proper copy semantics
          deletedFiles(other.deletedFiles),  // FileStack uses array, safe
          recentFiles(other.recentFiles),  // FileQueue uses array, safe
          starredFiles(other.starredFiles),  // FileMaxHeap uses vector, safe
          fileNavigator(other.fileNavigator)  // FileDoubleLinkedList now has copy semantics
    {}
    
    // Copy assignment operator - CRITICAL: Ensures safe assignment
    Folder& operator=(const Folder& other) {
        if (this != &other) {
            name = other.name;
            owner = other.owner;
            id = other.id;
            fileIDCounter = other.fileIDCounter;
            files = other.files;
            deletedFiles = other.deletedFiles;
            recentFiles = other.recentFiles;
            starredFiles = other.starredFiles;
            fileNavigator = other.fileNavigator;
        }
        return *this;
    }

    void SetValues(const string &n, int i, const string &own) {
        name = n; id = i; owner = own;
    }
    
    string GetName() const { return name; }
    int GetID() const { return id; }
    int GetFileIDCounter() const { return fileIDCounter; }

    void CreateFile() {
        PrintHeader("CREATE NEW FILE");
        string fname = InputString(" Enter file name: ");
        string type = InputString(" Enter file type (txt/doc/cpp): ");
        string content = InputString(" Enter content: ");
        int prio = InputInt(" Enter Priority (1-10): ", 1, 10);
        
        File f;
        f.SetValues(fileIDCounter, fname, type, owner, content, prio);
        files.Insert(f);
        
        if (prio >= 8) starredFiles.Insert(f); // Auto-star high priority

        cout << " [SUCCESS] File '" << fname << "' created (ID: " << fileIDCounter << ").\n";
        sysLog.Log("FileCreated", "File " + fname + " created in " + name);
        fileIDCounter++;
    }

    void InsertSharedFile(File f) {
        f.SetID(fileIDCounter++);
        files.Insert(f);
        if(f.GetPriority() >= 8) starredFiles.Insert(f);
    }

    void SearchFile() {
        int searchId = InputInt(" Enter File ID to search: ");
        File* f = files.Search(searchId);
        if (f) {
            f->DisplayDetailed();
            recentFiles.Enqueue(*f);
            sysLog.Log("FileAccessed", "Viewed file ID " + to_string(searchId));
        } else {
            cout << " [ERROR] File not found.\n";
        }
    }

    void DeleteFile() {
        int delId = InputInt(" Enter File ID to delete: ");
        File f = files.Delete(delId);
        if (f.GetID() > 0) {
            deletedFiles.Push(f);
            cout << " [SUCCESS] File moved to Trash.\n";
            sysLog.Log("FileDeleted", "Deleted file ID " + to_string(delId));
        } else {
            cout << " [ERROR] File not found.\n";
        }
    }

    void RecoverFile() {
        if(deletedFiles.IsEmpty()) {
            cout << " [INFO] Trash is empty.\n";
            return;
        }
        File f = deletedFiles.Pop();
        files.Insert(f);
        cout << " [SUCCESS] Restored '" << f.GetName() << "'.\n";
        sysLog.Log("FileRestored", "Restored file " + f.GetName());
    }

    void BrowseFiles() {
        vector<File> fileList = files.GetFilesVector();
        if (fileList.empty()) {
            cout << " [INFO] No files to browse.\n";
            return;
        }
        
        fileNavigator.BuildFromVector(fileList);
        fileNavigator.Reset();
        
        while (true) {
            ClearScreen();
            PrintHeader("BROWSE FILES (Double Linked List)");
            File* current = fileNavigator.GetCurrent();
            if (current) {
                current->DisplayDetailed();
                cout << "\n Navigation:\n";
                cout << " [N] Next File  [P] Previous File  [Q] Quit\n";
                cout << " [V] View Version History\n";
                PrintLine();
                
                char choice;
                cout << " Enter choice: ";
                cin >> choice;
                cin.ignore();
                choice = tolower(choice);
                
                switch(choice) {
                    case 'n':
                        if (fileNavigator.GetNext()) {
                            cout << " Moved to next file.\n";
                        } else {
                            cout << " [INFO] Already at last file.\n";
                        }
                        break;
                    case 'p':
                        if (fileNavigator.GetPrev()) {
                            cout << " Moved to previous file.\n";
                        } else {
                            cout << " [INFO] Already at first file.\n";
                        }
                        break;
                    case 'v': {
                        File* f = fileNavigator.GetCurrent();
                        if (f) {
                            PrintHeader("VERSION HISTORY (Linked List)");
                            f->DisplayVersionHistory();
                        }
                        break;
                    }
                    case 'q':
                        return;
                    default:
                        cout << " Invalid choice.\n";
                }
                cout << "\n (Press Enter to continue...)";
                cin.get();
            } else {
                cout << " [ERROR] No files available.\n";
                return;
            }
        }
    }

    void ViewFileVersions() {
        int fileId = InputInt(" Enter File ID to view versions: ");
        File* f = files.Search(fileId);
        if (f) {
            PrintHeader("VERSION HISTORY (Linked List)");
            f->DisplayVersionHistory();
            cout << "\n Total versions: " << f->GetVersionCount() << endl;
        } else {
            cout << " [ERROR] File not found.\n";
        }
    }

    void ShowMenu() {
        while(true) {
            PrintHeader("FOLDER: " + name);
            cout << " 1. Create File\n";
            cout << " 2. List All Files\n";
            cout << " 3. Search File\n";
            cout << " 4. Delete File\n";
            cout << " 5. Trash Bin (Undo)\n";
            cout << " 6. Recover Last Deleted\n";
            cout << " 7. Recent Files\n";
            cout << " 8. Starred/Priority Files\n";
            cout << " 9. Browse Files (Double Linked List)\n";
            cout << " 10. View File Version History (Linked List)\n";
            cout << " --- SORTING ALGORITHMS ---\n";
            cout << " 11. Sort: By Size (Bubble Sort)\n";
            cout << " 12. Sort: By Size (Insertion Sort)\n";
            cout << " 13. Sort: By Size (Selection Sort)\n";
            cout << " 14. Sort: By Size (Merge Sort)\n";
            cout << " 15. Sort: By Size (Heap Sort)\n";
            cout << " 16. Sort: By Size (Counting Sort)\n";
            cout << " 17. Sort: By Name (Quick Sort)\n";
            cout << " 18. Sort: By Name (Radix Sort)\n";
            cout << " 19. Back to Drive\n";
            PrintLine();
            
            int ch = InputInt(" Select Action: ", 1, 19);
            
            switch(ch) {
                case 1: CreateFile(); break;
                case 2: files.DisplayAll(); break;
                case 3: SearchFile(); break;
                case 4: DeleteFile(); break;
                case 5: deletedFiles.Display(); break;
                case 6: RecoverFile(); break;
                case 7: recentFiles.Display(); break;
                case 8: starredFiles.DisplayTop(); break;
                case 9: BrowseFiles(); break;
                case 10: ViewFileVersions(); break;
                case 11: files.SortBubbleSize(); break;
                case 12: files.SortInsertionSize(); break;
                case 13: files.SortSelectionSize(); break;
                case 14: files.SortMergeSize(); break;
                case 15: files.SortHeapSize(); break;
                case 16: files.SortCountingSize(); break;
                case 17: files.SortQuickName(); break;
                case 18: files.SortRadixName(); break;
                case 19: return;
            }
            
            cout << "\n (Press Enter to continue...)";
            cin.get(); 
        }
    }

    File* GetFileById(int fid) { return files.Search(fid); }
};

class TreeNode {
public:
    Folder data;
    TreeNode* left;
    TreeNode* right;
    int height;
    TreeNode(Folder f) : data(f), left(nullptr), right(nullptr), height(1) {}
};

class AVLTreeFolders {
private:
    TreeNode* root;

    int Height(TreeNode* n) { return n ? n->height : 0; }
    int BalanceFactor(TreeNode* n) { return Height(n->left) - Height(n->right); }

    TreeNode* RotateRight(TreeNode* y) {
        if (!y || !y->left) return y;  // Safety check
        TreeNode* x = y->left;
        TreeNode* T2 = x->right;
        x->right = y; 
        y->left = T2;
        y->height = max(Height(y->left), Height(y->right)) + 1;
        x->height = max(Height(x->left), Height(x->right)) + 1;
        return x;
    }

    TreeNode* RotateLeft(TreeNode* x) {
        if (!x || !x->right) return x;  // Safety check
        TreeNode* y = x->right;
        TreeNode* T2 = y->left;
        y->left = x; 
        x->right = T2;
        x->height = max(Height(x->left), Height(x->right)) + 1;
        y->height = max(Height(y->left), Height(y->right)) + 1;
        return y;
    }

    TreeNode* Insert(TreeNode* node, Folder f) {
        if (!node) return new TreeNode(f);

        if (f.GetID() < node->data.GetID())
            node->left = Insert(node->left, f);
        else if (f.GetID() > node->data.GetID())
            node->right = Insert(node->right, f);
        else return node; // No duplicates

        node->height = 1 + max(Height(node->left), Height(node->right));
        int balance = BalanceFactor(node);

        // Left Left
        if (balance > 1 && node->left && f.GetID() < node->left->data.GetID())
            return RotateRight(node);
        // Right Right
        if (balance < -1 && node->right && f.GetID() > node->data.GetID())
            return RotateLeft(node);
        // Left Right
        if (balance > 1 && node->left && f.GetID() > node->left->data.GetID()) {
            node->left = RotateLeft(node->left);
            return RotateRight(node);
        }
        // Right Left
        if (balance < -1 && node->right && f.GetID() < node->right->data.GetID()) {
            node->right = RotateRight(node->right);
            return RotateLeft(node);
        }
        return node;
    }

    void InOrderDisplay(TreeNode* root) {
        if(root) {
            InOrderDisplay(root->left);
            cout << " [ID: " << setw(3) << root->data.GetID() << "] " << root->data.GetName() << "\n";
            InOrderDisplay(root->right);
        }
    }

    TreeNode* Search(TreeNode* root, int id) {
        if (!root || root->data.GetID() == id) return root;
        if (id < root->data.GetID()) return Search(root->left, id);
        return Search(root->right, id);
    }

private:
    // Helper function to delete all nodes recursively
    void DeleteTree(TreeNode* node) {
        if (node) {
            DeleteTree(node->left);
            DeleteTree(node->right);
            delete node;
        }
    }

public:
    AVLTreeFolders() : root(nullptr) {}
    
    // Destructor - CRITICAL: Prevents memory leaks
    ~AVLTreeFolders() {
        DeleteTree(root);
        root = nullptr;
    }
    
    // Copy constructor - CRITICAL: Prevents shallow copy issues
    AVLTreeFolders(const AVLTreeFolders& other) : root(nullptr) {
        if (other.root) {
            root = CopyTree(other.root);
        }
    }
    
    // Copy assignment operator
    AVLTreeFolders& operator=(const AVLTreeFolders& other) {
        if (this != &other) {
            DeleteTree(root);
            root = nullptr;
            if (other.root) {
                root = CopyTree(other.root);
            }
        }
        return *this;
    }
    
    // Helper to copy tree recursively - creates new nodes
    TreeNode* CopyTree(TreeNode* source) {
        if (!source) return nullptr;
        TreeNode* newNode = new TreeNode(source->data);  // Copy folder data
        newNode->left = CopyTree(source->left);
        newNode->right = CopyTree(source->right);
        newNode->height = source->height;
        return newNode;
    }

    void AddFolder(Folder f) {
        if (root) {
            // Check for duplicate ID before inserting
            TreeNode* existing = Search(root, f.GetID());
            if (existing) {
                cout << " [WARNING] Folder with ID " << f.GetID() << " already exists. Skipping.\n";
                return;
            }
        }
        root = Insert(root, f);
    }

    Folder* GetFolder(int id) {
        if (!root) return nullptr;  // Safety check
        TreeNode* res = Search(root, id);
        return res ? &res->data : nullptr;
    }

    void DisplayAll() {
        if(!root) {
            cout << " No folders yet.\n";
        } else {
            cout << " --- FOLDER LIST (In-Order) ---\n";
            InOrderDisplay(root);
        }
    }
};

const int ALPHABET_SIZE = 26;

struct TrieNode {
    TrieNode *children[ALPHABET_SIZE];
    bool isEndOfWord;
    
    TrieNode() {
        isEndOfWord = false;
        for (int i = 0; i < ALPHABET_SIZE; i++)
            children[i] = nullptr;
    }
};

class TrieUsers {
private:
    TrieNode *root;

public:
    TrieUsers() { root = new TrieNode(); }

    void Insert(string key) {
        TrieNode *pCrawl = root;
        for (int i = 0; i < key.length(); i++) {
            int index = tolower(key[i]) - 'a';
            if (index < 0 || index >= 26) continue; // Skip non-alpha for simplicity
            if (!pCrawl->children[index])
                pCrawl->children[index] = new TrieNode();
            pCrawl = pCrawl->children[index];
        }
        pCrawl->isEndOfWord = true;
    }

    bool Search(string key) {
        TrieNode *pCrawl = root;
        for (int i = 0; i < key.length(); i++) {
            int index = tolower(key[i]) - 'a';
            if (index < 0 || index >= 26) return false;
            if (!pCrawl->children[index])
                return false;
            pCrawl = pCrawl->children[index];
        }
        return (pCrawl != nullptr && pCrawl->isEndOfWord);
    }
    
    // Helper for autocomplete suggestion
    void SuggestHelper(TrieNode* root, string currPrefix) {
        if (!root) return;  // Safety check
        if (root->isEndOfWord) {
            cout << " - " << currPrefix << endl;
        }
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            if (root->children[i]) {
                char child = 'a' + i;
                SuggestHelper(root->children[i], currPrefix + child);
            }
        }
    }

    void AutoComplete(string prefix) {
        TrieNode *pCrawl = root;
        for (int i = 0; i < prefix.length(); i++) {
            int index = tolower(prefix[i]) - 'a';
            if (!pCrawl->children[index]) {
                cout << " No users found with prefix '" << prefix << "'.\n";
                return;
            }
            pCrawl = pCrawl->children[index];
        }
        cout << " Suggestions for '" << prefix << "':\n";
        SuggestHelper(pCrawl, prefix);
    }
};
class User {
private:
    string username;
    string password;
    string securityQ;
    string securityA;
    AVLTreeFolders myFolders;
    int folderCounter;
    CircularNotificationQueue notifications;  // Changed from vector to Circular Linked List

public:
    User() : folderCounter(1) {}

    void Setup(string u, string p, string sq, string sa) {
        username = u;
        password = p;
        securityQ = sq;
        securityA = sa;
    }

    bool CheckPassword(string p) { return password == p; }
    string GetName() const { return username; }
    string GetSecQ() const { return securityQ; }
    bool CheckSecA(string a) { return securityA == a; }
    void SetPassword(string p) { password = p; }

    void CreateFolder() {
        string fname = InputString(" Enter new folder name: ");
        if (fname.empty()) {
            cout << " [ERROR] Folder name cannot be empty.\n";
            return;
        }
        
        Folder f;
        f.SetValues(fname, folderCounter, username);
        myFolders.AddFolder(f);
        cout << " [SUCCESS] Folder '" << fname << "' created (ID: " << folderCounter << ").\n";
        sysLog.Log("FolderCreate", username + " created folder " + fname);
        folderCounter++;
    }

    void OpenFolder() {
        myFolders.DisplayAll();
        int fid = InputInt(" Enter Folder ID to open: ");
        Folder* f = myFolders.GetFolder(fid);
        if(f) {
            f->ShowMenu();
        } else {
            cout << " [ERROR] Invalid Folder ID.\n";
        }
    }

    void AddNotification(string msg) {
        notifications.AddNotification("[" + CurrentTimestamp() + "] " + msg);
    }

    void ShowNotifications() {
        PrintHeader("NOTIFICATIONS");
        notifications.DisplayAll();
        PrintLine();
    }
    
    // Needed for sharing
    Folder* GetFolder(int id) { return myFolders.GetFolder(id); }
    AVLTreeFolders* GetFolderTree() { return &myFolders; }
};

class UserGraph {
private:
    vector<User*> users;
    vector<vector<int>> adj; // Adjacency Matrix
    TrieUsers userTrie; // For fast search

    int GetUserIndex(string name) {
        for(size_t i=0; i<users.size(); i++) {
            if(users[i]->GetName() == name) return i;
        }
        return -1;
    }

public:
    UserGraph() {}

    void RegisterUser() {
        PrintHeader("NEW USER REGISTRATION");
        string u = InputString(" Choose Username: ");
        if(userTrie.Search(u)) {
            cout << " [ERROR] Username taken.\n";
            return;
        }

        string p = InputString(" Choose Password: ");
        string sq = InputString(" Security Question: ");
        string sa = InputString(" Security Answer: ");

        User* newUser = new User();
        newUser->Setup(u, p, sq, sa);
        
        users.push_back(newUser);
        // Expand Adjacency Matrix
        for(auto& row : adj) row.push_back(0); // Add col to existing rows
        adj.push_back(vector<int>(users.size(), 0)); // Add new row
        
        userTrie.Insert(u);
        sysLog.Log("UserRegister", "New user registered: " + u);
        cout << " [SUCCESS] User registered!\n";
    }

    User* Login() {
        PrintHeader("LOGIN");
        string u = InputString(" Username: ");
        int idx = GetUserIndex(u);
        if(idx == -1) {
            cout << " [ERROR] User not found.\n";
            return nullptr;
        }
        
        string p = InputString(" Password: ");
        if(users[idx]->CheckPassword(p)) {
            sysLog.Log("Login", "User " + u + " logged in.");
            return users[idx];
        } else {
            cout << " [ERROR] Incorrect password.\n";
            return nullptr;
        }
    }
    
    void RecoverAccount() {
        string u = InputString(" Enter Username to recover: ");
        int idx = GetUserIndex(u);
        if(idx == -1) { cout << " User not found.\n"; return; }
        
        cout << " Security Question: " << users[idx]->GetSecQ() << endl;
        string ans = InputString(" Answer: ");
        if(users[idx]->CheckSecA(ans)) {
            string newP = InputString(" Enter New Password: ");
            users[idx]->SetPassword(newP);
            cout << " [SUCCESS] Password reset.\n";
            sysLog.Log("PassReset", "Password reset for " + u);
        } else {
            cout << " [ERROR] Wrong answer.\n";
        }
    }

    // --- FRIENDSHIP & GRAPH ALGORITHMS ---

    void AddFriend(User* currentUser) {
        cout << " Find Friend (Autocomplete):\n";
        string prefix = Trim(InputString(" Enter prefix to search: "));
        userTrie.AutoComplete(prefix);

        string target = Trim(InputString(" Enter exact username to add: "));

        int u1 = GetUserIndex(currentUser->GetName());
        int u2 = GetUserIndex(target);

        if(u2 == -1 || u1 == u2) {
            cout << " [ERROR] Invalid user.\n";
            return;
        }

        if(adj[u1][u2] == 1) {
            cout << " [INFO] Already friends.\n";
            return;
        }

        adj[u1][u2] = 1;
        adj[u2][u1] = 1;

        cout << " [SUCCESS] You are now friends with " << users[u2]->GetName() << "!\n";
}

    // BFS Algorithm to find "Friend of a Friend"
    void RecommendFriends(User* currentUser) {
        int startNode = GetUserIndex(currentUser->GetName());
        if(startNode == -1) return;

        bool visited[100] = {false};
        queue<int> q;
        
        visited[startNode] = true;
        q.push(startNode);
        
        cout << "\n --- FRIEND RECOMMENDATIONS (BFS) ---\n";
        bool found = false;

        while(!q.empty()) {
            int u = q.front();
            q.pop();

            for(size_t v=0; v<users.size(); v++) {
                if(adj[u][v] == 1 && !visited[v]) {
                    visited[v] = true;
                    q.push(v);
                    
                    // If v is not directly connected to startNode, suggest it
                    if(adj[startNode][v] == 0 && v != startNode) {
                        cout << " Suggestion: " << users[v]->GetName() << " (Friend of " << users[u]->GetName() << ")\n";
                        found = true;
                    }
                }
            }
        }
        if(!found) cout << " No suggestions available.\n";
    }

    // DFS Algorithm - Depth First Search
    void DFSHelper(int node, bool visited[], vector<int>& component) {
        visited[node] = true;
        component.push_back(node);
        
        for (size_t i = 0; i < users.size(); i++) {
            if (adj[node][i] == 1 && !visited[i]) {
                DFSHelper(i, visited, component);
            }
        }
    }

    void FindConnectedComponents(User* currentUser) {
        int startNode = GetUserIndex(currentUser->GetName());
        if (startNode == -1) return;
        
        bool visited[100] = {false};
        vector<int> component;
        
        DFSHelper(startNode, visited, component);
        
        cout << "\n --- CONNECTED USERS (DFS - Depth First Search) ---\n";
        cout << " Your connected network includes:\n";
        bool found = false;
        for (int idx : component) {
            if (idx != startNode) {
                cout << " - " << users[idx]->GetName() << endl;
                found = true;
            }
        }
        if (!found) cout << " No connected users found.\n";
    }

    void FindAllPathsHelper(int start, int end, bool visited[], vector<int>& path, vector<vector<int>>& allPaths) {
        visited[start] = true;
        path.push_back(start);
        
        if (start == end) {
            allPaths.push_back(path);
        } else {
            for (size_t i = 0; i < users.size(); i++) {
                if (adj[start][i] == 1 && !visited[i]) {
                    FindAllPathsHelper(i, end, visited, path, allPaths);
                }
            }
        }
        
        path.pop_back();
        visited[start] = false;
    }

    void FindPathBetweenUsers(User* currentUser) {
        string targetName = InputString(" Enter target username: ");
        int startNode = GetUserIndex(currentUser->GetName());
        int endNode = GetUserIndex(targetName);
        
        if (startNode == -1 || endNode == -1) {
            cout << " [ERROR] Invalid user(s).\n";
            return;
        }
        
        if (startNode == endNode) {
            cout << " [INFO] Same user selected.\n";
            return;
        }
        
        bool visited[100] = {false};
        vector<int> path;
        vector<vector<int>> allPaths;
        
        FindAllPathsHelper(startNode, endNode, visited, path, allPaths);
        
        cout << "\n --- PATHS BETWEEN USERS (DFS) ---\n";
        if (allPaths.empty()) {
            cout << " No path found between " << currentUser->GetName() << " and " << targetName << ".\n";
        } else {
            cout << " Found " << allPaths.size() << " path(s):\n";
            for (size_t i = 0; i < allPaths.size(); i++) {
                cout << " Path " << (i+1) << ": ";
                for (size_t j = 0; j < allPaths[i].size(); j++) {
                    cout << users[allPaths[i][j]]->GetName();
                    if (j < allPaths[i].size() - 1) cout << " -> ";
                }
                cout << endl;
            }
        }
    }

    // --- FILE SHARING LOGIC ---

    void ShareFile(User* sender) {
        string targetName = InputString(" Enter username to share with: ");
        int receiverIdx = GetUserIndex(targetName);
        
        if(receiverIdx == -1) { cout << " User not found.\n"; return; }
        
        User* receiver = users[receiverIdx];
        
        // Select File
        sender->GetFolderTree()->DisplayAll();
        int folderID = InputInt(" Select source Folder ID: ");
        Folder* srcFolder = sender->GetFolder(folderID);
        
        if(!srcFolder) { cout << " Invalid folder.\n"; return; }
        
        // We need a way to list files - for now, just ask for file ID
        // In a real implementation, we'd have a method to list files
        cout << "\n Note: You'll need to know the File ID from the folder.\n";
        int fileID = InputInt(" Enter File ID to share: ");
        
        File* file = srcFolder->GetFileById(fileID);
        if(!file) { cout << " File not found.\n"; return; }
        
        // Create or find "Shared with Me" folder
        // Use a special high ID (9999) for shared folder to avoid conflicts
        Folder* sharedFolder = receiver->GetFolder(9999);
        if (!sharedFolder) {
            // Create shared folder
            Folder newShared;
            newShared.SetValues("Shared with Me", 9999, receiver->GetName());
            receiver->GetFolderTree()->AddFolder(newShared);
            sharedFolder = receiver->GetFolder(9999);
        }
        
        // Actually copy the file
        File sharedFile = *file;  // Copy file
        // Get a new ID from the shared folder's counter
        int newFileID = sharedFolder->GetFileIDCounter();
        sharedFile.SetID(newFileID);
        sharedFolder->InsertSharedFile(sharedFile);
        
        receiver->AddNotification("User " + sender->GetName() + " shared file: " + file->GetName());
        
        cout << " [SUCCESS] File '" << file->GetName() << "' shared and copied to " << receiver->GetName() << "'s 'Shared with Me' folder.\n";
        sysLog.Log("Share", sender->GetName() + " shared " + file->GetName() + " with " + targetName);
    }
};

class GoogleDriveSystem {
private:
    UserGraph network;
    User* currentUser;

public:
    GoogleDriveSystem() : currentUser(nullptr) {}

    void UserDashboard() {
        while (currentUser) {
            ClearScreen();
            PrintHeader("DASHBOARD: " + currentUser->GetName());
            cout << " 1. Create New Folder\n";
            cout << " 2. Open Folder\n";
            cout << " 3. Notifications\n";
            cout << " 4. Add Friend (Search)\n";
            cout << " 5. Friend Recommendations (BFS)\n";
            cout << " 6. Find Connected Users (DFS)\n";
            cout << " 7. Find Path Between Users (DFS)\n";
            cout << " 8. Share File\n";
            cout << " 9. System Logs (Admin)\n";
            cout << " 10. Logout\n";
            PrintLine();

            int choice = InputInt(" Select Action: ", 1, 10);

            switch (choice) {
                case 1: currentUser->CreateFolder(); break;
                case 2: currentUser->OpenFolder(); break;
                case 3: currentUser->ShowNotifications(); break;
                case 4: network.AddFriend(currentUser); break;
                case 5: network.RecommendFriends(currentUser); break;
                case 6: network.FindConnectedComponents(currentUser); break;
                case 7: network.FindPathBetweenUsers(currentUser); break;
                case 8: network.ShareFile(currentUser); break;
                case 9: sysLog.DisplayLogs(); break;
                case 10: 
                    currentUser = nullptr; 
                    cout << " Logging out...\n";
                    return;
            }
            if(choice != 10) {
                cout << "\n (Press Enter to continue...)";
                cin.get();
            }
        }
    }

    void Run() {
        while (true) {
            ClearScreen();
            cout << R"(
   _____                   _        _____       _           
  / ____|                 | |      |  __ \     (_)          
 | |  __  ___   ___   __ _| | ___  | |  | |_ __ ___   _____ 
 | | |_ |/ _ \ / _ \ / _` | |/ _ \ | |  | | '__| \ \ / / _ \
 | |__| | (_) | (_) | (_| | |  __/ | |__| | |  | |\ V /  __/
  \_____|\___/ \___/ \__, |_|\___| |_____/|_|  |_| \_/ \___|
                      __/ |                                 
                     |___/    Ultimate Edition v2.0
            )" << endl;
            PrintLine('=');
            cout << " 1. Login\n";
            cout << " 2. Register New User\n";
            cout << " 3. Forgot Password\n";
            cout << " 4. About & Credits\n";
            cout << " 5. Exit\n";
            PrintLine('=');
            
            int choice = InputInt(" Select Option: ", 1, 5);

            switch (choice) {
                case 1:
                    currentUser = network.Login();
                    if (currentUser) UserDashboard();
                    break;
                case 2:
                    network.RegisterUser();
                    break;
                case 3:
                    network.RecoverAccount();
                    break;
                case 4:
                    PrintHeader("CREDITS");
                    cout << " Developed for: Data Structures and Alogorithm Final Semester Project\n";
                    cout << " Features: AVL Trees, Hash Tables, Graphs, Tries, Heaps.\n";
                    cout << " Submitted by:\n -M.Zawar Fahim \n -Hamza Sami \n -M.Asad Akmal";
                    cout << "\n Press Enter to return.";
                    cin.get();
                    break;
                case 5:
                    cout << " Shutting down system. Goodbye!\n";
                    exit(0);
            }
            if (choice != 1) { // Login handles its own pause
                cout << "\n (Press Enter to continue...)";
                cin.get();
            }
        }
    }
};

int main(){
    srand(time(0));
    GoogleDriveSystem app;
    app.Run();
    return 0;
}