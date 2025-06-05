#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <cstdlib>

using namespace std;

// Forward declarations
class User;
class Admin;
class Teacher;
class Student;

// Utility functions
void clearScreen();
void pressEnterToContinue();
void printHeader(const string& title);
void printMenu(const vector<string>& options);
void saveMarks(const vector<Student*>& students);
void loadMarks(vector<Student*>& students);
void loadAllData();
User* authenticateUser();
void cleanup();

// Global vectors
vector<Student*> students;
vector<Teacher*> teachers;

// Utility functions implementation
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void pressEnterToContinue() {
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void printHeader(const string& title) {
    clearScreen();
    cout << "╔══════════════════════════════════════╗\n";
    cout << "║ " << left << setw(36) << title << "║\n";
    cout << "╚══════════════════════════════════════╝\n";
}

void printMenu(const vector<string>& options) {
    for (size_t i = 0; i < options.size(); i++) {
        cout << " " << i+1 << ". " << options[i] << "\n";
    }
    cout << "╚══════════════════════════════════════╝\n";
    cout << " Enter choice: ";
}

class User {
protected:
    string username;
    string password;
    string role;

public:
    User(const string& uname, const string& pwd, const string& r) 
        : username(uname), password(pwd), role(r) {}

    string getUsername() const { return username; }
    string getRole() const { return role; }
    string getPasswordCopy() const { return password; }

    bool authenticate(const string& pwd) const {
        return password == pwd;
    }

    virtual void displayMenu() = 0;
    virtual ~User() {}
};

class Student : public User {
    map<string, int> marks;
public:
    Student(const string& uname, const string& pwd) 
        : User(uname, pwd, "student") {}

    void setMark(const string& subject, int mark) {
        marks[subject] = mark;
    }

    char calculateGrade(int mark) {
        if (mark >= 90) return 'A';
        if (mark >= 80) return 'B';
        if (mark >= 70) return 'C';
        if (mark >= 60) return 'D';
        return 'F';
    }

    const map<string, int>& getMarks() const { return marks; }

    void displayMenu() override;
    void viewMarks();
};

class Teacher : public User {
    string subject;
public:
    Teacher(const string& uname, const string& pwd, const string& sub) 
        : User(uname, pwd, "teacher"), subject(sub) {}

    string getSubject() const { return subject; }

    void displayMenu() override;
    void updateMarks();
    void updateSingleStudentMark(Student* student);
};

class Admin : public User {
public:
    Admin(const string& uname, const string& pwd) 
        : User(uname, pwd, "admin") {}

    void displayMenu() override;
    void addStudent();
    void addTeacher();
};

// Student member functions
void Student::viewMarks() {
    printHeader("Your Marks");
    
    if (marks.empty()) {
        cout << " No marks available yet.\n";
        pressEnterToContinue();
        return;
    }

    cout << "╔══════════════════════════════════════╗\n";
    cout << "║ " << left << setw(15) << "Subject" 
         << setw(10) << "Marks" << "Grade" << " ║\n";
    cout << "╠══════════════════════════════════════╣\n";
    
    for (const auto& entry : marks) {
        cout << "║ " << left << setw(15) << entry.first 
             << setw(10) << entry.second 
             << calculateGrade(entry.second) << " ║\n";
    }
    cout << "╚══════════════════════════════════════╝\n";
    pressEnterToContinue();
}

void Student::displayMenu() {
    while(true) {
        printHeader("Student Menu");
        printMenu({
            "View Marks",
            "Logout"
        });

        int choice;
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1: viewMarks(); break;
            case 2: return;
            default:
                cout << " Invalid choice!\n";
                pressEnterToContinue();
        }
    }
}

