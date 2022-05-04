# ModalWX
A 'source-code toolkit' for designing Modal (kybd-only) UIs for PCs 
based on wxWidgets.
In a kybd-only GUI, the kybd serves the dual functions of
text entry and screen-space selection.
Such a GUI is therefore neccesarily "modal"
in that it has modes of operation 
in which the meaning of keystrokes possibly changes.
Unlike a WIMP GUI where the screen is functionally divided into windows and subwindows,
in a Modal GUI, functional division is over time.
At any given time, one mode is painting to the entire screen,
and at different times, different operational modes are running the UI.
The advantage of a Modal kybd-only GUI
is that it is simpler to design and simpler to use.
Engineering apps such as CAD tools and developer tools
that involve a lot of app-user interaction benefit most from kybd only UIs.
This is a source-code toolkit in the sense that this file can be edited to produce an app,
keeping the parts that are needed for the app being produced,
discarding parts that are not needed and adding new parts specific to the app.
There are no libraries involved and all the core functions provided by the toolkit
are visible as source code, making debugging easier.

The toolkit consists of a ModeManager C++ structure
that interfaces with the windowing system.
In the case of wxWidgets, it interfaces with the wxWindow class.
And a few modes of operation designed for commun GUI functions
such as text input, message display, selection from a set of choices
and selection of a file from the platfrom's file system.
Modes are pushed onto and popped off the mode manager.
This file (modalwx.cpp) also contains a mode of operation
designed for editing the source code for a Modal app.
At present it only implements source code navigation features.
Source code editing, codebase compilation and debugging features will be added
to make this a full-blown IDE.
The approach we have taken in the design of this source code editor
is to target the narrow domain of Modal source code.
By design, all of a Modal app's source code
is contained in a single .cpp file.
This is unlike a typical codebase that has several different .cpp
files with associated .h files.
Instead of several small .cpp files that are navigated using a project directory browser,
we use a large single .cpp file that contains a large number of functions.  
The source editor mode implemented here is designed
to navigate this large set of C functions (this is a 5000+ line codefile).
We use 3 techniques to manage a codefile of this size.
1. We define 2 sectional constructs above the C++ language level --
the Block and the Sub-Block.
Special demarcators inside comments are used to define blocks and sub-blocks.
Blocks and sub-blocks serve the purpose of coarse, high-level navigation.
2. We display the source code in a 3 column format
with a wide center column that is used to work on the code
and 2 narrower columns to see the preceding and following code sections.
This makes is possible to view roughtly 150 lines of code at a time
which covers even the longest functions.
3. We use kybd based code folding (which we call summarization).
Any sectional element in the code can be summarized.
Blocks and sub-blocks, functions and structures, enums and
comment blocks can all be summarized.
Summarization or de-summarization is done using Ctrl-S or Command-S.
It is very efficient and blends in well with code-editing and navigation.
The developer is already using the kybd for these functions
and since summarisation is also kybd controlled,
they do not have to switch over to the mouse at all.

By using these 3 techniques, 
it is possible to view this entire 5000 line codebase in a single page
then open a block or sub-block, open a function or structure inside it
and study the details of the implementation.
Then close that block, open another block and study it.
There is no other navigational mechanism needed.
The purpose of posting this codebase at this stage 
is to get people to use these codebase navigation techniques
and see if they are effective in understanding this 5000 line codebase. 
This serves as an introduction to this source-code toolkit.

ModalWX.cpp, when viewed in an IDE is rather large and unwieldy. 
We are working on adding a source code editor to it 
so that it becomes easier to edit it and create a Modal app based on it.
Modal app codefiles are designed to be viewed using ModalWX
since the block and sub-block level constructs are only parsed by ModalWX
and code folding, while available on all IDEs, is not commonly used
since it is mouse-based, time-consuming, breaks the flow and is hence not convenient.

# Building and Running the app:

The app is based on the wxWidgets cross-platform UI library.
To build this app you first have to download and build the wxWidgets library.
We recommend using the current development version of wxWidgets which is 3.1.6.
You can get help for setting up wxWidgets on your PC from this forum:
https://forums.wxwidgets.org/viewforum.php?f=19&sid=0083f4684647607be2aef5bc34b48d82
The build process for the library depends on your platform:

OSX: 
We recommend building the library from source.
The simplest way is to download the source for wxWidgets.
Then open %wxWidgetsDir%/Samples/minimal/minimal_cocoa.xcodeproj
You should be able to build and run this sample in XCode.
It builds the wxWidgets library from source as part of its build process.
Then you can edit the project settings to replace minimal.cpp with modalwx.cpp.
This will build the modal app.

