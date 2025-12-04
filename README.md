Advanced Console Family Tree System (FileName1.cpp)

This C++ program is a complex, console-based genealogical system that uses Object-Oriented Programming (OOP) principles, dynamic memory management, and linked list structures to model and display a family tree.

I. System Overview

The system allows the user to build a family tree starting from a root ancestor, add new members with dual-parent relationships, mark members as deceased ("Late"), and display the complete tree structure generation by generation in a centered, readable format.

Core Functionality:

Creation and Membership: Interactively create the Root Ancestor and add subsequent FamilyMember nodes, linking them to their father and mother (which can be existing members or newly created placeholders).

Life Status Management: Update a member's status using the markLateInteractive function.

Visual Display (showCenteredTree): The primary feature displays the tree top-down, grouping children under their parent pairs (FamilyPair) and calculating spacing to center the information horizontally in the console.

II. Class Structure and Data Management

The system is organized around two main classes:

1. FamilyMember Class

This class represents a single individual in the tree.

Attributes: Stores the individual's name, gender ('M'/'F'), and alive status.

Pointers (Tree Structure): Uses explicit pointers for genealogical links: father, mother, firstChild, and nextSibling. This linked structure allows for traversing and building the tree relationship.

Memory Pool: The FamilyTree class maintains a dynamic array (memberPool) of FamilyMember pointers to track all created members for easy cleanup and search functionality (findByName).

2. FamilyPair Class

This class is a helper structure used only during the tree display process (showCenteredTree) to group children under a single set of parents.

Attributes: Holds pointers to the father and mother who constitute the pair.

Children List: Manages the list of children belonging to this pair using a dynamically resized array of FamilyMember pointers.

III. Implementation Details

Dynamic Arrays and Memory Management: Dynamic memory (new/delete[]) is used within the FamilyTree to manage the collection of members (memberPool) and within FamilyPair to manage its list of children, showcasing manual memory control.

Tree Traversal Logic: The showCenteredTree function uses an iterative, generation-based approach (similar to a Breadth-First Search logic) to build the tree. It identifies all unique parent pairs in the current generation, prints them, and then uses their children to determine the families for the next generation's display.

Console Formatting: Includes logic within the FamilyPair methods (parentLine, childrenLine) and the showCenteredTree function to truncate names (MAX_NAME = 15), calculate display widths, and pad the output with spaces to achieve a centered layout for each family block, making the complex data structure legible in a text-only interface.

Robust Input Handling: The FamilyTree class implements several static utility functions (readLine, readGender, readYesNo) to ensure valid input is captured from the user, preventing common C++ input stream errors.# Family-Tree
