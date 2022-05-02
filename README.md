# Modal
A toolkit for designing Modal (kybd only) UIs for PCs 
using the wxWidgets cross-platform GUI library.
In a kybd only GUI, the kybd serves the dual functions of
text entry and screen-space selection.
Such a GUI is therefore neccesarily "modal"
in that it has modes of operation 
in which the meaning of keystrokes possibly changes.
The advantage of a kybd only GUI
is that it is simpler to design and simpler to use.
Engineering apps such as CAD tools
that involve a lot of interaction benefit most from kybd only UIs.
The modal UI toolkit consists of a ModeManager C++ structure
that interfaces with the windowing system.
In the case of wxWidgets, it interfaces with the wxWindow class.
And a few modes of operation designed for commun GUI functions
such as text input, message display and selection and file selection.
Modes are push onto and popped off the mode manager.
This file (modalwx.cpp) also contains a mode of operation
designed for editing the source code for a modal app.
At present it only implements source code navigation features.
We plan to add source code editing, codebase compilation and debugging features
which will make this a full-blown IDE for writing cross-platform modal apps.
The approach we have taken in the design of this source code editor
is to target the narrow domain of modal source code.
All modal source code is contained in a single .cpp file.
This file has a large number of functions.
The source editor mode implemented here is designed
to navigate this large set of C functions (this is a 5000+ line codefile).
We use 3 techniques to manage a codefile of this size.
1. We define 2 constructs above the C++ language level --
the Block and the Sub-Block.
Special demarcators inside comments are used to define blocks and sub-blocks.
2. We layout the source code in a 3 column format
with a wide center column that is used to work on the code
and 2 narrower columns to see what was before and after the code currently being worked on.
This makes is possible to view roughtly 150 lines of code at a time
which covers even the longest functions.
3. We use kybd based code folding (which we call summarization).
Blocks and sub-blocks can be summarized, functions and structures can be summarized.
Summarization or de-summarization is done using Ctrl-S or Command-S.
It is very efficient and blends in well with code-editing and navigation
since there is no mouse involved in the operation.
By using these 3 techniques, 
it is possible to view this entire 5000 line codebase in a single page
then open a block or sub-block, open a function or structure inside it
and study the details of the implementation.
The purpose of posting this codebase at this stage 
is to get people to use these codebase navigation techniques
and see if they are effective on understanding this 5000 line codebase. 

