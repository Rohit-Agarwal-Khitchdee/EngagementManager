# EngageUI
## Motivation
EngageUI is a simpler, more efficient, keyboard-focused approach   
to building PC desktop app GUIs     
than the prevalent WIMP, control-panel metaphor based approach.  
EngageUI apps get all or most of their input from the keyboard.  

## History
After several years of using PCs,  
we decided that the PC user interface would be better off  
if we didn't have to use the mouse in the interaction.  
However, after DOS, all operating system UIs were designed  
with the mouse in mind based on the use of WIMP  
which stands for Windows, Icons, Menus and Pointers.  
PC OSs use the desktop metaphor to organize the OS interface  
and for apps they use the control panel metaphor.  
Industrial machines had control panels with a bunch of switches, gauges etc.  
to control them.  
The control panel metaphor for PC apps derives from that physical counterpart.  
A WIMP app has windows (control panels)   
with a bunch of controls laid out spatially within them.  
Controls such as button, text input fields, info displays etc.  
The user interacts with these controls are selected using the pointer (mouse).  
OSs differentiate themselves in the look and feel of their controls and control panels.  
The control panel metaphor used in PC app design  
straigtjackets the app developer in terms of what they can design.  
Design is mostly a choice of which controls to use  
and how to lay them out inside Windows.  

## Design
The first question we had to answer was  
would it be possible to design an app's user interface without the use of a pointer  
for on-screen space selection.  
An analysis of WIMP apps showed that  
most apps did not require fine-grained screen space selection.  
The exceptions being graphics apps and CAD tools.  
Using the arrow keys on a keyboard,  
it would be possible to provide the coarse screen-space selection  
needed by most apps.  
A further analysis indicated that  
if we did away with the spatial control-panel metaphor  
it could be replaced with a temporal, user-activity based design metaphor.  
The EngageUI user interface toolkit that was designed as a result    
turned out to be simple and easy to implement.  
It is described later in this document.  

Then we set out to create an IDE (based on EngageUI) to design  
EngageUI apps.  
That's what we are working on now.  

