# Line Clipping Algorithms (C++)

This project demonstrates the implementation of two fundamental line clipping algorithms in computer graphics:

- Cohen-Sutherland Line Clipping
- Liang-Barsky Line Clipping

## 📷 Output

The application visualizes:
- Original lines
- Clipped lines using Cohen-Sutherland
- Clipped lines using Liang-Barsky

Each algorithm produces the same visible result using different approaches.

## Technologies Used

- C++
- Windows Forms (C++/CLI)
- Visual Studio

## Description

A 2D graphical interface is created where several lines are drawn. Some of these lines extend beyond the clipping window. Both algorithms are applied to determine the visible portions of the lines.

Cohen-Sutherland uses region codes and logical operations, while Liang-Barsky uses parametric equations for more efficient computation.

## Purpose

The purpose of this project is to understand how line clipping works and to compare two different algorithms that solve the same problem using different methods.

## Author

Azra Culhacı