// Teacher member functions
void Teacher::updateMarks() {
    while(true) {
        printHeader("Update Marks for " + subject);
        
        cout << "╔══════════════════════════════════════╗\n";
        cout << "║ " << left << setw(5) << "No." 
             << setw(25) << "Student" 
             << "Current Mark ║\n";
        cout << "╠══════════════════════════════════════╣\n";
        
        int studentNumber = 1;
        vector<Student*> myStudents;
        
        for (auto& student : students) {
            if (student->getUsername() == "admin") continue;
            
            const auto& marks = student->getMarks();
            auto it = marks.find(subject);
            int currentMark = (it != marks.end()) ? it->second : -1;
            
            cout << "║ " << left << setw(5) << studentNumber++
                 << setw(25) << student->getUsername()
                 << (currentMark != -1 ? to_string(currentMark) : "N/A") << " ║\n";
            myStudents.push_back(student);
        }
        cout << "╚══════════════════════════════════════╝\n";
        
        cout << "\n Options:\n";
        cout << " 1-"<< (myStudents.size()) << ". Update specific student\n";
        cout << " 0. Return to main menu\n";
        cout << " Enter choice: ";
        
        int choice;
        cin >> choice;
        
        if (choice == 0) {
            return;
        }
        else if (choice > 0 && choice <= myStudents.size()) {
            updateSingleStudentMark(myStudents[choice-1]);
        }
        else {
            cout << " Invalid choice!\n";
            pressEnterToContinue();
        }
    }
}

void Teacher::updateSingleStudentMark(Student* student) {
    printHeader("Update Marks for " + student->getUsername());
    
    const auto& marks = student->getMarks();
    auto it = marks.find(subject);
    if (it != marks.end()) {
        cout << " Current mark in " << subject << ": " << it->second << "\n";
    }
    else {
        cout << " No existing mark for " << subject << "\n";
    }
    
    int mark;
    while(true) {
        cout << " Enter new mark (0-100) or -1 to cancel: ";
        cin >> mark;
        
        if (mark == -1) {
            return;
        }
        else if (mark >= 0 && mark <= 100) {
            break;
        }
        cout << " Invalid mark! Please enter value between 0-100.\n";
    }
    
    student->setMark(subject, mark);
    saveMarks(students);
    
    cout << "\n Mark updated successfully!\n";
    pressEnterToContinue();
}

void Teacher::displayMenu() {
    while(true) {
        printHeader("Teacher Menu (" + subject + ")");
        printMenu({
            "Update Marks",
            "Logout"
        });

        int choice;
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1: updateMarks(); break;
            case 2: return;
            default:
                cout << " Invalid choice!\n";
                pressEnterToContinue();
        }
    }
}

// Admin member functions
void Admin::addStudent() {
    printHeader("Add New Student");
    
    string username, password;
    cout << " Enter student username: ";
    cin >> username;
    
    if (username == "admin") {
        cout << " Cannot create student with username 'admin'\n";
        pressEnterToContinue();
        return;
    }
    
    cout << " Enter student password: ";
    cin >> password;

    students.push_back(new Student(username, password));
    ofstream out("students.dat", ios::app);
    out << username << " " << password << endl;
    cout << "\n Student added successfully!\n";
    pressEnterToContinue();
}

void Admin::addTeacher() {
    printHeader("Add New Teacher");
    
    string username, password, subject;
    cout << " Enter teacher username: ";
    cin >> username;
    cout << " Enter teacher password: ";
    cin >> password;
    cout << " Enter subject: ";
    cin >> subject;

    teachers.push_back(new Teacher(username, password, subject));
    ofstream out("teachers.dat", ios::app);
    out << username << " " << password << " " << subject << endl;
    cout << "\n Teacher added successfully!\n";
    pressEnterToContinue();
}

void Admin::displayMenu() {
    while(true) {
        printHeader("Admin Dashboard");
        printMenu({
            "Add Student",
            "Add Teacher",
            "Logout"
        });

        int choice;
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1: addStudent(); break;
            case 2: addTeacher(); break;
            case 3: return;
            default:
                cout << " Invalid choice!\n";
                pressEnterToContinue();
        }
    }
}

// File operations
void saveMarks(const vector<Student*>& students) {
    ofstream out("marks.dat");
    for (const auto& student : students) {
        const auto& marks = student->getMarks();
        out << student->getUsername() << " " << marks.size() << " ";
        for (const auto& entry : marks) {
            out << entry.first << " " << entry.second << " ";
        }
        out << endl;
    }
}

