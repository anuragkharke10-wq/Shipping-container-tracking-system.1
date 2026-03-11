#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <limits>
#include <cctype>
using namespace std;

// ============ Class: Container ============
class Container {
private:
    string containerID, origin, destination, currentLocation, status;
    double weight;

public:
    Container() : weight(0.0) {}

    Container(string id, string orig, string dest, string loc, double wt, string stat)
        : containerID(id), origin(orig), destination(dest),
          currentLocation(loc), weight(wt), status(stat) {}

    string getContainerID() const { return containerID; }
    string getStatus() const { return status; }
    string getCurrentLocation() const { return currentLocation; }

    void setCurrentLocation(string loc) { currentLocation = loc; }
    void setStatus(string stat) { status = stat; }

    void display() const {
        cout << "\n========================================" << endl;
        cout << " Container ID      : " << containerID << endl;
        cout << " Origin            : " << origin << endl;
        cout << " Destination       : " << destination << endl;
        cout << " Current Location  : " << currentLocation << endl;
        cout << " Weight (tonnes)   : " << fixed << setprecision(1) << weight << endl;
        cout << " Status            : " << status << endl;
        cout << "========================================" << endl;
    }

    void display(bool summary) const {
        if (summary) {
            cout << left
                 << setw(12) << containerID
                 << setw(15) << origin
                 << setw(15) << destination
                 << setw(18) << currentLocation
                 << setw(10) << fixed << setprecision(1) << weight
                 << setw(12) << status << endl;
        } else {
            display();
        }
    }

    string toFileString() const {
        return containerID + "|" + origin + "|" + destination + "|" +
               currentLocation + "|" + to_string(weight) + "|" + status;
    }

    void fromFileString(const string& line) {
        stringstream ss(line);
        string wStr;

        getline(ss, containerID, '|');
        getline(ss, origin, '|');
        getline(ss, destination, '|');
        getline(ss, currentLocation, '|');
        getline(ss, wStr, '|');
        weight = stod(wStr);
        getline(ss, status, '|');
    }
};

// ============ Globals ============
vector<Container> containers;
const string FILE_NAME = "containers.txt";

// ============ File Handling ============
void loadFromFile() {
    containers.clear();

    ifstream file(FILE_NAME);
    if (!file.is_open()) {
        cout << "No existing data file found. Starting fresh." << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        if (!line.empty()) {
            Container c;
            c.fromFileString(line);
            containers.push_back(c);
        }
    }

    file.close();
    cout << containers.size() << " container(s) loaded from file." << endl;
}

void saveToFile() {
    ofstream file(FILE_NAME);
    for (const auto& c : containers) {
        file << c.toFileString() << endl;
    }
    file.close();
}

// ============ Input Validation ============
bool isValidID(const string& id) {
    if (id.length() < 5 || id.substr(0, 4) != "CTR-")
        return false;

    for (size_t i = 4; i < id.length(); i++) {
        if (!isdigit(static_cast<unsigned char>(id[i])))
            return false;
    }

    return true;
}

bool isDuplicateID(const string& id) {
    for (const auto& c : containers) {
        if (c.getContainerID() == id)
            return true;
    }
    return false;
}

string getValidStatus() {
    int ch;

    while (true) {
        cout << "\nSelect Status:" << endl;
        cout << " 1. In Transit" << endl;
        cout << " 2. Delivered" << endl;
        cout << " 3. At Port" << endl;
        cout << "Enter choice (1-3): ";
        cin >> ch;

        if (!cin.fail() && ch >= 1 && ch <= 3) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (ch == 1) return "In Transit";
            if (ch == 2) return "Delivered";
            return "At Port";
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid choice! Enter 1, 2, or 3." << endl;
    }
}

// ============ Core Functions ============
void registerContainer() {
    string id, orig, dest, loc;
    double wt;

    cout << "\n--- Register New Container ---" << endl;

    while (true) {
        cout << "Enter Container ID (e.g., CTR-001): ";
        getline(cin, id);

        if (!isValidID(id)) {
            cout << "Invalid ID format!" << endl;
            continue;
        }

        if (isDuplicateID(id)) {
            cout << "ID already exists!" << endl;
            continue;
        }

        break;
    }

    cout << "Enter Origin Port: ";
    getline(cin, orig);

    cout << "Enter Destination Port: ";
    getline(cin, dest);

    cout << "Enter Current Location: ";
    getline(cin, loc);

    while (true) {
        cout << "Enter Weight (tonnes): ";
        cin >> wt;

        if (!cin.fail() && wt > 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }

        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid weight! Enter a positive number." << endl;
    }

    string status = getValidStatus();
    containers.push_back(Container(id, orig, dest, loc, wt, status));
    saveToFile();

    cout << "\nContainer " << id << " registered successfully!" << endl;
}

