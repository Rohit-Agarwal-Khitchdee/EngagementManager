# ModalWX
A 'source-code toolkit' for designing efficient workstation UIs based on wxWidgets.
Modal UIs are kybd-only and do not break screen-space into sub-windows.
Modal UIs are efficient for 2 reasons:
1. Not having to use the mouse in the interaction is the primary contributor.
This app demonstrates this.
2. Not breaking screen-space into sub-windows means each screen is a lot simpler
and the app gets to use the entire screen for its layout.
Instead of a spatial segregation of screen-space, a temporal segration is used.
Different modes of operation run the UI at different times.
So app complexity is distributed over time, not screen-space.

Modal UIs can replace WIMP GUIs in almost all cases
except where fine-grained screen-space selection is needed
such as graphics design tools.

# ModalWX Walkthrough / Evaluating ModalWX for your next workstation app
1. Build and run the app. This is described in a section below.
2. If you are on OSX, the fontsize might be too small.
To adjust it, press the Command key once.
This will pop-up a set of 5 options as shown below.
![alt text](https://hex-map.khitchdee.net/ModalWX-Ctrl-Popup.png)
Use the down arrow key to select "Adjust Fontsize".
Press Enter(Return).
You will get a message:
"Arrow to adjust fontsize, Esc to exit"
Use the arrow keys to adjust fontsize.
This is a live ajdustment.
Once you are satisfied, press escape.
3. At this point, your screen should look like:
![alt text](https://hex-map.khitchdee.net/ModalWX-source-loaded.png)

This is a source-code toolkit in the sense that this file can be edited to produce an app,
keeping the parts that are needed for the app being produced,
discarding parts that are not needed and adding new parts specific to the app.
There are no libraries involved and all the core functions provided by the toolkit
are visible as source code, making debugging easier.

The toolkit consists of:
1. A ModeManager C++ structure that manages the modes of operation
and interfaces with the windowing system.
In the case of wxWidgets, it interfaces with the wxWindow class.
2. A few modes of operation designed for commun GUI functions
such as text input, message display, selection from a set of choices
and selection of a file from the platform's file system.
Modes are pushed onto and popped off the mode manager.

This file (ModalWX.cpp) also contains a mode of operation
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
To navigate this large set of C functions (this is a 6000+ line codefile),
we use 3 techniques:

1. We define 2 sectional constructs above the C++ language level --
the Block and the Sub-Block.
Special demarcators inside comments are used to define blocks and sub-blocks.
Blocks and sub-blocks serve the function of coarse, high-level navigation.
2. We display the source code in a 3 column format
with a wider center column that is used to work on the code
and 2 narrower side columns to see the preceding and following code sections.
This makes is possible to view roughtly 150 lines of code at a time
which covers even the longest functions.
3. We use kybd based code folding (which we call summarization).
Any sectional element in the code can be summarized.
Blocks and sub-blocks, functions and structures, enums and
comment blocks can all be summarized.
Summarization or de-summarization is done using Ctrl-S (Command-S on OSX).
It is very efficient and blends in well with code-editing and navigation.
The developer is already using the kybd for these functions
and since summarisation is also kybd controlled,
they do not have to switch over to the mouse at all.

By using these 3 techniques, 
it is possible to view this entire 6000 line codebase in a single page
then open a block or sub-block, open a function or structure inside it
and study the details of the implementation.
Then close that block, open another block and study it.
i.e. At any given time, the developer only keeps open 
the section of code they are currently interested in.
There is no other navigational mechanism needed.
The entire screen is used to display source code.
The purpose of posting this codebase at this stage 
is to get people to use these codebase navigation techniques
and see if they are effective in understanding this 5000 line codebase. 
This serves as an introduction to this source-code toolkit
while the final toolkit will have code-editing features as well
and the ability to build and debug using platform-specific tools.

ModalWX.cpp, when viewed in an IDE is rather large and unwieldy. 
Once we add a source code editor, and build and debug features to it 
it will become easier to edit it and create a Modal app based on it.
Modal app codefiles are designed to be viewed using ModalWX
since the block and sub-block level constructs are only parsed by ModalWX
and code folding, while available on all IDEs, is not convenient ot use
since it is mouse-based, time-consuming, and breaks the flow.

# Building and Running the app:

The app is based on the wxWidgets cross-platform UI library.
To build this app you first have to download and build the wxWidgets library.

We recommend using the current development version of wxWidgets which is 3.1.6.
You can get help for setting up wxWidgets on your PC from this forum:
https://forums.wxwidgets.org/viewforum.php?f=19&sid=0083f4684647607be2aef5bc34b48d82
The build process for the library depends on your platform:

## OSX: 
We recommend building the library from source.
The simplest way is to download the source for wxWidgets.
Then open %wxWidgetsDir%/Samples/minimal/minimal_cocoa.xcodeproj
You should be able to build and run this sample in XCode.
It builds the wxWidgets library from source as part of its build process.
Then you can edit the project settings to replace minimal.cpp with ModalWX.cpp.
This will build the Modal app.

## Windows:
We recommend Visual Studio 2022 Community edition as the IDE.
You download the source code for wxWidgets.
Then you goto %wxWidgetsDir%/build/msw/
You open wx_vc17.sln in Visual Studio 2022
Build Debug and Release configurations (we recommend not dll but statically linked libraries).
This places the built libraries in %wxWidgetsDir%/lib/vc_x64_lib (or vc_lib).
Follow the instructions at https://forums.wxwidgets.org/viewtopic.php?p=196105#p196105
to create a new VS project and add ModalWX.cpp to it.
This should build the Modal app.

## Linux:
We recommend using the CodeLite IDE which comes with wxWidgets pre-installed.
Create a new bare-bones wxWidgets based project and add ModalWX.cpp to it.
This should build the Modal app.

## Running the app (UI controls):

When you run the app,
it will ask you for the full path of ModalWX.cpp.
This path will depend on your platform.
![alt text](https://hex-map.khitchdee.net/ModalWX-launch-screen.png)
If you enter an incorrect path, it will bring up a file selector
to select the file from the file system.
![alt text](https://hex-map.khitchdee.net/ModalWX-file-selector.png)
The file selector has upto 5 vertical panels on the screen.
Each panel represents the contents of a directory.
The active panel has a white outline rectangle highlighting the current selection
while the other panels have black outline rectangles.
To change your selection in the active panel, use up and down arrow.
To open a currently selected directory, use right arrow.
To open or navigate to a parent directory, use left arrow.
To commit to a selection (it has to be ModalWX.cpp in this case)
press Enter(Return).
If you make an invalid selection, it gets you back to the file selector.
Once you have correctly selected ModalWX.cpp,
it will load ModalWX.cpp, parse it and display its blocks.
The arrow keys, PgUp and PgDn move the caret.
To open a block you move the caret to the block's line
then you press Ctrl-S (Command-S on OSX)
To close a block, you move the caret to the first line in the block an press Ctrl-S.
Any line that ends in {...} is summarized (closed) and can be opened using Ctrl-S 

Additional controls:

If you press Ctrl-right arrow with the caret at any given line,
if there is a URL at that line, a browser is opened to view the URL.
If there is a symbol such as a struct or function at the caret,
the app navigates to that symbol's location in the file.
Ctrl-left arrow brings you back in this case.
If there is nothing at the caret location,
a text-entry search field is popped up which currently accepts a line number 
or a symbol in the codefile as input.
More navigational features will be added using this search mechanism.

If you press just Ctrl, it pops up a small menu of features
which can be selected using the Up/Down arrow keys
and exectued using Return(Enter)
Most of these are currently unimplemented.
One of these allows you to adjust the font size.
This is useful for OSX where the font size maybe off depending on your screen.
You adjust fontsize using the arrow keys and escape when you're done.

Pressing escape in any operational context (mode) exits that context.
If that context (mode) happens to be the primary mode, it exits the app.

When you exit the app, it saves its current state in a file called state.hxp.
When you reload the app, it reads this file to load the state at last exit.
If you delete this file, the app loads afresh.

# Future Plans:

1. We plan to add source editing features to the source editor mode.
2. Then add the ability to compile a Modal source code file and correct compilation errors.
This will be based on gcc (Win, Linux) and g++ on OSX.
3. Then add the ability to debug a modal app.
This will be based on gdb (Win, Linux) and lldb on OSX.
4. We plan to add more source code navigation features.
5. We plan to create ModalWin32, a native version of the Modal UI toolkit for Win32.
6. We plan to create ModalX a native version of the Modal UI toolkit for X-Windows on Linux.
7. We plan to create a toolkit based on Modal called Hex-map 
for designing kybd-only apps on a hexagonal grid.

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
It contains a stack of modes, methods to push and pop modes and get the current mode.
It also contains a method to serialize its current state to a file.
This enables the app to save the current state of the UI when it exits
and reload it when the app is relaunched.
ModalWindow calls modal_init in its constructor
where you initializes the mode manager member variable of ModalWindow.
ModalWindow calls modal_exit in its destructor
where you serialize the mode manager to a file and then free it.
3. SMode which is a C++ struct for representing the base properties of a mode of operation.
It contains a Union UModeExtension for representing mode specific extensions
to the base mode.
The Modal Toolkit provides a few extensions to the base mode for common UI operations.
These are:
a)   SModeMsg : a mode extension for displaying a user message
b)   SModeLineInp : a mode extension getting a line of user input 
c)   SModeLevAdj : a mode extension for enabling the user to adjust
some level.
d)   SModeFileSel : a mode extension for selecting a file from the file system

