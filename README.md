# Modal GUIs
## What is a Modal GUI?
![Less cluttered screens](https://hex-map.khitchdee.net/WIMPvsModal.png?v08-08-2022)  
Modal GUIs are time-sliced into "modes of operation".  
Each mode of operation paints to the entire screen   
and has exclusive control over user-input while it has focus.  
A mode can be primary which writes to the full-screen  
or pop-up which pops up in front of a primary mode.  
A mode is time-sliced into a set a "user intent" units,    
A user intent is the user's expression of intent to do something   
expressed as a specific input gesture,  
and an intent handler, that performs the desired action, for each intent it defines.  

The primary mode corresponds to the user's primary activity.  
Within this primary interaction context, transient modes may pop-up and then go away.  
In terms of interaction "intensity"  
all high-intensity user intents that are frequently used and require  immediacy  
are direct-mapped to keystroke-sequences.  
Lower intensity intents that are less frequently needed  
are indirect-mapped to transient, pop-up modes.  

This app (ModalWX.cpp) is a Modal GUI source-code navigation/editing app  
that serves 3 purposes for this source-code toolkit.  
1. It enables the dev to navigate and understand the source code of this toolkit.  
2. Its source code serves as a template for designing a modal GUI app.  
Specifically, this app's primary mode, SModeSrcEdr has to be replaced  
with a primary mode designed by the dev  
and the data structs used by SModeSrcEdr have to be replaced  
with their app-specific data structs.
3. It's source-code navigation efficiency can be compared with the IDE you currently use.  
It serves as an example that demonstrates a Modal GUI's advantage over a WIMP GUI.   

Screenshot of the ModalWX source code navigation app navigating ModalWX.cpp.  
Note that the entire client area is used for displaying source code.  
Using a 3 column format, ~120 lines can be displayed at a time.  
There are also no on-screen navigational controls  
because all navigational controls are direct-mapped to keystroke sequences.   
![alt text](https://hex-map.khitchdee.net/ModalWX_navigating_ModalWX.cpp.png)  

## Building the ModalWX app:
(interaction time ~1hr) 

The ModalWX app is a Modal GUI based navigation tool for Modal source code.  
A future version will be a standalone IDE.  
The app is based on the wxWidgets cross-platform UI library.  
To build this app you first have to download and build the wxWidgets library.  

We recommend using the current development version of wxWidgets which is 3.2.0.  
You can get help for setting up wxWidgets on your PC from this forum:  
https://forums.wxwidgets.org/viewforum.php?f=19&sid=0083f4684647607be2aef5bc34b48d82.  
The build process for the library depends on your platform:  

### OSX: 
We recommend building the library from source.  
The simplest way is to download the source for wxWidgets.  
Then open %wxWidgetsDir%/Samples/minimal/minimal_cocoa.xcodeproj  
You should be able to build and run this sample in XCode.  
It builds the wxWidgets library from source as part of its build process.  
Then you can edit the project settings to replace minimal.cpp with ModalWX.cpp.  
This will build the ModalWX app.  

### Windows:
We recommend Visual Studio 2022 Community edition as the IDE.  
You download the source code for wxWidgets.  
Then you goto %wxWidgetsDir%/build/msw/  
You open wx_vc17.sln in Visual Studio 2022  
Build Debug and Release configurations (we recommend not dll but statically linked libraries).  
This places the built libraries in %wxWidgetsDir%/lib/vc_x64_lib (or vc_lib).  
Follow the instructions at https://forums.wxwidgets.org/viewtopic.php?p=196105#p196105  
to create a new VS project and add ModalWX.cpp to it.  
This should build the ModalWX app.  

### Linux:
We recommend using the CodeLite IDE which comes with wxWidgets pre-installed.  
After installing codelite from codelite.org,  
Follow the instructions at:  
https://docs.codelite.org/wxWidgets/repo320/
to install wxWidgets 3.2.0.    
Now, run Codelite and create a new workspace of type C++.  
Then add a new project to the workspace of Category: GUI  
and type: wxWidgets GUI Application (wxFrame, using wxCrafter)  
with Build System: Default  
Build the project.  
Run the project, you should see a small minimized window  
with title My Frame and File and Help menu items.  
Close this window.  
Open your project and delete all the src, resource and include files.  
At this point your project is empty but configured for wxWidgets.   
Go to src, right click, add an existing file.  
Add ModalWX.cpp and build the project.  
You may get several warnings but no errors.  
Run the project. This should run ModalWX.  

## Running the ModalWX app (UI controls):
The app's function is to navigate a ModalWX codefile.    
ModalWX.cpp is an example of a Modal codefile.  
When you run the app,  
it will ask you for the full path of ModalWX.cpp.  
This path will depend on your platform.  
Examples:  
OSX -- /Users/<username>/Modal/ModalWX.cpp  
Win -- C:\Modal\ModalWX.cpp  
Linux -- /Home/Modal/ModalWX.cpp  
![alt text](https://hex-map.khitchdee.net/ModalWX-launch-screen.png)  
If you enter an incorrect path, it will bring up a file selector  
to select the file from the file system.  
![alt text](https://hex-map.khitchdee.net/ModalWX-file-selector.png)  
The file selector has upto 5 vertical panels on the screen.  
Each panel represents the contents of a directory.  
The left most panel is closest to the root directory.  
Each panel is the contents of a directory from the left adjacent panel. 
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

## ModalWX Code Walkthrough
  (interaction time ~30min)  
We guide you through a sequence of steps  
that explain how this code works and how it is navigated.  
The only pre-requisite is familiarity with C.  
At the end of this walkthough you will be able to evaluate  
whether ModalWX offers a better way to navigate a codebase  
compared to the IDE you currently use.  
Also, it will enable you to understand the code in this toolkit.  
This will enable you to design and produce your own Modal GUI app.  

1. Build and run the app. as described in the preceding section.  
At this point, your screen should look like this:  
![alt text](https://hex-map.khitchdee.net/ModalWX-source-loaded.png)
2. OSX font size adjustment.  
If you are on OSX, the fontsize might be too small.  
To adjust it, press the Command key once.  
This will pop-up a set of 5 options.  
Use the down arrow key to select "Adjust Fontsize".  
![alt text](https://hex-map.khitchdee.net/ModalWX-adjust-fontsize.png)  
Press Enter(Return).  
You will get a message:  
"arrows to adjust fontsize, esc to exit"  
Use the arrow keys to adjust fontsize.  
This is a live ajdustment.  
Once you are satisfied, press escape.  
3. Modal's interface with wxWidgets -- ModalWindow and the ModeManager.  
   - App init  
      Use the down arrow key to goto line WX APP & CLASS FUNCTION DEFINITIONS{...}.  
      All lines in this color are "sub-Blocks".  
      This line is a sub-block of line WX INTERFACING BOILERPLATE.  
      That line is a BLOCK. All lines in that color are BLOCKS.  
      Press Ctrl-S(Command-S on OSX) at line WX APP & CLASS FUNCTION DEFINITIONS.  
      This will open the sub-block.  

      Open Line: bool MyApp::OnInit(){...}  
      Read through this code, then close it.  
      Goto line 759 and move the caret to MyFrame (the one after the new).  
      Press Ctrl-Right Arrow.  
      This will take you the constructor of MyFrame.  
      Goto line 768 move the caret over ModalWindow and goto (Ctrl-Right).  
      ModalWindow is a subclass of wxWindow defined in (sub-block) WX BRIDGE STRUCTURES AND FUNCTIONS.      
      Goto line 776 and move the caret over modal_init and goto.  
      modal_init is where the modal toolkit is initalised.  

      Note that goto using ctrl-right goes to the destination, expands it  
      and collapses the previous viewing context.  
      Also, any line that ends in {...} is summarized and can be opened.  
      Conversely, any line that end's in a { can be summarized.

      Goto line 9072 and move the caret over load_UI_state and goto  
      load_UI_state is an acillary fn that creates a SModeSrcEdr mode,  
      creates a SModeManager, pushes the mode onto the mode manager,   
      and returns the mode manager.  
      This is returned by modal_init() to the calling ModalWindow.  
      Now press Ctrl-Left Arrow this takes you back to modal_init.  
      Go back again (Ctrl-left) that's ModalWindow's constructor  
      Go back again that's MyFrame's constructor  
      Go back again and we're at the entry point to the app.  
      Summarize MyApp::OnInit().  
      So a wxWidgets app enters at MyApp::OnInit, creates a MyFrame::wxFrame  
      which creates a ModalWindow::wxWindow, which contains a ModeManager  
      that serves as the interface between wxWidgets and Modal.  
      ModalWindow initializes Modal using a designated modal_init() fn.  
      The Modal App designer initializes modal in this fn by creating a primary mode  
      in this case SModeSrcEdr and pushing it into the ModeManager  
      before returning the ModeManager to ModalWindow.   

   - App lifetime -- Paint and Kybd event handling  
      Goto line 723 ModalWindow::OnKeyDown and open it.  
      This is where all key down events are handled by ModalWindow.  
      Goto kybd_map  
      That takes you to the modemanager's kybd_map fn.  
      This function set's pWin->m_bUsrActn which tells Modal  
      that the user has done something.  
      Then, call's the kybd_map fn of the mode at the top of the mode stack  
      which is the currently active mode.  
     
      Go Back (Ctrl-left) and close ModalWindow::OnKeyDown.  
      Open ModalWindow::OnPaint the paint event handler for ModalWindow.  
      This fn calls either disp_state if the event was not caused by the user  
      in which casethe state of the entire app needs to be reloaded.  
      or disp_update in which case the mode decides what needs to be updated.  
      Goto disp_state.  
      You can read the comments inside 529 then close it.  
      The mode manager contains a stack of modes  
      and displays each modes disp_state in back to front order (bottom to top of stack).  
      Go back.  
      Now goto disp_update and look that code and come back.  
      Note that disp_update only calls the mode at the top of the mode stack, the current mode.  

      So during execution stage, wxWidgets sends key down and paint events to ModalWindow.  
      ModalWindow delegates these to the modemanager  
      which dispatches them appropriately to modes it manages.  
  
   - App exit -- Modal shutdown and app state serialisation.    
    When a modal app is ready to exit, it tells the wxWidgets app to shutdown  
    which results in ~ModalWIndow being called.  
    Open 780. ModalWIndow::~ModalWindow and goto modal_exit(), study that code and return.  
    modal_exit serializes the mode manager and all the modes it contains to a state file.  
    Next time the app is launched, it reads state from this file   
    to reload the last operational state of the app.  
    It also free's the mode manager which in turn free's all the modes it contains.  
    The mode manager and the modes are all created on the heap.  

    Press Escape to exit the app.  
    Then relaunch the app. You should be back where you left off.  
    Close all open fns and the sub-block. PgUp.  
    You're back at the app's start-screen.  
  
4. Inside Modal -- Modes of Operation and User Intents   
   - The Modal GUI toolkit has been designed in such a way that for the most part  
    the developer does not need to have anything to do with it's operational context  
    in this case the wxWidgets WIMP GUI.  
    The GUI design of a Modal app is purely using modal constructs  
    namely the mode of operation and the user intent handler.  
    A modal app developer is concerned with Modal Init and Exit,  
    their app's data and the primary mode of operation they design.  
    This design may use UI elements from the toolkit.  
    This app is a modal source code editor and navigator.  
    We will walkthrough the design of SModeSrcEdr which is its primary mode of operation  
    and the user intent handlers that are part of this mode's design.  
  
   - Open INITIALIZING AND EXITING MODAL (BLOCK).  
    Open modal_init  
    Goto load_UI_state  
    Goto new_src_edr  
    Note we are inside the BLOCK: THIS APP'S PRIMARY MODE, THE SOURCE EDITOR  
    and the SUB_BLOCK: BASE DEFINITIONS  
    This is where the SModeSrcEdr struct is defined  
    and fns to "new", free and load from a file are defined.  
    Goto pBase->init  
    This is where an SMode struct is initialized.  
    Open the comment block above (311), read the comments in it then close it.  
    The base mode struct has the following fn ptrs:  
    1. fnKey_up  
      This fn is called when a key up event occurs  
    2. fnKybd_map  
      Key down event. It maps the event to an intent and dispatches it  
    to an entry in the fnIntentHandlers  
    3. fnDisp_state  
      Paint event, It paints the base state of the mode asscoaited with its data.  
    4. fnOn_load  
      Called when the mode is pushed onto the modemanager
    5. fnOn_unload
      Called when the mode is popped off the modemanager  
    6. fnSerialize  
      Called by the mode manager when mode's state needs to be loaded or stored.  
    7. fnIntentHandler[40]  
      Called by fnKybd_map to initiate intent handling  
    and by modemanager::disp_update to complete display update of the screen.

   - The base SMode struct's init provides implementations for (i), (iv), and (v).  
    A concrete mode provides for the rest and may override the base (i), (iv) and (v).  
    Now go back to new_src_edr (Ctrl-left).  
    pSrcEdr->init.  
    As you can see, SModeSrcEdr's init loads (i),(ii),(iii),(iv) and (vi) fn ptrs   
    with it's own implementation.  
    It also loads all the intent handler fns.  
    Open the enum{...} at 7247.  
    This is an enumeration of the intent handlers for SModeSrcEdr.  
    They all start with SEI_ to make them globally unique.  
    Close the enum.  
    PgDn to void load_intents() {...}  
    Open it. This is where the intent handlers are loaded. Close it.
  
   - Now we'll peep inside some of these mode behavior implementation fns.  
    Scroll up to SrcEdr->init() and goto src_edr_map.  
    Open 7456.  
    Here src_edr_map is detecting the intent SEI_UPDATE_CARET and dispatching it.  
    Close 7456.  
    Let's look at that intent handler.  
    Go back. Go Back. Go Back. Go Back.  
    Close modal_init().  
    Close(BLOCK) INITIALIZING AND EXITING MODAL  
    Open (BLOCK) THIS APP'S PRIMARY MODE, (SUB_BLOCK) INTENT_HANDLERS  
    Open src_edr_update_caret   
    This is the intent handler for SEI_UPDATE_CARET.  
    You may study this code then close it and its sub-block and block.  

    Finally we'll look at src_edr_disp_state.  
    You will find src_edr_disp_state inside  
    (BLOCK) THIS APP'S PRIMARY MODE (SUB_BLOCK) MODE IMPLEMENTATION FNs.  

Note:  
In navigating Modal source code, we only keep open  
the section of code we're currently studying.  
This is facilatated by the summarization mechanisms and the goto mechanisms.  

## Building your own Modal GUI App  
We suggest taking the walkthrough in the section above before reading this section.  
A simple Modal app has a single mode of operation  
which defnies the behavior of the app from when it's launched till it is exited.  
Within this mode of operation's interaction time  
pop-up modes may pop up and go away.  
The function of these pop-ups is to take care of common extensions  
to the base behavior of a primary mode of operation.  
To create your own Modal app using ModalWX.cpp as a template,  
you define data structs for the data your app will process.  
Then you you define your primary mode of operation that will process this data.  
As part of this definition, you may use pop-up modes provided by the toolkit.  
So, you would replace everything in (BLOCK) THIS APP's DATASTRUCTS
and everything in (BLOCK) THIS APP'S PRIMARY MODE  
with you own data structs and primary mode.  
For now, you can do this using your existing IDE.  
Our next release due Oct 06 will make ModalWX an IDE.  
It will containg explicit feaures to facilitate this design process.  

## Next Development Step:
1. Modal specific navigational features and source code editing.  
2. Build.  
3. Debug.  
This will make ModalWX a standalone IDE for building Modal apps based on wxWidgets.  
I estimate this will take 3 months so next release is scheduled for Thursday, Oct 06.  

## Modal & WIMP mix and match
Since a Modal UI is implemented entirely within a wxWindow subclass,  
it is possible to incorporate a Modal UI window into a regular WIMP style app  
using wxAUI with the Modal UI in one layer and a WIMP UI in another layer.  
Effectively mixing these 2 styles of UI design.  

## A note on code quality
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

## Contributing to ModalWX
I've written about 9000 lines of code so far.  
So I have fairly stable code design, code documentation and coding conventions.  
I welcome all contributions.    
Since this is an IDE, relatively simple and has a small and well-documented codebase   
it is amenable to customisations.  
I welcome developers sharing their customisations with this project.  
Some areas where work could be done are
1. Sample apps.
2. A different approach to parsing source code files.  
3. Dictionary based text input.
4. More pop-up modes to simplify not-so-common or domain-specific UI tasks.  
