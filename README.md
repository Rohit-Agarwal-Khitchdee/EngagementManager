# EngageUI
## Motivation
EngageUI is a new UI paradigm, a replacement for the WIMP UI paradigm.  

WIMP stands for Windows, Icons, Menus and Pointers and is the predominant paradigm  
used in the design of OS UIs and GUIs for desktop PC apps.  
An OS takes the WIMP UI paradigm and uses it to implement  
the OS interface with the desktop metaphor  
and provides a library and tool for writing apps  
that also conform to this metaphor.  
WIMP conformant apps use the control panel metaphor.  
Control panels with switches, displays and other controls  
were and are still used to control industrial machines.  
The contol panel metaphor is a virtual version of this physical interface.  
It consists of familiar controls (buttons, text input fields, data displays etc)  
laid out inside a control panel (window).  
Different OSs such as OSX, Windows and Ubuntu  
differentiate themselves in their choice of graphics, color schemes  
layout approaches etc in their WIMP conformant implementations of their UIs.  
What all this means to an app developer is  
the 3 major OSs offer similar functionality when it comes to GUI design  
and you are straghtjacketed in terms of what you can do  
by the control panel paradigm of UI design.  

The command line interface (CLI) is another UI approach that is popular on Linux.  
Such a UI does not have a graphical component  
but instead relies on putting together several orthogonal CLI apps   
to achieve a desired result which is typically a data processing operation.  

EngageUIs focus on user activity in the design of a UI and in its operation.  
They do not need to use the mouse and are typically keyboard-driven.  
They stand somewhere between CLIs and WIMP based GUIs.  
Emacs and the VIM editor are examples of keyboard-driven GUI apps.  
These 2 apps suffer from the drawback that they require the user  
to learn a large set of keyboard commands.  
Apps designed using EngageUI need only a handful of commands to be learned.  
This is because there is a mechanism provided   
to pop-up keyboard selectable on-screen controls for less often used commands  
that don't have to be remembered.  

