# ModalWX
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
Modes are pushed onto and popped off the mode manager.
This file (modalwx.cpp) also contains a mode of operation
designed for editing the source code for a modal app.
At present it only implements source code navigation features.
We plan to add source code editing, codebase compilation and debugging features
to make this a full-blown IDE for writing cross-platform modal apps.
The approach we have taken in the design of this source code editor
is to target the narrow domain of modal source code.
By design, all of a modal app's source code
is contained in a single .cpp file.
This is unlike a typical codebase that has several different .cpp
files with associated .h files.
Instead of several small .cpp files that are navigated using a project directory borwser,
we use a large single .cpp file that contains a large number of functions.  
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
Comment blocks can be summarized.
Summarization or de-summarization is done using Ctrl-S or Command-S.
It is very efficient and blends in well with code-editing and navigation
since there is no mouse involved in the operation.

By using these 3 techniques, 
it is possible to view this entire 5000 line codebase in a single page
then open a block or sub-block, open a function or structure inside it
and study the details of the implementation.
Then close that block, ope another block and study it.
There is no other navigational mechanism needed.
The purpose of posting this codebase at this stage 
is to get people to use these codebase navigation techniques
and see if they are effective on understanding this 5000 line codebase. 

Building and Running the app:

The app is based on the wxWidgets cross-platform UI library.
To build it you first have to download and build the wxWidgets library.
We recommend using the current development version of wxWidgets which is 3.1.6.
You can get help for setting up wxWidgets on your PC from this forum:
https://forums.wxwidgets.org/viewforum.php?f=19&sid=0083f4684647607be2aef5bc34b48d82
The build process for the library depends on your platform:

OSX: 
We recommend building the library from source.
The simplest way is to download the source for wxWidgets.
Then open %wxWidgetsDir%/Samples/minimal/minimal_cocoa.xcodeproj
You should be able to build and run this sample.
It builds the wxWidgets library from source as part of its build process.
Then you can edit the project settings to replace minimal.cpp with modal.cpp.
This will build the modal app.

Windows:
We recommend Visual Studio 2022 Community edition as the IDE.
You download the source code.
Then you goto %wxWidgetsDir%/build/msw/
You open wx_vc17.sln in Visual Studio 2022
Build Debug and Release configurations (we recommend not dll but statically linked libraries).
This places the built libraries in %wxWidgetsDir%/lib/vc_x64_lib (or vc_lib).
Follow the instruction at https://forums.wxwidgets.org/viewtopic.php?p=196105#p196105
to create a new VS project and add modal.cpp to it.
This should build the modal app.

Linux:
We recommend using the CodeLite IDE which comes with wxWidgets pre-installed.
Create a new bare-bones wxWidgets based project and add modal.cpp to it.
This should build the modal app.

Running the app:
When you run the app,
it will ask you for the full path of modal.cpp.
When you enter this path,
it will load modal.cpp, parse it and display its blocks.
The arrow keys are for moving up and down the lines in the display.
PgUp and PgDn have their usual function.
To open a block you go to the block's line using the arrow keys
then you press Ctrl-S (Win, Linux) or Command-S (OSX)
To close a block, you go to the first line in the block an press Ctrl-S.
Any line that ends in {...} is summarized (closed) and can be opened using Ctrl-S 