void updateLocation() {
    string id, newLoc;

    cout << "\n--- Update Container Location ---" << endl;
    cout << "Enter Container ID: ";
    getline(cin, id);

    for (auto& c : containers) {
        if (c.getContainerID() == id) {
            cout << "Current Location: " << c.getCurrentLocation() << endl;
            cout << "Enter New Location: ";
            getline(cin, newLoc);

            c.setCurrentLocation(newLoc);
            c.setStatus(getValidStatus());
            saveToFile();

            cout << "\nLocation updated successfully!" << endl;
            return;
        }
    }

    cout << "Error: Container " << id << " not found!" << endl;
}

void searchContainer() {
    string id;

    cout << "\n--- Search Container ---" << endl;
    cout << "Enter Container ID: ";
    getline(cin, id);

    for (const auto& c : containers) {
        if (c.getContainerID() == id) {
            c.display();
            return;
        }
    }

    cout << "Container " << id << " not found!" << endl;
}

void displayAll() {
    if (containers.empty()) {
        cout << "\nNo containers registered." << endl;
        return;
    }

    cout << "\n==================== All Containers ====================" << endl;
    cout << left
         << setw(12) << "ID"
         << setw(15) << "Origin"
         << setw(15) << "Destination"
         << setw(18) << "Location"
         << setw(10) << "Weight"
         << setw(12) << "Status" << endl;

    cout << string(82, '-') << endl;

    for (const auto& c : containers) {
        c.display(true);
    }

    cout << "\nTotal Containers: " << containers.size() << endl;
}

void statusReport() {
    if (containers.empty()) {
        cout << "\nNo containers registered." << endl;
        return;
    }

    int inTransit = 0, delivered = 0, atPort = 0;

    for (const auto& c : containers) {
        if (c.getStatus() == "In Transit")
            inTransit++;
        else if (c.getStatus() == "Delivered")
            delivered++;
        else if (c.getStatus() == "At Port")
            atPort++;
    }

    int total = containers.size();

    cout << "\n============= Status Report =============" << endl;
    cout << "Total Containers : " << total << endl;
    cout << "-----------------------------------------" << endl;
    cout << "In Transit : " << inTransit << " ("
         << fixed << setprecision(1) << (inTransit * 100.0 / total) << "%)" << endl;
    cout << "Delivered  : " << delivered << " ("
         << fixed << setprecision(1) << (delivered * 100.0 / total) << "%)" << endl;
    cout << "At Port    : " << atPort << " ("
         << fixed << setprecision(1) << (atPort * 100.0 / total) << "%)" << endl;
    cout << "=========================================" << endl;
}

void deleteContainer() {
    string id;
    char confirm;

    cout << "\n--- Delete Container ---" << endl;
    cout << "Enter Container ID to delete: ";
    getline(cin, id);

    for (auto it = containers.begin(); it != containers.end(); ++it) {
        if (it->getContainerID() == id) {
            it->display();

            cout << "Are you sure you want to delete? (y/n): ";
            cin >> confirm;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (confirm == 'y' || confirm == 'Y') {
                containers.erase(it);
                saveToFile();
                cout << "Container " << id << " deleted!" << endl;
            } else {
                cout << "Deletion cancelled." << endl;
            }
            return;
        }
    }

    cout << "Error: Container " << id << " not found!" << endl;
}

// ============ Menu & Main ============
void displayMenu() {
    cout << "\n*********************************************" << endl;
    cout << "* SHIPPING CONTAINER TRACKING SYSTEM        *" << endl;
    cout << "*********************************************" << endl;
    cout << "* 1. Register New Container                 *" << endl;
    cout << "* 2. Update Container Location              *" << endl;
    cout << "* 3. Search Container                       *" << endl;
    cout << "* 4. Display All Containers                 *" << endl;
    cout << "* 5. Generate Status Report                 *" << endl;
    cout << "* 6. Delete Container                       *" << endl;
    cout << "* 7. Exit                                   *" << endl;
    cout << "*********************************************" << endl;
    cout << "Enter your choice (1-7): ";
}

int main() {
    cout << "\n=== Shipping Container Tracking System ===" << endl;
    loadFromFile();

    int choice;

    while (true) {
        displayMenu();
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input! Please enter a number." << endl;
            continue;
        }

        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                registerContainer();
                break;
            case 2:
                updateLocation();
                break;
            case 3:
                searchContainer();
                break;
            case 4:
                displayAll();
                break;
            case 5:
                statusReport();
                break;
            case 6:
                deleteContainer();
                break;
            case 7:
                saveToFile();
                cout << "\nData saved. Goodbye!" << endl;
                return 0;
            default:
                cout << "Invalid choice! Select 1-7." << endl;
        }
    }

    return 0;
}