## What is an EngageUI?
![Less cluttered screens](https://hex-map.khitchdee.net/WIMPvsModal.png?v08-18-2022)  
An EngageUI is an activity based UI paradigm that consist of  
a sequence of interactive screens called "activity contexts".  
Each activity context has access to the entire screen   
and exclusive control over user-input while it is loaded.  
A primary activity-context writes to the full-screen  
while a pop-up activity-context loaded by, and pops up in front of,  
a primary activity-context.  

![Alt Text](https://hex-map.khitchdee.net/modal-illustration.png?v08-21-2022)

An activity-context consists of a set a "user intents",  
which is an input gesture that expresses intent to do something   
and an intent handler, that performs the action intended by the user.  

## EngageWX: The EngageUI design toolkit for wxWidgets
EngageWX.cpp is a "source-code toolkit" for designing actity-based Engage UIs.  
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
A developer should customize it for their specific code writing workflow.  

Brief screen-capture demo of an EnageUI app.  
EngageWX source code navigation app is being used to navigate EngageWX.cpp.  
Note that the entire client area is used for displaying source code.  
Using a 3 column format, ~120 LOC can be displayed at a time.  
There are also no on-screen navigational controls  
All navigational controls are direct-mapped to keystroke sequences.   
![alt text](https://hex-map.khitchdee.net/ModalWX.gif?v08-22-2022)  

## Building EngageWX:
(interaction time ~1hr) 
Should you build this app?  
1. If you want to build a simple keyboard-driven app using wxWidgets.  
2. If you are already a wxWidgets developer.  
The process is relatively brief in this case  
and this toolkit gives you a GUI design alternative to WIMP  
that you can start using right away.  

EngageWX.cpp uses the wxWidgets cross-platform UI library.  
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
Then you can edit the project settings to replace minimal.cpp with ModalWX.cpp.  
This will build the EngageWX app.  

### Windows:
We recommend Visual Studio 2022 Community edition as the IDE.  
You download the source code for wxWidgets.  
Then you goto %wxWidgetsDir%/build/msw/  
You open wx_vc17.sln in Visual Studio 2022  
Build Debug and Release configurations (we recommend not dll but statically linked libraries).  
This places the built libraries in %wxWidgetsDir%/lib/vc_x64_lib (or vc_lib).  
Follow the instructions at https://forums.wxwidgets.org/viewtopic.php?p=196105#p196105  
to create a new VisualStudio project and add EngageWX.cpp to it.  
This should build the EngageWX app.  

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
Add EngageWX.cpp and build and run the project.  

## Running EngageWX (UI controls):
When you first run the app, it will ask you for the full path of EngageWX.cpp.  
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
3. EngageUI's interface with wxWidgets -- EngageUIWindow and the ActivityContextManager.  
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
      load_UI_state is an acillary fn that creates a SACSrcEdr activity-context,  
      creates a SACManager, pushes the activity-context onto the ACManager,   
      and returns the ACManager.  
      This is returned by engageUI_init() to the calling EngageUIWindow.  
      Now press Ctrl-Left Arrow this takes you back to engageUI_init.  
      Go back again (Ctrl-left) that's EngageUIWindow's constructor  
      Go back again that's MyFrame's constructor  
      Go back again and we're at the entry point to the app.  
      Summarize MyApp::OnInit().  
      So a wxWidgets app enters at MyApp::OnInit, creates a MyFrame::wxFrame  
      which creates a EngageUIWindow::wxWindow, which contains a ACManager  
      that serves as the interface between wxWidgets and EngageUI.  
      EngageUIWindow initializes EngageUI using a designated engageUI_init() fn.  
      The EngageUI App designer initializes EngageUI in this fn  
      by creating a primary activity-context, in this case SACSrcEdr  
      and pushing it into the ACManager before returning the ACManager to EngageUIModalWindow.   

   - App lifetime -- Paint and Kybd event handling  
      ![Alt Text](https://hex-map.khitchdee.net/Modal-operation.png?v08-11-2022)  
      Goto line 723 EngageUIWindow::OnKeyDown and open it.  
      This is where all key down events are handled by EngageUIWindow.  
      Goto kybd_map  
      That takes you to the activity-context manager's kybd_map fn.  
      This function set's pWin->m_bUsrActn which tells EngageUI  
      that the user has done something.  
      Then, call's the kybd_map fn of the activity-context at the top of the AC stack  
      which is the currently active activity-context.  
     
      Go Back (Ctrl-left) and close EngageUIWindow::OnKeyDown.  
      Open EngageUIWindow::OnPaint the paint event handler for EngageUIWindow.  
      This fn calls either disp_state if the event was not caused by the user  
      in which case the state of the entire app needs to be reloaded.  
      or disp_update in which case the activity-context decides what needs to be updated.  
      Goto disp_state.  
      You can read the comments inside 529 then close it.  
      The activity context manager contains a stack of activity-contexts  
      and displays each AC's disp_state in back to front order (bottom to top of stack).  
      Go back.  
      Now goto disp_update and look that code and come back.  
      Note that disp_update only calls the AC at the top of the AC stack, the current AC.  

      So during execution stage, wxWidgets sends key down and paint events to EngageUIWindow.  
      EngageUIWindow delegates these to the activity-context manager  
      which dispatches them appropriately to activity-contexts it manages.  
  
   - App exit -- EngageUI shutdown and app state serialisation.    
    When an EngageUI app is ready to exit, it tells the wxWidgets app to shutdown  
    which results in ~EngageUIWindow being called.  
    Open 780. EngageUIWIndow::~EngageUIWindow and goto engageUI_exit(), study that code and return.  
    enagegUI_exit serializes the AC manager and all the ACs it contains to a state file.  
    Next time the app is launched, it reads state from this file   
    to reloads the last operational state of the app.  
    It also free's the ACmanager which in turn free's all the ACs it contains.  
    The ACmanager and the ACs are all created on the heap.  

    Press Escape to exit the app.  
    Then relaunch the app. You should be back where you left off.  
    Close all open fns and the sub-blocks. PgUp.  
    You're back at the app's start-screen.  
  
4. Inside EngageUI -- Activity Contexts and User Intents   
   - The EngageUI toolkit has been designed in such a way that for the most part  
    the developer does not need to have anything to do with it's operational context  
    in this case the wxWidgets WIMP GUI.  
    The GUI design of an EngageUI app is purely using EngageUI constructs  
    namely the activity-context and the user intent handler.  
    An EngageUI app developer is concerned with EnagegeUI init and exit,  
    their app's data and the primary activity context/s they design.  
    This design may use canned pop-up activity contexts from the toolkit.  
    This app is a EngageUI source code editor and navigator.  
    We will walkthrough the design of SACSrcEdr which is its primary activity context  
    and the user intent handlers that are part of this AC's design.  
  
   - Open INITIALIZING AND EXITING ENGAGEUI (BLOCK).  
    Open engageUI_init  
    Goto load_UI_state  
    Goto new_src_edr  
    Note we are inside the BLOCK: THIS APP'S PRIMARY ACTIVITY-CONTEXT, THE SOURCE EDITOR  
    and the SUB_BLOCK: BASE DEFINITIONS  
    This is where the SACSrcEdr struct is defined  
    and fns to "new", free and load from a file are defined.  
    Goto pBase->init  
    This is where an SActivityContext struct is initialized.  
    Open the comment block above (311), read the comments in it then close it.  
    The base mode struct has the following fn ptrs:  
     1. fnKey_up  
      This fn is called when a key up event occurs  
     2. fnKybd_map  
      Key down event. It maps the event to an intent and dispatches it  
      to an entry in the fnIntent_handlers array.  
     3. fnDisp_state  
      Paint event, It paints the base state of the activity-context associated with its data.  
     4. fnOn_engage  
      Called when the activity-context is pushed onto the ACmanager.
      Any initialization code for the activity-context goes here. 
     5. fnOn_disengage
      Called when the activity-context is popped off the ACmanager.
      Any exit code for the activity-context goes here.
     6. fnSerialize  
      Called by the ACmanager when activity-context's state needs to be loaded or stored.
      The activity-context stores to or loads from a file it's state.
     7. fnIntentHandler[40]  
      Called by fnKybd_map to initiate intent handling  
      and by ACmanager::disp_update to complete display update of the screen.

   - The base SActivityContext struct's init provides implementations for (i), (iv), and (v).  
    A concrete activity-context provides for the rest and may override the base (i), (iv) and (v).  
    Now go back to new_src_edr (Ctrl-left).  
    pSrcEdr->init.  
    As you can see, SACSrcEdr's init loads (i),(ii),(iii),(iv) and (vi) fn ptrs   
    with it's own implementation.  
    It also loads all the intent handler fns.  
    Open the enum{...} at 7247.  
    This is an enumeration of the intent handlers for SACSrcEdr.  
    They all start with SEI_ to make them globally unique.  
    Close the enum.  
    PgDn to void load_intents() {...}  
    Open it. This is where the intent handlers are loaded. Close it.
  
   - Now we'll peep inside some of these activity-context behavior implementation fns.  
    Scroll up to SrcEdr->init() and goto src_edr_map.  
    Open 7456.  
    Here src_edr_map is detecting the intent SEI_UPDATE_CARET and dispatching it.  
    Close 7456.  
    Let's look at that intent handler.  
    Go back. Go Back. Go Back. Go Back.  
    Close engageUI_init().  
    Close(BLOCK) INITIALIZING AND EXITING ENGAGEUI  
    Open (BLOCK) THIS APP'S PRIMARY ACTIVITY-CONTEXT, (SUB_BLOCK) INTENT_HANDLERS  
    Open src_edr_update_caret   
    This is the intent handler for SEI_UPDATE_CARET.  
    You may study this code then close it and its sub-block and block.  

    Finally we'll look at src_edr_disp_state.  
    You will find src_edr_disp_state inside  
    (BLOCK) THIS APP'S PRIMARY ACTIVITY_CONTEXT (SUB_BLOCK) ACTIVITY-CONTEXT IMPLEMENTATION FNs.  

Note:  
In navigating EngageUI source code, we only keep open  
the section of code we're currently studying.  
This is facilatated by the summarization mechanisms and the goto mechanisms.  

## Building your own EngageUI App  
We suggest taking the walkthrough in the section above before reading this section.  
A simple EngageUI app has a single activity-context  
which defines the behavior of the app from when it's launched till it is exited.  
Within this primary activity-context's interaction time  
pop-up activity-contexts may pop up and go away.  
The function of these pop-ups is to take care of common extensions  
to the base behavior of a primary activity-context.  
To create your own EngageUI app using EngageWX.cpp as a template,  
you define data structs for the data your app will process.  
Then you you define your primary activity-context that will process this data.  
As part of this definition, you may use pop-up activity-contexts provided by the toolkit.  
So, you would replace everything in (BLOCK) THIS APP's DATASTRUCTS  
and everything in (BLOCK) THIS APP'S PRIMARY ACTIVITY-CONTEXT  
with you own data structs and primary activity context.  
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

## Contributing to EngageWX
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
