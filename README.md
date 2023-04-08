# What is EngageUI?
### A user-engagement focused UI paradigm alternative to WIMP  
EngageUI is always focused on handling the user's current activity  
when engaging with the UI  
which is a temporal rather than spatial approach to the GUI.  
It does not use windows and there are no spatial layout constructs.  
It uses keyboard input for all user controls  
(the mouse may be used, if needed, for onscreen space selection only).  

### Apps are simpler to produce and simpler to use 
EngageUI apps are simpler to produce,  
have less cluttered screens,    
self-documenting interfaces   
and automatic persistence across user interaction sessions.  

### Provided as a free 'source-code toolkit'
The free EngageUI source-code toolkit contains C++ source-code for:
1. The implementation of a small API designed to work with    
the wxWidgets cross-platform app design toolkit.
2. An IDE (based on EngageUI) for developing EngageUI apps.  

There are no libraries involved,  
the source code for the API is small (about 3KLOC) and well documented,  
it is easy to understand how everything works and make modifications if needed.  

### The API is 2 simple constructs  
The EngageUI API for wxWidgets consists of 2 simple constructs:  
1. A (user) <b>EngagementManager</b> class that subclasses wxWidgets' wxWindow class  
to interface with the native windowing system  
and manages a set of (user) "activity-handlers" that handle the user's current activity.  
2. <b>SActivityHandler</b>, an "abstract" struct for handling a particular user activity.  
An activity-handler handles (keyboard and mouse) user-input and draws to the screen.  
An app populates and extends SActivityHandler to create activity-handlers  
for app-specific user activities.  
A handful of concrete activity-handlers are also provided for user activities  
that are common across apps such as selecting a file from the native file system  
and implementing an app's high-level dashboard.

