
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
using namespace std;

// 1. Waste class
class Waste {
public:
    string category;
    double quantity;

    Waste() {
        category = "Dry";
        quantity = 0;
    }

    Waste(string c, double q) {
        category = c;
        quantity = q;
    }
};

// 2. Abstract User class
class User {
protected:
    string name, location;

public:
    User(string n, string l) {
        name = n;
        location = l;
    }

    virtual void displayRole() = 0;
    virtual ~User() {}
};

// 3. Citizen class
class Citizen : public User {
public:
    int points;

    Citizen(string n, string l) : User(n, l) {
        points = 0;
    }

    void displayRole() {
        cout << "\nWelcome Citizen: " << name << endl;
    }
};

// 4. Staff class
class Staff : public User {
public:
    Staff() : User("Staff", "Office") {}

    void displayRole() {
        cout << "\nStaff Portal\n";
    }
};

// 5. Admin class
class Admin : public User {
public:
    Admin() : User("Admin", "Office") {}

    void displayRole() {
        cout << "\nAdmin Portal\n";
    }
};

// 6. PickupRequest class
class PickupRequest {
public:
    int id;
    string name, location, status;
    Waste waste;

    PickupRequest(int i, string n, string l, Waste w) {
        id = i;
        name = n;
        location = l;
        waste = w;
        status = "Pending";
    }

    void display() {
        cout << "\nRequest ID: " << id
             << "\nCitizen: " << name
             << "\nLocation: " << location
             << "\nCategory: " << waste.category
             << "\nQuantity: " << waste.quantity << " kg"
             << "\nStatus: " << status << endl;
    }
};

// 7. RewardManager class
class RewardManager {
public:
    int totalPoints = 0;

    int calculateReward(Waste w) {
        if (w.category == "Dry")
            return w.quantity * 10;
        else if (w.category == "Wet")
            return w.quantity * 5;
        else if (w.category == "E-Waste")
            return w.quantity * 15;
        else
            return 0;
    }
};

// 8. Main system class
class WasteManagementSystem {
    vector<PickupRequest> requests;
    RewardManager rewards;
    int nextID = 1;

public:
    WasteManagementSystem() {
        loadData();
    }

    void saveData() {
        ofstream file("ecotrack.txt");

        for (auto &r : requests) {
            file << r.id << "|" << r.name << "|"
                 << r.location << "|" << r.waste.category
                 << "|" << r.waste.quantity << "|"
                 << r.status << endl;
        }

        file.close();
    }

    void loadData() {
        ifstream file("ecotrack.txt");
        string line;

        while (getline(file, line)) {
            string fields[6];
            int start = 0;

            for (int i = 0; i < 5; i++) {
                int pos = line.find('|', start);
                if (pos == string::npos) break;
                fields[i] = line.substr(start, pos - start);
                start = pos + 1;
            }

            fields[5] = line.substr(start);

            if (fields[0] == "") continue;

            Waste w(fields[3], stod(fields[4]));
            PickupRequest r(stoi(fields[0]), fields[1],
                            fields[2], w);

            r.status = fields[5];
            requests.push_back(r);

            if (r.status == "Collected")
                rewards.totalPoints += rewards.calculateReward(w);

            if (r.id >= nextID)
                nextID = r.id + 1;
        }

        file.close();
    }

    void registerRequest() {
        string name, location;
        int choice;
        double quantity;

        cout << "\nEnter your name: ";
        getline(cin, name);

        cout << "Enter your location: ";
        getline(cin, location);

        cout << "\n1. Dry\n2. Wet\n3. E-Waste\n4. Hazardous";
        cout << "\nSelect waste category: ";
        cin >> choice;

        cout << "Enter quantity in kg: ";
        cin >> quantity;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (choice < 1 || choice > 4 || quantity <= 0) {
            cout << "Invalid input!\n";
            return;
        }

        string categories[] = {
            "Dry", "Wet", "E-Waste", "Hazardous"
        };

        Waste w(categories[choice - 1], quantity);
        PickupRequest r(nextID++, name, location, w);

        requests.push_back(r);
        saveData();

        cout << "\nRequest registered successfully!";
        cout << "\nYour Request ID: " << r.id << endl;
    }

    void viewCitizenRequests() {
        string name;
        cout << "\nEnter your name: ";
        getline(cin, name);

        bool found = false;
        int points = 0;

        for (auto &r : requests) {
            if (r.name == name) {
                r.display();
                found = true;

                if (r.status == "Collected")
                    points += rewards.calculateReward(r.waste);
            }
        }

        if (!found)
            cout << "\nNo requests found.";

        cout << "\nYour Reward Points: " << points << endl;
    }

