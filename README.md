# ModalWX
(reading time ~ 3 min)  
wxWidgets is a cross-platform GUI toolkit for building WIMP style GUIs.  
Such GUIs divide the screen spatially into Windows.  
The smallest windows are controls such as clickable Buttons   
which serve as the units of the GUI.  
The mouse is used to select space on-screen,   
making it easy to navigate this spatial organization of the screen.  
WIMP UIs are non-modal even though they may contain modal elements such as dialogs.  

ModalWX extends wxWidgets with the ability to create "Modal" GUIs.  

Modal GUIs do not layout the screen spatially using windows.  
Instead they layout the app's interaction time temporally using "modes of operation".  
Each mode of operation paints to the entire screen   
and has exclusive control over user input while it has focus.  
A mode defines a set a "user intents",    
which is the user's expression of their intent to do something   
expressed as a specific input gesture.  
A mode implements an intent handler for each intent it defines.  
This intent handler performs the action the user wants and updates the screen.  
An intent (and it's handler) is the unit of the UI in a Modal UI,  
much like a control is in a WIMP GUI.  
There is one primary mode that a Modal app launches with.  
Subsequently, over the app's lifetime, different modes may pop-up   
in front of the primary mode and then go away -- i.e. they are transient,  
Much like dialogs in WIMP.  
Modal apps benefit from not having to deal with windows, and layout issues.    
This results in simpler, less cluttered screens.  
They also benefit from not needing to use the mouse in the GUI.  
This improves the interaction efficiency of the UI and makes it simpler to design.  
Modal UIs are simpler and more efficient for both developer and user.  

From a WIMP app designer's perspective,  
a Modal app is like a WIMP app with a bunch of kybd short-cuts  
and mouse-based control disabled.  
i.e. all control is via kybd shortcuts.  
Since it is modal, the number of shortcuts needed per mode is small.  

This app is a source-code navigation/editing app.  
A comparison with your IDE of choice such as XCode, Visual Studio, or Code::Blocks   
in the ability to navigate code demonstrates this efficiency advantage.  
ModalWX.cpp (~500KB) is the only file needed to run the app.  
The app only caters to C/C++ Modal apps and does not have editing, build or debug features.  
Adding these features will likely double its size to 1M.  
That compares with 10s of MBytes for all of the above tools  
which demonstrates that there's a lot less code needed. 
implying simplicity for the developer.  

Modal UIs can replace WIMP GUIs in almost all cases  
except where fine-grained screen-space selection is needed  
such as graphic design tools.  

## Building and Running the app:

The app is based on the wxWidgets cross-platform UI library.  
To build this app you first have to download and build the wxWidgets library.  

We recommend using the current development version of wxWidgets which is 3.1.6.  
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
This will build the Modal app.  

### Windows:
We recommend Visual Studio 2022 Community edition as the IDE.  
You download the source code for wxWidgets.  
Then you goto %wxWidgetsDir%/build/msw/  
You open wx_vc17.sln in Visual Studio 2022  
Build Debug and Release configurations (we recommend not dll but statically linked libraries).  
This places the built libraries in %wxWidgetsDir%/lib/vc_x64_lib (or vc_lib).  
Follow the instructions at https://forums.wxwidgets.org/viewtopic.php?p=196105#p196105  
to create a new VS project and add ModalWX.cpp to it.  
This should build the Modal app.  

### Linux:
We recommend using the CodeLite IDE which comes with wxWidgets pre-installed.  
Create a new bare-bones wxWidgets based project and add ModalWX.cpp to it.  
This should build the Modal app.  

### Running the app (UI controls):

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

## ModalWX Code Walkthrough
We guide you through a sequence of steps  
that explain how this code works, how it is navigated  
and what it takes it takes to write a Modal app using this toolkit.  
The only pre-requisite is familiarity with C.  
At the end of this walkthough you will be able to evaluate  
whether ModalWX would be a better way to design your next workstation app  
compared to a WIMP-based design.  

1. Build and run the app. as described in the preceding section.  
At this point, your screen should look like this:  
![alt text](https://hex-map.khitchdee.net/ModalWX-source-loaded.png)
2. OSX font size adjustment.  
If you are on OSX, the fontsize might be too small.  
To adjust it, press the Command key once.  
This will pop-up a set of 5 options.  
Use the down arrow key to select "Adjust Fontsize".  
Press Enter(Return).  
You will get a message:  
"arrows to adjust fontsize, esc to exit"  
Use the arrow keys to adjust fontsize.  
This is a live ajdustment.  
Once you are satisfied, press escape.  
3. Modal's interface with wxWidgets -- the ModeManager.  
4. Modes of Operation.  

## Next Development Step:

1. Modal specific navigational features and source code editing.  
2. Build.  
3. Debug.  

I estimate this will take 3 months.  

## Modal UIs and WIMP UIs

WIMP UIs have been around a long time and a broad set of controls is available  
for developers to quickly put together a UI  
with not a lot of code involved since the controls encapsulate the needed functionality.  
By contrast, writing a Modal UI requires a lot of coding  
specially in terms of graphic drawing for your app's primary mode.  
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
We also have longer-term plans to create native versions of Modal  
for C/Win32 and C/X-Windows and ObjectiveC/Cocoa  
which will remove this dependency.  

## Contributing to ModalWX
We've written about 9000 lines of code so far.  
So we have fairly stable code design, development and coding conventions.  
We welcome contributions in any form.  
Since this is an IDE, relatively simple and has a small and well-documented codebase   
it is amenable to customisations.  
We welcome developers sharing their customisations with this project.  
Some areas where work could be done are
1. A different approach to parsing source code files.  
2. Dictionary based text input.
3. More pop-up modes to simplify not-so-common or domain-specific UI tasks.  
Some larger projects would include  
- Modal for C/Win32, which is an implementation of the Modal toolkit  
for the Win32 API.  
- Modal for C/XWindows, which is an implementation of the Modal toolkit  
for X Windows on Linux.  
- Modal for ObjectiveC/Cocoa, an implementation of the Modal toolkit in ObjectiveC  
targeting Cocoa on OSX.
