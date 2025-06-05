# 🎓 Student-Scores Management System
A lightweight **C++ console application** to manage student grades using **file-based storage**.

---

## 📌 Description  
A simple and intuitive system where **teachers can update marks** for their students, and **students can log in to view their scores**. Designed for learning purposes with clean OOP principles and file handling.

---

## 🚀 Features  

- 🔐 **Role-Based Access**  
  - **Admin**: `admin / admin123` (default)  
  - **Teacher**: Update marks  
  - **Student**: View grades  

- 🧮 **Auto-Grading System**  
  Automatically converts marks (0-100) into grades (A-F).

- 💾 **Persistent Storage**  
  Saves data in `students.dat`, `teachers.dat`, and `marks.dat`.

- 🖥️ **Minimalist UI**  
  Clean, keyboard-driven menus in the terminal.

---

## 🛠️ How to Run  

**Compile:**  
```bash
g++ main.cpp -o student_marks