    void segregationGuide() {
        cout << "\n--- Waste Segregation Guide ---\n";
        cout << "Dry: Paper, plastic, cardboard\n";
        cout << "Wet: Food scraps, vegetable peels\n";
        cout << "E-Waste: Batteries, phones, chargers\n";
        cout << "Hazardous: Chemicals, paint, bulbs\n";
    }

    void showAllRequests() {
        if (requests.empty())
            cout << "\nNo requests available.\n";

        for (auto &r : requests)
            r.display();
    }

    void updateRequest() {
        int id, choice;
        cout << "\nEnter Request ID: ";
        cin >> id;

        for (auto &r : requests) {
            if (r.id == id) {
                if (r.status == "Collected" ||
                    r.status == "Rejected") {
                    cout << "\nThis request cannot be updated.\n";
                    return;
                }

                cout << "\n1. Assigned\n2. Collected\n3. Rejected";
                cout << "\nSelect new status: ";
                cin >> choice;

                if (choice < 1 || choice > 3) {
                    cout << "\nInvalid choice!";
                    return;
                }

                string status[] = {
                    "Assigned", "Collected", "Rejected"
                };

                r.status = status[choice - 1];

                if (r.status == "Collected")
                    rewards.totalPoints +=
                        rewards.calculateReward(r.waste);

                saveData();
                cout << "\nStatus updated successfully!";
                return;
            }
        }

        cout << "\nRequest not found.";
    }

    void staffMenu() {
        Staff s;
        User *u = &s;
        u->displayRole();

        int choice;

        do {
            cout << "\n1. View All Requests";
            cout << "\n2. Update Request Status";
            cout << "\n3. Exit";
            cout << "\nEnter choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 1)
                showAllRequests();
            else if (choice == 2)
                updateRequest();

        } while (choice != 3);
    }

    void adminMenu() {
        Admin a;
        User *u = &a;
        u->displayRole();

        int choice;

        do {
            cout << "\n1. View All Requests";
            cout << "\n2. View Reports";
            cout << "\n3. Exit";
            cout << "\nEnter choice: ";
            cin >> choice;

            if (choice == 1) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                showAllRequests();
            }
            else if (choice == 2) {
                int collected = 0, pending = 0;
                double quantity = 0;
                int dry = 0, wet = 0, ewaste = 0, hazardous = 0;

                for (auto &r : requests) {
                    if (r.status == "Collected") {
                        collected++;
                        quantity += r.waste.quantity;

                        if (r.waste.category == "Dry") dry++;
                        else if (r.waste.category == "Wet") wet++;
                        else if (r.waste.category == "E-Waste") ewaste++;
                        else hazardous++;
                    }

                    if (r.status == "Pending")
                        pending++;
                }

                cout << "\n--- Admin Report ---";
                cout << "\nTotal Requests: " << requests.size();
                cout << "\nCollected Requests: " << collected;
                cout << "\nPending Requests: " << pending;
                cout << "\nCollected Waste: " << quantity << " kg";
                cout << "\nDry Waste: " << dry;
                cout << "\nWet Waste: " << wet;
                cout << "\nE-Waste: " << ewaste;
                cout << "\nHazardous Waste: " << hazardous;
                cout << "\nTotal Reward Points: "
                     << rewards.totalPoints << endl;
            }

        } while (choice != 3);
    }

    void citizenMenu() {
        Citizen c("", "");
        User *u = &c;
        u->displayRole();

        int choice;

        do {
            cout << "\n1. Register Waste";
            cout << "\n2. View My Requests and Points";
            cout << "\n3. Waste Segregation Guide";
            cout << "\n4. Exit";
            cout << "\nEnter choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 1)
                registerRequest();
            else if (choice == 2)
                viewCitizenRequests();
            else if (choice == 3)
                segregationGuide();

        } while (choice != 4);
    }

    void run() {
        int choice;

        do {
            cout << "\n\n===== ECOTRACK =====";
            cout << "\n1. Citizen Portal";
            cout << "\n2. Staff Portal";
            cout << "\n3. Admin Portal";
            cout << "\n4. Exit";
            cout << "\nEnter choice: ";
            cin >> choice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            if (choice == 1)
                citizenMenu();
            else if (choice == 2)
                staffMenu();
            else if (choice == 3)
                adminMenu();
            else if (choice == 4) {
                saveData();
                cout << "\nThank you for using EcoTrack!\n";
            }
            else
                cout << "\nInvalid choice!";

        } while (choice != 4);
    }
};

int main() {
    WasteManagementSystem system;
    system.run();
    return 0;
}

