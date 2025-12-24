# Flight Reservation System (DSTR Assignment)

**Course:** Data Structures & Algorithms (CT077-3-2-DSTR)  
**Group:** [Group **]  
**Semester:** [Year 2 Sem 1]

## ✈️ Project Overview
This project involves the design and development of a **Flight Reservation & Seating Management System**. The system is implemented using two different data structures to analyze and compare their performance:
1.  **Array-Based Component:** Uses 2D arrays for seating grids and 1D arrays for passenger lists.
2.  **Linked List-Based Component:** Uses a Hybrid approach (Doubly Linked List for main records + Singly Linked List for waitlist).

## 👥 Team Members & Roles

| Student ID | Name | Role | Responsibilities |
| :--- | :--- | :--- | :--- |
| **[Leader ID]** | **[Leader Name]** | **Leader** | Linked List (Insertion/Deletion), Waitlist Logic, System Architecture |
| [Member 4 ID] | [Name] | Member | Linked List (Search/Manifest), Performance Analysis, Bubble Sort |
| [Member 1 ID] | [Name] | Member | Array (Insertion/Visual Map), Coordinate Mapping |
| [Member 2 ID] | [Name] | Member | Array (Deletion/Search), File I/O, Data Shifting |

## 📂 Project Structure

This project follows a modular design to separate Array and Linked List implementations.

```text
├── flight_passenger_data.csv    # Initial dataset (Do not modify manually)
├── FlightCommon.hpp             # [CORE] Data structures & Abstract Base Class (Do not edit without permission)
├── main.cpp                     # Main driver program (Menu & Integration)
├── ArraySystem.cpp              # Implementation for Array Logic (Member 1 & 2)
└── LinkedListSystem.cpp         # Implementation for Linked List Logic (Member 3 & 4)