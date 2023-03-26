# What is EngageUI?
### A simpler, keyboard-focused alternative to WIMP  
EngageUI is a user-engagement focused UI paradigm  
that takes a temporal rather than spatial approach to the GUI.  
It is always focused on handling the user's current activity when engaging with the UI.  
It does not use windows and there are no spatial layout constructs.  
It uses keyboard input for all user controls  
(the mouse may be used, if needed, for onscreen space selection only).  

EngageUI apps are simpler to design,  
have less cluttered screens  
and are easier to use for their users.  
They have self-documenting interfaces  
and automatic persistence across user interaction sessions.  

### EngageUI apps are simple to design  
The EngageUI API for wxWidgets consists of 2 simple constructs:  
1. An EngagementManager class that subclasses wxWidgets' wxWindow class  
to interface with the native windowing system  
and manages a set of (user) "activity-handlers" that handle the user's current activity.  
2. SActivityHandler, an "abstract" struct for handling a particular user activity.  
An app populates and extends this struct to create an activity-handler  
for an app specific activity.
A handful of concrete activity-handlers are also provided to handle user activities  
that are common across apps such as selecting a file from the native file system  
and implementing an app's high-level dashboard.

An EngageUI app is designed at 2 temporal levels:
1. <b>Gross temporal design</b>.  
At a high level, an app's usage consists of low intensity (frequency) interactions  
where the user decides what activity they want to be involved in next.  
Since these are low intensity interactions, keyboard selectable onscreen controls are used.  
An app-dashboard with an onscreen selection of possible user activities is designed.  
[Designing an app's dashboard](#designing-an-apps-dashboard) 
2. <b>Fine-grained temporal design</b>.  
At a lower level, an app's usage consists of high intensity interactions  
where the user is involved a primary app-usage activity.  
For these high-intensity interactions, direct-mapped keyboard controls are used.  
A set of activity-handlers for the user activities provided by the app are designed.    
Once an activity has been selected (via the dashboard),   
an activity handler for that activity is launched.  
Within an activity handler all user controls are direct-mapped to the keyboard.    
[Designing an activity-handler](#designing-an-activity-handler)  

The EngageUI toolkit includes an IDE   
specially designed for producing EngageUI apps.  
It simplifies the production of EngageUI apps.  

### EngageUI apps have self-documenting interfaces  
An EngageUI app's user interface is automatically documented at 2 levels:  
1. High-level -- The app dashboard provides high-level documentation for the app.  
Pressing Ctrl launches the app dashboard in its current context.  
2. Low-level -- Each activity-handler contains an interface descriptor    
used by the toolkit to implement an automatic interface documentation system.    
Pressing Ctrl-H within any activity-handler context  
presents a description to the user on how to perform that activity.

### EngageUI apps have automatic state persistence
When the app is launched, if it's for the first time,  
the EngagementManager presents the user with the app's dashboard.  
When the app is exited, the EngagementManager serializes its state to disk.  
Subsequently, when the app is reloaded, the EngagementManager resumes it from its previous state.  

### The form and contents of the EngageUI toolkit  
EngageUI is provided as an MIT licenced source-code toolkit (C++)  
based on the wxWidgets cross-platform GUI toolkit.  
It's source code consists of  
1. An EngagementManager class that subclasses wxWindow  
and manages the user's engagement with the app    
via the app-dashboard and various activity-handlers. 
2. An abstract SActivityHandler struct that the app populates and extends     
with the ability to handle an app-specific user activity.
3. A small set of concrete SActivityHandler extensions    
that handle user-activities common across apps  
such as providing access to the OSs file system  
and implementing an app's dashboard.  

The toolkit also includes the source code for the SAHAppProducer,  
an activity-handler specially designed for producing EngageUI apps.   

## EngageUI Illustrations 
![Alt Text](https://hex-map.khitchdee.net/EngageUI-illustration.png?v08-23-2022)

![Less cluttered screens](https://www.khitchdee.net/WIMPvsEngage.png?v03-23-2023)  

# EngageIDE
### What is EngageIDE?
EngageIDE is an IDE for building PC desktop GUI apps based on EngageUI.    
It is an EngageUI app implemented in C++ using wxWidgets.        
It can be used to produce a single C++ app codebase for an EngageUI app    
that runs on Linux, Windows and OSX.    
It has about 15KLOC so it's not too big to understand/modify and loads instantly.  
It is fairly well documented based on its own documentation system.  
### EngageIDE features
1. <b>No mouse usage, keyboard-only interface</b>.  
EngageIDE does not use the mouse.  
All of it's editing building and debugging controls are keyboard based.  
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
c. A global code navigator has EngageUI specific mechanisms for code navigation  
that greatly simplify code navigation during development.  
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

### EngageIDE source-code
It's source code has 2 parts:
1. Source code for the classes, structs and functions  
that comprise the EngageUI toolkit -- about 3000 LOC.  
This code can directly be used to create an EngageUI app.  
All EngageUI apps are based on this source code.  
2. A dashboard descriptor and an activity-handler for producing EngageUI apps (about 7000 LOC).  
The IDE will be fully functional in our next release.  
This IDE will be used internally by Khitchdee to develop its X-Draft app.  

# Building EngageIDE:
(interaction time ~1hr) 
Should you build this app?  
1. If you want to build a simple keyboard-driven app using wxWidgets.  
2. If you are already a wxWidgets developer.  
The process is relatively brief in this case.    
and this toolkit gives you an easy to learn GUI design alternative to WIMP  
that you can start using right away.  

## Recommended IDEs and building wxWidgets and EngageIDE for your dev platform
EngageIDE is implemented in a single file EngageIDE.cpp  
that uses the wxWidgets cross-platform UI libraries.  
To build it you first have to download and build the wxWidgets libraries  
for your development platform.  

We recommend using the current development version of wxWidgets which is 3.2.1.  
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

## EngageUI & WIMP mix-and-match
Since an EngageUI is implemented entirely within a wxWindow subclass,  
it is possible to incorporate an EngageUI window into a regular WIMP style app  
using wxAUI with the EngageUI in one layer and a WIMP UI in another layer.  
Effectively mixing these 2 styles of UI design.  

## Development Roadmap
1. Dashboard designer activity-handler.  
2. Activity-Handler designer activity-handler.

## Contributing to EngageIDE
I've written about 10000 lines of code so far.  
So I have fairly stable code design, code documentation and coding conventions.  
I welcome all contributions.    
Since this is an IDE, relatively simple and has a small and well-documented codebase   
it is amenable to customisations.  
I welcome developers sharing their customisations with this project.  
Some areas where work could be done are
1. Sample apps. i.e. More primary activity-handlers.
2. A different approach to parsing source code files.  
3. Dictionary based text input.
4. More pop-up activity-handlers.  

## EngageUI History
After several years of using PCs,  
we decided that the PC user interface would be better off  
if we didn't have to constantly switch to the mouse in the interaction.  
However, after DOS, all operating system UIs were designed  
focused on using a separate pointing device based on the use of WIMP  
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

The latest annual developer survey by stack ovewflow:
https://insights.stackoverflow.com/survey/2021#section-most-popular-technologies-integrated-development-environment.  
shows that a sizeable number of developers use keyboard-input focused development tools  
such as  
Notepad++ 29.71%.  
VIM 24.19%.  
Sublime Text 20.46%.  
Emacs 5.3%.  
Neovim 4.99%  
This corroborates our conclusion that for text intensive tasks such as  
writing 10s of thousands of lines of code  
a keyboard focused GUI is more efficient than one that is based on mouse input  
and thus requires the user to switch between keyboard and mouse input.  
EngageIDE and its keyboard-focused GUI (EngageUI) could be used to write  
cross-platform versions of any of the above list of tools.  
## EngageUI Design
The first question we had to answer was  
would it be possible to design an app's user interface  
without the use of a pointer for on-screen space selection.  
An analysis of WIMP apps showed that  
most apps did not require fine-grained screen space selection.  
The exceptions being graphics apps and CAD tools.  
Using the arrow keys on a keyboard,  
it would be possible to provide the coarse screen-space selection  
needed by most apps.  
A further analysis indicated that  
if we did away with the spatial control-panel metaphor  
it could be replaced with a temporal, user-activity based design metaphor.  
We call this new UI design approach EngageUI.  
The EngageUI user interface toolkit that was designed as a result    
turned out to be simple and easy to implement.  

Then we set out to create EngageIDE an IDE (that uses EngageUI) to design  
EngageUI apps.  

## Designing an app's dashboard
An app's dashboard describes its gross level temporal interface.  
Since it is a gross level temporal interface with low intensity interactions  
it is implemented using a keyboard selectable set of onscreen options.   
It has 2 parts.
1. A start-up dashboard that defines available user options  
when the app is first launched or when there are no activity-handlers active.  
2. An activity-specific dashboard that defines available user options  
within the context of an activity.  

## Designing an activity-handler

### What is an activity-handler?
An activity-handler within an app describes its fine-grained temporal interface.  
Since it has high-intensity interactions,  
it is implemented using direct-mapped keyboard or mouse input.   
Since these maps have to be remembered by the user,  
they can be discovered within any operational context by pressing Ctrl-H.  
This is a feature provided by the toolkit.  
Each activity handler has access to the entire screen   
and exclusive control over user-input while it is loaded.  
A user activity is defined as a set of possible "user intents",  
which is an input gesture that expresses intent to do something.     
Each user-intent has an associated "intent-handler",  
that performs the action intended by the user.  
An activity-handler therefore contains a set of intent-handlers.  

### An activity-handler's descriptor
An activity-handler's descriptor contains a user level description  
of how to perform that activity.  
It is used by the EngagementManager to provide automatic activity documentation.  

### An activity-handler's activity specific data  
EngageIDE is an IDE that has been specially designed for producing EngageUI apps.  
An activity-handler typically has some activity specific data that it manipulates.  
Data structures for this activity-specific data are designed  
and stored in an ActivityHandlerExtension structure.  
The base ActivityHandler struct contains a ptr to a union of ActivityHandlerExtension structs.  
A specific ActivityHandler defines their own ActivityHandlerExtension struct  
and adds it to this union.  
Since the base ActivityHandler struct is passed to the intent-handler functions  
All intent-handlers of an activity-handler can access activity-specific data through it.   

### User input handling: The keyboard-map and mouse-map
An input gesture can be a mouse move or click or a keyboard key-press or release.  
An activity-handler has mapping functions for key-down, key-up, mouse-move and a mouse-click.  
Through these maps, user input gestures are associated with intent-handlers.  

### The activity-handler's display: State display and partial display updates  
An activity-handler always has a current display state and a function to display it.  
This function is always called by the EngagementManager.  
This state is displayed when the activity-handler is first loaded into the app  
or if the apps gets switched out and back in by the OS.  
An activity-handler's state display can also be initiated by one of its intent-handlers.

An activity-handler's display may also be updated partially by an intent-handler.  
The intent-handler specifies a rectanguler sub-area of the screen  
in an update request to the EngagementManager  
then draws the area when the request's execution is signalled by the EngagementManager.  
Partial display updates are more efficient  
but require more book-keeping by the intent-handler.  

wxWidgets provides a library of graphic drawing functions  
and the activity-handler stores display parameters such as screen dimensions and fonts.  

### Engage and Disengage from the EngagementManager
An activity-handler may initialize itself when it is engaged by the EngagementManager  
via its on_engage() function  
or wind itself up when it is disengaged by the EngagementManager
via its on_disengage() function

### Serialization
An activity-handler must serialize it's state to/from a file  
when told to do so by the EngagementManager via a serialize() function.

### Intent-Handlers
An intent-handler is a single function that operates in 2 phases.  
1. It is called in "NOTIFY" phase by a user-input-map to initiate intent handling.  
In this phase it performs any of it's own actions and then does one of two things:  
a. It initiates a partial display udpate request to the EngagementManager   
specifying a rectangular area of the screen it will update.  
b. It updates the activity-handler's data state  
and initiates a full-screen display-state request to the EngagementManager.  
2. It is called in "EXECUTE" phase by the EngagementManager  
if it made a partial display update request.  
In this phase it draws to the update area it had requested.  