Windows:
We recommend Visual Studio 2022 Community edition as the IDE.
You download the source code for wxWidgets.
Then you goto %wxWidgetsDir%/build/msw/
You open wx_vc17.sln in Visual Studio 2022
Build Debug and Release configurations (we recommend not dll but statically linked libraries).
This places the built libraries in %wxWidgetsDir%/lib/vc_x64_lib (or vc_lib).
Follow the instructions at https://forums.wxwidgets.org/viewtopic.php?p=196105#p196105
to create a new VS project and add modalwx.cpp to it.
This should build the modal app.

Linux:
We recommend using the CodeLite IDE which comes with wxWidgets pre-installed.
Create a new bare-bones wxWidgets based project and add modalwx.cpp to it.
This should build the modal app.

Running the app:

When you run the app,
it will ask you for the full path of modalwx.cpp.
When you enter this path,
it will load modalwx.cpp, parse it and display its blocks.
The arrow keys are for moving up and down the lines in the display.
PgUp and PgDn have their usual function.
To open a block you go to the block's line using the arrow keys
then you press Ctrl-S (Win, Linux) or Command-S (OSX)
To close a block, you go to the first line in the block an press Ctrl-S.
Any line that ends in {...} is summarized (closed) and can be opened using Ctrl-S 

Additional controls:

If you press Ctrl-right arrow with the cursor at any given line,
if there is a URL at that line, a browser is opened to view the URL.
If there is a symbol such as a struct or function at the cursor,
the app navigates to that symbol's location in the file.
Ctrl-left arrow brings you back in this case.
If there is nothing at the cursor location,
a text entry field is popped up which currently takes line number input.
More navigational features will be added using this mechanism.

If you press just Ctrl, it pops up a small menu of features
which can be selected using the arrow keys.
Most of these are currently unimplemented.
One of these allows you to change the font size.

When you exit the app, it saves its current state in a file called state.hxp.
When you reload the app, it reads this file to load the state at last exit.
If you delete this file, the app loads afresh.

# Future Plans:
1. We plan to add source editing features to the source editor mode.
2. Then add the ability to compile a Modal source code file and correct compilation errors.
This will be based on gcc (Win, Linux) and g++ on OSX.
3. Then add the ability to debug a modal app.
This will be based on gdb (Win, Linux) and lldb on OSX.
4. We plan to create ModalWin32, a native version of the Modal UI toolkit for Win32.
5. We plan to create ModalX a native version of the Modal UI toolkit for X-Windows on Linux.

# Using this source code toolkit to build your own wxWidgets based modal app:

The core parts of this toolkit are:
1. ModalWindow which derives from wxWindow,
and contains an SModeManager that manages the modes of a modal UI.
You instantiate a ModalWindow, instantiate a ModeManager and add it to the ModalWindow.
Then you push a starting mode onto the mode manager
that will be the mode of operation when the app launches and the ModalWindow is shown.
Over the course of the app's operation, you push and pop modes on the ModeManager.
2. SModeManager which is a c++ struct for managing the modes of operation of a modal UI.
SModeManager is initialized with the screen size and a font
that will be passed to all modes of operation.
It contains a stack of modes and methods to push and pop modes and get the current mode.
It also contains a method to serialize its current state to a file.
This enables the app to save the current state of the UI when it exits
and reload it when the app is relaunched.
ModalWindow calls modal_init in its constructor
which initializes the mode manager member variable of ModalWindow.
ModalWindow calls modal_exit in its destructor
which serializes the mode manager and then frees it.
3. SMode which is a C++ struct for representing the base properties of a mode of operation.
It contains a Union UModeExtension for representing mode specific extensions
to the base mode.
The Modal Toolkit provides a few extensions to the base mode for common UI operations.
These are:
a)   SModeMsg : a mode extension for displaying a user message
b)   SModeLineInp : a mode extension getting a line of user input 
c)   SModeSetSel : a mode extension for getting a choice from the user
from a set of selections.
d)   SModeFileSel : a mode extension for selecting a file from the file system
A modal app defines its own mode of operation 
which implements the desired UI function of that app.
For example modalwx.cpp defines SModeSrcEdr, a mode extension that is not part of the toolkit
but which implements the functionality needed for browsing a modal app's source code.
SModeSrcEdr uses SModeMsg, SModeLineInp, SModeSetSel and SModeFileSel in its implementation
to realize the desired behavior of its UI.