void loadMarks(vector<Student*>& students) {
    ifstream in("marks.dat");
    if (!in) return;

    string username;
    while (in >> username) {
        int count;
        in >> count;
        
        auto it = find_if(students.begin(), students.end(), 
            [&](const Student* s) { return s->getUsername() == username; });
        
        if (it != students.end()) {
            for (int i = 0; i < count; i++) {
                string subject;
                int mark;
                in >> subject >> mark;
                (*it)->setMark(subject, mark);
            }
        }
    }
}

User* authenticateUser() {
    printHeader("Login");
    
    string username, password;
    cout << " Username: ";
    cin >> username;
    cout << " Password: ";
    cin >> password;

    // Check for admin first
    if(username == "admin" && password == "admin123") {
        return new Admin("admin", "admin123");
    }

    // Check students
    for (const auto& student : students) {
        if(student && student->getUsername() == username) {
            if(student->authenticate(password)) {
                Student* newStudent = new Student(student->getUsername(), student->getPasswordCopy());
                const auto& marks = student->getMarks();
                for (const auto& entry : marks) {
                    newStudent->setMark(entry.first, entry.second);
                }
                return newStudent;
            }
            break;
        }
    }

    // Check teachers
    for (const auto& teacher : teachers) {
        if(teacher && teacher->getUsername() == username) {
            if(teacher->authenticate(password)) {
                return new Teacher(teacher->getUsername(), 
                                 teacher->getPasswordCopy(),
                                 teacher->getSubject());
            }
            break;
        }
    }

    return nullptr;
}

void loadAllData() {
    // Create files if they don't exist
    ofstream createFile;
    if (!ifstream("students.dat")) createFile.open("students.dat");
    if (!ifstream("teachers.dat")) createFile.open("teachers.dat");
    if (!ifstream("marks.dat")) createFile.open("marks.dat");
    createFile.close();

    // Load students
    ifstream studentFile("students.dat");
    if (studentFile) {
        string sUser, sPass;
        while(studentFile >> sUser >> sPass) {
            students.push_back(new Student(sUser, sPass));
        }
        studentFile.close();
    }

    // Load teachers
    ifstream teacherFile("teachers.dat");
    if (teacherFile) {
        string tUser, tPass, tSub;
        while(teacherFile >> tUser >> tPass >> tSub) {
            teachers.push_back(new Teacher(tUser, tPass, tSub));
        }
        teacherFile.close();
    }

    loadMarks(students);
}

void cleanup() {
    for (auto& s : students) {
        if (s) {
            delete s;
            s = nullptr;
        }
    }
    students.clear();

    for (auto& t : teachers) {
        if (t) {
            delete t;
            t = nullptr;
        }
    }
    teachers.clear();
}

int main() {
    try {
        loadAllData();

        // Create admin if first run
        bool adminExists = false;
        for (const auto& s : students) {
            if (s && s->getUsername() == "admin") {
                adminExists = true;
                break;
            }
        }
        
        if (!adminExists) {
            students.push_back(new Student("admin", "admin123"));
        }

        while(true) {
            printHeader("Student Management System");
            printMenu({
                "Login",
                "Exit"
            });

            int choice;
            cin >> choice;
            cin.ignore();

            if(choice == 1) {
                User* user = authenticateUser();
                if(user) {
                    cout << "\n Login successful!\n";
                    pressEnterToContinue();
                    user->displayMenu();
                    delete user;
                } else {
                    cout << " Invalid username or password!\n";
                    pressEnterToContinue();
                }
            }
            else if(choice == 2) {
                saveMarks(students);
                break;
            }
            else {
                cout << " Invalid choice!\n";
                pressEnterToContinue();
            }
        }

        cleanup();
        return 0;
    }
    catch(const exception& e) {
        cerr << "Error: " << e.what() << endl;
        cleanup();
        return 1;
    }
}