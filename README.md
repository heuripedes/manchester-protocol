# Manchester Protocol Simulator

This project simulates the use of the manchester protocol in an Emergency Room.

The manchester protocol which is a protocol for patient risk classification
invented in 1994 by Kevin Mackway-Jones. It was created to help clinics and
hospitals to deal with incoming patients in a efficient way while minimizing
risk for each patient.

The intent here is to simulate waves of patients arriving at the hospital with
variying levels of risk. Upon arrival, each patient receives a tag or purse with
a color that represent the risk:

* `red` - This patient must be taken care of RIGHT NOW or they might die.
* `orange` - This patient can wait about 10 minutes.
* `yellow` - This patient can wait up to 60 minutes.
* `green` - This patient should wait up to 120 minutes or sent elsewhere.
* `blue` - This patient should wait up to 240 minutes or sent elsewhere.

As time passes, the health state of `yellow` and `orange` patients will become
worse and they will gravitate towards `red` state.

By default, the simulator runs at approximately two ticks per second and each
tick represents one minute.

## Requirements

We require ncurses development files, install it with:

    sudo apt install libncurses-dev

## Building

    mkdir build
    cd build
    cmake ..
    make

## Running

    cd build
    ./manchester_protocol

Shortcuts:

* `q` - quit
* `space` - add more patients
* `↑` - increase the number of doctos/rooms
* `↓` - decrease the number of doctors/rooms
* `←` - decrease simulation speed
* `→` - increase simulation speed

## CAPSTONE Project Rubric

### README (All Rubric Points REQUIRED)

* [x] The README is included with the project and has instructions for building/running the project.
* [x] If any additional libraries are needed to run the project, these are indicated with cross-platform installation instructions.
* [x] You can submit your writeup as markdown or pdf.
* [x] The README describes the project you have built
* [ ] The README also indicates the file and class structure, along with the expected behavior or output of the program.
* [x] The README indicates which rubric points are addressed. The README also indicates where in the code (i.e. files and line numbers) that the rubric points are addressed.

### Compiling and Testing (All Rubric Points REQUIRED)

* [x] The project code must compile and run without errors.
* [x] We strongly recommend using cmake and make, as provided in the starter repos. If you choose another build system, the code must compile on any reviewer platform.

### Loops, Functions, I/O

* [x] A variety of control structures are used in the project.
    > while loop, switch statement and if-else used in main.cpp
* [x] The project code is clearly organized into functions.
* [ ] The project reads data from an external file or writes data to a file as part of the necessary operation of the program.
* [x] The project accepts input from a user as part of the necessary operation of the program.
    > Program uses ncurses for input

### Object Oriented Programming

* [x] The project code is organized into classes with class attributes to hold the data, and class methods to perform tasks.
* [x] All class data members are explicitly specified as public, protected, or private.
    > all classes
* [x] All class members that are set to argument values are initialized through member initialization lists.
    > Class Tui's constructors in Tui.cpp
* [ ] All class member functions document their effects, either through function names, comments, or formal documentation. Member functions do not change program state in undocumented ways.
* [ ] Appropriate data and functions are grouped into classes. Member data that is subject to an invariant is hidden from the user. State is accessed via member functions.
* [x] Inheritance hierarchies are logical. Composition is used instead of inheritance when appropriate. Abstract classes are composed of pure virtual functions. Override functions are specified. 
    > Classes Tui, TuiImpl and NcursesTui in Tui.cpp and Tui.h
* [ ] One function is overloaded with different signatures for the same function name.
* [x] One member function in an inherited class overrides a virtual base class member function.
    > Class NcursesTui in file Tui.cpp overrides functions declared in abstract class Tui from Tui.h
* [x] One function is declared with a template that allows it to accept a generic parameter.
    > Class Channel in Util.h requires a template argument. Methods send() and receive() are written in a generic form.

### Memory Management

* [x] At least two variables are defined as references, or two functions use pass-by-reference in the project code.
    > Function "swap" in Patient.h receives arguments as references and so does Tui::_drawPatient() in Tui.h
* [x] At least one class that uses unmanaged dynamically allocated memory, along with any class that otherwise needs to modify state upon the termination of an object, uses a destructor.
    > Class NcursesTui in Tui.cpp uses a destructor to terminate the ncurses session
* [x] The project follows the Resource Acquisition Is Initialization pattern where appropriate, by allocating objects at compile-time, initializing objects when they are declared, and utilizing scope to ensure their automatic destruction.
* [x] For all classes, if any one of the copy constructor, copy assignment operator, move constructor, move assignment operator, and destructor are defined, then all of these functions are defined.
* [x] For classes with move constructors, the project returns objects of that class by value, and relies on the move constructor, instead of copying the object.
    > Tui, and NcursesTui classes have "deleted" copy constructors and operators
* [x] The project uses at least one smart pointer: unique_ptr, shared_ptr, or weak_ptr. The project does not use raw pointers.
    > unique_ptr used in Tui class in files Tui.h and Tui.cpp

### Concurrency
* [x] The project uses multiple threads in the execution.
* [x] A promise and future is used to pass data from a worker thread to a parent thread in the project code.
* [x] A mutex or lock (e.g. std::lock_guard or `std::unique_lock) is used to protect data that is shared across multiple threads in the project code.
    > Classes Event and Channel in Util.h
* [x] A std::condition_variable is used in the project code to synchronize thread execution.
    > Classes Event and Channel in Util.h