Each modal app defines its own mode extension
which implements the primary UI function of that app.
For example ModalWX.cpp defines SModeSrcEdr, 
a mode extension for browsing a Modal app's source code.
SModeSrcEdr uses SModeMsg, SModeLineInp, SModeLevAdj and SModeFileSel in its implementation
to realize the desired behavior of its UI.

# How a Modal UI operates / Designing a mode of operation:

The main task in creating a Modal app is
designing a mode of operation that specifically caters to the app's primary function.
ModalWX.cpp and its design of the SModeSrcEdr mode of operation
serves as a template for designing a mode of operation for some other purpose.

## User intents and intent handlers:

While a mode of operation defines the behavior of the app 
during an interaction session with the user,
the behavior of the app at any given instant in time is defined by an intent handler.
An intent is the user's intention of performing some specific action 
in the context of the current mode of operation.
The user expresses this intent through a specific kybd gesture.
This gesture is detected by the OS as a key event,
dispatched to the wxWindow currently in focus which is a ModalWindow.
The ModalWindow in its key event handler dispatches the key event to the mode manager
which determines the mode at the top of its mode stack and dispathes the event to that mode.
The mode has a fnKybd_map function which processes this event.
In its fnKybd_map function, the mode determines the intent of the user
based on the key strokes it received
and dispatches the intent to be processed by a corresponding intent handler.
![alt text](https://hex-map.khitchdee.net/intent-dispatch.png)
Designing a mode therefore involves designing a set of user intents,
each one of which is identified by specific user keystrokes,
and implementing the apps expected behavior in response to a given user intent
in an intent handler.
Typically, user intents are designed and implemented sequentially --
the primary intents are designed and implemented first
then secondary intents are added on.
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
SMode contains fnIntent_dispatch[MAX_INTENTS], a fixed size array of intent dispatch functions.
As an app designer designs intents for a given mode of operation,
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
creating a new Mode of Operation is fairly straightforward.
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

# A note on code quality
I've only just gotten this code to work.
There are a several hardcoded values in the code
and several places where error conditions are not checked.
This code has not been tested and is very brittle.
It's posted as a proof-of-concept demo.
However, being a source code toolkit,
and since the code is small and relatively simple,
you can add error-checking to core parts
and do a lot of testing on your targeted platforms
to make it suitable for deployment in a production environment.
wxWidgets, which this code uses to access platform features
is a fairly robust and well tested library.
We also plan to create native versions of Modal for Win32 and X-Windows
which will remove this dependency (on Windows and Linux).

