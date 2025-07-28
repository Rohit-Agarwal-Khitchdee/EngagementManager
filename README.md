## The InterUnit Desktop UI
### What is InterUnit?
InterUnit is a temporal approach to UI design that replaces WIMP.
In the interaction between a user and a PC app  
the temporal aspects of the interaction need to be well defined.  
WIMP does not do that. 
It only defines spatial and layout related aspects of a GUI.  
InterUnit that defines (temporal) interface units 
and focuses on tracking and managing user-engagement 
as the user interacts with different InterUnits.

### The Engagement Manager
User-engagement starts when a new user first starts using an app.  
At this point they have to be broadly introduced to what the app does and how they may access its capabilities.  
Subsequently, the user may have queries regarding details about the app's fine-grained usage.  
If the user exits the app and relaunches it  
the app's previous data state needs to be stored at exit and retrieved at relaunch.  
All these aspects of user engagement are tracked and managed by a class called the EngagementManager.
The EngagementManager interfaces with the platforms windowing system.

In effect the InterUnit UI acts as a go-between between the user and the app designer.  
Such a go-between does not exist in any WIMP based GUI library implementations  
such as Win32, Cocoa and X-Windows.  

### What is WIMP?
All PC UIs are based on the WIMP UI paradigm.  
WIMP stands for Windows, Icons, Menus and Pointers.  
Apps are designed using a control-panel metaphor.  
The UI includes a bunch of spatially laid out control panels (called controls)  
each one of which is designed to handle some user activity.  
The design consists of selecting which controls to use  
and where to place them on the screen.  
The approach to the UI is to provide the user access  
to all the available features of the app  
and let them choose when and what they want to do with it.  
The "when" or temporal aspects of the UI are not defined by WIMP.   

### User-engagement focus  
The InterUnit UI, is always focused on handling the user's <i>current</i> activity  
when engaging with the app.  
It treats a user activity as a temporal (and not spatial) unit  
and provides mechanisms for handling it.  
The InterUnit UI does not use windows or any spatial layout constructs. 
Instead it defines a unit of the interface -- InterUnit, that handles a user activity.
An InterUnit uses keyboard input (for text input, selection and action UI functions)
and other input modalities, such as a mouse, trackpad or pen (for description related UI functions).
It controls the entire screen for output.  
The InterUnit UI API defines 5 InterUnits for common app activities
-text input
-selection from a list of option
-(continuous) level adjustment
-file selection from the platform file system
-an app dashboard -- a high level app interface that can be customized for a particular app

### InterUnit Benefits: Apps are simpler to produce and simpler to use 
InterUnit apps are simpler to produce,  
have less cluttered screens,    
self-documenting interfaces   
and automatic persistence across user interaction sessions.  

### Simple API to develop for -- 2 simple constructs  

1. A (user) <b>EngagementManager</b> class that subclasses wxWidgets' wxWindow class  
to interface with the native windowing system  
and manage the user's engagement with the app.  
The EngagementManager tracks the state of the user  
in terms of whether they are a first time user,  
what aspects of the app they have already explored and what they have not etc.  
On the app side, the EngagementManaer manages  
a set of InterUnits that handle the user's current activity.  
A primary InterUnit is defined by the app designer and this InterUnit may use other general purpose InterUnits to perform its function.
In effect, the EngagementManager acts as a go-between  
between the app's designer and the app's user.   

2. <b>SInterUnit</b>, an "abstract" struct for handling a particular user activity.  
An InterUnit handles (keyboard and mouse) user-input and draws to the screen.  
An app designer populates and extends SInterUnit to create an InterUnit  
for app-specific user activities.  
5 concrete InterUnits are also provided by the SDK  
for user activities that are common across apps.
Users of InterUnit apps become familiar with.