### App design is a simple process
An EngageUI app is designed at 2 temporal levels of user-engagement:
1. <b>Gross user engagement</b>.  
At a high level, a user's engagement consists of low intensity (frequency) interactions  
where the user decides what activity they want to be involved in next.  
Since these are low intensity interactions,  
(slower, documented) keyboard selectable onscreen controls are used.  
An app-dashboard with an onscreen selection of possible user activities is designed.  
[Designing an app's dashboard](#designing-an-apps-dashboard) 
2. <b>Fine-grained user engagement</b>.  
At a lower level, a user's engagement consists of high intensity interactions  
where the user is engaged in a primary app-usage activity.  
For these high-intensity interactions,  
(faster, undocumented) direct-mapped keyboard controls are used.  
A set of activity-handlers for the user activities accessible via the app's dashboard  
are designed.    
[Designing an activity-handler](wiki/#designing-an-activity-handler)  

The toolkit's free IDE makes it easy to design and produce EngageUI apps.  

### Apps have self-documenting interfaces  
An EngageUI app's user interface is automatically documented  
via documentation constructs included in the API.  
Interface documentation is at 2 levels:  
1. High-level -- The app dashboard provides high-level documentation for the app.  
Pressing Ctrl launches the app dashboard in its current context.  
2. Low-level -- Each activity-handler contains an interface descriptor    
used by the toolkit to implement an automatic interface documentation system.    
Pressing Ctrl-H within any activity-handler context  
presents a description to the user on how to perform that activity.

### Apps have automatic state persistence
When the app is launched, if it's for the first time,  
the EngagementManager presents the user with the app's dashboard.  
When the app is exited, the EngagementManager serializes its state to disk.  
Subsequently, when the app is reloaded, the EngagementManager resumes it from its previous state.  
## EngageUI Illustrations
### Less cluttered screens
![Less cluttered screens](https://www.khitchdee.net/WIMPvsEngage.png?v03-23-2023)  
### Automatic app state persistence
![Alt Text](https://hex-map.khitchdee.net/EngageUI-illustration.png?v08-23-2022)
### Built-in app interface documentation
The app dashboard  
The activity handlers interface doc.  

# EngageIDE
### What is EngageIDE?
EngageIDE is a free IDE for building PC desktop GUI apps based on EngageUI.    
It is itself an EngageUI PC desktop GUI app.        
It can be used to produce a single C++ app codebase     
that can be built to run on Linux, Windows and OSX.    
### EngageIDE features
1. <b>No mouse usage, keyboard-only interface</b>.  
EngageIDE does not use the mouse.  
All of it's editing, navigation, building and debugging controls are keyboard based.  
You never need to switch to the mouse while typing out your code  
so it's efficient like Vim, Emacs, Sublime Text and Notepad++.  

2. <b>Efficient code-documentation system</b>.  
Is has a very efficient way of folding code.  
The code is documented inline using C comment blocks  
and these comment blocks are folded down to single lines  
to get out of the way of code readability.  
You can be fairly verbose in your documentation  
knowing that a long comment will fold down to a single line  
to be opened when needed.  

3. <b>Efficient code navigation</b>.  
   a. Special demarcation symbols are introduced inside comment blocks  
to create higher level navigational constructs in the code  
above the language level.  
These constructs -- blocks and sub-blocks -- can also be folded  
making it possible to easily navigate medium sized codebases  
in the 10s of KLOCs.  
   b. A goto mechanism enables jumps to any selected struct, class or function.  
   c. A global code navigator has EngageUI specific mechanisms for code navigation.  

4. <b>Simplified codebase organisation</b>.  
The apps you write are all included in a single .cpp file.  
EngageIDE does not use header files except for library (such as wxWidgets) headers.  
This "all in one place" approach makes the codebase easier to navigate and understand.  

5. <b>Efficient code display</b>.  
The entire screen is used to display code in a 3 column format.  
With a wider primary center column that is used to edit code  
and narrower right and left columns  
to see the preceding and following code sections.  
Even the longest functions can entirely fit in a single screen.  
At any given time you only keep the code you're working on open  
everything else is kept summarized (folded).  
This feature along with foldable blocks and sub-blocks  
makes scrolling through the entire codebase very easy.  

6. <b>Integrated compiler/debugger controls</b>.  
EngageIDE connects to a command line compiler and debugger  
to provide visual build error correction and debugging.  

# Building EngageIDE:
Should you build this app?  
1. If you are new to wxWidgets.  
You will have to download and build the wxWidgets library for your platform.  
This is a somewhat convoluted and time consuming process. (interaction time ~1hr)  
You will need to become somewhat familiar with wxWidgets  
particularly its wxDC class and its graphic drawing primitives.   
This is a relatively simple process.  
If you jump these hurdles, you can produce a simple keyboard-driven cross-platform PC app.  
2. If you are already a wxWidgets developer.  
You only have to build EngageIDE.cpp which takes a few minutes    
and you get an easy to learn GUI design alternative to WIMP  
that you can start using right away.  

## Recommended IDEs, building wxWidgets and EngageIDE
EngageIDE is implemented in a single file EngageIDE.cpp  
that uses the wxWidgets cross-platform app design libraries.  
To build it you first have to download and build the wxWidgets libraries  
for your development platform.  

We recommend using the current development version of wxWidgets which is 3.2.2.1.  
You can get help for setting up wxWidgets on your PC from this forum:  
https://forums.wxwidgets.org/viewforum.php?f=19&sid=0083f4684647607be2aef5bc34b48d82.  
The build process for the library depends on your platform:  

### OSX: 

Recommended IDE: XCode  

Building wxWidgets:  
We recommend building the library from source.  
The simplest way is to download the source for wxWidgets.  
Then open %wxWidgetsDir%/Samples/minimal/minimal_cocoa.xcodeproj in XCode.  
You should be able to build and run this sample.  
It builds the wxWidgets library from source as part of its build process.  

Building EngageIDE:
Then you can edit the project settings to replace minimal.cpp with EngageIDE.cpp.  
This will build the EngageIDE app.  

### Windows:

Recommended IDE: Visual Studio 2022 Community edition   

Building wxWidgets:
You download the source code for wxWidgets.  
Then you goto %wxWidgetsDir%/build/msw/  
You open wx_vc17.sln in Visual Studio 2022  
Build Debug and Release configurations (we recommend not dll but statically linked libraries).  
This places the built libraries in %wxWidgetsDir%/lib/vc_x64_lib (or vc_lib).  

Building EngageIDE:
Follow the instructions at https://forums.wxwidgets.org/viewtopic.php?p=196105#p196105  
to create a new VisualStudio project and add EngageIDE.cpp to it.  
This should build the EngageIDE app.  

### Linux:

Recommended IDE: CodeLite (codelite.org)

Building wxWidgets:
Follow the instructions at:  
https://docs.codelite.org/wxWidgets/repo321/
to install wxWidgets 3.2.1.    

Building EngageIDE:
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

## Development Status  
EngageIDE is not ready yet.  
We are working on getting its first release out.  
We estimate this will happen by June 2023.  