# Designing a mode of operation:

The main task in creating a wxWidgets based modal app is
designing a mode of operation that specifically caters to the app's primary function.
ModalWX.cpp and its design of the SModeSrcEdr mode of operation
serves as a template for designing a mode of operation for some other purpose.
While a mode operation defines the behavior of the app during an interaction session with the user,
the behavior of the app at any given instant is defined by an intent dispatcher.
An intent is the user's intention of performing some action 
in the context of the current mode of operation.
The user expresses this intent through a specific kybd gesture.
This gesture is detected by the OS as a key event,
dispatched to the wxWindow currently in focus which is a ModalWindow.
The ModalWindow in its key event handler dispatcher the key event to the mode manager
which determines the mode at the top of its mode stack and dispathes the event to that mode.
The mode has a fnKybd_map function which processes this event.
In its fnKybd_map function, the mode determines the intent of the user
based on the key strokes it received
and dispatches the intent to be processed by a corresponding intent handler.
Desigining a mode therefore involves designing a set of user intents,
each one of which is identified by specific user keystrokes,
and implementing the apps expected behavior in response to a given user intent
in an intent handler.
An intent handler will do some processing associated with its expected behavior
and then provide feedback to the user by updating the display in some way.
The display update happens in 2 phases.
In the first phase, when the intent handler is called by fnKybd_map,
the intent handler determines the region of the screen it intends to update
and sends the OS a display update request via wxWindow::Refresh.
The OS processes this request and sends a paint event to the wxWindow with this region info.
The ModalWindow's handler for this paint event, dispatches this paint event
to the currently active intent handler in the currently active mode.
Which means the intent handler that initiated the refresh request
gets called in a second phase during which it does the actual drawing to update the display.
Every intent handler therefore has 2 phases of operation
PH_NOTIFY when is called by fnKybd_map 
and PH_EXECUTE when it is called by ModalWindow::OnPaint.
Once an app designer has designed all the intents for a given mode of operation,
they initialize the intent handler functions for each intent in SMode::load_intents.
In a typical app, while the most frequently used intents will have "direct-mapped" controls
which means they are activated directly by keystrokes or keystroke sequences.
Less frequently used user intents can be mapped to (indirect) onscreen screen controls.
For this the toolkit provides another mode of operation called SModeIntDisp which 
is an intent dispatcher for intents controlled by on-screen controls.
If you press Ctrl or Command, the SModeSrcEdr launches an intent dispatcher mode
for things such as setting font sizes.

In addition to designing the interaction part of a mode of operation
the app designer also has to create representations for they data they intend to process.
They need to attach these data structures to the mode of operation they are designing.
SModeSrcEdr, for example, creates several data structures to represent and process
modal source code files.
These data structures are used by the intent handlers of SModeSrcEdr
to display the relevant data. SModeSrcEdr contains pointers to these data structures.

# Modal UIs and WIMP UIs
WIMP UIs have been around a long time and a broad set of controls is available
for developers to quickly put together a UI
with not a lot of code involved since the controls encapsulate the needed functionality.
By contrast, writing a Modal UI requires a lot of coding
specially in terms of graphic drawing.
Only a few off-the-shelf controls are available to assist the process.
On the other hand, Modal UIs give a developer a lot more control
over the outcome of the design process since very little is canned or mandated.
They are also simpler to code.
Once you get familiar with the drawing options offered by wxDC,
creating a new Mode of Operation becomes fairly straightforward.
Also, in terms of visual layout they offer the entire screen for the app's display
since there are no menus or other visual elements to take away screen space.
The SModeSrcEdr mode demonstrates this in its ability to view large segments of code
all on a single screen as compared WIMP based code editors
such as XCode, Visual Studio, Code Blocks and CodeLite.
Modal UIs could be suitably used for Graphing tools, Big data processing tools
or anywhere else where you need greater control over the screen-space
and where your users are willing to learn your kybd-only control conventions.
If you have a vision of how you want your screen to look 
don't find ways of realising that using WIMP controls
and are willing to draw the screen yourself
then a Modal UI is a good option.

Since a Modal UI is implemented entirely within a wxWindow subclass, 
it is possible to incorporate a Modal UI window into a regular WIMP style app
using wxAUI with the Modal UI in one layer and a WIMP UI in another layer.
Effectively mixing these 2 styles of UI design.