### App design is a simple process.
An EngagementManager app is designed at 2 temporal levels of user-engagement:
1. <b>Gross user engagement</b>.  
At a high level, a user's engagement consists of low intensity (frequency) interactions  
where the user decides what activity they want to be involved in next.  
Since these are low intensity interactions,  
(slower, documented) keyboard selectable onscreen controls are used.  
An app-dashboard with an onscreen selection of possible user activities is designed.  
[Designing an app's dashboard](https://github.com/Rohit-Agarwal-Khitchdee/EngagementManager/wiki/#designing-an-apps-dashboard) 

2. <b>Fine-grained user engagement</b>.  
At a lower level, a user's engagement consists of high intensity interactions  
where the user is engaged in a primary app-usage activity.  
For these high-intensity interactions,  
(faster, undocumented) direct-mapped keyboard controls are used.  
[Designing an activity-handler](https://github.com/Rohit-Agarwal-Khitchdee/EngagementManager/wiki/#designing-an-activity-handler)  

### Apps have self-documenting interfaces  
An InterUnit UI app's user interface is automatically documented  
via documentation constructs included in the API.  
Interface documentation is at 2 levels:  

1. High-level -- The app dashboard provides high-level documentation for the app.  
Pressing Ctrl launches the app dashboard in its current context.  

2. Low-level -- Each InterUnit contains an interface descriptor    
used by the toolkit to implement an automatic interface documentation system.    
Pressing Ctrl-H within any InterUnit context  
presents a description to the user on how to perform that activity.

### Apps have automatic state persistence
When the app is launched, if it's for the first time,  
the EngagementManager presents the user with the app's dashboard.  
When the app is exited, the EngagementManager serializes its state to disk.  
Subsequently, when the app is reloaded, the EngagementManager resumes it from its previous state.  
### Provided as free source-code for an API
The EngagementManager is provided as  
the implementation of a small API designed to work with    
the wxWidgets cross-platform app design toolkit.

There are no libraries involved,  
the source code for the API is small (about 3KLOC) and well documented,  
it is easy to understand how everything works and make modifications if needed.  

## EngagementManager Illustrations
### Less cluttered screens
![Less cluttered screens](https://www.khitchdee.net/WIMPvsEngage.png?v03-23-2023)  
### Automatic app state persistence
![Alt Text](https://hex-map.khitchdee.net/EngageUI-illustration.png?v08-23-2022)
### Built-in app interface documentation
The app dashboard  
The activity handlers interface doc.  

# Development Status  
We are working on getting first release of  the InterUnit UI API for wxWidgets out.  

# Building the InterUnit UI API:
Should you build this API?  

1. If you are new to wxWidgets.  
You will have to download and build the wxWidgets library for your platform.  
This is a somewhat convoluted and time consuming process. (interaction time ~1hr)  
You will need to become somewhat familiar with wxWidgets  
particularly its wxDC class and its graphic drawing primitives.   
This is a relatively simple process.  
If you jump these hurdles, you can use this API  
to produce a simple keyboard-driven cross-platform PC app.  

2. If you are already a wxWidgets developer.  
You only have to build the API which takes a few minutes    
and you get an easy to learn GUI design alternative to WIMP  
that you can start using right away.  

## Recommended IDEs, building wxWidgets and the InterUnit UI API
The API is implemented in a single file EngagementManager.cpp  
that uses the wxWidgets cross-platform app design libraries.  
To build it you first have to download and build the wxWidgets libraries  
for your development platform.  

We recommend using the current development version of wxWidgets which is 3.3.1.  
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

Building EngagementManager:
Then you can edit the project settings to replace minimal.cpp with EngagementManager.cpp.  
This will build a minimal app using the EngagementManager API.  

### Windows:

Recommended IDE: Visual Studio 2022 Community edition   

Building wxWidgets:
You download the source code for wxWidgets.  
Then you goto %wxWidgetsDir%/build/msw/  
You open wx_vc17.sln in Visual Studio 2022  
Build Debug and Release configurations (we recommend not dll but statically linked libraries).  
This places the built libraries in %wxWidgetsDir%/lib/vc_x64_lib (or vc_lib).  

Building EngagementManager:
Follow the instructions at https://forums.wxwidgets.org/viewtopic.php?p=196105#p196105  
to create a new VisualStudio project and add EngagementManager.cpp.  
This will build a minimal app using the EngagementManager API.  

### Linux:

Recommended IDE: CodeLite (codelite.org)

Building wxWidgets:
Follow the instructions at:  
https://docs.codelite.org/wxWidgets/repo321/
to install wxWidgets 3.2.1.    

Building EngagementManager:
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
Add EngagementManager.cpp and build and run the project.  
This will build a minimal app using the InterUnit UI API.    

