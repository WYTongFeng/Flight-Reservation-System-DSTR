# Flight Reservation System (DSTR Assignment)

**Course:** Data Structures & Algorithms (CT077-3-2-DSTR)  
**Group:** [Group 11]  
**Semester:** [Year 2 Sem 1]

## ✈️ Project Overview
This project involves the design and development of a **Flight Reservation & Seating Management System**. The system is implemented using two different data structures to analyze and compare their performance:
1.  **Array-Based Component:** Uses 2D arrays for seating grids and 1D arrays for passenger lists.
2.  **Linked List-Based Component:** Uses a Hybrid approach (Doubly Linked List for main records + Singly Linked List for waitlist).

## 👥 Team Members & Roles

| Student ID | Name | Role | Responsibilities |
| :--- | :--- | :--- | :--- |
| **[TP080544]** | **[Wong Zi Yee]** | **Leader** | Linked List (Insertion/Deletion), Waitlist Logic, System Architecture |
| [TP080508] | [Wong Jin Jie] | Member | Linked List (Search/Manifest), Performance Analysis, Bubble Sort |
| [TP080559] | [Ngo Kar Hong] | Member | Array (Insertion/Visual Map), Coordinate Mapping |
| [TP082825] | [Jay Chew Jie Lun] | Member | Array (Deletion/Search), File I/O, Data Shifting |

## 📂 Project Structure

This project follows a modular design to separate Array and Linked List implementations.

```text
├── flight_passenger_data.csv    # Initial dataset (Do not modify manually)
├── FlightCommon.hpp             # [CORE] Data structures & Abstract Base Class (Do not edit without permission)
├── main.cpp                     # Main driver program (Menu & Integration)
├── ArraySystem.cpp              # Implementation for Array Logic (Member 1 & 2)
└── LinkedListSystem.cpp         # Implementation for Linked List Logic (Member 3 & 4)