## What is EngageUI?
![Less cluttered screens](https://hex-map.khitchdee.net/WIMPvsModal.png?v08-18-2022)  
EngageUI is an activity based UI paradigm  
that is always focused on the user's current activity when interacting with the UI.  
It consists of a sequence of interactive screens called "activity handlers".  
Each activity handler has access to the entire screen   
and exclusive control over user-input while it is loaded.  
A primary activity-handler writes to the full-screen  
while a pop-up activity-handler is loaded by, and pops-up in front of,  
a primary activity-handler.  
An activity manager (window) interfaces with the OSs windowing system  
and sends events to the activity handlers it manages.  

![Alt Text](https://hex-map.khitchdee.net/EngageUI-illustration.png?v08-23-2022)

A user activity consists of a set a "user intents",  
which is an input gesture that expresses intent to do something   
and an associated intent handler, that performs the action intended by the user.  
An activity handler therefore consists of a set of user intent handlers.  

## EngageIDE: The EngageUI IDE for wxWidgets
EngageIDE is an IDE for designing (activity-based) Engage UIs using wxWidgets.  
It has 2 parts:
1. Source code for the classes, structs and functions  
that comprise the core toolkit -- about 3000 LOC.  
This code can directly be used to create an EngageUI app.  
We support this toolkit and it's ready for use by developers.  
2. Code for an IDE for producing an EngageUI app (about 6000 LOC).  
The IDE will be fully functional in our next release which is Oct 06.  
As you would expect, the IDE itself is an EngageUI app and uses the toolkit.    
It does not use the mouse, all input controls are through the keyboard.  
This IDE has been designed for Khitchdee's internal use.  
A developer can customize it for their specific code writing workflow.  

We have designed this IDE to have the minimum features needed
for our own app development,
which is a suite of CAD tools for land-vehicle design.  
IDE features:  
1.  
This IDE has keyboard-only input, no mouse input is used.
So code editing, navigation, building and debugging are all done
using keyboard commands.  
2.  
It follows an "all in one place" approach to codebase organisation.
We don't break our codebase into separate .cpp with associated headers.
All the functions and classes are declared in a single .cpp file.
This file has in the 10s of Ks of lines of code.
How do we navigate such a large codefile?  
3.  
We introduce 2 navigational constructs above the language level
The Block and the Sub-block.
These demarcators for these constructs are contained inside comment blocks. 
We make it very easy to fold code using a single keyboard command
and blocks and sub-blocks can also be folded. 
Large comment blocks can be included in the code
without interfering with code readability
by keeping them folded until they need to be read.
This helps create an efficient process for code documentation.  
4.  
The entire display area is used to display code in a 3 column format
with a wider active center column and read-only side columns.
This makes it possible to view 120 lines of code on a hi-res screen.
At any given time, only one section of code is kept open (not folded)
and a mechanism is provided to jump to (or back from) the definition of any symbol.
This facilitates keeping only one section of code open at a time.  
5.  
This IDE has been designed to develop EngageUI apps.  

## Building EngageIDE:
(interaction time ~1hr) 
Should you build this app?  
1. If you want to build a simple keyboard-driven app using wxWidgets.  
2. If you are already a wxWidgets developer.  
The process is relatively brief in this case  
and this toolkit gives you a GUI design alternative to WIMP  
that you can start using right away.  

EngageIDE.cpp uses the wxWidgets cross-platform UI library.  
To build it you first have to download and build the wxWidgets library.  

We recommend using the current development version of wxWidgets which is 3.2.0.  
You can get help for setting up wxWidgets on your PC from this forum:  
https://forums.wxwidgets.org/viewforum.php?f=19&sid=0083f4684647607be2aef5bc34b48d82.  
The build process for the library depends on your platform:  

### OSX: 
We recommend building the library from source.  
The simplest way is to download the source for wxWidgets.  
Then open %wxWidgetsDir%/Samples/minimal/minimal_cocoa.xcodeproj in XCode.  
You should be able to build and run this sample.  
It builds the wxWidgets library from source as part of its build process.  
Then you can edit the project settings to replace minimal.cpp with EngageIDE.cpp.  
This will build the EngageIDE app.  

### Windows:
We recommend Visual Studio 2022 Community edition as the IDE.  
You download the source code for wxWidgets.  
Then you goto %wxWidgetsDir%/build/msw/  
You open wx_vc17.sln in Visual Studio 2022  
Build Debug and Release configurations (we recommend not dll but statically linked libraries).  
This places the built libraries in %wxWidgetsDir%/lib/vc_x64_lib (or vc_lib).  
Follow the instructions at https://forums.wxwidgets.org/viewtopic.php?p=196105#p196105  
to create a new VisualStudio project and add EngageIDE.cpp to it.  
This should build the EngageIDE app.  

### Linux:
We recommend using the CodeLite IDE.  
After installing codelite from codelite.org,  
Follow the instructions at:  
https://docs.codelite.org/wxWidgets/repo320/
to install wxWidgets 3.2.0.    
Now, run Codelite and create a new workspace of Type C++.  
Then add a new project to the workspace of Category: GUI  
and Type: wxWidgets GUI Application (wxFrame, using wxCrafter)  
with Build System: Default  
Build the project.  
Run the project, you should see a small minimized window  
with title My Frame and File and Help menu items.  
Close this window.  
Open your project and delete all the src, resource and include files.  
At this point your project is empty but configured for wxWidgets.   
Go to src, right click, add an existing file.  
Add EngageIDE.cpp and build and run the project.  

## Running EngageIDE (UI controls):
When you first run the app, it will ask you for the full path of EngageIDE.cpp.  
This path will depend on your platform.  
Examples:  
OSX -- /Users/$username$/EngageUI/EngageWX.cpp  
Win -- C:\EngageUI\EngageWX.cpp  
Linux -- /Home/$username$/EngageUI/EngageWX.cpp  
![alt text](https://hex-map.khitchdee.net/ModalWX-launch-screen.png)  
If you enter an incorrect path, it will bring up a file selector  
to select the file from the file system.  
![alt text](https://hex-map.khitchdee.net/ModalWX-file-selector.png)  
The file selector has upto 5 vertical panels on the screen.  
Each panel represents the contents of a directory.  
The left most panel is closest to the root directory.  
Each panel is the contents of a directory from the left adjacent panel. 
The active panel has a red outline rectangle highlighting the current selection  
while the other panels have black outline rectangles.
The current selection is displayed at the top-center of the window.  
To change your selection in the active panel, use up and down arrow.  
To open a currently selected directory, use right arrow.  
To open or navigate to a parent directory, use left arrow.  
To commit to a selection (it has to be EngageWX.cpp in this case)  
press Enter(Return).  
If you make an invalid selection, it gets you back to the file selector.  
Once you have correctly selected EngageWX.cpp,  
it will load EngageWX.cpp, parse it and display its blocks.  
Note: On OSX the OS ask you to give permission to EngageWX   
to access certain folders in your home dir.  
You should give this permission.  

If you are interested in understanding how the code works  
take the code walkthough in the next section.  
Otherwise, basic navigational controls are as follows:
1. Arrows and PgUp/PgDn to move the caret.  
2. Any line ending in {...} can be opened with Ctrl(Command)-S.  
An open section can be summarized using the same command.
3. You can jump to any symbol using Ctrl-Right and return using Ctrl-Left.  
4. Escape exits the app.  
5. Pressing and releasing Ctrl pops-up a small menu.  
One of the options enables you to adjust the font size.  

## EngageWX Code Walkthrough
  (interaction time ~30min)   
We guide you through a sequence of steps  
that explain how this code works and how it is navigated.  
The only pre-requisite is familiarity with C/C++.  
At the end of this walkthough you will be able to evaluate  
whether EngageWX offers a better way to navigate a codebase  
compared to the IDE you currently use.  
Also, it will enable you to understand the code in this toolkit.  
This will enable you to design and produce your own EngageUI app.  

1. Build and run the app. as described in the preceding section.  
At this point, your screen should look like this:  
![alt text](https://hex-map.khitchdee.net/ModalWX-source-loaded.png)
2. Font size adjustment.  
If you find the fontsize too small,  
press the Command(OSX) or Ctrl(Win,Linux) key once.  
This will pop-up a set of 5 options.  
Use the down arrow key to select "Adjust Fontsize".  
Press Enter(Return).  
You will get a message:  
"arrows to adjust fontsize, esc to exit"  
Use the arrow keys to adjust fontsize.  
This is a live ajdustment.  
Once you are satisfied, press escape.  
3. EngageUI's interface with wxWidgets -- EngageUIWindow and the UserActivityManager.  
   - App init  
      Use the down arrow key to goto line WX APP & CLASS FUNCTION DEFINITIONS{...}.  
      All lines in this color are "sub-Blocks".  
      This line is a sub-block of line WX INTERFACING BOILERPLATE.  
      That line is a "block". All lines in that color are BLOCKS.  
      Press Ctrl-S(Command-S on OSX) at line WX APP & CLASS FUNCTION DEFINITIONS.  
      This will open the sub-block.  

      Open Line: bool MyApp::OnInit(){...}  
      Read through this code, then close it.  
      Goto line 759 and move the caret to MyFrame (the one after the new).  
      Press Ctrl-Right Arrow.  
      This will take you the constructor of MyFrame.  
      Goto line 768 move the caret over EngageUIWindow and goto (Ctrl-Right).  
      EngageUIWindow is a subclass of wxWindow defined in (sub-block) WX BRIDGE STRUCTURES AND FUNCTIONS.      
      Goto line 776 and move the caret over engageUI_init and goto.  
      engageUI_init is where the EngageUI is initalised.  

      Note that goto using ctrl-right goes to the destination, expands it  
      and collapses the previous viewing context.  
      Also, any line that ends in {...} is summarized and can be opened.  
      Conversely, any line that end's in a { can be summarized.

      Goto line 9072 and move the caret over load_UI_state and goto  
      load_UI_state is an acillary fn that creates a SACHSrcEdr activity-context handler,  
      creates a SUAManager, pushes the activity-context handler onto the UAManager,   
      and returns the UAManager.  
      This is returned by engageUI_init() to the calling EngageUIWindow.  
      Now press Ctrl-Left Arrow this takes you back to engageUI_init.  
      Go back again (Ctrl-left) that's EngageUIWindow's constructor  
      Go back again that's MyFrame's constructor  
      Go back again and we're at the entry point to the app.  
      Summarize MyApp::OnInit().  
      So a wxWidgets app enters at MyApp::OnInit, creates a MyFrame::wxFrame  
      which creates a EngageUIWindow::wxWindow, which contains a UAManager  
      that serves as the interface between wxWidgets and EngageUI.  
      EngageUIWindow initializes EngageUI using a designated engageUI_init() fn.  
      The EngageUI App designer initializes EngageUI in this fn  
      by creating a primary activity-context handler, in this case SACHSrcEdr  
      and pushing it into the UAManager before returning the UAManager to EngageUIModalWindow.   

   - App lifetime -- Paint and Kybd event handling  
      ![Alt Text](https://hex-map.khitchdee.net/Modal-operation.png?v08-11-2022)  
      Goto line 723 EngageUIWindow::OnKeyDown and open it.  
      This is where all key down events are handled by EngageUIWindow.  
      Goto kybd_map  
      That takes you to the user activity manager's kybd_map fn.  
      This function set's pWin->m_bUsrActn which tells EngageUI  
      that the user has done something.  
      Then, call's the kybd_map fn of the activity-context handler 
      at the top of the ACH stack  
      which is the currently active activity-context handler.  
     
      Go Back (Ctrl-left) and close EngageUIWindow::OnKeyDown.  
      Open EngageUIWindow::OnPaint the paint event handler for EngageUIWindow.  
      This fn calls either disp_state if the event was not caused by the user  
      in which case the state of the entire app needs to be reloaded.  
      or disp_update in which case the activity-context handler decides what needs to be updated.  
      Goto disp_state.  
      You can read the comments inside 529 then close it.  
      The user activity manager contains a stack of activity-context handlers  
      and displays each handler's disp_state in back to front order (bottom to top of stack).  
      Go back.  
      Now goto disp_update and look that code and come back.  
      Note that disp_update only calls the handler at the top of the ACH stack, the current handler.  

      So during execution stage, wxWidgets sends key down and paint events to EngageUIWindow.  
      EngageUIWindow delegates these to the user activity manager  
      which dispatches them appropriately to activity-context handlers it manages.  
  
   - App exit -- EngageUI shutdown and app state serialisation.    
    When an EngageUI app is ready to exit, it tells the wxWidgets app to shutdown  
    which results in ~EngageUIWindow being called.  
    Open 780. EngageUIWIndow::~EngageUIWindow and goto engageUI_exit(), study that code and return.  
    enagegUI_exit serializes the UA manager and all the ACHs it contains to a state file.  
    Next time the app is launched, it reads state from this file   
    to reloads the last operational state of the app.  
    It also free's the UAmanager which in turn free's all the ACHs it contains.  
    The UAmanager and the AC Handlers are all created on the heap.  

    Press Escape to exit the app.  
    Then relaunch the app. You should be back where you left off.  
    Close all open fns and the sub-blocks. PgUp.  
    You're back at the app's start-screen.  
  
4. Inside EngageUI -- Activity Contexts and User Intents   
   - The EngageUI toolkit has been designed in such a way that for the most part  
    the developer does not need to have anything to do with it's operational context  
    in this case the wxWidgets WIMP GUI.  
    The GUI design of an EngageUI app is purely using EngageUI constructs  
    namely the activity-context handler and the user intent handler.  
    An EngageUI app developer is concerned with EngageUI init and exit,  
    their app's data and the primary activity context handler/s they design.  
    This design may use canned pop-up activity context handlers from the toolkit.  
    This app is a EngageUI source code editor and navigator.  
    We will walkthrough the design of SACHSrcEdr which is its primary activity context handler  
    and the user intent handlers that are part of this ACH's design.  
  
   - Open INITIALIZING AND EXITING ENGAGEUI (BLOCK).  
    Open engageUI_init  
    Goto load_UI_state  
    Goto new_src_edr  
    Note we are inside the BLOCK: THIS APP'S PRIMARY ACTIVITY-CONTEXT HANDLER, THE SOURCE EDITOR  
    and the SUB_BLOCK: BASE DEFINITIONS  
    This is where the SACHSrcEdr struct is defined  
    and fns to "new", free and load from a file are defined.  
    Goto pBase->init  
    This is where an SACHandler struct is initialized.  
    Open the comment block above (311), read the comments in it then close it.  
    The base ACHandler struct has the following fn ptrs:  
     1. fnKey_up  
      This fn is called when a key up event occurs  
     2. fnKybd_map  
      Key down event. It maps the event to an intent and dispatches it  
      to an entry in the fnIntent_handlers array.  
     3. fnDisp_state  
      Paint event, It paints the base state of the activity-context handler associated with its data.  
     4. fnOn_engage  
      Called when the activity-context is pushed onto the UAmanager.
      Any initialization code for the activity-context handler goes here. 
     5. fnOn_disengage
      Called when the activity-context handler is popped off the UAmanager.
      Any exit code for the activity-context handler goes here.
     6. fnSerialize  
      Called by the UAmanager when activity-context handler's state needs to be loaded or stored.
      The activity-context handler stores to or loads from a file it's state.
     7. fnIntentHandler[40]  
      Called by fnKybd_map to initiate intent handling  
      and by UAmanager::disp_update to complete display update of the screen.

   - The base SACHandler struct's init provides implementations for (i), (iv), and (v).  
    A concrete activity-context handler provides for the rest and may override the base (i), (iv) and (v).  
    Now go back to new_src_edr (Ctrl-left).  
    pSrcEdr->init.  
    As you can see, SACHSrcEdr's init loads (i),(ii),(iii),(iv) and (vi) fn ptrs   
    with it's own implementation.  
    It also loads all the intent handler fns.  
    Open the enum{...} at 7247.  
    This is an enumeration of the intent handlers for SACHSrcEdr.  
    They all start with SEI_ to make them globally unique.  
    Close the enum.  
    PgDn to void load_intents() {...}  
    Open it. This is where the intent handlers are loaded. Close it.
  
   - Now we'll peep inside some of these activity-context handler behavior implementation fns.  
    Scroll up to SrcEdr->init() and goto src_edr_map.  
    Open 7456.  
    Here src_edr_map is detecting the intent SEI_UPDATE_CARET and dispatching it.  
    Close 7456.  
    Let's look at that intent handler.  
    Go back. Go Back. Go Back. Go Back.  
    Close engageUI_init().  
    Close(BLOCK) INITIALIZING AND EXITING ENGAGEUI  
    Open (BLOCK) THIS APP'S PRIMARY ACTIVITY-CONTEXT HANDLER, (SUB_BLOCK) INTENT_HANDLERS  
    Open src_edr_update_caret   
    This is the intent handler for SEI_UPDATE_CARET.  
    You may study this code then close it and its sub-block and block.  

    Finally we'll look at src_edr_disp_state.  
    You will find src_edr_disp_state inside  
    (BLOCK) THIS APP'S PRIMARY ACTIVITY_CONTEXT HANDLER (SUB_BLOCK) ACTIVITY-CONTEXT HANDLER IMPLEMENTATION FNs.  

Note:  
In navigating EngageUI source code, we only keep open  
the section of code we're currently studying.  
This is facilatated by the summarization mechanisms and the goto mechanisms.  

## Building your own EngageUI App  
We suggest taking the walkthrough in the section above before reading this section.  
A simple EngageUI app has a single activity-context handler  
which defines the behavior of the app from when it's launched till it is exited.  
Within this primary activity-context's interaction time  
pop-up activity-context handlers may pop up and go away.  
The function of these pop-ups is to take care of common extensions  
to the base behavior of a primary activity-context handler.  
To create your own EngageUI app using EngageWX.cpp as a template,  
you define data structs for the data your app will process.  
Then you you define your primary activity-context handler that will process this data.  
As part of this definition, you may use pop-up activity-context handlers provided by the toolkit.  
So, you would replace everything in (BLOCK) THIS APP's DATASTRUCTS  
and everything in (BLOCK) THIS APP'S PRIMARY ACTIVITY-CONTEXT HANDLER
with you own data structs and primary activity context handler.  
For now, you can do this using your existing IDE.  
Our next release due Oct 06 will make EngageWX an IDE.  

## Next Development Step:
1. Source code editing.  
2. Build and fix compile/link time errors and warnings.  
3. Debug.  
This will make EngageWX a standalone IDE for building EngageUI apps based on wxWidgets.  
I estimate this will take 3 months so next release is scheduled for Thursday, Oct 06.  

## EngageUI & WIMP mix and match
Since an EngageUI is implemented entirely within a wxWindow subclass,  
it is possible to incorporate an EngageUI window into a regular WIMP style app  
using wxAUI with the EngageUI in one layer and a WIMP UI in another layer.  
Effectively mixing these 2 styles of UI design.  

## A note on code quality
I've only just gotten this code to work.  
There are a several hardcoded values in the code  
and several places where error conditions are not checked.  
This code has not been tested and is very brittle.  
It's posted as a proof-of-concept demo.  
However, the source code toolkit part of the code is relatively stable,    
since the code is small and relatively simple.    
wxWidgets, which this code uses to access platform features  
is a fairly robust and well tested library.  

## Contributing to EngageIDE
I've written about 9000 lines of code so far.  
So I have fairly stable code design, code documentation and coding conventions.  
I welcome all contributions.    
Since this is an IDE, relatively simple and has a small and well-documented codebase   
it is amenable to customisations.  
I welcome developers sharing their customisations with this project.  
Some areas where work could be done are
1. Sample apps. i.e. More primary activity-contexts.
2. A different approach to parsing source code files.  
3. Dictionary based text input.
4. More pop-up activity contexts.  
