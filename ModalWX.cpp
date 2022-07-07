// PREAMBLE: Modal UI Toolkit for wxWidgets
// Modal GUIs do not layout the screen spatially using windows.
// Instead they layout the app's interaction time temporally
// using "modes of operation".
// Each mode of operation paints to the entire screen
// and has exclusive control of kybd input
// which is the only form of input needed in a Modal app.
// Mouse input is not needed or used.
// Modal apps benefit from not having to deal with windows, and layout issues.
// They also benefit from not needing to use the mouse in the GUI.
// They are simpler and more efficient for both developer and user.
//
// A mode of operation is the GUI's behavior
// in terms of its visual response to kybd inputs.
// Modes can be loaded into and unloaded from the GUI
// via a mode manager that both manages the modes of the Modal GUI
// and interfaces with the wxWidgets wxWindow class.
// The first mode loaded into the mode manager is called the primary mode
// and serves as the visual backdrop for the Modal app.
// All subsequent modes are "pop-up" modes
// in that they are transient, they pop-up and pop-off.
// Much like Dialogs in a WIMP app.
// The toolkit defines an abstract base mode struct
// and some concrete, generally useful, pop-up mode structs,
// This file implements a primary mode called SModeSrcEditor
// for editing and navigating a Modal C source-code file.
// This serves as a template for an app designer
// to produce their own mode in a Modal app.
// The SrcEditor mode provides a UI to navigate and understand Modal source-code
// starting with this file, ModalWX.cpp, which is the first Modal source-code file.
//
// THE SOURCE EDITOR MODE : A TEMPLATE FOR DESIGNING A MODE
// The approach we have taken in the design of this source-code editor
// is to target the narrow domain of Modal source-code. 
// By design, all of a Modal app's source-code is contained in a single .cpp file. 
// This is unlike a typical codebase 
// that has several different .cpp files with associated .h files.
// Instead of several small .cpp files that are navigated using a project directory browser,
// we use a large single .cpp file that contains a large number of functions.
// To navigate this large set of C functions (this is a 8000+ line codefile), 
// we use 3 techniques:
//
// 1. We define 2 sectional constructs above the C++ language level -- the Block and the Sub-Block.
// Special demarcators inside comments are used to define blocks and sub-blocks. 
// Blocks and sub-blocks serve the function of high-level codebase navigation.
//
// 2. We display the source-code in a 3 column format 
// with a wider center column that is used to work on the code 
// and 2 narrower side columns to see the preceding and following code sections. 
// This makes is possible to view roughtly 120 lines of code at a time
// which covers even the longest functions.
//
// 3. We use kybd based code folding (which we call summarization). 
// Any sectional element in the code can be summarized. 
// Blocks and sub-blocks, functions and structures, enums and comment blocks can all be summarized.
// Any line that ends in a {...} is summarized and can be opened.
// Summarisation is very efficient and blends in well with code-editing and navigation. 
// The developer is already using the kybd for these functions 
// and since summarisation is also kybd controlled,
// they do not have to switch over to the mouse at all.
//
// 4. We provide the ability to directly navigate to any symbol (struct, class or fn)
// by moving the caret to a symbol's name and pressing Ctrl-Right.
// If there is a symbol at the caret location,
// the current editor view-context is collapsed, 
// and the editor goes to and expands the symbols definition.
// If there is no symbol at the caret location
// A text entry field is opened into which the name of a symbol can be typed in.
// A history of such goto's is maintained.
// Pressing Ctrl-Left returns from the goto by collapsing it
// and returning to and expading the previous context.
// This is similar to the functionalty provided by other IDEs 
// but it's kybd based and hence more efficient.
//
// NAVIGATIONAL CONTROLS
// This codefile currently contains only navigational mechanisms
// enabling developers new to the Modal UI toolkit to understand it.
// Later I will add editing, building and debugging features.
// 
// 1. The arrow keys and PgUp, PgDn are used to move the caret
//
// 2. Summarization is done by moving the caret the first line of any sectional element
// and pressing Ctrl-S.
// Pressing Ctrl-S again (on the summarised line) expands the section.
// The way you navigate a Modal codefile using this app is
// you open a section to study its code,
// then you close it, open another section and study its code.  
//
// 3. Ctrl-Right serves as a goto.
// It's behavior depends on what is under the caret.
// If a function name or Class or Struct name is under the caret
// the app closes the current section
// and goes to and opens the section associated with the symbol.
// If a known symbol is not under the caret, a text entry field is popped up
// into which the desired destination can be typed in.
// Ctrl-Left returns from the goto by closing it and re-opening the earlier location.
//
// 4. Pressing and releasing Ctrl pop's up a small menu of selectable commands.
// Only the last of these, "adjust fontsize", is implemented.
// On selecting it using up-down arrow and pressing enter
// fontsize can be adjusted using the arrow keys.
//
// 5. Pressing Escape in any operational context exits that operational context.
// If that context is the primary mode, it exits the app.
// BLOCK: WX INTERFACING BOILERPLATE
// Modal interfaces with the wxWindow class of wxWidgets.
// A Modal UI is concerned with receiving kybd events
// sending screen redraw (refresh) requests
// and receiving notifications (events) for redrawing the screen.
// It is also concerned with being initialized on app init
// and being called to exit when the user exits the app
// The Modal toolkit's SModeManager structure interfaces with a wxWindow.
// The mode manager manages the "modes of operation" of the Modal UI.
// ModalWindow, subclass of wxWindow provides all the interfacing needed.
// It inits the Modal toolkit in it constructor
// and exits the Modal toolkit in its destructor
// It also dispatches kybd and paint events to the mode manager
// and provides access to wxWindow::Refresh methods to send redraw requests
// SUBBLOCK: WX BRIDGE STRUCTURES AND FUNCTIONS
// ModalWindow is declared here
#ifdef __WXMSW__
  #define _CRTDBG_MAP_ALLOC
  #define _CRT_SECURE_NO_WARNINGS
  #include <stdlib.h>
  #include <crtdbg.h>
#endif
#include "wx/wxprec.h"
#include "wx/wx.h"
#include "wx/textfile.h"
#include "wx/utils.h"
#include "wx/file.h"
#include "wx/font.h"
#include "wx/dcbuffer.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/filefn.h"
#include "wx/stdpaths.h"


class MyFrame;
struct SModeManager;
// The Modal Window is a subclass of wxWindow that interfaces with the ModeManager
// It contains a ModeManager member struct
// this struct get's initialized in ModalWindow's constructor
// via a call to modal_init()
// and closed in ModalWindow's destructor via modal_exit()
// ModalWindow passes key events to the ModeManager
// and also wxPaint events
// modes (contained in the mode manager) also use wxWindow::Refresh()
// via ModalWindow to refresh parts or all of the screen
class ModalWindow : public wxWindow {
public:
  ModalWindow( MyFrame *pOwner, wxSize Size );
  virtual ~ModalWindow();
  void EraseBG(wxEraseEvent& Event);
  void OnPaint(wxPaintEvent &Event);
  void OnKeyDown(wxKeyEvent &Event);
  void OnKeyUp(wxKeyEvent &Event);
  void OnLostFocus(wxFocusEvent &Event);
  MyFrame *m_pOwner;
  SModeManager *m_pModeManager; // Modal's mode manager
  bool m_bUsrActn; // for OnPaint. Did the paint event come from a user action or from the OS
  wxDECLARE_EVENT_TABLE();
};
struct SModeMsg;
struct SModeFileSel;
struct SModeLineInp;
struct SModeIntDisp;
struct SModeSrcEdr;
struct SModeLevAdj;
struct SMode;
void mode_on_load( SMode *pThis, SModeManager *pManager );
void mode_on_unload( SMode *pThis, SModeManager *pManager );
bool mode_key_up( SMode * pBase , wxKeyEvent &event, ModalWindow *pWin );
SMode *load_mode( int scrnW, int scrnH, wxFile &File );
void free_mode(SMode* pMode);

// SUBBLOCK: MODAL'S BASE STRUCTURES -- MODE-MANAGER AND MODE
// The base structs of the Modal toolkit -- SMode and SModeManager

// the maximum number of "user intents" any given mode can accomodate
#define MAX_INTENTS 40
// mode types
enum {
  MODE_BASE=0,
  MODE_INTENT_DISPATCHER,
  MODE_MESSAGE,
  MODE_LINE_INPUT,
  MODE_FILE_SELECTOR,
  MODE_LEVEL_ADJUSTER,
  MODE_SOURCE_EDITOR
};
// union for extending the base mode structure
// it is contained by the base Mode structure
// If you define a new mode for your app,
// it mode-specific  data structure needs to be added here
// for example, this app added SModeSrcEdr
// You also need to add code to free this mode-specific data struct
// in the implementation of free_mode()
// and an element to the mode types enum
typedef union UModeExtension {
  SModeIntDisp *pIntDisp;
  SModeMsg * pMsg;
  SModeLineInp * pLineInput;
  SModeFileSel *pFileSel;
  SModeLevAdj * pLevAdj;
  SModeSrcEdr * pSrcEdr;
} UModeExtension;
// A mode (of operation) is the primary UI construct of the Modal Toolkit
// SMode is a struct that is like an abstract class.
// It contains several fn ptrs that have to be loaded
// and it contains a ptr to a union UModeExtension
// that a conrete extension of SMode uses to define it's specific behavior.
// SMode has function pointers for all the functions need in its primary operation
// such as responding to kybd and display events
// load or unload events when it is loaded or unloaded from the mode manager
// and a serialization fn for when its state is stored/loaded to/from disk
// A concrete extension of SMode has to provide implementations for all of these fns
// and load them into the SMode fn ptrs.
//
// USER INTENTS: HOW A MODE IS DESIGNED
// A mode is designed as a set of "user intents".
// The designer determines what user keystroke combinations
// will correspond to what app function.
// Each one of these is called a User Intent.
// The designer defines these intents 
// and creates intent handlers that will perform the function associated with that intent.
// SMode contains an array of intent_handler fn ptrs.
// By convention, a mode extension loads these in a load_intents() fn
// that is called by its init() fn
// A mode extension, in it's kybd_map() processor
// determines and sets current user intent (index)
// and dispatches processing to the mode's intent handler array.
// that intent handler does the processing needed to handle that user intent
// then either refresh's a part of or all of the screen using wxWindow::Refresh().
// this generates a wxPaint event that get's send down by ModalWindow to the mode manager.
// the modemanager determines the currently operating mode and intent
// and dispatches the display processing to that mode intent_handler fn
// the intent handler therefore has 2 phases of operation
// the first when the mode's kybd_map() fn calls it (PH_NOTIFY)
// and the second when the modemanager's paint evt handler calls it (PH_EXEC). 
//
// THE MODE'S DISPLAY STATE: INITIAL DRAWING
// Every mode must maintain a display state
// which is displayed when the mode is first loaded by the modemanager
// or in response to an OS event such as switching out and in of the app.
// The mode must implement this in a fn loaded into the mode's fnDisp_state fn ptr.
// Whenever an intent handler draws directly to the display,
// it must update this display state to reflect the changes it has made.
// The intent handler may also choose to only make changes to the display state of its mode
// and not do any drawing to the screen of its own
// then set ModalWindow::m_bUsrActn to false and issue a fullscreen refresh
// which will call fnDispState.
// This is less efficient than updating only a part of the screen
// but a lot simpler to implement.
typedef struct SMode {
  void init( int scrnW, int scrnH, wxFont *pFont ) {
    this->fnKey_up = mode_key_up;
    this->fnOn_load = mode_on_load;
    this->fnOn_unload = mode_on_unload;
    this->scrnW = scrnW;
    this->scrnH = scrnH;
    this->bHasFocus = false;
    this->bShiftDown = false;
    this->bCtrlDown = false;
    this->pFont = pFont;
    this->dFontScale = 1.0;
	if (pFont != NULL)
	  this->set_font(pFont);
    else 
      this->dBaseFontPointSize = 0.0;
    this->bReset = true;
  };
  bool (*fnKey_up)( SMode *pMode, wxKeyEvent &Event, ModalWindow *pWin );
  bool (*fnKybd_map)( SMode *pMode, wxKeyEvent &Event, ModalWindow *pWin );
  void (*fnDisp_state)( SMode *pMode, ModalWindow *pWin, wxDC &DC );
  void (*fnOn_load)( SMode *pBase, SModeManager *pManager );
  void (*fnOn_unload)( SMode *pBase, SModeManager* pManager);
  bool (*fnSerialize)( SMode *pBase, wxFile &File, bool bToFrom );
  void (*fnIntent_handler[MAX_INTENTS])( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC );
  // update the display for this mode in response to a user action
  void disp_update( int phase, ModalWindow * pWin, wxDC &DC ) {
    // dispatch the current user intent for display
    this->fnIntent_handler[this->intent]( this, phase, pWin, DC );
  };
  void set_font( wxFont *pFont ) {
    this->pFont = pFont;
    this->dBaseFontPointSize = pFont->GetFractionalPointSize();
  };
  // load's the currently set font with it's scale
  void load_font() {
    if( this->pFont != NULL )
      this->pFont->SetFractionalPointSize(this->dBaseFontPointSize * this->dFontScale);
  };    
  void adjust_font_scale(double dFontScaleAdjust) {
	  this->dFontScale = (this->dFontScale * dFontScaleAdjust);
    this->load_font();
  }
  // set the location of this mode if it is a pop-up
  // the location is relative to the center of the screen
  // and is in pixel coordinates.
  void set_location( wxPoint Location ) {
    this->Location = Location;
  };
  // serializes the base data of a mode
  void serialize( wxFile &File, bool bToFrom ) {
    if( bToFrom ) {
      File.Write( &(this->numIntents), sizeof(int) );
      File.Write( &(this->type), sizeof(int) );
      // serialize the FontScale
      File.Write( &(this->dFontScale), sizeof(double) );
    }
    else {
      File.Read( &(this->numIntents), sizeof(int) );
      File.Read( &(this->type), sizeof(int) );
      // serialize the font
      File.Read( &(this->dFontScale), sizeof(double) );
    }
  }
  int numIntents;
  int intent;
  int intentPrev;
  wxPoint Location;
  UModeExtension sExt;
  int scrnW;
  int scrnH;
  int type;
  bool bHasFocus;
  int key;
  int uniKey;
  bool bShiftDown;
  bool bCtrlDown;
  wxFont *pFont;
  double dFontScale;
  double dBaseFontPointSize;
  bool bReset;
} SMode;
// phases of an intent implementation function
// the kybd map function initiates intent dispatch
// by calling an intent function in PH_NOTIFY phase
// the intent function either generates its own refresh
// in which case, when the paint event is called,
// the paint event handler completes intent dispatch
// by calling the intent function with PH_EXEC
// in which case the intent function draws to the screen the rects it refreshed.
// The intent function during PH_NOTIFY may also generate a global refresh
// by setting g_UsrActn false and calling ModalWindow : wxWindow::Refresh
// in this case paint event processing calls the mode manager
// which calls the disp_state method of the mode
// causing a full redraw
// using this technique, we can localize the RefreshRect and it's associated drawing
// to a single function
enum {
  PH_NOTIFY, PH_EXEC
};
// a mode link structure used to implemnt a stack of modes
// such a stack is used by the ModeManager
typedef struct SModeLink {
  SMode *pMode;
  SModeLink *pNextLink;
} SModeLink;
// the ModeManager that interfaces with wxWidgets
// and manages the modes (of operation) of a Modal UI
// modes are pushed onto and popped off the mode manager
// the mode manager manages the dispatch of kybd and paint events
// that are sent down to it from the ModalWindow.
// Kybd events are sent to the mode at the top of the stack.
// Paint events if they are full-screen refresh's 
// cause the back-to-front state display of each mode in the stack
// if they are caused by a user action, they are sent 
// to the display update method of the mode at the top of the stack. 
typedef struct SModeManager {
  SModeLink Stack;
  SMode *pCurMode;
  int scrnW;
  int scrnH;
  wxFont* pFont;
  // init with the screen dimensions
  void init( int scrnW, int scrnH, wxFont *pFont ) {
    Stack.pMode = NULL;
    Stack.pNextLink = NULL;
    this->scrnW = scrnW;
    this->scrnH = scrnH;
    this->pCurMode = NULL;
    this->pFont = pFont;
  };
  void push( SMode *pMode ) {
	  pMode->set_font(this->pFont);
    bool bLast = false;
    SModeLink *pThisLink = &(this->Stack);
    if( pThisLink->pMode == NULL ) {
      pThisLink->pMode = pMode;
      pThisLink->pNextLink = NULL;
      this->pCurMode = pMode;
      this->pCurMode->fnOn_load( this->pCurMode, this );
      this->pCurMode->bHasFocus = true;
    }
    else {
      while( !bLast ) {
      if( pThisLink->pNextLink == NULL ) {
        bLast = true;
        pThisLink->pNextLink = (SModeLink *) malloc( sizeof( SModeLink ) );
        if (pThisLink->pNextLink != NULL) {
          pThisLink->pNextLink->pMode = pMode;
          pThisLink->pNextLink->pNextLink = NULL;
          this->pCurMode = pMode;
          this->pCurMode->fnOn_load(this->pCurMode, this);
          this->pCurMode->bHasFocus = true;
        }
      }
      else
        pThisLink = pThisLink->pNextLink;
      }
    }
  };
  // pop a mode from the mode manager
  // returns false if the stack is now at init state
  bool pop() {
    bool bRetVal = true;
    bool bLast = false;
    SModeLink *pThisLink = &(this->Stack);
    if( pThisLink->pNextLink == NULL ) {
      bRetVal = false;
    }
    else {
      while( !bLast ) {
      if( pThisLink->pNextLink->pNextLink == NULL ) {
        bLast = true;
        pThisLink->pNextLink->pMode->bHasFocus = false;
        pThisLink->pNextLink->pMode->fnOn_unload( pThisLink->pNextLink->pMode, this );
        free( pThisLink->pNextLink );
        pThisLink->pNextLink = NULL;
        this->pCurMode = pThisLink->pMode;
        this->pCurMode->fnOn_unload( this->pCurMode, this );
        this->pCurMode->bHasFocus = true;
      }
      else
        pThisLink = pThisLink->pNextLink;
      }
    }
	return( bRetVal );
  };
  // replace the node at the top of mode stack
  void replace( SMode *pNewMode ) {
    pNewMode->set_font(this->pFont);
    bool bLast = false;
    SModeLink *pThisLink = &(this->Stack);
    while( !bLast ) {
      if( pThisLink->pNextLink == NULL ) {
      bLast = true;
      pThisLink->pMode->fnOn_unload( pThisLink->pMode, this );
      pThisLink->pMode = pNewMode;
      this->pCurMode = pNewMode;
      this->pCurMode->fnOn_load( this->pCurMode, this );
      this->pCurMode->bHasFocus = true;
      }
      else
      pThisLink = pThisLink->pNextLink;
    }
    return;
  };
  // displays the current state of the mode stack
  // this method is called by ModalWindow : wxWindow :: OnPaint
  // when it receives a paint event that was not caused by a user action
  // which implies the entire screen is to be repainted.
  // it calls disp_state on each of the modes in the stack
  // starting from the primary mode
  void disp_state( ModalWindow *pWin, wxDC& DC ) {
    wxRect rect;
    rect.x = 0;
    rect.y = 0;
    rect.width = this->scrnW;
    rect.height = this->scrnH;
    wxPen Pen = DC.GetPen();
    DC.SetPen( *wxTRANSPARENT_PEN );
    DC.SetBrush( wxColour( 208,208,200 ) );
    DC.DrawRectangle( rect );
    DC.SetPen( Pen );
    bool bLast = false;
    SModeLink *pThisLink = &(this->Stack);
    if( pThisLink->pMode != NULL ) {
      while( !bLast ) {
        if( pThisLink->pNextLink == NULL ) {
          bLast = true;
          pThisLink->pMode->fnDisp_state( pThisLink->pMode, pWin, DC );
        }
        else {
          pThisLink->pMode->fnDisp_state( pThisLink->pMode, pWin, DC );
          pThisLink = pThisLink->pNextLink;
        }
      }
    }
    return;
  };
  // updates the display by calling the currently active mode's disp_update
  void disp_update( ModalWindow *pWin, wxDC& DC ) {
		if (this->pCurMode != NULL) {
			this->pCurMode->disp_update( PH_EXEC, pWin, DC );
			pWin->m_bUsrActn = false;
		}
    return;
  };  
  // calls the fnKybd_map() fn of the currently active mode
  bool kybd_map( wxKeyEvent &Event, ModalWindow *pWin ) {
		pWin->m_bUsrActn = true;
		if (this->pCurMode != NULL)
			this->pCurMode->fnKybd_map(this->pCurMode, Event, pWin);  
		return(true);
  }
  // calls the fnKey_up() fn of the currently active mode
  bool key_up( wxKeyEvent &Event, ModalWindow *pWin ) {
		pWin->m_bUsrActn = true;
		if (this->pCurMode != NULL)
			this->pCurMode->fnKey_up(this->pCurMode, Event, pWin);    
		return(true);
  }
  // resets the stored kybd state for all contained modes.
  // this fn is called when a paint evt occurs due to a system action
  // such as a window swap out swap in.
  void reset_kybd_state() {
	  bool bNextLink = false;
	  // reset kybd state on each next link (if !NULL) starting from the first
    SModeLink* pThisLink = &(this->Stack);
    bool bLast = false;
    if (pThisLink->pMode != NULL) {
      while (!bLast) {
        // reset kybd state 
        pThisLink->pMode->bCtrlDown = false;
        pThisLink->pMode->bShiftDown = false;
        if (pThisLink->pNextLink == NULL) 
          bLast = true;
        else 
          pThisLink = pThisLink->pNextLink;
      }
    }
  }
  // serializes to/from a file the state of this mode manager
  // called when a Modal app exits
  // stores the mode stack and every mode in the mode stack
  bool serialize( wxFile &File, bool bToFrom ) {
	bool bRetVal = true;
	bool bNextLink = false;
	// store each next link (if !NULL) starting from the first
	if( bToFrom ) {
	  SModeLink *pThisLink = &(this->Stack);
	  bool bLast = false;
	  if( pThisLink->pMode != NULL ) {
		  while( !bLast && bRetVal ) {
			if( pThisLink->pNextLink == NULL ) {
				bLast = true;
				// write out a 0 for next link
				bNextLink = false;;
				File.Write( &bNextLink, sizeof(bool) );
				// store tyhe state of this link's mode
				pThisLink->pMode->serialize( File, true );
				pThisLink->pMode->set_font(this->pFont);
				bRetVal = pThisLink->pMode->fnSerialize( pThisLink->pMode, File, true );
			}
			else {
				// write out a 1 for next link
				bNextLink = true;
				File.Write( &bNextLink, sizeof(bool) );
				// store the state of this link's mode
				pThisLink->pMode->serialize( File, true );
				bRetVal = pThisLink->pMode->fnSerialize( pThisLink->pMode, File, true );
				pThisLink = pThisLink->pNextLink;
			}
		  }
	  }
	}
	else {
	  bool bNextLink = true;
	  SModeLink *pNextLink = &(this->Stack);
	  if (pNextLink != NULL) {
		  while (bNextLink) {
			  File.Read(&(bNextLink), sizeof(bool));
			  pNextLink->pMode = load_mode(scrnW, scrnH, File);
			  pNextLink->pMode->set_font(this->pFont);
			  if (bNextLink) {
				  pNextLink->pNextLink = (SModeLink*)malloc(sizeof(SModeLink));
				  pNextLink = pNextLink->pNextLink;
			  }
			  else {
				  pNextLink->pNextLink = NULL;
				  this->pCurMode = pNextLink->pMode;
			  }
		  }
	  }
	}
	return( bRetVal );
  };
} SModeManager;
// allocs inits a mode manager ptr on the heap and returns it.
// The mode manager is inited with an initial mode, the width and height of the screen
SModeManager * new_mode_manager( int scrnW, int scrnH, wxFont *pFont ) {
  SModeManager *pModeManager = (SModeManager *) malloc( sizeof(SModeManager) );
  pModeManager->init( scrnW, scrnH, pFont );
  return( pModeManager );
}
// loads a mode manager from state stored in a file
SModeManager *load_mode_manager( int scrnW, int scrnH, wxFont *pFont, wxFile &File ) {
  SModeManager *pModeManager = (SModeManager *) malloc( sizeof(SModeManager) );
  pModeManager->init( scrnW, scrnH, pFont );
  pModeManager->serialize( File, false );
  return( pModeManager );
}
// frees a mode manager
void free_mode_manager( SModeManager *pModeManager ) {
	// free each modelink (and its associated mode) in the mode stack
  // starting from the last link
  // to do this, we make a linear list of links and free it in reverse order
  int numLinks = 0;
  SModeLink* pThisLink = &(pModeManager->Stack);
  while( pThisLink != NULL ) {
	  pThisLink = pThisLink->pNextLink;
	  numLinks++;
  }
  SModeLink** ppLinkList = (SModeLink **)malloc(numLinks * sizeof(SModeLink *));

  int index = 0;
  ppLinkList[index] = &(pModeManager->Stack);
  pThisLink = &(pModeManager->Stack);
  while (pThisLink != NULL) {
    pThisLink = pThisLink->pNextLink;
    if (pThisLink != NULL) {
      index++;
      ppLinkList[index] = pThisLink;
    }
  }

  for (int i = numLinks - 1; i >= 0; i--) {
    free_mode( ppLinkList[i]->pMode );
    if( i>0 )
      free(ppLinkList[i]);
  }
  free( ppLinkList );
  delete pModeManager->pFont;
  free( pModeManager );
}
SModeManager * modal_init( int scrnWidth, int scrnHeight );
void modal_exit( SModeManager *pModeManager );
// SUBBLOCK: WX CLASS FUNCTION DEFINITIONS
// This sub-block contains the definitions of wx classes
// and their interfaces to the Modal UI toolkit's bridge functions
class MyFrame : public wxFrame {
public:
  MyFrame(const wxString& strTitle, const wxPoint& Pos, const wxSize& Size);
private:
  ModalWindow   *m_pWin;
};
// an event table to map the events for ModalWindow : wxWindow
wxBEGIN_EVENT_TABLE(ModalWindow, wxWindow)
  EVT_PAINT  (ModalWindow::OnPaint)
  EVT_ERASE_BACKGROUND (ModalWindow::EraseBG)
  EVT_KEY_DOWN (ModalWindow::OnKeyDown)
  EVT_KEY_UP (ModalWindow::OnKeyUp)
  EVT_KILL_FOCUS(ModalWindow::OnLostFocus)
wxEND_EVENT_TABLE()
// the app class
class MyApp : public wxApp {
public:
  virtual bool OnInit() wxOVERRIDE;
};
// macro to implement the entry point for the app class
// in a cross-platform way
wxIMPLEMENT_APP(MyApp);
// the app's entry point
// calls wxApp:init
// retrieves the width and height of the screen
// creates a frame
// which in turn creates a ModalWindow which init's Modal
// "shows" the frame
bool MyApp::OnInit() {
  if( !wxApp::OnInit() )
    return false;
  int scrnWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X,NULL);
  int scrnHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y,NULL)-20;
  // we offset the y location by 20 so the OS's menu is visible at the top above this app's window
  MyFrame *pFrame = new MyFrame("Modal UI Toolkit for wxWidgets", wxPoint(0,20), wxSize(scrnWidth, scrnHeight));
  pFrame->Show(true);
  return true;
}
// Creates a ModalWindow which init's Modal
MyFrame::MyFrame(const wxString& strTitle, const wxPoint& Pos, const wxSize& Size) : 
wxFrame((wxFrame *)NULL, wxID_ANY, strTitle, Pos, Size) {
  wxSize SizeCanvas = Size;
  // we deduct 25 from the height to account for the title bar of the frame
  SizeCanvas.SetHeight(Size.GetHeight()-25);
  m_pWin = new ModalWindow( this, SizeCanvas );
  return;
}
// Calls modal_init()
ModalWindow::ModalWindow(MyFrame *pOwner, wxSize Size ) : wxWindow(pOwner, wxID_ANY, wxPoint( 0, 0 ), Size ) {
	SetBackgroundStyle(wxBG_STYLE_PAINT); // this is needed by wxWidgets for a certain feature
  m_pModeManager = modal_init( Size.GetWidth(), Size.GetHeight() );
  m_bUsrActn = false;
  m_pOwner = pOwner;
}
// Calls modal_exit()
ModalWindow::~ModalWindow() {
	modal_exit(m_pModeManager);
}
// Checks to see if the event was initiated by user action
// dispatches it to the current mode's current intent processor
// otherwise calls the ModeManager's disp_state
void ModalWindow::OnPaint(wxPaintEvent& event) {
	wxAutoBufferedPaintDC DC(this);

	// event was produced by the OS (load or relaod app) not the user
	// an intent handler may also unset m_bUsrActn to cause a full window refesh
	if (!m_bUsrActn) 
		m_pModeManager->disp_state(this, DC);
	// event is a response to a user action
	else 
		m_pModeManager->disp_update(this, DC);
	return;
}
// Used to reduce flicker
void ModalWindow::EraseBG(wxEraseEvent& event) {
	return;
}
// Dispatches the key event to the modemanager's current mode
void ModalWindow::OnKeyDown(wxKeyEvent& event) {
  if (m_pModeManager != NULL) 
    m_pModeManager->kybd_map( event, this );
  return;
}
// Dispatches the key up event to the modemanager's current mode
void ModalWindow::OnKeyUp(wxKeyEvent& event) {
  if (m_pModeManager != NULL) 
    m_pModeManager->key_up( event, this );
  return;
}
// Processes the event when window loses focus
// resets the kybd state of the mode manager
void ModalWindow::OnLostFocus(wxFocusEvent& event) {
  if (m_pModeManager != NULL) 
    m_pModeManager->reset_kybd_state();
  event.Skip();
  return;
}
// BLOCK: UTILITIES PROVIDED BY MODAL
// Some utlity structs and fns provided by Modal
// Text processing for line and pages of text
// Kybd event handling helpers
// SUBBLOCK: TEXT PROCESSING UTILITIES
// This sub-block contains text processing related utilities
// 2 structures and associated function are provided
// STxtLine for representing and processing a text line
// and STxtPage for representing and processing a page of TxtLines
// e.g. They are used by the source editor mode implementation
struct STxtLine;
#define MAX_TXT_LINE_LENGTH 200
#define MAX_OPS_CACHED 500
#define ABS(x) ((x)>0?(x):-(x))

// struct for representing a line of txt represented as chars (not unicode)
// and several functions to manipulate a line of text
typedef struct STxtLine {
  char *szBuf;
  int length;
  int maxLength;
} STxtLine;
// ancillary gets the length of a null terminated char string
int get_sz_length( const char pcData[] ) {
  int retVal = 0;
  if( pcData != NULL )
  {
    int len = 0;
    char cVal = 'a';
    while( cVal != 0 ) {
      cVal = pcData[len];
      len++;
    }
    retVal = len -1;
  }
  return( retVal );
}
// gets the screen location of the caret at the specified index in the line
// based on the current font loaded in the DC
int tl_caret_loc(STxtLine* pszThis, int index, wxDC& DC, ModalWindow *pWin ) {
  int retVal = 0;
  int strLen;
  if( index > 0 && pszThis->length > 0 ) {
    if( index > pszThis->length ) 
      strLen = pszThis->length;
    else 
      strLen = index;
      
    char szTemp[MAX_TXT_LINE_LENGTH];
    for( int i=0; i<strLen; i++ ) 
      szTemp[i] = pszThis->szBuf[i];
      
    szTemp[strLen] = 0;
    int width;
    int height;
	wxFontMetrics font = DC.GetFontMetrics();
	wxString msg;
	msg.Printf("height %d, spcae %d width %d", font.height, font.internalLeading, font.averageWidth);
//	wxLogMessage(msg);
    DC.GetTextExtent( wxString( szTemp ), &width, &height );
	retVal = width - 1;
  }
  else 
    retVal = 0;
  return( retVal );
}
// creates a new txt_line struct ptr on the heap using the specified zero terminated string
// maxLength is how large this line can get
STxtLine* new_txt_line( char *pcData ) {
	STxtLine* pRetVal = (STxtLine*)malloc(sizeof(STxtLine));
  int len = 0;
  if (pRetVal != NULL) {
    if( pcData == NULL ) {
      len = 100;
      pRetVal->maxLength = len * 2 + 1; 
      pRetVal->length = len;
      pRetVal->szBuf = (char*)malloc((pRetVal->maxLength + 1) * sizeof(char));
    }
    else {  
      len = get_sz_length(pcData);
      pRetVal->maxLength = len * 2 + 1; 
      pRetVal->length = len;
      pRetVal->szBuf = (char*)malloc((pRetVal->maxLength + 1) * sizeof(char));
      if (pRetVal->szBuf != NULL) {
        for (int i = 0; i < pRetVal->length; i++)
          pRetVal->szBuf[i] = pcData[i];
        pRetVal->szBuf[pRetVal->length] = 0;
      }
    }
	}
  return( pRetVal );
};
// frees a txt line
void tl_free( STxtLine *pLine ) {
	if (pLine != NULL) {
		if (pLine->szBuf != NULL) {
			free(pLine->szBuf);
			pLine->szBuf = NULL;
		}
		free(pLine);
	}
};
// creates a new txt_line struct ptr on the heap using the specified wxString
// maxLength is how large this line can get
STxtLine * new_txt_line_wx( wxString strFrom ) {
  STxtLine * pRetVal = (STxtLine *) malloc( sizeof(STxtLine) );
  if( pRetVal != NULL ) {
	  const char *szTemp = static_cast<const char *>(strFrom.c_str());
	  int len = get_sz_length( szTemp );
    pRetVal->maxLength = len * 2 + 1; 
	  pRetVal->length = len;
	  pRetVal->szBuf = (char *) malloc( (pRetVal->maxLength+1) * sizeof(char) );
	  if (pRetVal->szBuf != NULL) {
		  for (int i = 0; i < pRetVal->length; i++)
			  pRetVal->szBuf[i] = szTemp[i];
		  pRetVal->szBuf[pRetVal->length] = 0;
	  }
  }
  return( pRetVal );
}
// creates a txt_line on the stack using the specified szString
// maxLength is set to #define MAX_TXT_LINE_LENGTH
STxtLine get_txt_line( char pcFrom[] ) {
  int length = get_sz_length( pcFrom );
  STxtLine RetVal;
  RetVal.maxLength = length * 2 + 1;
  RetVal.length = length;
  RetVal.szBuf = (char *) malloc( (RetVal.maxLength+1) * sizeof(char) );
  if (RetVal.szBuf != NULL) {
	  for (int i = 0; i < RetVal.length; i++)
		  RetVal.szBuf[i] = pcFrom[i];
	  RetVal.szBuf[RetVal.length] = 0;
  }
  return( RetVal );
}
// creates a txt_line on the stack using the specified wxString
// maxLength is set to #define MAX_TXT_LINE_LENGTH
STxtLine get_tl_from_wx( wxString & strFrom ) {
  const char *pcFrom = static_cast<const char *>(strFrom.c_str());
  int length = get_sz_length( pcFrom );
  STxtLine RetVal;
  RetVal.maxLength = length * 2 + 1;
  RetVal.length = length;
  RetVal.szBuf = (char *) malloc( (RetVal.maxLength+1) * sizeof(char) );
  if (RetVal.szBuf != NULL)
	  for( int i=0; i<RetVal.length+1; i++ )
		  RetVal.szBuf[i] = pcFrom[i];
  return( RetVal );
}
// creates a new txt_line ptr on the heap by cloning the specified STxtLine
// caller must free this ptr
STxtLine * tl_clone( STxtLine *pszFrom ) {
  STxtLine *pRetVal = (STxtLine *) malloc( sizeof(STxtLine) );
  if (pRetVal != NULL) {
	  pRetVal->maxLength = pszFrom->maxLength;
	  pRetVal->length = pszFrom->length;
	  pRetVal->szBuf = (char *) malloc((pRetVal->maxLength + 1) * sizeof(char));
	  if (pRetVal->szBuf != NULL) {
		  for (int i = 0; i < pRetVal->length; i++)
			  pRetVal->szBuf[i] = pszFrom->szBuf[i];
		  pRetVal->szBuf[pRetVal->length] = 0;
	  }
  }
  return( pRetVal );
}
// inserts the sepcified char at the specified location in the txtline
// if index is -1 or greater than txtline->length, char is appended to the end
void tl_insert_char( STxtLine *pThis, char cChar, int index ) {
  int idx;
  if( index == -1 || index > pThis->length )
    idx = pThis->length;
  else
	idx = index;
  if( idx >= pThis->maxLength ) {
	pThis->maxLength = pThis->maxLength * 2 + 1;
	pThis->szBuf = (char *) realloc( pThis->szBuf, pThis->maxLength * sizeof(char) );
  }
  for( int i=pThis->length-1; i>=idx; i-- )
	pThis->szBuf[i+1] = pThis->szBuf[i];
  pThis->szBuf[idx] = cChar;
  pThis->length += 1;
  return;
}
// deletes char at the specified location in the txtline
// if index is -1 or greater than txtline->length, char is deleted from the end
void tl_delete_char( STxtLine *pThis, int index ) {
  int idx;
  if( index == -1 || index > pThis->length )
	idx = pThis->length;
  else
	idx = index;
  for( int i=idx; i<(pThis->length); i++ )
	pThis->szBuf[i] = pThis->szBuf[i+1];
  if( pThis->length > 0 )
	pThis->length -= 1;
  return;
}
// cut's out the substring between from and to in pLine
// and returns the cutout as an szString
// caller must free
char * tl_cut_out( STxtLine *pThis, int from, int to ) {
  char * pcRetVal = NULL;
  if( from < 0 || to > pThis->length  || to < from)
	wxLogError("invalid indices in call to tl_cut_out");
  else {
	pcRetVal = (char *) malloc( (to-from+1) * sizeof( char ) );
	if (pcRetVal != NULL) {
		for (int i = from; i < to; i++)
			pcRetVal[i - from] = pThis->szBuf[i];
		pcRetVal[to - from] = 0;
		for (int i = from; i < from + pThis->length - to + 1; i++)
			pThis->szBuf[i] = pThis->szBuf[i + to - from];
		pThis->length -= (to - from);
	}
  }
  return( pcRetVal );
}
// inserts the specified szString at the specified location in TxtLine
void tl_insert( STxtLine *pThis, char szToken[], int at ) {
  int length = get_sz_length( szToken );
  if( (pThis->length + length + 1) > pThis->maxLength ) {
    pThis->maxLength = pThis->length + length + 1;
    pThis->szBuf = (char *) realloc( pThis->szBuf, pThis->maxLength * sizeof(char) );
  }
  for( int i=pThis->length-1; i>=at; i-- )
    pThis->szBuf[i+length] = pThis->szBuf[i];
  for( int i=0; i<length; i++)
    pThis->szBuf[i+at] = szToken[i];
  pThis->szBuf[pThis->length + length] = 0;

  pThis->length += length;
  return;
}
// find's the specified szString in the TxtLine
// returns its location or -1 if not found
int tl_find( STxtLine *pThis, char szToken[] ) {
  int retVal = -1;
  bool bFound = false;
  int length = get_sz_length( szToken );
  for( int i=0; i<=pThis->length - length && !bFound; i++ ) {
    bool bContinue = true;
    int j;
    for( j=0; j<length && bContinue; j++ ) {
      if( pThis->szBuf[i+j] == szToken[j] )
      bContinue = true;
      else
      bContinue = false;
    }
    if( bContinue && j==length ) {
      bFound = true;
      retVal = i;
    }
  } 
  return( retVal );
}
// tests for equality between th 2 TxtLines
bool tl_equals( STxtLine* pThis, STxtLine* pTarget ) {
	bool bRetVal = true;
	if (pTarget == NULL)
		bRetVal = false;
	else {
		if (pThis->length == pTarget->length) {
			for (int i = 0; i < pThis->length && bRetVal; i++) {
				if (pThis->szBuf[i] != pTarget->szBuf[i])
					bRetVal = false;
			}
		}
		else
			bRetVal = false;
	}
	return(bRetVal);
}
// tests for equality between a txt line and an szString
bool tl_equals_sz(STxtLine* pThis, char *szBuf) {
	bool bRetVal = true;
	if (szBuf == NULL)
		bRetVal = false;
	else {
		if (pThis->length == get_sz_length(szBuf) ) {
			for (int i = 0; i < pThis->length && bRetVal; i++) {
				if (pThis->szBuf[i] != szBuf[i])
					bRetVal = false;
			}
		}
		else
			bRetVal = false;
	}
	return(bRetVal);
}
// tests if this TxtLine is empty
// which means it only contains spaces or tabs
bool tl_isempty( STxtLine *pThis ) {
  bool bRetVal = true;
  for( int i=0; i<pThis->length && bRetVal; i++ )
	bRetVal = (pThis->szBuf[i] == ' ' || pThis->szBuf[i] == '\t' );
  return( bRetVal );
}
// removes the specified szString, if it exists from the TxtLine
// returns true if found and removed
bool tl_remove( STxtLine *pThis, char pcToken[] ) {
  bool bFound = false;
  int length = get_sz_length( pcToken );
  if( length == 0 )
    wxLogError("empty string passed to tl_remove");
  else {
    for( int i=0; i<=pThis->length - length && !bFound; i++ ) {
      bool bContinue = true;
      int j;
      for( j=0; j<length && bContinue; j++ ) {
      if( pThis->szBuf[i+j] == pcToken[j] )
        bContinue = true;
      else
        bContinue = false;
      }
      if( bContinue && j==length ) {
      // remove length chars from index i
      for( int k=i; k<pThis->length+1-length; k++ )
        pThis->szBuf[k] = pThis->szBuf[k+length];
      pThis->length -= length;
      bFound = true;
      }
    }
  }
  return( bFound );
}
// shortens pThis to the section after the first occurence
// of a deref delimiter '.' or '->'.
// returns the section before the deref or NULL
STxtLine* tl_before_first_deref( STxtLine *pThis ) {
  STxtLine *pRetVal = NULL;
  int derefType = 0;
  wxASSERT( pThis != NULL );
  int dotIndex = tl_find( pThis, (char*) "." );
  int ptrIndex = tl_find( pThis, (char*) "->" );
  if( dotIndex > 0 ) 
    if( ptrIndex > 0 ) 
      if( ptrIndex < dotIndex ) 
        derefType = 2;
      else 
        derefType = 1;
    else
      derefType = 1;
  else
    if( ptrIndex > 0 )
      derefType = 2;
    else
      derefType = 0;
  if( derefType > 0 ) {
    char *szBuf = NULL;
    if( derefType == 1 ) {
      szBuf = tl_cut_out( pThis, 0, dotIndex+1 );
      pRetVal = new_txt_line( szBuf );
      free( szBuf );
      tl_delete_char( pRetVal, pRetVal->length-1 );
    }
    else {
      szBuf = tl_cut_out( pThis, 0, ptrIndex+2 );
      pRetVal = new_txt_line( szBuf );
      free( szBuf );
      tl_delete_char( pRetVal, pRetVal->length-1 );
      tl_delete_char( pRetVal, pRetVal->length-1 );
    }
  }
  return( pRetVal );
}
// shortens the TxtLine to the section before the first occurence
// of the specified szString token.
// return true if found and shortened
bool tl_before_first( STxtLine *pThis, char pcToken[] ) {
  bool bFound = false;
  int length = get_sz_length( pcToken );
  if( length == 0 )
    wxLogError("empty string passed to tl_remove");
  else {
    for( int i=0; i<=pThis->length - length && !bFound; i++ ) {
      bool bContinue = true;
      int j;
      for( j=0; j<length && bContinue; j++ ) {
      if( pThis->szBuf[i+j] == pcToken[j] )
        bContinue = true;
      else
        bContinue = false;
      }
      if( bContinue && j==length ) {
      // clip the line past i
      pThis->szBuf[i] = 0;
      pThis->length = i;
      bFound = true;
      }
    }
  }
  return( bFound );
}
// gets the word at index in the passed in line
// the word is returned as a txtline and caller has to free it.
// word start and end delimiters are space, comma, ( and ) or start or end of line
STxtLine* tl_get_word_at(STxtLine* pLine, int index) {
	STxtLine* pRetVal = NULL;
	int indexStart = 0;
	int indexEnd = 0;
	bool bFound = false;
	char c;
	// move back from index till a starter is found
	for (int i = index; i >= 0 && !bFound; i--) {
		c = pLine->szBuf[i];
		if (c == ' ' || c == ',' || c == '(' || c == ')') {
			indexStart = i+1;
			bFound = true;
		}
	}
	if (!bFound)
		indexStart = 0;
	// move forward from index till a word end delimiter is found
  bFound = false; 
	for (int i = index; i <pLine->length && !bFound; i++) {
		c = pLine->szBuf[i];
		if (c == ' ' || c == ',' || c == '(' || c == ')' || c == ';') {
			indexEnd = i;
			bFound = true;
		}
	}
	if (!bFound)
		indexEnd = pLine->length;

	STxtLine *pLineT = tl_clone(pLine);
	char *szWord = tl_cut_out(pLineT, indexStart, indexEnd);
	pRetVal = new_txt_line(szWord);
	free(szWord);
	tl_free(pLineT);
  pLineT = NULL; 

	return(pRetVal);
}

// extracts a word from the start of the passed in txtline
// word end delimiters are space, comma, ( and ), ; or end of line
// the word and its subsequent delimiter are both removd from the txtline
// the extracted word is returned in a txtline
// if the line is empty, NULL is returned
// caller must free the returned TxtLine
STxtLine* tl_extract_word(STxtLine* pThis, char *pcSeparator) {
	STxtLine* pRetVal = NULL;
	// first find the end of word delimiter
	// then construct the word in the retVal txtLine
	// then remove the word and its delimiter from pThis
	if (!tl_isempty(pThis)) {
		int index = 0;
		bool bFound = false;
		bool bTwoDelim = false;
		char c;
		char cSep1 = ' ';
		char cSep2 = ' ';
		// we look ' ', non-space delim and non-space delim, ' ' combos
		for (int i = 0; i < pThis->length && !bFound; i++) {
			c = pThis->szBuf[i];
			if (c == ' ' || c == ',' || c == '(' || c == ')' || c == ';') {
				index = i;
				bFound = true;
				cSep1 = c;
				// look for ' ' non-space
				if (c == ' ') {
					if (i < pThis->length - 1) {
						c = pThis->szBuf[i + 1];
						if (c == ',' || c == '(' || c == ')' || c == ';') {
							bTwoDelim = true;
							cSep2 = c;
						}
					}
				}
				// look for non-space ' '
				else {
					if (i < pThis->length - 1) {
						c = pThis->szBuf[i + 1];
						if (c == ' ') {
							bTwoDelim = true;
							cSep2 = c;
						}
					}
				}
			}
		}
		if (cSep1 != ' ')
			*pcSeparator = cSep1;
		else
			*pcSeparator = cSep2;
		// construct the word in the retVal txtLine
		// then remove the word and its delimiter from pThis
		if (bFound) {
			// first element of line is not a word end demarcator
			if (index > 0) {
				char* szWord = tl_cut_out(pThis, 0, index);
				pRetVal = new_txt_line(szWord);
				free(szWord);
				szWord = tl_cut_out(pThis, 0, (bTwoDelim?2:1) );
				free(szWord);
			}
			// signal error
			else {
				bFound = false;
			}
		}
		// the end of line is the demarcator
		// clone pThis into pRetVal
		// clear pThis
		else {
			pRetVal = tl_clone(pThis);
			char* szBuf = tl_cut_out(pThis, 0, pThis->length);
			free(szBuf);
			*pcSeparator = 0;
		}
	}
	return(pRetVal);
}

// trims out any spaces and tabs from both sides of this TxtLine
// returns true if any trimming was done
bool tl_trim( STxtLine *pThis ) {
  bool bRetVal = false;
  bool bFound = true;
  char cTest;
  int leading = 0;
  int trailing = 0;
  while( bFound ) {
	cTest = pThis->szBuf[leading];
	if( cTest == ' ' || cTest == '\t' ) {
	  bFound = true;
	  leading++;
	}
	else
	  bFound = false;
  }
  // remove leading chars from pThis->szBuf
  if( leading > 0 ) {
	for( int i=0; i<pThis->length+1-leading; i++ )
	  pThis->szBuf[i] = pThis->szBuf[i+leading];
	pThis->length -= leading;
	bRetVal = true;
  }

  // detect and remove any trailing spaces or tabs
  bFound = true;
  while( bFound ) {
	cTest = pThis->szBuf[pThis->length-1-trailing];
	if( cTest == ' ' || cTest == '\t' ) {
	  bFound = true;
	  trailing++;
	}
	else
	  bFound = false;
  }
  // remove trailing chars from pThis->szBuf
  if( trailing > 0 ) {
	pThis->szBuf[pThis->length-trailing] = 0;
	pThis->length -= trailing;
	bRetVal = true;
  }

  return( bRetVal );
}
void tl_serialize( STxtLine *pLine, wxFile &File, bool bToFrom ) {
  if( bToFrom ) {
	File.Write( &(pLine->length), sizeof(int) );
	File.Write( pLine->szBuf, (pLine->length+1)*sizeof(char) );
  }
  else {
	File.Read( &(pLine->length), sizeof(int) );
	pLine->szBuf = (char *) malloc( (pLine->length+1) * sizeof(char) );
	pLine->maxLength = pLine->length+1;
	File.Read( pLine->szBuf, (pLine->length+1) * sizeof(char) );
  }
}

STxtLine * tl_load( wxFile &File ) {
  STxtLine *pLine = (STxtLine *) malloc( sizeof(STxtLine) );
  tl_serialize( pLine, File, false );
  return( pLine );
}


// struct to hold a page which is an ordered collection of lines
typedef struct STxtPage {
  void init( int maxLines ) {
	this->maxLines = maxLines;
	this->numLines = 0;
	this->ppLines = (STxtLine **) malloc( maxLines * sizeof(STxtLine *) );
  };
  // adds a line at the specified index in this TxtPage
  // if index == -1 adds it at the end
  void add_line( STxtLine *pAdd, int index ) {
    int idx;
    if( this->numLines >= this->maxLines ) {
      this->maxLines *= 2;
      this->ppLines = (STxtLine **) realloc( this->ppLines, this->maxLines * sizeof(STxtLine *) );
    }
    if( index == -1 || index > this->numLines )
      idx = this->numLines;
    else
      idx = index;
    for( int i=this->numLines-1; i>=idx; i-- )
      this->ppLines[i+1] = this->ppLines[i];
    this->ppLines[idx] = pAdd;
    this->numLines += 1;
  }
  // removes the line at specified index from this TxtPage
  void remove_line( int index ) {
	if( this->numLines == 0 )
	  wxLogError("cannot remove a line, page is empty");
	else
	{
	  int idx;
	  if( index == -1 || index > this->numLines )
		idx = this->numLines;
	  else
		idx = index;
	  for( int i=idx; i<this->numLines; i++ )
		this->ppLines[i] = this->ppLines[i+1];
	  this->numLines -= 1;
	}
  };
  void serialize( wxFile &File, bool bToFrom ) {
	if( bToFrom ) {
	  File.Write( &(this->numLines), sizeof(int) );
	  for( int i=0; i<this->numLines; i++ )
		tl_serialize( this->ppLines[i], File, true );
	}
	else {
	  File.Read( &(this->numLines), sizeof(int) );
	  this->init( this->numLines );
	  for( int i=0; i<this->numLines; i++ )
		this->ppLines[i] = tl_load( File );
	}
  };
  int maxLines;
  int numLines;
  STxtLine **ppLines;
} STxtPage;
// clones the specified page on the stack
STxtPage tp_clone( STxtPage From ) {
  STxtPage RetVal;
  RetVal.maxLines = From.maxLines;
  RetVal.numLines = From.numLines;
  RetVal.ppLines = (STxtLine **) malloc( RetVal.maxLines * sizeof( STxtLine * ) );
  for( int i=0; i<RetVal.numLines; i++ )
	RetVal.ppLines[i] = From.ppLines[i];
  return( RetVal );
}
// allocates a TxtPage ptr on the heap with the speicifed maxLines and returns it
// caller must free
STxtPage *new_txt_page( int maxLines ) {
  STxtPage *pRetVal = (STxtPage *) malloc( sizeof(STxtPage) );
  pRetVal->init( maxLines );
  return( pRetVal );
}
// frees  txt page ptr
void free_txt_page(STxtPage* pPage) {
	if (pPage != NULL) {
		if (pPage->ppLines != NULL) {
			for (int i = 0; i < pPage->numLines; i++)
				if (pPage->ppLines[i] != NULL) {
					tl_free(pPage->ppLines[i]);
          pPage->ppLines[i] = NULL;
        }
		}
		free(pPage->ppLines);
		free(pPage);
	}
}

// SUBBLOCK: KYBD PROCESSING UTILITIES
// This sub-block has some utilities to help with kybd event processing
// converts the char to upper if shift is true
// behavior is specific to wx.
// i.e. it is assumed the char came from wxKeyEvent::GetUnicodeKey
char to_upper( char c, bool bShift ) {
  char cRetVal = c;
  if( bShift ) {
	switch( c ) {
	  case '`': cRetVal = '~'; break;
	  case '1': cRetVal = '!'; break;
	  case '2': cRetVal = '@'; break;
	  case '3': cRetVal = '#'; break;
	  case '4': cRetVal = '$'; break;
	  case '5': cRetVal = '%'; break;
	  case '6': cRetVal = '^'; break;
	  case '7': cRetVal = '&'; break;
	  case '8': cRetVal = '*'; break;
	  case '9': cRetVal = '('; break;
	  case '0': cRetVal = ')'; break;
	  case '-': cRetVal = '_'; break;
	  case '=': cRetVal = '+'; break;
	  case '[': cRetVal = 123; break;
	  case ']': cRetVal = 125; break;
	  case '\\': cRetVal = '|'; break;
	  case ';': cRetVal = ':'; break;
	  case '\'': cRetVal = '"'; break;
	  case ',': cRetVal = '<'; break;
	  case '.': cRetVal = '>'; break;
	  case '/': cRetVal = '?'; break;
	  default: break;
	}
  }
  else {
	switch( c ) {
	  case 'A': cRetVal = 'a'; break;
	  case 'B': cRetVal = 'b'; break;
	  case 'C': cRetVal = 'c'; break;
	  case 'D': cRetVal = 'd'; break;
	  case 'E': cRetVal = 'e'; break;
	  case 'F': cRetVal = 'f'; break;
	  case 'G': cRetVal = 'g'; break;
	  case 'H': cRetVal = 'h'; break;
	  case 'I': cRetVal = 'i'; break;
	  case 'J': cRetVal = 'j'; break;
	  case 'K': cRetVal = 'k'; break;
	  case 'L': cRetVal = 'l'; break;
	  case 'M': cRetVal = 'm'; break;
	  case 'N': cRetVal = 'n'; break;
	  case 'O': cRetVal = 'o'; break;
	  case 'P': cRetVal = 'p'; break;
	  case 'Q': cRetVal = 'q'; break;
	  case 'R': cRetVal = 'r'; break;
	  case 'S': cRetVal = 's'; break;
	  case 'T': cRetVal = 't'; break;
	  case 'U': cRetVal = 'u'; break;
	  case 'V': cRetVal = 'v'; break;
	  case 'W': cRetVal = 'w'; break;
	  case 'X': cRetVal = 'x'; break;
	  case 'Y': cRetVal = 'y'; break;
	  case 'Z': cRetVal = 'z'; break;
	  default: break;
	} // switch
  } // if
  return( cRetVal );
} // to_upper
// BLOCK: DATASTRUCTS DEFINED BY THIS APP
// This block contains the data structures defined by this app.
// Since this app processes Modal source-code files,
// it defines data structure to process source-code.
// The primary data structure is the SCodeElement.
// The SCodeSection extends SCodeElement and SCodeBase extends SCodeSection.
// Some ancillary structure are also defined.
// The SSymbol structure to represent symbols in a codebase
// an the SEditOperation struct to represent edit ops on a codebase
// which is used to implement undo functionality for the editing.
// This block also contains the parsers for source-code.
// SUBBLOCK: SYMBOLS
// This sub-block contains defns for the symbols in a codebase.
// Symbols are typedefs and function signatures
// The codebase contains a symbol list
// which is used for parsing, editing and navigation
#define MAX_SYMBOLS_IN_CODEBASE 200
#define MODAL_NUMBLOCKS 6

struct SCodeBase;
struct SCodeElement;
struct SCodeSection;
struct SIntent;
struct SSymbolSet;

int ce_file_offset( SCodeElement *pElem );
SCodeElement *elem_from_file_offset( int fileOffset, SCodeBase *pCodeBase );
bool extract_var(STxtLine* pLine, SSymbolSet* pSymSet, STxtLine** ppClassName, STxtLine** ppVarName, STxtLine** ppVarType, char *pcSeparator);

typedef struct SLocation {
	SCodeElement* pCodeBaseLoc;
	int fileOffset;
	void init( SCodeElement *pCodeBaseLoc, int fileOffset) {
		this->pCodeBaseLoc = pCodeBaseLoc;
		this->fileOffset = fileOffset;
	};
  bool equals( SLocation *pCompare ) {
    if( this->fileOffset == pCompare->fileOffset )
      return( true );
    else
      return( false );
  }
	void serialize(wxFile& File, bool bToFrom) {
		if (bToFrom)
			File.Write(&(this->fileOffset), sizeof(int));
		else
			File.Read(&(this->fileOffset), sizeof(int));
	};
} SLocation;

SLocation* new_location( SCodeElement *pCodeBaseLoc, int fileOffset ) {
	SLocation* pRetVal = (SLocation*)malloc(sizeof(SLocation));
	pRetVal->init( pCodeBaseLoc, fileOffset );
	return(pRetVal);
}
SLocation* location_clone(SLocation* pFrom) {
	SLocation* pRetVal = NULL;
	if (pFrom != NULL) {
		pRetVal = (SLocation*)malloc(sizeof(SLocation));
		pRetVal->pCodeBaseLoc = pFrom->pCodeBaseLoc;
		pRetVal->fileOffset = pFrom->fileOffset;
	}
	return(pRetVal);
}
void free_location(SLocation* pLocation) {
	if( pLocation != NULL )
		free(pLocation);
}


// utility struct to the store the locations
// of a navigation trail from a sequence of Goto's
// contains a growable LIFO queue of (location, caretLoc)s
typedef struct SNavTrail {
  void init() {
    this->numSteps = 0;
    this->maxSteps = 10;
    this->ppSteps = (SLocation **) malloc( this->maxSteps * sizeof( SLocation * ) );
	this->pCaretLocs = (int *) malloc( this->maxSteps * sizeof( int ) );
  }
  // adds a step location to the trail
  void add_step( SLocation *pAdd, int caretLoc ) {
		if (this->numSteps == this->maxSteps) {
			this->maxSteps = 2 * this->maxSteps;
			this->ppSteps = (SLocation**)realloc(this->ppSteps, this->maxSteps * sizeof(SLocation*));
			this->pCaretLocs = (int*)realloc(this->pCaretLocs, this->maxSteps * sizeof(int));
		}
    // check if this step is the same as the last step and free it else add the step
    if( this->numSteps > 0 ) {
      if( !this->ppSteps[this->numSteps-1]->equals( pAdd ) ) {
        this->ppSteps[this->numSteps] = pAdd;
        this->pCaretLocs[this->numSteps] = caretLoc;
        this->numSteps += 1;
      }
      else {
        free_location(pAdd);
        pAdd = NULL;
      }
    }
    // this is the first add so just add it 
    else {
      this->ppSteps[this->numSteps] = pAdd;
      this->pCaretLocs[this->numSteps] = caretLoc;
      this->numSteps += 1;
    }
  };
  // removes the last step if any and returns it
  // caller must free
  SLocation * remove_step( int *pCaretLoc ) {
    SLocation *pRetVal = NULL;
    // set retVal to the last step,  dec numSteps
    if( this->numSteps > 0 ) {
      pRetVal = this->ppSteps[this->numSteps-1];
      *pCaretLoc = this->pCaretLocs[this->numSteps-1];
      this->numSteps -= 1;
    }
    return( pRetVal );
  };
  void serialize( wxFile &File, bool bToFrom ) {
    if( bToFrom ) {
      File.Write( &(this->numSteps), sizeof(int) );
      if( this->numSteps > 0 ) 
        for( int i=0; i<this->numSteps; i++ ) {
          this->ppSteps[i]->serialize(File, bToFrom);
          File.Write( &(this->pCaretLocs[i]), sizeof(int) );
        }
    }
    else {
      int numSteps = 0;
      File.Read( &(numSteps), sizeof(int) );
      if( numSteps > 0 ) {
        SLocation *pLocation = NULL;
        for( int i=0; i<numSteps; i++ ) {
          pLocation = new_location(NULL, -1);
          pLocation->serialize(File, bToFrom);
          int caretLoc = 0;
          File.Read( &(caretLoc), sizeof(int) );
          this->add_step( pLocation, caretLoc );
        }
      }
    }
    return;
  };
  int maxSteps;
  int numSteps;
  SLocation** ppSteps;
  int* pCaretLocs;
} SNavTrail;
// creates and inits a new nav trail on the heap 
SNavTrail *new_nav_trail() {
  SNavTrail *pRetVal = (SNavTrail*) malloc( sizeof(SNavTrail) );
  pRetVal->init();
  return( pRetVal );
}
void free_nav_trail( SNavTrail *pNavTrail ) {
  if( pNavTrail != NULL ) {
    if( pNavTrail->ppSteps != NULL ) {
	  for (int i = 0; i < pNavTrail->numSteps; i++) {
		free(pNavTrail->ppSteps[i]);
		pNavTrail->ppSteps[i] = NULL;
	  }
      free( pNavTrail->ppSteps );
      pNavTrail->ppSteps = NULL;
    }
    if( pNavTrail->pCaretLocs != NULL )
      free( pNavTrail->pCaretLocs );
    free( pNavTrail );
  }
  return;
}
// struct to store a var -- name type and location
typedef struct SVar {
	STxtLine *pTypeName;
	STxtLine* pName;
	SLocation* pLocation;
	// they may all be NULL
	void init(STxtLine* pTypeName, STxtLine* pName, SLocation* pLocation) {
		if (pTypeName != NULL)
			this->pTypeName = tl_clone( pTypeName );
		else
			this->pTypeName = NULL;
		if (pName != NULL)
			this->pName = tl_clone( pName );
		else
			this->pName = NULL;
		if (pLocation != NULL)
			this->pLocation = pLocation;
		else
			this->pLocation = NULL;
	}
	bool equals(SVar* pVar) {
		bool bRetVal = false;
		if (tl_equals(this->pTypeName, pVar->pTypeName))
			if (tl_equals(this->pName, pVar->pName))
				bRetVal = true;
		return(bRetVal);
	};
	void serialize(wxFile& File, bool bToFrom) {
		if (bToFrom) {
			tl_serialize(this->pName, File, bToFrom);
			tl_serialize(this->pTypeName, File, bToFrom);
			if (this->pLocation == NULL) {
				int fileOffset = -1;
				File.Write(&fileOffset, sizeof(int));
			}
			else 
				this->pLocation->serialize(File, bToFrom);
		}
		else {
			this->pName = tl_load(File);
			this->pTypeName = tl_load(File);
			this->pLocation = new_location(NULL, -1);
			this->pLocation->serialize(File, bToFrom);
		}
		return;
	}
} SVar;

SVar* new_var(STxtLine* pName, STxtLine* pTypeName, SLocation *pLocation) {
	SVar* pRetVal = (SVar*)malloc(sizeof(SVar));
	pRetVal->init(pTypeName, pName, pLocation);
	return(pRetVal);
}
SVar* var_clone(SVar* pVar) {
	SVar* pRetVal = (SVar*)malloc(sizeof(SVar));
	pRetVal->pTypeName = tl_clone(pVar->pTypeName);
	pRetVal->pName = tl_clone(pVar->pName);
	pRetVal->pLocation = location_clone(pVar->pLocation);
	return(pRetVal);
}
void free_var(SVar* pVar) {
	if (pVar != NULL) {
		if (pVar->pTypeName != NULL) {
			tl_free(pVar->pTypeName);
			pVar->pTypeName = NULL;
		}
		if (pVar->pName != NULL) {
			tl_free(pVar->pName);
			pVar->pName = NULL;
		}
		if (pVar->pLocation != NULL) {
			free_location(pVar->pLocation);
			pVar->pLocation = NULL;
		}
		free(pVar);
	}
}
// struct for a set of symvars
typedef struct SVarSet {
	void init() {
		// allocates an initial size of 10 SVars in ppVars
		// ppVars is growable
		this->numVars = 0;
		this->maxVars = 10;
		this->ppVars = (SVar**)malloc(10 * sizeof(SVar*));
	};
	void add_var(SVar* pAdd) {
		if (this->numVars == this->maxVars) {
			this->maxVars = 2 * this->maxVars;
			this->ppVars  = (SVar**)realloc(this->ppVars, this->maxVars * sizeof(SVar*));
		}
		this->ppVars[this->numVars] = pAdd;
		this->numVars += 1;
	};
	bool equals(SVarSet* pVarSet) {
		bool bRetVal = false;
		if (this->numVars == pVarSet->numVars) {
			bool bEquals = true;
			for (int i = 0; i < this->numVars && bEquals; i++)
				if (!this->ppVars[i]->equals(pVarSet->ppVars[i]))
					bEquals = false;
			bRetVal = bEquals;
		}
		return(bRetVal);
	}
	SLocation* get_var_location(SVar* pVar) {
		SLocation* pRetVal = NULL;
		bool bFound = false;
		for (int i = 0; i < numVars && !bFound; i++) 
			if (ppVars[i]->equals(pVar)) {
				bFound = true;
				pRetVal = ppVars[i]->pLocation;
			}
		return(pRetVal);
	}
  SVar* get_var( STxtLine* pName ) {
    SVar * pRetVal = NULL;
    bool bFound = false;
		for(int i = 0; i<numVars &&!bFound ; i++ ) 
			if ( tl_equals( ppVars[i]->pName, pName ) ) {
				bFound = true;
				pRetVal = ppVars[i];
			}
		return(pRetVal);
  } 

	void serialize(wxFile& File, bool bToFrom) {
		if (bToFrom) {
			File.Write(&(this->numVars), sizeof(int));
			for (int i = 0; i < this->numVars; i++)
				this->ppVars[i]->serialize(File, bToFrom);
		}
		else {
			File.Read(&(this->numVars), sizeof(int));
			if (this->numVars > this->maxVars) {
				this->maxVars = this->numVars;
				this->ppVars = (SVar**)realloc(this->ppVars, this->maxVars * sizeof(SVar*));
			}
			for (int i = 0; i < this->numVars; i++) {
				this->ppVars[i] = new_var(NULL, NULL, NULL);
				this->ppVars[i]->serialize(File, bToFrom);
			}
		}
	};
	SVar** ppVars;
	int numVars;
	int maxVars;
} SVarSet;

SVarSet* new_var_set() {
	SVarSet* pRetVal = (SVarSet*)malloc(sizeof(SVarSet));
	pRetVal->init();
	return(pRetVal);
}

void free_var_set(SVarSet* pVarSet) {
	if (pVarSet != NULL) {
		for (int i = 0; i < pVarSet->numVars; i++)
			if (pVarSet->ppVars[i] != NULL) {
				free_var(pVarSet->ppVars[i]);
				pVarSet->ppVars[i] = NULL;
			}
		free(pVarSet->ppVars);
		pVarSet->ppVars = NULL;
		free(pVarSet);
	}
}

// struct to represent a function symbol
typedef struct SSymFunc {
	SVarSet* pVarSet;
	STxtLine* pReturnType;
	SLocation* pLocation;
	STxtLine* pName;
	// these may all be NULL
	void init(STxtLine* pName, STxtLine* pReturnType, SVarSet* pVarSet, SLocation *pLocation) {
		if (pName != NULL)
			this->pName = tl_clone(pName);
		else
			this->pName = NULL;
		if (pReturnType != NULL)
			this->pReturnType = tl_clone(pReturnType);
		else
			this->pReturnType = NULL;
		if (pVarSet != NULL)
			this->pVarSet = pVarSet;
		else
			this->pVarSet = NULL;
		if (pLocation != NULL)
			this->pLocation = pLocation;
		else
			this->pLocation = NULL;
	};
	bool equals(SSymFunc* pFunc) {
		bool bRetVal = false;
		// checks for name, return type and varset equality
		if( tl_equals(this->pName, pFunc->pName) )
			if (this->pVarSet->equals(pFunc->pVarSet))
				if (tl_equals(this->pReturnType, pFunc->pReturnType))
					bRetVal = true;
		return(bRetVal);
	}
	void serialize(wxFile& File, bool bToFrom) {
		if (bToFrom) {
			tl_serialize(this->pName, File, bToFrom);
			tl_serialize(this->pReturnType, File, bToFrom);
			this->pVarSet->serialize(File, bToFrom);
			this->pLocation->serialize(File, bToFrom);
		}
		else {
			this->pName = tl_load(File);
			this->pReturnType = tl_load(File);
			this->pVarSet = new_var_set();
			this->pVarSet->serialize(File, bToFrom);
			this->pLocation = new_location(NULL, -1);
			this->pLocation->serialize(File, bToFrom);
		}
	};
} SSymFunc;
// create a function symbol extension on the heap, caller must free
SSymFunc* new_symfunc(STxtLine *pName, STxtLine* pReturnType, SVarSet* pVarSet, SLocation *pLocation) {
	SSymFunc* pRetVal = (SSymFunc*)malloc(sizeof(SSymFunc));
	pRetVal->init(pName, pReturnType, pVarSet, pLocation);
	return(pRetVal);
}
void free_symfunc(SSymFunc* pSymFunc) {
	if (pSymFunc != NULL) {
		if (pSymFunc->pName != NULL) {
			tl_free(pSymFunc->pName);
			pSymFunc->pName = NULL;
		}
		if (pSymFunc->pReturnType != NULL) {
			tl_free(pSymFunc->pReturnType);
			pSymFunc->pReturnType = NULL;
		}
		if (pSymFunc->pVarSet != NULL) {
			free_var_set(pSymFunc->pVarSet);
			pSymFunc->pVarSet = NULL;
		}
		if (pSymFunc->pLocation != NULL) {
			free_location(pSymFunc->pLocation);
			pSymFunc->pLocation = NULL;
		}
		free(pSymFunc);
	}
}
// ancillary
// checks if the given funcName is a composite
// returns true if it is
// sets ppClassName and ppSubName if it is 
bool check_composite_funcname(STxtLine* pFuncName, STxtLine** ppClassName, STxtLine** ppSubName) {
	bool bRetVal = false;
	if (tl_find(pFuncName, (char*)"::") != -1) {
		*ppClassName = tl_clone(pFuncName);
		if (tl_before_first(*ppClassName, (char*)"::")) {
			*ppSubName = tl_clone(pFuncName);
			tl_remove(*ppSubName, (*ppClassName)->szBuf);
			tl_remove(*ppSubName, (char*)"::");
			bRetVal = true;
		}
		else {
			tl_free(*ppClassName);
      *ppClassName = NULL;
    }
	}
	return(bRetVal);
}

typedef struct SFuncSet {
	void init() {
		this->ppFuncs = (SSymFunc**)malloc(10 * sizeof(SSymFunc*));
		this->numFuncs = 0;
		this->maxFuncs = 10;
	};
	// adds a class to this class set
	void add_func(SSymFunc* pAdd) {
		// if the func name exists in the the list replace it, otherwise add it
		// does not check for return type or param list match
		bool bReplaced = false;
		if (this->numFuncs > 0) {
			for (int i = 0; i < this->numFuncs && !bReplaced; i++) {
				if (tl_equals(this->ppFuncs[i]->pName, pAdd->pName)) {
					free_symfunc(this->ppFuncs[i]);
					this->ppFuncs[i] = pAdd;
					bReplaced = true;
				}
			}
		}
		if (!bReplaced) {
			if (this->numFuncs == this->maxFuncs) {
				this->maxFuncs *= 2;
				this->ppFuncs = (SSymFunc**)realloc(this->ppFuncs, this->maxFuncs * sizeof(SSymFunc*));
			}
			this->ppFuncs[this->numFuncs] = pAdd;
			this->numFuncs += 1;
		}
	};
	SLocation* get_func_location(SSymFunc* pFunc) {
		SLocation* pRetVal = NULL;
		bool bFound = false;
		for(int i = 0; i<numFuncs &&!bFound ; i++ )
			if (ppFuncs[i]->equals(pFunc)) {
				bFound = true;
				pRetVal = ppFuncs[i]->pLocation;
			}
		return(pRetVal);
	}
  SSymFunc* get_func( STxtLine* pName ) {
    SSymFunc * pRetVal = NULL;
    bool bFound = false;
		for(int i = 0; i<numFuncs &&!bFound ; i++ )
			if ( tl_equals( ppFuncs[i]->pName, pName ) ) {
				bFound = true;
				pRetVal = ppFuncs[i];
			}
		return(pRetVal);
  } 
	void serialize(wxFile& File, bool bToFrom) {
		if (bToFrom) {
			File.Write(&(this->numFuncs), sizeof(int));
			for (int i = 0; i < numFuncs; i++)
				ppFuncs[i]->serialize(File, bToFrom);
		}
		else {
			File.Read(&(this->numFuncs), sizeof(int));
			if (this->numFuncs > this->maxFuncs) {
				this->maxFuncs = this->numFuncs;
				this->ppFuncs = (SSymFunc**)realloc(this->ppFuncs, this->maxFuncs * sizeof(SSymFunc*));
			}
			for (int i = 0; i < numFuncs; i++) {
				ppFuncs[i] = new_symfunc(NULL, NULL, NULL, NULL);
				ppFuncs[i]->serialize(File, bToFrom);
			}
		}
		return;
	}
	SSymFunc** ppFuncs;
	int numFuncs;
	int maxFuncs;
} SFuncSet;
SFuncSet* new_func_set() {
	SFuncSet* pRetVal = (SFuncSet*)malloc(sizeof(SFuncSet));
	pRetVal->init();
	return(pRetVal);
}
void free_func_set(SFuncSet* pFuncSet) {
	if (pFuncSet != NULL) {
		if (pFuncSet->ppFuncs != NULL) {
			for (int i = 0; i < pFuncSet->numFuncs; i++) {
				free_symfunc(pFuncSet->ppFuncs[i]);
				pFuncSet->ppFuncs[i] = NULL;
			}
			free(pFuncSet->ppFuncs);
			pFuncSet->ppFuncs = NULL;
		}
		free(pFuncSet);
	}
	return;
}

typedef struct SClass {
	// both pName and pLocation may be NULL
	// in case their values are unknown at time of init
	void init( STxtLine *pName, SLocation *pLocation ) {
		pConstr = NULL;
		pDestr = NULL;
		pFuncSet = new_func_set();
		pVarSet = new_var_set();
		if (pName != NULL)
			this->pName = tl_clone(pName);
		else
			this->pName = NULL;
		if (pLocation != NULL)
			this->pLocation = pLocation;
		else
			this->pLocation = NULL;
	}
	void set_constr(SSymFunc* pFunc) {
		if (this->pConstr != NULL)
			free_symfunc(this->pConstr);
		this->pConstr = pFunc;
		return;
	}
	void set_destr(SSymFunc* pFunc) {
		if (this->pDestr != NULL)
			free_symfunc(this->pDestr);
		this->pDestr = pFunc;
		return;
	}
	void add_func(SSymFunc* pAdd) {
		// check for constr and destr and add if found else add to func set
		if (tl_equals(pAdd->pName, pAdd->pReturnType))
			set_constr(pAdd);
		else {
			STxtLine* pTemp = tl_clone(pAdd->pReturnType);
			tl_insert(pTemp, (char*)"~", 0);
			if (tl_equals(pAdd->pName, pTemp))
				set_destr(pAdd);
			else
				pFuncSet->add_func(pAdd);
			tl_free(pTemp);
      pTemp = NULL;
		}
	}
	void add_var(SVar* pAdd) {
		pVarSet->add_var(pAdd);
	}
	SLocation* get_func_location(SSymFunc* pFunc) {
		return(pFuncSet->get_func_location(pFunc));
	}
	SLocation* get_var_location(SVar* pVar) {
		return(pVarSet->get_var_location(pVar));
	}
	void serialize(wxFile& File, bool bToFrom) {
		if (bToFrom) {
			tl_serialize(this->pName, File, bToFrom);
			this->pLocation->serialize(File, bToFrom);
			bool bHasConstr = (this->pConstr != NULL? 1 : 0);
			File.Write(&bHasConstr, sizeof(bool));
			if (bHasConstr)
				this->pConstr->serialize(File, bToFrom);
			bool bHasDestr = (this->pDestr != NULL ? 1 : 0);
			File.Write(&bHasDestr, sizeof(bool));
			if (bHasDestr)
				this->pDestr->serialize(File, bToFrom);
			this->pFuncSet->serialize(File, bToFrom);
			this->pVarSet->serialize(File, bToFrom);
		}
		else {
			this->pName = tl_load(File);
			this->pLocation = new_location(NULL, -1);
			this->pLocation->serialize(File, bToFrom);
			bool bHasConstr = false;
			File.Read(&bHasConstr, sizeof(bool));
			if (bHasConstr) {
				this->pConstr = new_symfunc(NULL, NULL, NULL, NULL);
				this->pConstr->serialize(File, bToFrom);
			}
			bool bHasDestr = false;
			File.Read(&bHasDestr, sizeof(bool));
			if (bHasDestr) {
				this->pDestr = new_symfunc(NULL, NULL, NULL, NULL);
				this->pDestr->serialize(File, bToFrom);
			}
			this->pFuncSet->serialize(File, bToFrom);
			this->pVarSet->serialize(File, bToFrom);
		}
		return;
	}
	SSymFunc* pConstr;
	SSymFunc* pDestr;
	SFuncSet* pFuncSet;
	SVarSet *pVarSet;
	STxtLine* pName;
	SLocation* pLocation;
} SClass;

SClass* new_class(STxtLine* pName, SLocation *pLocation) {
	SClass* pRetVal = (SClass*)malloc(sizeof(SClass));
	pRetVal->init(pName, pLocation);
	return(pRetVal);
}

void free_class(SClass* pClass) {
	if (pClass != NULL) {
		if (pClass->pConstr != NULL) {
			free_symfunc(pClass->pConstr);
			pClass->pConstr = NULL;
		}
		if (pClass->pDestr != NULL) {
			free_symfunc(pClass->pDestr);
			pClass->pDestr = NULL;
		}
		if (pClass->pFuncSet != NULL) {
			free_func_set(pClass->pFuncSet);
			pClass->pFuncSet = NULL;
		}
		if (pClass->pVarSet != NULL) {
			free_var_set(pClass->pVarSet);
			pClass->pVarSet = NULL;
		}
		if (pClass->pLocation != NULL) {
			free_location(pClass->pLocation);
			pClass->pLocation = NULL;
		}
		if (pClass->pName != NULL) {
			tl_free(pClass->pName);
			pClass->pName = NULL;
		}
		free(pClass);
	}
}

typedef struct SStruct {
	// both pName and pLocation may be NULL
	// in case their values are unknown at time of init
	void init(STxtLine* pName, SLocation* pLocation) {
		pFuncSet = new_func_set();
		pVarSet = new_var_set();
		if (pName != NULL)
			this->pName = tl_clone(pName);
		else
			this->pName = NULL;
		if (pLocation != NULL)
			this->pLocation = pLocation;
		else
			this->pLocation = NULL;
	};
	void add_func(SSymFunc* pAdd) {
		pFuncSet->add_func(pAdd);
	};
	void add_var(SVar* pAdd) {
		pVarSet->add_var(pAdd);
	};
	SLocation* get_func_location(SSymFunc* pFunc) {
		return( pFuncSet->get_func_location(pFunc) );
	};
	SLocation* get_var_location(SVar* pVar) {
		return(pVarSet->get_var_location(pVar));
	};
	void serialize(wxFile& File, bool bToFrom) {
		if (bToFrom) {
			tl_serialize(this->pName, File, bToFrom);
			this->pLocation->serialize(File, bToFrom);
			this->pFuncSet->serialize(File, bToFrom);
			this->pVarSet->serialize(File, bToFrom);
		}
		else {
			this->pName = tl_load( File );
			this->pLocation = new_location(NULL, -1);
			this->pLocation->serialize(File, bToFrom);
			this->pFuncSet->serialize(File, bToFrom);
			this->pVarSet->serialize(File, bToFrom);
		}
		return;
	}

	SFuncSet * pFuncSet;
	SVarSet* pVarSet;
	STxtLine* pName;
	SLocation* pLocation;
} SStruct;

SStruct* new_struct(STxtLine* pName, SLocation *pLocation) {
	SStruct* pRetVal = (SStruct*)malloc(sizeof(SStruct));
	pRetVal->init(pName, pLocation);
	return(pRetVal);
}

void free_struct(SStruct* pStruct) {
	if (pStruct != NULL) {
		if (pStruct->pFuncSet != NULL) {
			free_func_set(pStruct->pFuncSet);
			pStruct->pFuncSet = NULL;
		}
		if (pStruct->pVarSet != NULL) {
			free_var_set(pStruct->pVarSet);
			pStruct->pVarSet = NULL;
		}
		if (pStruct->pLocation != NULL) {
			free_location(pStruct->pLocation);
			pStruct->pLocation = NULL;
		}
		if (pStruct->pName != NULL) {
			tl_free(pStruct->pName);
			pStruct->pName = NULL;
		}
		free(pStruct);
	}
	return;
}

typedef struct SClassSet {
	void init() {
		this->ppClasses = (SClass**)malloc(10 * sizeof(SClass*));
		this->numClasses = 0;
		this->maxClasses = 10;
	};
	// adds a class to this class set
  // returns the index at which the struct was added.
  //this usually at the end of the list but if it was replaced, it's not 
	int add_class(SClass* pAdd) {
    int retVal = -1;
		// if the class exists in the the list replace it, otherwise add it
		bool bReplaced = false;
		if (this->numClasses > 0) {
			for (int i = 0; i < this->numClasses && !bReplaced; i++) {
				if (tl_equals(this->ppClasses[i]->pName, pAdd->pName)) {
					free_class(this->ppClasses[i]);
					this->ppClasses[i] = pAdd;
          retVal = i;
					bReplaced = true;
				}
			}
		}
		if (!bReplaced) {
			if (this->numClasses == this->maxClasses) {
				this->maxClasses *= 2;
				this->ppClasses = (SClass**)realloc(this->ppClasses, this->maxClasses * sizeof(SClass*));
			}
			this->ppClasses[this->numClasses] = pAdd;
			this->numClasses += 1;
      retVal = this->numClasses-1; 
		}
      return( retVal );
	};
	// retruns NULL if not found
	SClass* get_class_from_name(STxtLine* pClassName) {
		bool bFound = false;
		SClass* pRetVal = NULL;
		for (int i = 0; i < this->numClasses && !bFound; i++)
			if (tl_equals(this->ppClasses[i]->pName, pClassName)) {
				bFound = true;
				pRetVal = this->ppClasses[i];
			}
		return(pRetVal);
	};
	void serialize(wxFile& File, bool bToFrom) {
		if (bToFrom) {
			File.Write(&(this->numClasses), sizeof(int));
			for (int i = 0; i < this->numClasses; i++)
				ppClasses[i]->serialize(File, bToFrom);
		}
		else {
			File.Read(&(this->numClasses), sizeof(int));
			if (this->numClasses > this->maxClasses) {
				this->maxClasses = this->numClasses;
				this->ppClasses = (SClass**)realloc(this->ppClasses, this->maxClasses * sizeof(SClass*));
			}
			for (int i = 0; i < this->numClasses; i++) {
				ppClasses[i] = new_class(NULL, NULL);
				ppClasses[i]->serialize(File, bToFrom);
			}
		}
		return;
	}
	SClass** ppClasses;
	int numClasses;
	int maxClasses;
} SClassSet;
SClassSet* new_class_set() {
	SClassSet* pRetVal = (SClassSet*)malloc(sizeof(SClassSet));
	pRetVal->init();
	return(pRetVal);
}
void free_class_set(SClassSet* pClassSet) {
	if (pClassSet != NULL) {
		if (pClassSet->ppClasses != NULL) {
			for (int i = 0; i < pClassSet->numClasses; i++) 
				if (pClassSet->ppClasses[i] != NULL) {
					free_class(pClassSet->ppClasses[i]);
					pClassSet->ppClasses[i] = NULL;
				}
			free(pClassSet->ppClasses);
			pClassSet->ppClasses = NULL;
		}
		free(pClassSet);
	}
	return;
}

typedef struct SStructSet {
	void init() {
		this->ppStructs = (SStruct**) malloc(10 * sizeof(SStruct*));
		this->numStructs = 0;
		this->maxStructs = 10;
	}
	// adds a struct to this struct set
  // returns the index at which the struct was added.
  //this usually at the end of the list but if it was replaced, it's not 
	int add_struct(SStruct* pAdd) {
    int retVal = -1;
		// if the struct exists in the the list replace it, otherwise add it
		bool bReplaced = false;
		if (this->numStructs > 0) {
			for (int i = 0; i < this->numStructs && !bReplaced; i++) {
				if (tl_equals(this->ppStructs[i]->pName, pAdd->pName)) {
					free_struct(this->ppStructs[i]);
					this->ppStructs[i] = pAdd;
          retVal = i;
					bReplaced = true;
				}
			}
		}
		if (!bReplaced) {
			if (this->numStructs == this->maxStructs) {
				this->maxStructs *= 2;
				this->ppStructs = (SStruct**)realloc(this->ppStructs, this->maxStructs * sizeof(SStruct*));
			}
			this->ppStructs[this->numStructs] = pAdd;
			this->numStructs += 1;
      retVal = this->numStructs-1; 
		}
    return( retVal );
	};
	void serialize(wxFile& File, bool bToFrom) {
		if (bToFrom) {
			File.Write(&(this->numStructs), sizeof(int));
			for (int i = 0; i < this->numStructs; i++)
				ppStructs[i]->serialize(File, bToFrom);
		}
		else {
			File.Read(&(this->numStructs), sizeof(int));
			if (this->numStructs > this->maxStructs) {
				this->maxStructs = this->numStructs;
				this->ppStructs = (SStruct**)realloc(this->ppStructs, this->maxStructs * sizeof(SStruct*));
			}
			for (int i = 0; i < this->numStructs; i++) {
				ppStructs[i] = new_struct(NULL, NULL);
				ppStructs[i]->serialize(File, bToFrom);
			}
		}
		return;
	}
	SStruct** ppStructs;
	int numStructs;
	int maxStructs;
} SStructSet;
SStructSet* new_struct_set() {
	SStructSet* pRetVal = (SStructSet*)malloc(sizeof(SStructSet));
	pRetVal->init();
	return(pRetVal);
}
void free_struct_set(SStructSet* pStructSet) {
	if (pStructSet != NULL) {
		if (pStructSet->ppStructs != NULL) {
			for (int i = 0; i < pStructSet->numStructs; i++) 
				if (pStructSet->ppStructs[i] != NULL) {
					free_struct(pStructSet->ppStructs[i]);
					pStructSet->ppStructs[i] = NULL;
				}
			free(pStructSet->ppStructs);
			pStructSet->ppStructs = NULL;
		}
		free(pStructSet);
	}
	return;
}

// consists of a class set for all the classes defined in the codebase
// a struct set for all the structs defined 
// and a func set for all the standalne funcs 
// used by SCodeBase
typedef struct SSymbolSet {
  // initializes with maxSymbols
  void init() {
	  this->pClassSet = new_class_set();
	  this->pStructSet = new_struct_set();
	  this->pFuncSet = new_func_set();
  }
  // this function assumes this SymSet has been initialized (on the load side)
  void serialize( wxFile &File, bool bToFrom ) {
    this->pClassSet->serialize(File, bToFrom);
    this->pStructSet->serialize(File, bToFrom);
    this->pFuncSet->serialize(File, bToFrom);
  };
  SClassSet *pClassSet;
  SStructSet *pStructSet;
  SFuncSet *pFuncSet;
} SSymbolSet;
// allocates and inits a symbol set on the heap and returns that ptr
// caller must free
SSymbolSet * new_symbol_set() {
  SSymbolSet *pSymSet = (SSymbolSet *) malloc( sizeof( SSymbolSet ) );
  pSymSet->init();
  return( pSymSet );
}
// frees a symbol set
void free_symbol_set( SSymbolSet *pSymSet ) {
	if (pSymSet != NULL) {
		free_class_set(pSymSet->pClassSet);
		pSymSet->pClassSet = NULL;
		free_struct_set(pSymSet->pStructSet);
		pSymSet->pStructSet = NULL;
		free_func_set(pSymSet->pFuncSet);
		pSymSet->pFuncSet = NULL;
		free(pSymSet);
	}
	return;
}
// SUBBLOCK: CODE ELEMENT
// This sub-block defines a code element struct and associated fns
// A code element is our unitary element of source-code
// it can either be a single line of code or a section of code
// In conformance with our goal of representing a Modal codefile,
// we define sections of code as follow.
// At the top level of the entire codefile,
// the codefile is broken into sections of code called blocks.
// A Modal code file contains 5 blocks
// A block is a type of section
// which contains comment sections and sub-blocks.
// Since the C language does not have any sectional constructs at this level
// we identify the start of a block with a specially formatted comment section.
// Below the block is the sub-block, another type of code section
// A sub-block contains elements defined by the C++ language
// single line code elements and C++ sectional elements such as
// enums, typedef structs or class declarations, function definitions
// and class function definitions.
// A sub-block is also identified with a specially formatted comment section.
// Defining the block and sub-block sectional elements
// makes the code which is mostly C functions a lot better organised
// and easier to navigate.
// All sections can be summarized down to a single line
// or unsummarized (expanded) back to their origional state
// by going to the start-line of the section and pressing Ctrl-S
// From the C++ sectional element level, we go 4 levels further dowm.
// referred to as L1, L2, L3 and L4 sections
// A C++ sectional element may contain L1 sections
// and an L1 section may contain L2 sections and so on.
// Typcially L2 sections which are if, else, else if, for and while
// flow of control sections demarcated by an open and close brace
// are in 10s of line in length and fit into a single displayed code-page
// We define 2 types of code elements
// single line and multi-line
// single line elements can be of type
// CDE_S_INCLUDE, CDE_S_FWDDECL_FN, CDE_S_FWDDECL_STR, CDE_S_BLANK, CDE_S_GLOBAL
// CDE_S_DEFINE, CDE_S_CODELINE
// multiline elements can be of type
// CDE_ENUM, CDE_COMMENT, CDE_L2SECTION, CDE_L1SECTION,
// (CDE_TYPEDEF, CDE_CLASSDECL, CDE_FNDEFN and CDE_CLASS_FNDEFN), CDE_SUBBLOCK
// and CDE_BLOCK, CDE_PREAMBLE and CDE_CODEBASE
// All multiline elements can be summarised.
// While the block and sub-block sections neccesarily start with a comment section
// that describes them,
// All multiline elements are required to have at least one comment line
// preceding the point where they are defined.
// If you don't put one in, the parser will prompt you to do so.
// There are also some special sections that are contain wx specific Macros
//.such as an event table definition section we club these under type CDE_WX
// Defining these types makes the parser for these elements easier to write
// and also makes the display function easier

bool parse_not( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet, bool bInlineBrace );
bool parse_local_scope( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet, bool bInlineBrace );
bool parse_global_scope( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet );
bool parse_struct( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet, bool bInlineBrace, int symLinkType, int symLinkIndex );
bool parse_block( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet );
bool parse_preamble( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet );
bool parse_nm_codebase( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet );
bool parse_codebase( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet );
bool find_enum( SCodeSection *pThis, STxtPage *pPage, int index, int *pnLength, SSymbolSet *pSymSet );
bool find_class( SCodeSection *pThis, STxtPage *pPage, int index, int *pnLength, SSymbolSet *pSymSet );
bool find_typedef_struct( SCodeSection *pThis, STxtPage *pPage, int index, int *pnLength, SSymbolSet *pSymSet );
bool find_end_brace( STxtPage *pPage, int index, int *pnLength );
bool find_comment( SCodeSection *pThis, STxtPage *pPage, int index, int *pnLength, SSymbolSet *pSymSet );
bool find_L1L2( SCodeSection *pThis, STxtPage *pPage, int index, int *pnLength );
bool find_ifdef( SCodeSection *pThis, STxtPage *pPage, int index, int *pnLength );


#define MAX_ELEMENTS_IN_CS 200
// the different types of code elements
enum {
  CDE_S_INCLUDE,
  CDE_S_FWDDECL_FN,
  CDE_S_FWDDECL_STR,
  CDE_S_BLANK,
  CDE_S_GLOBAL,
  CDE_S_DEFINE,
  CDE_S_COMMENT,
  CDE_S_VARDECL,
  CDE_S_CODELINE,
  CDE_S_BLOCKSTART,
  CDE_S_SUBBLOCKSTART,
  CDE_ENUM,
  CDE_COMMENT,
  CDE_L1SECTION,
  CDE_L2SECTION,
  CDE_L3SECTION,
  CDE_L4SECTION,
  CDE_TYPEDEF,
  CDE_CLASSDECL,
  CDE_FNDEFN,
  CDE_CLASS_FNDEFN,
  CDE_SUBBLOCK,
  CDE_BLOCK,
  CDE_PREAMBLE,
  CDE_CODEBASE,
  CDE_WX,
  CDE_IFDEF
};
// the code element structure
typedef struct SCodeElement {
  int type;
  bool bSingle; // is this a single line element CDE_S_*
  SCodeSection *pContainer; // the section that contains this element
  int indexContainer; // the index of this element in its parent's element list
  STxtLine *pLine; // the line representing this element
  SCodeSection *pSec; // if this element is a section the sec structure describing it
} SCodeElement;
// allocates, inits and return a code element ptr
// of the given type, with container, it's index in its container and its txt line
// this new is used for single line elements
SCodeElement *new_code_element( int type, SCodeSection *pContainer, int indexContainer, STxtLine *pLine ) {
  SCodeElement *pRetVal = (SCodeElement *) malloc( sizeof(SCodeElement) );
  if (pRetVal != NULL) {
	  pRetVal->type = type;
	  pRetVal->bSingle = true; // for now
	  pRetVal->pContainer = pContainer;
	  pRetVal->indexContainer = indexContainer;
	  pRetVal->pSec = NULL;
	  if (pLine != NULL)
		  pRetVal->pLine = tl_clone(pLine);
  }
  return( pRetVal );
}
int ce_length( SCodeElement *pElem );
int ce_file_offset( SCodeElement *pElem );
// frees a code element
void free_code_element( SCodeElement *pThis ) {
	if (pThis != NULL) {
		if (pThis->pLine != NULL) {
			tl_free(pThis->pLine);
			pThis->pLine = NULL;
		}
		free(pThis);
	}
}
// writes out the source-code of this single code element to the wxTextFile
void ce_write_source_single( SCodeElement *pElem, wxTextFile *pFile ) {
  // write out pLine to pFile
  pFile->AddLine( wxString(pElem->pLine->szBuf) );
}
SCodeSection *new_code_section( SCodeElement *pBaseElem, SSymbolSet *pSymSet, int symLinkType, int symLinkIndex );
SCodeElement *load_code_element( wxFile &File, SCodeSection *pContainer, int indexContainer );
void ce_serialize_base( SCodeElement *pElem, wxFile &File, bool bToFrom );
// struct for a (multi-line) code section element
typedef struct SCodeSection {
  // initializes with a type, parent, index in parent's element list
  // the symbol set that is used in the parsing of the section
  void init( SCodeElement *pBaseElem, SSymbolSet *pSymSet, int symLinkType, int symLinkIndex ) {
    this->pBaseElem = pBaseElem;
    this->pBaseElem->bSingle = false;
    this->pBaseElem->pSec = this;
    this->pSymSet = pSymSet;
    this->ppElements = (SCodeElement **) malloc( MAX_ELEMENTS_IN_CS * sizeof(SCodeElement *));
    this->numElements = 0;
    this->maxElements = MAX_ELEMENTS_IN_CS;
    this->pVarSet = new_var_set();
    this->symLinkType = symLinkType;
    this->symLinkIndex = symLinkIndex;
  }
  void set_symbol_set( SSymbolSet *pSymSet ) {
	this->pSymSet = pSymSet;
  }
  // parses the given page starting from index and parsing length lines
  // load the parsed elements into this Code Section
  // different types of sections have different parsers
  // this fn calls the parser designed for a given section type
  bool parse( STxtPage *pPage, int index, int length, bool bInlineBrace ) {
	bool bRetVal = false;
	// call the parser associated with this section type
	switch( this->pBaseElem->type ) {
	  case CDE_CODEBASE: {
		bRetVal = parse_codebase( this, pPage, index, length, this->pSymSet );
	  }
	  break;
	  case CDE_PREAMBLE: {
		bRetVal = parse_preamble( this, pPage, index, length, this->pSymSet );
	  }
	  break;
	  case CDE_BLOCK: {
		bRetVal = parse_block( this, pPage, index, length, this->pSymSet );
	  }
	  break;
	  case CDE_IFDEF: {
    int type = this->pBaseElem->pContainer->pBaseElem->type; 
		if( type == CDE_CODEBASE || type == CDE_BLOCK || type == CDE_SUBBLOCK )
		  bRetVal = parse_global_scope( this, pPage, index, length, this->pSymSet );
		else // CDE_FNDEFN etc
		  bRetVal = parse_local_scope( this, pPage, index, length, this->pSymSet, bInlineBrace );
	  }
	  break;
	  case CDE_SUBBLOCK: {
		bRetVal = parse_global_scope( this, pPage, index, length, this->pSymSet );
	  }
	  break;
	  case CDE_WX:
	  case CDE_ENUM:
	  case CDE_L4SECTION: {
		bRetVal = parse_not( this, pPage, index, length, this->pSymSet, bInlineBrace );
	  }
	  break;
	  case CDE_FNDEFN:
	  case CDE_CLASS_FNDEFN:
	  case CDE_L1SECTION:
	  case CDE_L2SECTION:
	  case CDE_L3SECTION: {
		bRetVal = parse_local_scope( this, pPage, index, length, this->pSymSet, bInlineBrace );
	  }
	  break;
	  case CDE_CLASSDECL:
	  case CDE_TYPEDEF: {
		bRetVal = parse_struct( this, pPage, index, length, this->pSymSet, bInlineBrace, this->symLinkType, this->symLinkIndex );
	  }
	  break;
	  default: {
		wxLogError("invalid code section type in code section parse");
	  }
	  break;
	}
	return( bRetVal );
  };
  // gets the element at the given lineLocation and walking 'steps' steps
  // where steps maybe positive or negative
  // the walk steps over summarized sections (treats them as 1 step)
  // and steps inside unsummarised sections
  // Hence, the returned element is always a single or a summarized section
  // never an unsummarized section.
  // returns the line offset of the beginning of the returned element from lineLocation
  // lineOffset can be added to lineLocation to skip to the element returned
  // if the specified location is out of range, returns the last in-range element
  // such a case implies lineLocation + *pnLineOffset == 0 or the length of the section - 1
  // this function is extensively used by the SSrcEditor mode's implementation functions
  SCodeElement * get_element_at( int lineLocation, int steps, int *pnLineOffset ) {
    // get the first element in this section
    SCodeElement *pElemNext = get_next_element(NULL, true);
    // first detrmine the element at the start of the walk
    SCodeElement *pElem = NULL;
    // process the first element
    pElem = pElemNext;
    *pnLineOffset = 0;
    // line location > 0, search for the element at the end of lineLocation lines
    if( lineLocation > 0 ) {
      int accum = 0;
      if( pElem->bSingle )
      accum += 1;
      else
      accum += pElem->pSec->get_length();
      bool bEOSec = false;
      // find the element at line location by accumulating lines
      while( accum < lineLocation && !bEOSec ) {
        pElemNext = get_next_element(pElem, true);
        // EOSec, return the last non NULL element and its offsets
        if( pElemNext == NULL )
          bEOSec = true;
        // Not EOSec, accumulate the next element's lines
        else {
          pElem = pElemNext;
          if( pElem->bSingle )
          accum++;
          // it's a summarized section
          else
          accum += pElem->pSec->get_length();

        }
      }
      // if the accumulation overflowed due to a summarized section
      // set *pnLineOffset accordingly
      if( accum > lineLocation ) {
        if( steps >=0 )
          *pnLineOffset = (accum-pElem->pSec->get_length()) - lineLocation;
        else
          *pnLineOffset = lineLocation - accum;
      }
      else if( !bEOSec ) {
        if( steps >= 0 )
          pElem = get_next_element( pElem, true );
      }
    } // end search for staring element at lineOffset
    // now perform the walk

    // forward walk
    if( steps > 0 ) {
      bool bEOSec = false;
      // accumulate element lines for each step in *pnLineOffset
      // special processing if end of section reached
      for( int s=0; s<steps && !bEOSec; s++ ) {
        if( pElem->bSingle )
          *pnLineOffset += 1;
        else
          *pnLineOffset += pElem->pSec->get_length();
        pElemNext = get_next_element( pElem, true );
        // if end of section reached set *pnLineOffset
        // to take lineLocation to the beginning of
        // the last element of the section
        if( pElemNext == NULL ) {
          bEOSec = true;
          *pnLineOffset = this->get_length() - lineLocation;
          if( pElem->bSingle )
          *pnLineOffset -= 1;
          else
          *pnLineOffset -= pElem->pSec->get_length();
        }
        else
          pElem = pElemNext;
      }
    }
    // backward walk
    else if( steps < 0 ) {
      bool bEOSec = false;
      // accumulate element lines for each step in *pnLineOffset
      // special processing if beginning of section reached
      for( int s=0; s<-steps && !bEOSec; s++ ) {
        if( pElem->bSingle )
          *pnLineOffset -= 1;
        else
          *pnLineOffset -= pElem->pSec->get_length();
        pElemNext = get_next_element( pElem, false );
        // if beginning of section reached set *pnLineOffset
        // to take lineLocation to 0
        if( pElemNext == NULL ) {
          bEOSec = true;
          *pnLineOffset = -lineLocation;
        }
        else
          pElem = pElemNext;
      }
    }
    return( pElem );
  };
  // returns the next or previous element depending on bNextPrev
  // or NULL if its the beginning of end of the codebase
  // if pElemFrom is NULL, returns the first  or last element depending on bNextPrev.
  // Note the returned element is either a single or a summarize section
  // never an unsummarized section
  SCodeElement *get_next_element( SCodeElement *pElemFrom, bool bNextPrev ) {
	SCodeElement *pElem = NULL;
	// get next element (not previous)
	if( bNextPrev ) {
	  // load this sections first element into pElem
	  // if we're starting from the beggining of this section
	  if( pElemFrom == NULL ) {
      if( this->ppElements[0]->bSingle )
        pElem = this->ppElements[0];
      else {
        if( this->ppElements[0]->pSec->bSummarized )
        pElem = this->ppElements[0];
        else
        pElem = this->ppElements[0]->pSec->get_next_element( NULL, true );
      }
	  }
	  // find the next element from pElemFrom
	  // we're not starting from the beginning of the section
	  else {
		pElem = pElemFrom;
		bool bEOF = false;
		bool bFound = false;
		// find the next element
		while( !bEOF && !bFound) {
		  // there is no next element
		  // if we are at the root element
		  if( pElem->pContainer == NULL ) {
        pElem = NULL;
        bEOF = true;
      }
		  // we're not at the root element
		  else {
        // goto the next element in the parent section
        // if there are more elements in the parent section
        if( pElem->indexContainer < pElem->pContainer->numElements-1 ) {
          pElem = pElem->pContainer->ppElements[pElem->indexContainer+1];
          // if it's an unsummarized section, get the first element in it
          if( !pElem->bSingle ) {
            if( !pElem->pSec->bSummarized ) {
              pElem = pElem->pSec->get_next_element( NULL, true );
              bFound = true;
            }
            else {
              bFound = true;
            }
          }
          else 
            bFound = true;
        }
        // if there is an element after the parent section return it
        // this is the last element in its parent
        else {
          pElem = get_next_element( pElem->pContainer->pBaseElem, true );
          if( pElem == NULL )
            bEOF = true;
          bFound = true;
        }
		  }
		}
	  }
	}
	// get prev element
	else {
	  // load this section's last element into pElem
	  // if we're starting from the beggining of this section
	  if( pElemFrom == NULL ) {
		// if the last elem is a single, load it
		if( this->ppElements[this->numElements-1]->bSingle )
		  pElem = this->ppElements[this->numElements-1];
		// load a summarized section or the last element of unsummarized
		else {
		  if( this->ppElements[this->numElements-1]->pSec->bSummarized )
			pElem = this->ppElements[this->numElements-1];
		  else
			pElem = this->ppElements[this->numElements-1]->pSec->get_next_element( NULL, false );
		}
	  }
	  // find the prev element from pElemFrom
	  // we're not starting from the beginning of the section
	  else {
		pElem = pElemFrom;
		bool bBOF = false;
		bool bFound = false;
		// find the previous element
		while( !bBOF && !bFound ) {
		  // there is no prev element
		  // if we are at the root section
		  if( pElem->pContainer == NULL ) {
			pElem = NULL;
			bBOF = true;
		  }
		  // we're not at the root section`
		  else {
			// if this is not the first elem in it's container
			// get the prevous element in its container4`
			if( pElem->indexContainer > 0 ) {
			  pElem = pElem->pContainer->ppElements[pElem->indexContainer-1];
			  // if its an unsummarized section get its last element
			  if( !pElem->bSingle ) {
				if( !pElem->pSec->bSummarized ) {
				  pElem = pElem->pSec->get_next_element( NULL, false );
				  bFound = true;
				}
				else
				  bFound = true;
			  }
			  else
				bFound = true;
			}
			// this is the first element in it's container
			else {
			  // if there is an element before this in the container return it
			  // else set EOF and return this element
			  pElem = get_next_element( pElem->pContainer->pBaseElem, false );
			  if( pElem == NULL )
				bBOF = true;
			  bFound = true;
			}
		  }
		}
	  }
	}
	return( pElem );
  }
  // gets the fileoffset of this section
  int get_file_offset() {
	// get the file offset of the container of this element
	// then add the lengths of elements before this one in its container
	int fileOffset = 0;
	// get the file offset of this element's container
	// add the lengths of all elements before this in its container
	// ruling out the trivial case
	if( this->pBaseElem->type != CDE_CODEBASE ) {
	  fileOffset = this->pBaseElem->pContainer->get_file_offset();
	  for( int i=0; i<this->pBaseElem->indexContainer; i++ )
		fileOffset += ce_length( this->pBaseElem->pContainer->ppElements[i] );
	}
	return( fileOffset );
  }

  // get the length in file lines of this section
  int get_length() {
	int length = 0;
	for( int i=0; i<this->numElements; i++ ) {
	  SCodeElement *pElem = this->ppElements[i];
	  if( pElem->bSingle )
		length++;
	  else
		length += pElem->pSec->get_length();
	}
	return( length );
  }
  // adds a single line element of type to the end of the element list
  void add_single( int type, STxtLine *pLine ) {
	SCodeElement *pElem = new_code_element( type, this, this->numElements, pLine );
	this->ppElements[this->numElements] = pElem;
	this->numElements++;
  };
  // adds a parsed section to the end of the element list
  void add_parsed_section( SCodeSection *pSec ) {
	if( this->pBaseElem->type != CDE_PREAMBLE )
	  pSec->bSummarized = true;
	this->ppElements[this->numElements] = pSec->pBaseElem;
	this->numElements++;
  }
  // adds a section to the end of the element list
  bool add_section( int type, int index, int length, STxtPage *pPage, bool bInlineBrace, int symLinkType, int symLinkIndex ) {
	SCodeElement *pBaseElem = new_code_element( type, this, this->numElements, NULL );
	SCodeSection *pSec = new_code_section( pBaseElem, this->pSymSet, symLinkType, symLinkIndex );
	// everything is summarized except the parent block of a sub-block
	// or a preamable
	pSec->bSummarized = true;
	if (pSec->pBaseElem->type == CDE_SUBBLOCK )
		this->bSummarized = false;
	if (pSec->pBaseElem->type == CDE_PREAMBLE)
		pSec->bSummarized = false;
	this->ppElements[this->numElements] = pSec->pBaseElem;
	this->numElements++;
	return(pSec->parse(pPage, index, length, bInlineBrace));
  }
  // collapses this code-section
  // which means summarizing it
  // and recursively summarizing it's parent section
  // until default summarization stage is reached
  // returns the collapsed element's parent
  SCodeElement * collapse() {
	SCodeElement *pElemRoot = NULL;
	switch( this->pBaseElem->type ) {
	  case CDE_CODEBASE: {
		pElemRoot = this->pBaseElem;
	  }
	  case CDE_BLOCK: {
		// if this block has no sub-blocks summarize it
		bool bFound = false;
		for( int i=0; i<this->numElements && !bFound; i++ )
		  if( this->ppElements[i]->type == CDE_SUBBLOCK )
			bFound = true;
		if( !bFound ) {
		  this->bSummarized = true;
		  pElemRoot = this->pBaseElem;
		}
	  }
	  break;
	  case CDE_SUBBLOCK: {
		this->bSummarized = true;
		pElemRoot = this->pBaseElem;
	  }
	  break;
	  default: {
		this->bSummarized = true;
		pElemRoot = this->pBaseElem->pContainer->collapse();
	  }
	  break;
	}
	return( pElemRoot );
  };
  // exapnds this code-section
  // which means un-summarizing it
  // and recursively un-summarizing it's parent section
  // until default summarization stage is reached
  void expand() {
    switch( this->pBaseElem->type ) {
      case CDE_CODEBASE:
      case CDE_SUBBLOCK:          
      case CDE_BLOCK: {
      this->bSummarized = false;
      }
      break;
      default: {
      this->bSummarized = false;
      this->pBaseElem->pContainer->expand();
      }
      break;
    }
  };
  // writes out the source of this code section to the specified source-code file
  // this is recursive operation as it calls write_source on each of it's contained elements
  void write_source( wxTextFile *pFile ) {
	// write out each element in this section
	for( int i=0; i<this->numElements; i++ ) {
	  if( this->ppElements[i]->bSingle )
		ce_write_source_single( this->ppElements[i], pFile );
	  else
		this->ppElements[i]->pSec->write_source( pFile );
	}
  };
  void serialize( wxFile &File, bool bToFrom ) {
    if( bToFrom ) {
      File.Write( &(this->numElements), sizeof( int ) );
      File.Write( &(this->bSummarized), sizeof(bool) );
      for( int i=0; i<this->numElements; i++ ) {
        ce_serialize_base( this->ppElements[i], File, true );
        if( !this->ppElements[i]->bSingle )
          this->ppElements[i]->pSec->serialize( File, true );
      }
      File.Write( &this->symLinkType, sizeof(int) );
      File.Write( &this->symLinkIndex, sizeof(int) );
	  this->pVarSet->serialize(File, bToFrom);
    }
    else {
      File.Read( &(this->numElements), sizeof( int ) );
      File.Read( &(this->bSummarized), sizeof(bool) );
      for (int i = 0; i < this->numElements; i++) {
        this->ppElements[i] = load_code_element(File, this, i);
      }
      File.Read( &this->symLinkType, sizeof(int) );
      File.Read( &this->symLinkIndex, sizeof(int) );
	  this->pVarSet->serialize(File, bToFrom);
	}
  };
  // prints this section
  // this can be used for debugging purposes
  void print() {
	wxLogMessage( this->pBaseElem->pLine->szBuf );
  };
  SCodeElement **ppElements; // the list of elements contained by this section
  SCodeElement *pBaseElem; // pointer to base code element of which this code section is a sub-struct
  SCodeBase *pCodeBase; // if type CDE_CODEBASE, this ptr contains it else NULL
  SSymbolSet *pSymSet; // the symbol set used to parse this section
  SVarSet* pVarSet; // the set of vars declared in this CS. Empty for comments, enums etc
  int symLinkType;
  int symLinkIndex; // stores the symbol info (if any) associated with this codesec.
  // symLinkType = 0 no info, 1 struct, 2 class
  // symLinkIndex = -1, no info >=0 index into pSymSet->pStructSet/pClassSet
  // this info is used in the parsing process
  int numElements;
  int maxElements;
  bool bSummarized;
} SCodeSection;
// news a code section from the base element and the given symbol set
SCodeSection *new_code_section( SCodeElement *pBaseElem, SSymbolSet *pSymSet, int symLinkType, int symLinkIndex ) {
  SCodeSection *pSec = (SCodeSection *) malloc( sizeof(SCodeSection) );
  pSec->init( pBaseElem, pSymSet, symLinkType, symLinkIndex );
  return( pSec );
}
// frees a code section
// this operation is recursive as free is called on ach of the contained elements
void free_code_section( SCodeSection *pSec ) {
	if (pSec != NULL) {
		if (pSec->ppElements != NULL) {
			// free each element in this section
			for (int i = pSec->numElements-1; i >= 0; i--) {
				if (pSec->ppElements[i] != NULL) {
					if (pSec->ppElements[i]->bSingle) {
						free_code_element(pSec->ppElements[i]);
						pSec->ppElements[i] = NULL;
					}
					else {
						free_code_section(pSec->ppElements[i]->pSec);
					}
				}
			}
			free(pSec->ppElements);
		}
		if (pSec->pBaseElem != NULL) {
			free_code_element(pSec->pBaseElem);
			pSec->pBaseElem = NULL;
		}
		if (pSec->pVarSet != NULL) {
			free_var_set(pSec->pVarSet);
			pSec->pVarSet = NULL;
		}
		free(pSec);
	}
}
// writes to or reads from File the state of this single code element
void ce_serialize_base( SCodeElement *pElem, wxFile &File, bool bToFrom ) {
  if( bToFrom ) {
	File.Write( &(pElem->type), sizeof(int) );
	File.Write( &(pElem->bSingle), sizeof(bool) );
	tl_serialize( pElem->pLine, File, true );
  }
  else {
	File.Read( &(pElem->type), sizeof(int) );
	File.Read( &(pElem->bSingle), sizeof(bool) );
	pElem->pLine = tl_load( File );
  }
}
// gets the length of this element
int ce_length( SCodeElement *pElem ) {
  if( pElem->bSingle )
	return( 1 );
  else
	return( pElem->pSec->get_length() );
}
// gets the file offset of this element
// it ignores summarization
int ce_file_offset( SCodeElement *pElem ) {
  // get the file offset of the container of this element
  // then add the length of element before this one in its conatiner
  int fileOffset = 0;
  // get the file offset of this section's  container
  // and add to it the lengths of all the elements
  // contained by the container preceding this one
  // ruling out the trivial case
  if( pElem->type != CDE_CODEBASE ) {
	fileOffset = pElem->pContainer->get_file_offset();
	for( int i=0; i<pElem->indexContainer; i++ )
	  fileOffset += ce_length( pElem->pContainer->ppElements[i] );
  }
  return( fileOffset );
}

// collapses this code-element
// by collapsing it parent section.
// collapsing a section means summarizing it
// and recursively summarizing it's parent section
// until default summarization stage is reached
// returns the root of the collapsed element
SCodeElement * ce_collapse( SCodeElement *pElem ) {
  SCodeElement *pElemRoot;
  pElemRoot = pElem->pContainer->collapse();
  if( !pElem->bSingle )
	pElem->pSec->bSummarized = true;
  return( pElemRoot );
}
// expands (un-collapses) this code-element
// by expanding it parent section.
// expanding a section means unsummarizing it
// and recursively un-summarizing it's parent section
// until default summarization stage is reached
void ce_expand( SCodeElement *pElem ) {
  pElem->pContainer->expand();
}
// SUBBLOCK: PARSING FUNCTIONS
// Functions to parse a source-code file into an SCodeBase.
// We use a top-down line-based parsing approach.
// At the top level which is the code file (SCodeBase),
// we detect blocks that are demarcated by special
// tokens in the comment lines.
// We parse each block detected by detecting sub-blocks
// (which are also demarcated by special tokens in the comment lines)
// and parsing them.
// A sub-block is parsed by looking for tokens at the start of lines
// such as //, #define, #include, typedef, class etc
// if not found, we look for variable declarations
// such as void, int, bool etc.
// We maintain a symbol list of symbols defined by the program
// These symbols are also used to look for variable declarations.
// Once we find a starting token or a variable declaration,
// We look for sectional elements such as open and close braces
// Then we parse the section found depending on the type of section
// e.g. comment section, fn definition section, enum section

bool find_var_decl(STxtPage* pPage, int index, SCodeSection* pThis, int* pnLength, SSymbolSet* pSymSet);

// ancillary fn
// checks if the var type in the pLine is a * or ** type
// we assume format is var* or var *
// returns the length of the var in pLine including the detected *s
int check_var_type_for_ptrs(STxtLine* pLine, char* szVar, bool* pbIsPtr, bool* pbIsPtrPtr, bool* pbIsDeref) {
	int indexEnd = get_sz_length(szVar);
	int retVal = indexEnd;
	if (indexEnd < pLine->length) {
		if (pLine->szBuf[indexEnd] == '*') {
			*pbIsPtr = true;
			retVal = indexEnd + 1;
			if (indexEnd + 1 < pLine->length)
				if (pLine->szBuf[indexEnd + 1] == '*') {
					*pbIsPtrPtr = true;
					retVal = indexEnd + 2;
				}
		}
		else if (pLine->szBuf[indexEnd] == '&') {
			*pbIsDeref = true;
			retVal = indexEnd + 1;
		}
		else if (pLine->szBuf[indexEnd] == ' ') {
			if (indexEnd + 1 < pLine->length)
				if (pLine->szBuf[indexEnd + 1] == '*') {
					*pbIsPtr = true;
					retVal = indexEnd + 2;
					if (indexEnd + 2 < pLine->length)
						if (pLine->szBuf[indexEnd + 2] == '*') {
							*pbIsPtrPtr = true;
							retVal = indexEnd + 3;
						}
				}
				else if (pLine->szBuf[indexEnd + 1] == '&') {
					*pbIsDeref = true;
					retVal = indexEnd + 2;
				}
		}
	}
	return(retVal);
}
// ancillary fn
// checks if the wx var type in the pLine is a * or ** type
// we assume format is wxvar* or wxvar *
// returns the length of the var in pLine including the detected *s
// the szVar is extracted from the line as a word.
// since * is not a separator in tl_extract_word
// szVar may contain a * or ** at its end.
// otherwise the * or ** will come after a space
// the check is based on these 2 aspects
// also edits szVar to remove * or ** if neccesary 
int check_wxvar_type_for_ptrs(STxtLine* pLine, char* szVar, bool* pbIsPtr, bool* pbIsPtrPtr, bool *pbIsDeref) {
	int indexEnd = get_sz_length(szVar);
	int retVal = indexEnd;
	// first check for * or ** at the end of szVar
	// edit szVar to remove trailing *s in this case
	// set retVal
	if (pLine->szBuf[indexEnd - 1] == '*') {
		*pbIsPtr = true;
		if (pLine->szBuf[indexEnd - 2] == '*') {
			*pbIsPtrPtr = true;
			szVar[indexEnd - 2] = 0;
		}
		szVar[indexEnd - 1] = 0;
		retVal = indexEnd;
	}
	else if (pLine->szBuf[indexEnd - 1] == '&') {
		*pbIsDeref = true;
		szVar[indexEnd - 1] = 0;
		retVal = indexEnd;
	}
	// check for space followed by * or **
	else {
		// check for space followed by * or **
		if (indexEnd < pLine->length) {
			// check for * or **
			if (pLine->szBuf[indexEnd] == ' ') {
				// check for * or ** set isPtr bools and retVal
				if (indexEnd + 1 < pLine->length) {
					if (pLine->szBuf[indexEnd+1] == '*') {
						*pbIsPtr = true;
						retVal = indexEnd + 2;
						// check for **
						if (indexEnd + 2 < pLine->length)
							if (pLine->szBuf[indexEnd + 2] == '*') {
								*pbIsPtrPtr = true;
								retVal = indexEnd + 3;
							}
					}
					else if (pLine->szBuf[indexEnd+1] == '&') {
						*pbIsDeref = true;
						retVal = indexEnd + 2;
					}
				}
			}
		}
	}
	return(retVal);
}

// ancillary fn
// checks for a variable type definition at the start of a line
// if found extracts from the line and returns the extracted var type
// accounts for upto 2 type qualifiers in the type defn
// if it's a constructor or destructor *pbConstructor is set true
// caller must free the returned type
bool extract_var(STxtLine* pLine, SSymbolSet *pSymSet, STxtLine **ppClassName, STxtLine **ppVarName, STxtLine **ppVarType, char *pcSeparator) {
	bool bFound = false;
	char szCVars[13][100] = {
	  "void",
	  "bool",
	  "int",
	  "unsigned int",
	  "char",
	  "unsigned char",
	  "float",
	  "double",
	  "virtual",
	  "static",
	  "explicit",
	  "const",
	  "wx"
	};
	bool bConstr = false;
	bool bDestr = false;
	bool bQualifier = false;
	char* szAdded = NULL;
	int varLength = 0;
	tl_trim(pLine);
	// look for a qualifier, add it to the type
	// if qualifier, look for another qualifier add it to the type
	// look for a language type, add it to the type
	// if found, look for ptr or deref
	// look for a wx type, add it to the type
	// if found, look for ptr or deref
	// look for a one of the user defined struct type
	// look for a user defined class type with special casing for constr and destr
	// remove the type info from the pLine
	// look for the name if it's not a constr or destr

	// look for a qualifier, add it to the type
	for( int i=8; i<12 && !bQualifier; i++) 
		if (tl_find(pLine, szCVars[i]) == 0) {
			bQualifier = true;
			szAdded = szCVars[i];
			*ppVarType = new_txt_line(szAdded);
			char* szCutout = tl_cut_out(pLine, 0, get_sz_length(szAdded));
			free(szCutout);
			tl_trim(pLine);
		}
	// if qualifier, look for another qualifier add it to the type
	if (bQualifier) {
		bool bQual2 = false;
		for (int i = 8; i < 12 && !bQual2; i++)
			if (tl_find(pLine, szCVars[i]) == 0) {
				bQual2 = true;
				szAdded = szCVars[i];
				tl_insert(*ppVarType, (char*)" ",  (*ppVarType)->length);
				tl_insert(*ppVarType, szAdded, (*ppVarType)->length);
				char* szCutout = tl_cut_out(pLine, 0, get_sz_length(szAdded));
				free(szCutout);
				tl_trim(pLine);
			}
	}
	bFound = false;
	// look for a language type, add it to the type
	// if found, look for ptr or deref
	for (int i = 0; i < 8 && !bFound; i++)
		if (tl_find(pLine, szCVars[i]) == 0) {
			bFound = true;
			szAdded = szCVars[i];
			// check for * or ** or &
			bool bIsDeref = false;
			bool bIsPtr = false;
			bool bIsPtrPtr = false;
			varLength = check_var_type_for_ptrs(pLine, szAdded, &bIsPtr, &bIsPtrPtr, &bIsDeref);
			if (bQualifier) {
				tl_insert(*ppVarType, (char*)" ", (*ppVarType)->length);
				tl_insert(*ppVarType, szAdded, (*ppVarType)->length);
			}
			else {
				*ppVarType = new_txt_line(szAdded);
			}
			if (bIsPtr && !bIsPtrPtr) {
				tl_insert(*ppVarType, (char*)" *", (*ppVarType)->length);
			}
			else if (bIsPtr && bIsPtrPtr) {
				tl_insert(*ppVarType, (char*)" **", (*ppVarType)->length);
			}
			else if (bIsDeref) {
				tl_insert(*ppVarType, (char*)" &", (*ppVarType)->length);
			}
		}
	// look for a wx type, add it to the type
	// if found, look for ptr or deref
	if (!bFound) {
		int i = 12;
		if (tl_find(pLine, szCVars[i]) == 0) {
			STxtLine* pLineTemp = tl_clone(pLine);
			char cSeparator;
			STxtLine* pLineT = tl_extract_word(pLineTemp, &cSeparator);
			tl_free(pLineTemp);
			pLineTemp = NULL;   
			if (cSeparator == ' ') {
				bFound = true;
				szAdded = pLineT->szBuf;
				// check for * or ** or &
				bool bIsDeref = false;
				bool bIsPtr = false;
				bool bIsPtrPtr = false;
				varLength = check_wxvar_type_for_ptrs(pLine, szAdded, &bIsPtr, &bIsPtrPtr, &bIsDeref);
				if (bQualifier) {
					tl_insert(*ppVarType, (char*)" ", (*ppVarType)->length);
					tl_insert(*ppVarType, szAdded, (*ppVarType)->length);
				}
				else {
					(*ppVarType) = new_txt_line(szAdded);
				}
				if (bIsPtr && !bIsPtrPtr) {
					tl_insert(*ppVarType, (char*)" *", (*ppVarType)->length);
				}
				else if (bIsPtr && bIsPtrPtr) {
					tl_insert(*ppVarType, (char*)" **", (*ppVarType)->length);
				}
				else if (bIsDeref) {
					tl_insert(*ppVarType, (char*)" &", (*ppVarType)->length);
				}
			}
			else
				bFound = false;
			tl_free(pLineT);
      pLineT = NULL;
		}
	}
	// look for a one of the user defined struct type
	if (!bFound) {
		char* szLongest = NULL;
		int lengthLongest = 0;
		for (int i = 0; i < pSymSet->pStructSet->numStructs; i++) {
			if (tl_find(pLine, pSymSet->pStructSet->ppStructs[i]->pName->szBuf) == 0) {
				if (pSymSet->pStructSet->ppStructs[i]->pName->length > lengthLongest) {
					szLongest = pSymSet->pStructSet->ppStructs[i]->pName->szBuf;
					lengthLongest = pSymSet->pStructSet->ppStructs[i]->pName->length;
				}
			}
		}
		if (szLongest != NULL) {
			bFound = true;
			szAdded = szLongest;
			// check for * or **
			bool bIsDeref = false;
			bool bIsPtr = false;
			bool bIsPtrPtr = false;
			varLength = check_var_type_for_ptrs(pLine, szAdded, &bIsPtr, &bIsPtrPtr, &bIsDeref);
			if (bQualifier) {
				tl_insert(*ppVarType, (char*)" ", (*ppVarType)->length);
				tl_insert(*ppVarType, szAdded, (*ppVarType)->length);
			}
			else {
				*ppVarType = new_txt_line(szAdded);
			}
			if (bIsPtr && !bIsPtrPtr) {
				tl_insert(*ppVarType, (char*)" *", (*ppVarType)->length);
			}
			else if (bIsPtr && bIsPtrPtr) {
				tl_insert(*ppVarType, (char*)" **", (*ppVarType)->length);
			}
			else if (bIsDeref) {
				tl_insert(*ppVarType, (char*)" &", (*ppVarType)->length);
			}
		}
	}
	// look for a user defined class type with special casing for constr and destr
	if( !bFound ) {
		char* szLongest = NULL;
		int lengthLongest = 0;
		// check for ClassName (*) var type or constructor or destr defn 
		// all start with ClassName
		for (int i = 0; i < pSymSet->pClassSet->numClasses; i++) 
			// find the longest classname that matches matches pLine
			if (tl_find(pLine, pSymSet->pClassSet->ppClasses[i]->pName->szBuf) == 0) {
				if (pSymSet->pClassSet->ppClasses[i]->pName->length > lengthLongest) {
					szLongest = pSymSet->pClassSet->ppClasses[i]->pName->szBuf;
					lengthLongest = pSymSet->pClassSet->ppClasses[i]->pName->length;
					bFound = true;
				}
			}
		*ppClassName = new_txt_line(szLongest);
		// check for constructor (or destructor defn) 
		// a constructor is either ClassName( paramlist ) ... if it' within a classdecl
		// or ClassName::ClassName( paramlist ) ... if it's a defn
		// or ClassName::~ClassName( parmaList ) .. if it's a detsr defn
		if (szLongest != NULL) {
			if (pLine->szBuf[lengthLongest] == '(') {
				bConstr = true;
				wxASSERT(!bQualifier);
				*ppVarType = new_txt_line(szLongest);
				*ppVarName = tl_clone( *ppVarType );
				// detrmine the length of the var including it's delimiter to be cutout
				varLength = (*ppVarType)->length + 1;
			}
			else {
				STxtLine* pLineT = tl_clone(pLine);
				tl_before_first(pLineT, (char*)"::");
				if (tl_equals_sz(pLineT, szLongest)) {
					tl_free(pLineT);
					pLineT = tl_clone(pLine);
					char* szTemp = tl_cut_out(pLineT, 0, lengthLongest + 2);
					free(szTemp);
					if (tl_find(pLineT, szLongest) == 0) {
						bConstr = true;
						wxASSERT(!bQualifier);
						*ppVarType = new_txt_line(szLongest);
						*ppVarName = tl_clone(*ppVarType);
						// detrmine the length of the var including it's delimiter to be cutout
						varLength = (*ppVarType)->length + 2 + (*ppVarName)->length + 1;
					}
					// check for destr defn
					else {
						STxtLine* pDestr = new_txt_line((char*)"~");
						tl_insert(pDestr, szLongest, pDestr->length);
						if (tl_find(pLineT, pDestr->szBuf) == 0) {
							bConstr = true;
							wxASSERT(!bQualifier);
							*ppVarType = new_txt_line(szLongest);
							*ppVarName = new_txt_line(pDestr->szBuf);
							// detrmine the length of the var including it's delimiter to be cutout
							varLength = (*ppVarName)->length + 2 + (*ppVarType)->length + 1;
						}
						tl_free(pDestr);
						pDestr = NULL;
					}
					tl_free(pLineT);
					pLineT = NULL;
				}
				// it's not a constr check for *, ** and & 
				else {
					bConstr = false;
					wxASSERT(!bQualifier);
					szAdded = szLongest;
					// check for * or **
					bool bIsDeref = false;
					bool bIsPtr = false;
					bool bIsPtrPtr = false;
					varLength = check_var_type_for_ptrs(pLine, szAdded, &bIsPtr, &bIsPtrPtr, &bIsDeref);
					*ppVarType = new_txt_line(szAdded);
					if (bIsPtr && !bIsPtrPtr) {
						tl_insert(*ppVarType, (char*)" *", (*ppVarType)->length);
					}
					else if (bIsPtr && bIsPtrPtr) {
						tl_insert(*ppVarType, (char*)" **", (*ppVarType)->length);
					}
					else if (bIsDeref) {
						tl_insert(*ppVarType, (char*)" &", (*ppVarType)->length);
					}
					tl_free(pLineT);
					pLineT = NULL;
				}
			}
		}
		else {
			// check for destr
			// we only need to check for the the ~ClassName( case, 
			// since the ClassName::~ClassName( case has already been checked
			bDestr = false;
			for (int i = 0; i < pSymSet->pClassSet->numClasses && !bDestr; i++) {
				STxtLine* pDestr = new_txt_line((char*)"~");
				tl_insert(pDestr, pSymSet->pClassSet->ppClasses[i]->pName->szBuf, pDestr->length);
				tl_insert(pDestr, (char*)"(", pDestr->length);
				if (tl_find(pLine, pDestr->szBuf) == 0) {
					// remove the trailing ( from pDestr
					tl_remove(pDestr, (char*)"(");
					szAdded = pDestr->szBuf;
					bDestr = true;
					if (bQualifier) {
						tl_insert(*ppVarType, (char*)" ", (*ppVarType)->length);
						tl_insert(*ppVarType, pSymSet->pClassSet->ppClasses[i]->pName->szBuf, (*ppVarType)->length);
						*ppVarName = new_txt_line(szAdded);
						// detrmine the length of the var including it's delimiter to be cutout
						varLength = (*ppVarName)->length + 1;
					}
					else {
						*ppVarType = new_txt_line(pSymSet->pClassSet->ppClasses[i]->pName->szBuf);
						*ppVarName = new_txt_line(szAdded);
						// detrmine the length of the var including it's delimiter to be cutout
						varLength = (*ppVarName)->length + 1;
					}
				}
				tl_free(pDestr);
				pDestr = NULL;
			}
			bConstr = bDestr;
			bFound = bDestr;
		}
	}

	// remove the type info from the pLine
	// look for the name if it's not a constr or destr
	if (bFound) {
		char* szCutout = tl_cut_out(pLine, 0, varLength);
		free(szCutout);
		tl_trim(pLine);
		if (!bConstr && !bDestr) {
			// first the fn ptr case
			// a fn ptr's name takes the from (*fnPtrName)
			// so we look for the start ( and then the end ) 
			// and set the entire set the contained string as the name
			// we don't parse out the * and the braces for now
			// this can be done at a later stage
			if (pLine->szBuf[0] == '(') {
				bool bFound = false;
				int index = 1;
				while (!bFound && index < pLine->length) {
					if (pLine->szBuf[index] == ')')
						bFound = true;
					else
						index++;
				}
				if (bFound) {
					char* szName = tl_cut_out(pLine, 0, index + 1);
					*ppVarName = new_txt_line(szName);
					free(szName);
					if (pLine->szBuf[0] == '(') {
						szName = tl_cut_out(pLine, 0, 1);
						free(szName);
						*pcSeparator = '(';
					}
				}
			}
			// extract the word and set the name
			else {
				*ppVarName = tl_extract_word(pLine, pcSeparator);
				wxASSERT(*ppVarName != NULL);
				STxtLine* pDummy = NULL;
				STxtLine* pSubName = NULL;
				// check if the name is a composite and separate out the class
				bool bComposite = check_composite_funcname(*ppVarName, &pDummy, &pSubName);
				if (bComposite) {
					tl_free(*ppVarName);
					*ppVarName = pSubName;
          *ppClassName = pDummy;
				}
			}
		}
	}
	return(bFound);
}

// ancillary fn
// finds a CDE_IFDEF section from a line of code in a page
// and parses the section if possible
// if parsed returns true and the length parsed in *pnLength
bool find_ifdef(SCodeSection* pThis, STxtPage* pPage, int index, int* pnLength) {
	bool bFound = false;
	STxtLine* pLine = NULL;
	int idx = index;

	pLine = tl_clone(pPage->ppLines[idx]);
	tl_trim(pLine);
	// look for a starter for a CDE_IFDEF
	if (tl_find(pLine, (char*)"#ifdef") == 0 || tl_find(pLine, (char*)"#ifndef") == 0 || tl_find(pLine, (char*)"#if") == 0) {
		int ifdefIdx = idx;
		bool bEndIfFound = false;
		bool bElseFound = false;
		idx += 1;
		// find the end through an #endif or a #else
		while (!(bEndIfFound || bElseFound) && idx - index < pPage->numLines) {
			STxtLine* pLineThis = tl_clone(pPage->ppLines[idx]);
			tl_trim(pLineThis);
			if (tl_find(pLineThis, (char*)"#endif") == 0)
				bEndIfFound = true;
			else if (tl_find(pLineThis, (char*)"#else") == 0)
				bElseFound = true;
			else
				idx++;
			tl_free(pLineThis);
      pLineThis = NULL;
		}
		if (!bEndIfFound && !bElseFound)
			bFound = false;
		// either #endif or #else found
		else {
			// if #else not found
			// inc idx to include the #endif
		   // and set *pnLength
			if (!bElseFound)
				idx++;
			*pnLength = idx - ifdefIdx;
			// add the #ifdef section
			bFound = pThis->add_section(CDE_IFDEF, ifdefIdx, *pnLength, pPage, false, 0, -1);
			// #else found look for endif and add another section
			if (bElseFound) {
				int indexElse = idx;
				bEndIfFound = false;
				idx++;
				// look for the #endif
				while (!bEndIfFound && idx - index < pPage->numLines) {
					STxtLine* pLineThis = tl_clone(pPage->ppLines[idx]);
					tl_trim(pLineThis);
					if (tl_find(pLineThis, (char*)"#endif") == 0)
						bEndIfFound = true;
					else
						idx++;
					tl_free(pLineThis);
          pLineThis = NULL;     
				}
				if (!bEndIfFound)
					bFound = false;
				// add the second section's length to *pnLength
				// and add the #endif section
				else {
					*pnLength += (idx + 1 - indexElse);
					bFound = pThis->add_section(CDE_IFDEF, indexElse, idx + 1 - indexElse, pPage, false, 0, -1);
				}
			}
		}
	}
	tl_free(pLine);
  pLine = NULL; 
	return(bFound);
}
// ancillary fn used by parse_local_scope
// finds a L1/L2/L3/L4 code-section from a line in  page
// such as if, else, else if, while, for
// and parses the section if possible and adds it to pThis
// if parsed returns true and the length of the section in *pnLength
bool find_L1L2( SCodeSection *pThis, STxtPage *pPage, int index, int *pnLength ) {
  bool bFound = false;
  char szStarts[7][20] = {"if", "else", "else if", "for", "while","switch", "case "};
  STxtLine *pLine = NULL;
  char szOpenBrace[2] = {123, 0};
  int i;
  int idx=index;
  bool bInlineBrace = true;
  int secLength = 0;
  int tempLength = pThis->get_length();

  pLine = tl_clone( pPage->ppLines[idx] );
  tl_trim( pLine );
  // look for one of the 7 start strings
  for( i=0; i<7 && !bFound; i++ ) {
	if( tl_find( pLine, szStarts[i] ) == 0 )
	  bFound = true;
  }
  // if found a startstring
  // find the startstring's open brace
  // which may or may not be on the first line
  // find the end brace
  // add a new section of type depeding on current section's type
  // e.g. current L1, add L2
  if( bFound ) {
	// find the starting brace
	// it's either on the first or second line
	if( tl_find( pPage->ppLines[idx], szOpenBrace ) != -1 )
	  bFound = true;
	else {
	  if( idx+1 < pPage->numLines ) {
		idx++;
		if( tl_find( pPage->ppLines[idx], szOpenBrace ) != -1 )
		  bFound = true;
		else
		  bFound = false;
	  }
	  else
		bFound = false;
	}
	// found a startstring brace, find the end brace
	// and add a subsection to this section depending on type
	if( bFound ) {
	  if( idx == index )
		bInlineBrace = true;
	  else
		bInlineBrace = false;
	  // find the end brace and add a subsection to this section depending on type
	  if( find_end_brace( pPage, idx, &secLength) ) {
		bFound = true;
		idx += secLength;
		*pnLength = idx - index;
		if (pThis->pBaseElem->type == CDE_L3SECTION) {
			pThis->add_section(CDE_L4SECTION, index, *pnLength, pPage, bInlineBrace, 0, -1);
			tempLength = pThis->get_length();
		}
		else if( pThis->pBaseElem->type == CDE_L2SECTION ) {
		  pThis->add_section( CDE_L3SECTION, index, *pnLength, pPage, bInlineBrace, 0, -1 );
		  tempLength = pThis->get_length();
		}
		else if( pThis->pBaseElem->type == CDE_L1SECTION ) {
		  pThis->add_section( CDE_L2SECTION, index, *pnLength, pPage, bInlineBrace, 0, -1 );
		  tempLength = pThis->get_length();
		}
		else {
		  pThis->add_section( CDE_L1SECTION, index, *pnLength, pPage, bInlineBrace, 0, -1 );
		  tempLength = pThis->get_length();
		}
	  }
	  else
		bFound = false;
	}
	else
	  bFound = false;
  }
  else
	bFound = false;
  tl_free( pLine );
  pLine = NULL;
  return( bFound );
}
// ancillary fn
// finds a comment section or single comment from a line in a page
// and parses the section if possible and adds it to pThis
// if parsed returns true and the length of the parsed section in *pnLength
bool find_comment( SCodeSection *pThis, STxtPage *pPage, int index, int *pnLength, SSymbolSet *pSymSet ) {
  int idx = index;
  bool bFound = false;
  STxtLine *pLine = tl_clone( pPage->ppLines[idx] );
  tl_trim( pLine );
  // look for a comment starter
  // look for a multiline comment in which case its a section
  // determine the end of the section
  // and add it
  if( tl_find( pLine, (char*)"//" ) == 0 ) {
	// differentiate between comment section and single line comment
	if( idx+1 < pPage->numLines ) {
	  STxtLine *pLineTemp = tl_clone( pPage->ppLines[idx+1] );
	  tl_trim( pLineTemp );
	  // second line is not a comment so add a single
	  if( tl_find( pLineTemp, (char*)"//" ) != 0 ) {
		pThis->add_single( CDE_S_COMMENT, pPage->ppLines[idx] );
		*pnLength = 1;
		bFound = true;
	  }
	  // it's a section
	  // determine the end of the section
	  // and add it
	  else {
		bFound = false;
		idx += 1;
		// determine the end of the section
		// and add it
		while( !bFound && idx < pPage->numLines ) {
		  // if it's still a comment line inc idx check for end of page
		  STxtLine *pLineT = tl_clone(pPage->ppLines[idx]);
		  tl_trim( pLineT );
		  if( tl_find( pLineT, (char*)"//" ) == 0 && tl_find(pLineT, (char*)"// BLOCK:")==-1 && tl_find(pLineT, (char*)"// SUBBLOCK:")==-1 )
			idx++;
		  // it's no longer a comment line, close and add the comment section
		  else {
			*pnLength = idx - index;
			SCodeElement *pBaseElem = new_code_element(CDE_COMMENT, pThis, pThis->numElements, NULL);
			SCodeSection *pSec = new_code_section( pBaseElem, pSymSet, 0, -1 );
			for( int i=0; i<*pnLength; i++ ) {
			  pSec->add_single( CDE_S_COMMENT, pPage->ppLines[index+i] );
			}
			// create and set the summary line for this section
			STxtLine *pLineSum = tl_clone( pPage->ppLines[index] );
			char szAppend[7] = {' ',123,'.','.','.',125,0};
			tl_insert( pLineSum, szAppend, pLineSum->length );
			pSec->pBaseElem->pLine = pLineSum;
			pThis->add_parsed_section( pSec );
			bFound = true;
		  }
		  tl_free( pLineT );
      pLineT = NULL;  
		} // end look for end of a comment section
	  } // end case it's not the end of the page
	  tl_free(pLineTemp);
    pLineTemp = NULL;  
	}
	// it's the end of the page so just add a comment line
	else {
	  pThis->add_single( CDE_S_COMMENT, pPage->ppLines[idx] );
	  bFound = true;
	  *pnLength = 1;
	}
  } // end case // comment
  // look for a /* */ comment block
  else if( tl_find( pLine, (char*)"/*" ) == 0 ) {
	// look for close comment
	bool bCloseComment = false;
	int ccIdx = idx;
	idx++;
	// look for close comment
	while( !bCloseComment && idx < pPage->numLines ) {
	  if( tl_find( pPage->ppLines[idx], (char*)"*/") != -1 )
		bCloseComment = true;
	  else
		idx++;
	}
	// found the close comment token
	// create a parsed section and add it to pThis
	if( bCloseComment ) {
	  *pnLength = idx + 1 - ccIdx;
	  SCodeElement *pBaseElem = new_code_element(CDE_COMMENT, pThis, pThis->numElements, NULL);
	  SCodeSection *pSec = new_code_section( pBaseElem, pSymSet, 0, -1 );
	  for( int i=0; i<*pnLength; i++ ) {
		pSec->add_single( CDE_S_COMMENT, pPage->ppLines[index+i] );
	  }
	  // create and set the summary line for this section
	  STxtLine *pLineSum = tl_clone( pPage->ppLines[index] );
	  char szAppend[7] = {' ',123,'.','.','.',125,0};
	  tl_insert( pLineSum, szAppend, pLineSum->length );
	  pSec->pBaseElem->pLine = pLineSum;
	  pThis->add_parsed_section( pSec );
	  bFound = true;
	}
	else
	  bFound = false;
  }
  else
	bFound = false;
  tl_free(pLine);
  pLine = NULL;  
  return( bFound );
}
// ancillary fn
// finds an end brace from a line in a page
// it is assumed a startbrace was found on the preceding line
// if found returns true and the number of lines parsed  in *pnLength
bool find_end_brace( STxtPage *pPage, int index, int *pnLength ) {
  char szOpenBrace[2] = {123, 0};
  char szCloseBrace[2] = {125,0};
  int idx = index;
  int numBraces = 1;
  bool bFound = false;
  // special case, end brace on same line as startbrace
  if( tl_find( pPage->ppLines[idx], szCloseBrace ) != -1 ) {
	bFound = true;
	*pnLength = 1;
  }
  // normal processing
  else {
	// find the close brace
	// by searching for open or close braces
	// and inc or dec numBraces
	// till all braces are paired and numBraces == 0
	while( numBraces > 0 && idx < pPage->numLines-1 ) {
	  idx++;
	  if( tl_find( pPage->ppLines[idx], szOpenBrace ) != -1 )
		numBraces++;
	  if( tl_find( pPage->ppLines[idx], szCloseBrace ) != -1 )
		numBraces--;
	}
	if( numBraces > 0 )
	  bFound = false;
	// set the *pnLength retval
	else {
	  *pnLength = idx + 1 - index;
	  bFound = true;
	}
  }
  return( bFound );
}
// ancillary fn
// finds a typedef struct or union section from a line in a page
// and parses the section if possible and adds it to pThis
// if parsed returns true and the length of the parsed section in *pnLength
bool find_typedef_struct( SCodeSection *pThis, STxtPage *pPage, int index, int *pnLength, SSymbolSet *pSymSet ) {
  int idx = index;
  bool bFound = false;
  int secLength = 0;
  STxtLine *pLine = tl_clone( pPage->ppLines[idx] );
  tl_trim( pLine );

  // find the startstring and the startbrace
  // find the endbrace
  // add a CDE_TYPEDEF section to pThis
  // extract the name of the symbol found
  // and add it to pSymSet
  if( tl_find( pLine, (char*)"typedef struct" ) == 0 || tl_find( pLine, (char*)"typedef union" ) == 0 ) {
	bool bStartBraceFound = false;
	bool bInlineBrace = true;
	char szBraceOpen[2] = {123, 0};
	// find the startBrace
	while( !bStartBraceFound && idx < pPage->numLines ) {
	  if( tl_find( pPage->ppLines[idx], szBraceOpen ) != -1 )
		bStartBraceFound = true;
	  else
		idx++;
	}
	// find the endbrace
	// add a CDE_TYPEDEF section to pThis
	// extract the name of the symbol found
	// and add it to pSymSet
	if( bStartBraceFound ) {
	  if( idx == index )
		bInlineBrace = true;
	  else
		bInlineBrace = false;
	  // add a CDE_TYPEDEF section to pThis
	  // extract the name of the symbol found
	  // and add it to pSymSet
	  if( find_end_brace( pPage, idx, &secLength ) ) {
		idx += secLength;
		*pnLength = idx - index;
		// extract the name of the symbol from the string
		char szBraceOpen[2] = { 123, 0 };
		STxtLine* pLineTemp = tl_clone(pPage->ppLines[index]);
		tl_remove(pLineTemp, (char*)"typedef struct");
		tl_remove(pLineTemp, (char*)"typedef union");
		tl_remove(pLineTemp, szBraceOpen);
		tl_trim(pLineTemp);
		SStruct* pSymStruct = new_struct(pLineTemp, NULL);
		tl_free(pLineTemp);
    pLineTemp = NULL;  
		int symLinkIndex = pSymSet->pStructSet->add_struct(pSymStruct);
		pThis->add_section( CDE_TYPEDEF, index, *pnLength, pPage, bInlineBrace, 1, symLinkIndex );
		bFound = true;
		int fileOffset = ce_file_offset(pThis->ppElements[pThis->numElements - 1]);
		SLocation* pLocation = new_location(pThis->ppElements[pThis->numElements - 1], fileOffset);
		pSymStruct->pLocation = pLocation;
	  }
	  else
		bFound = false;
	}
	else
	  bFound = false;
  }
  tl_free(pLine);
  pLine = NULL;  
  return( bFound );
}
// ancillary fn
// finds a class (fwd decl or section) from a line in a page
// and parses the section if possible and adds it to pThis
// if parsed returns true and the length of the parsed section in *pnLength
bool find_class( SCodeSection *pThis, STxtPage *pPage, int index, int *pnLength, SSymbolSet *pSymSet ) {
  int idx = index;
  bool bFound = false;
  int secLength = 0;
  int prevSecLength = pThis->get_length();
  STxtLine *pLine = tl_clone( pPage->ppLines[idx] );
  tl_trim( pLine );

  // differentiate between fwd decl and class def
  // if its a fwd decl add a single
  // if its a class def
  // determine its start and end
  // and add a section to pThis to parse it
  if( tl_find( pLine, (char*)"class" ) == 0 ) {
	char szBraceOpen[2] = {123, 0};
	bool bInlineBrace = true;
	bool bClassDefn = false;
	// its a fwd decl
	if( tl_find( pPage->ppLines[idx], (char*)";" ) != -1  ) {
	  pThis->add_single( CDE_S_FWDDECL_STR, pPage->ppLines[idx] );
	  *pnLength = 1;
	  bFound = true;
	  // extract the name of the symbol from the string
	  STxtLine *pLineTemp = tl_clone( pPage->ppLines[index] );
	  tl_remove( pLineTemp, (char *) "class ");
	  tl_before_first(pLineTemp, (char *) ";");
	  int fileOffset = ce_file_offset( pThis->ppElements[pThis->numElements-1] );
	  SClass* pSymClass = new_class(pLineTemp, new_location(pThis->ppElements[pThis->numElements - 1], fileOffset));
	  pSymSet->pClassSet->add_class( pSymClass );
	  tl_free(pLineTemp);
    pLineTemp = NULL;  
	}
	// it's a class defn
	// determine its start and end
	// and add a section to pThis to parse it
	else {
	  // find brace open
	  bFound = false;
	  // find brace open
	  while( !bFound && idx < pPage->numLines ) {
		if( tl_find( pPage->ppLines[idx], szBraceOpen) != -1 )
		  bFound = true;
		else
		  idx++;
	  }
	  // find the end brace
	  // add class decl section to pThis
	  // extract the symbol and add it to the symbol set
	  if( bFound ) {
		if( idx == index )
		  bInlineBrace = true;
		else
		  bInlineBrace = false;
		// add class decl section to pThis
		// extract the symbol and add it to the symbol set
		if( find_end_brace(pPage, idx, &secLength ) ) {
		  idx += secLength;
		  *pnLength = idx - index;
		  // extract the name of the symbol from the string
		  STxtLine* pLineTemp = tl_clone(pPage->ppLines[index]);
		  tl_remove(pLineTemp, (char*)"class ");
		  tl_before_first(pLineTemp, (char*)" ");
		  SClass* pSymClass = new_class(pLineTemp, NULL);
		  tl_free(pLineTemp);
      pLineTemp = NULL;  
		  int symLinkIndex = pSymSet->pClassSet->add_class(pSymClass);
		  pThis->add_section(CDE_CLASSDECL, index, *pnLength, pPage, bInlineBrace, 2, symLinkIndex );
		  int afterAddLength = pThis->get_length();
		  if ( afterAddLength - prevSecLength != *pnLength) {
			  wxString msg;
			  msg.Printf("parse error at %d", idx - secLength);
			  wxLogMessage(msg);
		  }
		  // set the symbols location, now that the section has been added
		  int fileOffset = ce_file_offset( pThis->ppElements[pThis->numElements-1] );
		  SLocation *pLocation = new_location(pThis->ppElements[pThis->numElements - 1], fileOffset);
		  pSymClass->pLocation = pLocation;
		  bFound = true;
		}
		else
		  bFound = false;
	  }
	  // did not find a brace open
	  else
		bFound = false;
	}
  }
  // startstring not found
  else
	bFound = false;
  tl_free(pLine);
  pLine = NULL;  
  return( bFound );
}
// ancillary fn
// finds an enum starting from a line in a page
// and adds it to pThis for parsing
// if parsed returns true and the length of the parsed section in *pnLength
bool find_enum( SCodeSection *pThis, STxtPage *pPage, int index, int *pnLength, SSymbolSet *pSymSet ) {
  int idx = index;
  bool bFound = false;
  int secLength = 0;

  STxtLine *pLine = tl_clone( pPage->ppLines[idx] );
  tl_trim( pLine );

  // find the open brace which may or may not be on the first line
  // find the close brace
  // add a CDE_ENUM section to pThis
  if( tl_find( pLine, (char*)"enum" ) == 0 ) {
	bool bInlineBrace = true;
	char szBraceOpen[2] = {123, 0};
	bFound = false;
	// find the startbrace
	while( !bFound && idx < pPage->numLines ) {
	  if( tl_find( pPage->ppLines[idx], szBraceOpen ) != -1 )
		bFound = true;
	  else
		idx++;
	}
	// find the close brace
	// add a CDE_ENUM section ot pThis
	if( bFound ) {
	  if( idx == index )
		bInlineBrace = true;
	  else
		bInlineBrace = false;
	  // add a CDE_ENUM section to pThis
	  if( find_end_brace( pPage, idx, &secLength ) ) {
		idx += secLength;
		*pnLength = idx - index;
		pThis->add_section( CDE_ENUM, index, *pnLength, pPage, bInlineBrace, 0, -1 );
		bFound = true;
	  }
	  else
		bFound = false;
	}
	else
	  bFound = false;
  }
  tl_free(pLine);
  pLine = NULL;  
  return( bFound );
}
// parses the elements in a fn scope code section
// i.e. a block of code inside a function definition
// it is block of control flow such as if, else, for, while
// demarcated by braces.
// AN L2 section is a control flow block inside an L1 section
// We parse L1, L2,L3 and L4 sections in order to be able to summarize them
// which makes the code easier to read.
// The parsing of L1, L2, L3 and L4 sections is identical
bool parse_local_scope( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet, bool bInlineBrace ) {
  // look for if( .. , else , else if( .. , for( .. and while( ..
  // as subsection starters and detect their corresponding ends
  // by tracking brace counts
  int secLength;
  int prevSecLength = pThis->get_length();
  int idx = index;
  bool bRetVal = true;

  // first create and set the summary line for this section
  STxtLine *pLineSum = tl_clone( pPage->ppLines[index] );
  // append a close brace to the summary line
  if( bInlineBrace ) {
	char szAppend[5] = {'.','.','.',125,0};
	tl_insert( pLineSum, szAppend, pLineSum->length );
  }
  // append an open and close brace to the summary line
  else {
	char szAppend[7] = {' ',123,'.','.','.',125,0};
	tl_insert( pLineSum, szAppend, pLineSum->length );
  }
  pThis->pBaseElem->pLine = pLineSum;
  // add the first line and start with the second
  pThis->add_single( CDE_S_CODELINE, pPage->ppLines[index] );
  prevSecLength = pThis->get_length();
  int fileOffset = ce_file_offset(pThis->ppElements[pThis->numElements - 1]);
  idx++;
  // search for L1L2 section starters
  // ifdefs
  // comment sections and blanks
  // else add a codeline
  while( idx-index < length ) {
	  if (find_L1L2(pThis, pPage, idx, &secLength)) {
		  idx += secLength;
		  if (pThis->get_length() - prevSecLength != secLength) {
			  wxString msg;
			  msg.Printf("parse error at %d", idx - secLength);
			  wxLogMessage(msg);
		  }
		  else
			  prevSecLength = pThis->get_length();
	  }
	  else if (find_ifdef(pThis, pPage, idx, &secLength)) {
		  idx += secLength;
		  if (pThis->get_length() - prevSecLength != secLength) {
			  wxString msg;
			  msg.Printf("parse error at %d", idx - secLength);
			  wxLogMessage(msg);
		  }
		  else
			  prevSecLength = pThis->get_length();
	  }
	  else if (find_comment(pThis, pPage, idx, &secLength, pSymSet)) {
		  idx += secLength;
		  if (pThis->get_length() - prevSecLength != secLength) {
			  wxString msg;
			  msg.Printf("parse error at %d", idx - secLength);
			  wxLogMessage(msg);
		  }
		  else
			  prevSecLength = pThis->get_length();
	  }
	  // add a codeline or a blank
	else {
		if (tl_isempty(pPage->ppLines[idx])) {
			pThis->add_single(CDE_S_BLANK, pPage->ppLines[idx]);
			prevSecLength = pThis->get_length();
			idx++;
		}
	  else {
		  int secLength = 0;
		  if (find_var_decl(pPage, idx, pThis, &secLength, pSymSet)) {
			  idx += secLength;
			  if (pThis->get_length() - prevSecLength != secLength) {
				  wxString msg;
				  msg.Printf("parse error at %d", idx - secLength);
				  wxLogMessage(msg);
			  }
			  else
				  prevSecLength = pThis->get_length();
		  }
		  else {
			  pThis->add_single(CDE_S_CODELINE, pPage->ppLines[idx]);
			  prevSecLength = pThis->get_length();
			  idx++;
		  }
	  }
	}
  } // while idx < length
  return( bRetVal );
}
// ancillary fn used by parse_global_scope
// parses startstring based lines in a txtpage and returns the number of lines parsed.
// puts the results of the parse into the Code Section
bool find_startstring_parse( STxtPage *pPage, int index, SCodeSection *pThis, int *pnLength, SSymbolSet *pSymSet ) {
  int secLength = 0;
  int idx = index;
  bool bFound = false;
  bool bExit = false;
  STxtLine *pLine = NULL;
  int prevSecLength = pThis->get_length();

  // exit conditions for this parse search
  // a startstring line or section was found,
  // explicit exit if nothing was found
  // or a section parse ran out of lines in looking for a brace set
  while( !bFound && idx < pPage->numLines && !bExit ) {
    pLine = tl_clone( pPage->ppLines[idx] );
    tl_trim( pLine );
    // look for #include and add a single
    if( tl_find( pLine, (char*)"#include" ) == 0 ) {
      pThis->add_single( CDE_S_INCLUDE, pPage->ppLines[idx] );
      *pnLength = 1;
      prevSecLength = pThis->get_length();
      bFound = true;
    }
    // look for class
    else if( find_class( pThis, pPage, idx, &secLength, pSymSet ) ) {
      *pnLength = secLength;
      int afterAddLength = pThis->get_length();
      if ( afterAddLength - prevSecLength != secLength) {
        wxString msg;
        msg.Printf("parse error at %d", idx - secLength);
        wxLogMessage(msg);
        bFound = false;
      }
      else {
        prevSecLength = pThis->get_length();
        bFound = true;
      }
    }
    // look for struct add a single, extract the symbol and add to pSymSet
    else if( tl_find( pLine, (char*)"struct" ) == 0 ) {
      // we assume this is a fwd decl
      pThis->add_single( CDE_S_FWDDECL_STR, pPage->ppLines[idx] );
      *pnLength = 1;
      prevSecLength = pThis->get_length();
      bFound = true;
      // extract the name of the symbol from the string
      STxtLine * pLineTemp = tl_clone( pPage->ppLines[idx] );
      tl_remove( pLineTemp, (char *) "struct " );
      tl_remove( pLineTemp, (char *) ";" );
      tl_trim( pLineTemp );

      int fileOffset = ce_file_offset( pThis->ppElements[pThis->numElements-1] );
      SStruct* pSymStruct = new_struct(pLineTemp, new_location(pThis->ppElements[pThis->numElements - 1], fileOffset));
      tl_free(pLineTemp);
      pLineTemp = NULL;  
      pSymSet->pStructSet->add_struct(pSymStruct);
    }
    // look for enum
    else if( find_enum( pThis, pPage, idx, &secLength, pSymSet ) ) {
      *pnLength = secLength;
      int afterAddLength = pThis->get_length();
      if (afterAddLength - prevSecLength != secLength) {
        wxString msg;
        msg.Printf("parse error at %d", idx - secLength);
        wxLogMessage(msg);
        bFound = false;
      }
      else {
        prevSecLength = pThis->get_length();
        bFound = true;
      }
    }
    // look for typedef struct or union
    else if( find_typedef_struct( pThis, pPage, idx, &secLength, pSymSet ) ) {
      *pnLength = secLength;
      int afterAddLength = pThis->get_length();
      if (afterAddLength - prevSecLength != secLength) {
        wxString msg;
        msg.Printf("parse error at %d", idx - secLength);
        wxLogMessage(msg);
        bFound = false;
      }
      else {
        prevSecLength = pThis->get_length();
        bFound = true;
      }
    }
    // look for #define
    else if( tl_find( pLine, (char*)"#define" ) == 0 ) {
      pThis->add_single( CDE_S_DEFINE, pPage->ppLines[idx] );
      *pnLength = 1;
      prevSecLength = pThis->get_length();
      bFound = true;
    }
    // look for Comment
    else if( find_comment( pThis, pPage, idx, &secLength, pSymSet ) ) {
      *pnLength = secLength;
      int afterAddLength = pThis->get_length();
      if (afterAddLength - prevSecLength != secLength) {
        wxString msg;
        msg.Printf("parse error at %d", idx - secLength);
        wxLogMessage(msg);
        bFound = false;
      }
      else {
        prevSecLength = pThis->get_length();
        bFound = true;
      }
    }
    // look for wxBEGIN - wxEND
    else if( tl_find( pLine, (char*)"wxBEGIN" ) == 0 ) {
      bool bWXFound = false;
      idx++;
      // find the wxEND
      while( !bWXFound && idx < pPage->numLines ) {
        if( tl_find( pPage->ppLines[idx], (char *) "wxEND") != -1 )
          bWXFound = true;
        else
          idx++;
        }
        if( !bWXFound ) {
          bExit = true;
          bFound = false;
        }
        // add a CDE_WX section to pThis
        else {
        *pnLength = idx + 1 - index;
        pThis->add_section( CDE_WX, index, *pnLength, pPage, false, 0, -1 );
        int afterAddLength = pThis->get_length();
        if (afterAddLength - prevSecLength != *pnLength) {
          wxString msg;
          msg.Printf("parse error at %d", idx - *pnLength);
          wxLogMessage(msg);
          bFound = false;
        }
        else {
          prevSecLength = pThis->get_length();
          bFound = true;
        }
      }
    }
    // look for wxIMPLEMENT and add a codeline
    else if( tl_find( pLine, (char*)"wxIMPLEMENT" ) == 0 ) {
      *pnLength = 1;
      pThis->add_single( CDE_S_CODELINE, pPage->ppLines[idx] );
      prevSecLength = pThis->get_length();
      bFound = true;
    }
    // look for #ifdef or #ifndef or #if
    else if( find_ifdef( pThis, pPage, idx, &secLength ) ) {
      *pnLength = secLength;
      int afterAddLength = pThis->get_length();
      if (afterAddLength - prevSecLength != secLength) {
        wxString msg;
        msg.Printf("parse error at %d", idx - secLength);
        wxLogMessage(msg);
        bFound = false;
      }
      else {
        prevSecLength = pThis->get_length();
        bFound = true;
      }
    }
    // look for special case #endif which concludes a CDE_IFDEF
    else if( tl_find( pLine, (char*)"#endif" ) != -1 ) {
      *pnLength = 1;
      pThis->add_single( CDE_S_CODELINE, pPage->ppLines[idx] );
      prevSecLength = pThis->get_length();
      bFound = true;
    }
    // look for special case #error
    else if( tl_find( pLine, (char*)"#error" ) != -1 ) {
      *pnLength = 1;
      pThis->add_single( CDE_S_CODELINE, pPage->ppLines[idx] );
      prevSecLength = pThis->get_length();
      bFound = true;
    }
    // look for blank line
    else if( tl_isempty( pLine ) ) {
      *pnLength = 1;
      pThis->add_single( CDE_S_BLANK, pPage->ppLines[idx] );
      prevSecLength = pThis->get_length();
      bFound = true;
    }
    // couldn't find any startstring signal exit
    else {
      bFound = false;
      bExit = true;
    }
    tl_free( pLine );
    pLine = NULL;  
  } // end while search for a startstring
  return( bFound );
}
#define NUM_LANG_VARS 13

// ancillary fn 
// 
// parses variable declaration based lines in a txtpage and returns the number of lines parsed.
// adds the parsed elements or sections to the CodeSection
// and any vars or functions to respective varsets or function sets
// depending on the context of the call
// this fn is called in 2 contexts:
// 1. While parsing a struct or class
// 2. While parsing a block or sub-block or from parse_global_scope
// In the first case, the fns or vars are added to the struct or class 
// in the latter they are added to the codebase symbol-set
bool find_var_decl( STxtPage *pPage, int index, SCodeSection *pThis, int *pnLength, SSymbolSet *pSymSet ) {
	int numLines = pPage->numLines;
	int idx = index;
	int secLength = 0;
	bool bInlineBrace = true;
	bool bFound = false;
	bool bRetVal = false;
	bool bGlobalContext = true;
	int type = pThis->pBaseElem->type;
	if (type == CDE_CODEBASE || type == CDE_BLOCK || type == CDE_SUBBLOCK)
		bGlobalContext = true;
	else
		bGlobalContext = false;

	STxtLine *pLineT = tl_clone( pPage->ppLines[idx] );
	tl_trim( pLineT );
	STxtLine* pVarType = NULL;
	STxtLine* pVarName = NULL;
	STxtLine* pClassName = NULL;
	// it is assumed that the var fits in a single line
	SClass* pContext = NULL;
	if (pThis->pBaseElem->type == CDE_CLASSDECL)
		pContext = pSymSet->pClassSet->ppClasses[pThis->symLinkIndex];
	char cSeparator;
	bool bIsVar = extract_var(pLineT, pSymSet, &pClassName, &pVarName, &pVarType, &cSeparator);

	if (bIsVar) {
		// first determine if it's a var decl, fn decl or fn defn
		// if var decl or fn decl parse and complete
		// if it's a fn defn, further process
		bool bFnDecl = false;
		bool bVarDecl = false;
		bool bFnDefn = false;
		int fnDefnLength = 0;
		// check if it's just a var decl
		// add the var to this section's var set
		// add a var decl single to pThis
		// the check accounts for the case where a var decl includes an assignment
		// or the var is a class with a constructor
		if (cSeparator == ';' || pLineT->szBuf[0] == '=') {
			bVarDecl = true;
			pThis->add_single(CDE_S_VARDECL, pPage->ppLines[idx]);
			*pnLength = 1;
			bFound = true;
			int fileOffset = ce_file_offset(pThis->ppElements[pThis->numElements - 1]);
			SLocation* pLocation = new_location(pThis->ppElements[pThis->numElements - 1], fileOffset);
			SVar* pVar = new_var(pVarName, pVarType, pLocation);
			if (bGlobalContext)
				pThis->pVarSet->add_var(pVar);
			else
				if (pThis->pBaseElem->type == CDE_CLASSDECL) {
					SClass* pClass = pSymSet->pClassSet->ppClasses[pThis->symLinkIndex];
					pClass->add_var(pVar);
				}
				else if( pThis->pBaseElem->type == CDE_TYPEDEF ) {
					SStruct* pStruct = pSymSet->pStructSet->ppStructs[pThis->symLinkIndex];
					pStruct->add_var(pVar);
				}
				else
					pThis->pVarSet->add_var(pVar);
			bRetVal = true;
		}
		// check if it's a func decl. We assume it's on a single line
		// we also special case for a wxOVERRIDE; at the end of fn decl
		else if (tl_find(pLineT, (char*)");") != -1 || tl_find(pLineT, (char*)"wxOVERRIDE;") != -1) {
			*pnLength = 1;
			// a fn decl can only be in a global context or in a class decl
			// otherwise it's a class var decl
			if (!(bGlobalContext || pThis->pBaseElem->type == CDE_CLASSDECL) ) {
				bVarDecl = true;
				pThis->add_single(CDE_S_VARDECL, pPage->ppLines[idx]);
				bFound = true;
				int fileOffset = ce_file_offset(pThis->ppElements[pThis->numElements - 1]);
				SLocation* pLocation = new_location(pThis->ppElements[pThis->numElements - 1], fileOffset);
				SVar* pVar = new_var(pVarName, pVarType, pLocation);
				if (pThis->pBaseElem->type == CDE_CLASSDECL) {
					SClass* pClass = pSymSet->pClassSet->ppClasses[pThis->symLinkIndex];
					pClass->add_var(pVar);
				}
				else if (pThis->pBaseElem->type == CDE_TYPEDEF) {
					SStruct* pStruct = pSymSet->pStructSet->ppStructs[pThis->symLinkIndex];
					pStruct->add_var(pVar);
				}
				else
					pThis->pVarSet->add_var(pVar);
				bRetVal = true;
			}
			else {
				bFnDecl = true;
        fnDefnLength = 1;
      }
		}
		// check for fn defn
		else {
			char szBraceOpen[2] = { 123, 0 };
			char szBraceClose[2] = { 125, 0 };
			int idx = index;
			bool bBraceFound = false;
			// find the braceOpen
			while (!bBraceFound && idx < pPage->numLines) {
				if (tl_find(pPage->ppLines[idx], szBraceOpen) != -1)
					bBraceFound = true;
				else
					idx++;
			}
			fnDefnLength = idx - index + 1;
			// find the brace close
			if (bBraceFound) {
				if (idx == index)
					bInlineBrace = true;
				else
					bInlineBrace = false;
				// add a CDE_FNDEFN section to pThis
				if (find_end_brace(pPage, idx, &secLength)) {
					idx += secLength;
					*pnLength = idx - index;
					bFnDefn = true;
				}
			}
		}

		// find the fn signature
		// create a fn and add it depending on context
		if (bFnDefn || bFnDecl) {
			// the fucntion declaration is potentially spread across multiple code line.
			// the first 2 elements of the decl are return type and name
			// thereafter is a param list with a variable number of params till decl close
			// so we start with the first 2 elements which are fixed
			// then keep extracting params till decl close
			int idx = index;
			STxtLine* pLine = tl_clone(pPage->ppLines[idx]);
			bool bContinue = true;
			bool bComposite = false;
			STxtLine* pClass = NULL;
			STxtLine* pReturnType = NULL;
			STxtLine* pFuncName = NULL;
			STxtLine* pSubName = NULL;
			SVarSet* pVarSet = NULL;
			// extract the return type and name of the func
			// then extract each of the params till decl close
			char cSeparator;
			bool bIsFunc = extract_var(pLine, pSymSet, &pClass, &pFuncName, &pReturnType, &cSeparator);
			// func type and name detected
			if (bIsFunc) {
				pVarSet = new_var_set();
				STxtLine *pParamType = NULL;
				STxtLine *pParamName = NULL;
				// extracting params
				// we assume there are one or more params on each line
				// but they are not broken across lines
				while (bContinue) {
					// if w've parsed out this line, load the next line
					if (tl_isempty(pLine)) {
						tl_free(pLine);
						pLine = NULL;
						idx++;
						// load the next line
						if (idx - index < fnDefnLength) {
							pLine = tl_clone(pPage->ppLines[idx]);
							tl_trim(pLine);
						}
						// we've reached the last line but no decl close so exit
						else {
							wxLogMessage("error extracting fn defn");
							bContinue = false;
						}
					}
					// continue parsing out params from the line sequence if no errs in prev step
					if (bContinue) {
						STxtLine* pDummy = NULL;
						char cSeparator;
						bool bIsParam = extract_var(pLine, pSymSet, &pDummy, &pParamName, &pParamType, &cSeparator);
						if (bIsParam) {
							pVarSet->add_var(new_var(pParamName, pParamType, NULL));
							if (cSeparator != ')')
								bContinue = true;
							else
								bContinue = false;
						}
						// this is the no params case
						else {
							bContinue = false;
						}
						tl_free(pParamName);
						pParamName = NULL;
						tl_free(pParamType);
						pParamType = NULL;
						tl_free(pDummy);
						pDummy = NULL;
					}
				}
				// create symfunc and add to fnset depending on context
				// depending of element type detected, add the element to pThis
				// update the location of pFunc
				void* pSymLink = NULL;
				SSymFunc* pFunc = new_symfunc(pFuncName, pReturnType, pVarSet, NULL);
				
				if (bGlobalContext) {
					// if it's a class fn defn
					// find the class in the symset's class set and add it to that
					// else add it to the sysmset's func set
					int longestLength = 0;
					int longestIndex = -1;
					if (pClassName != NULL) {
						for (int i = 0; i < pSymSet->pClassSet->numClasses; i++) {
							if (tl_equals(pClassName, pSymSet->pClassSet->ppClasses[i]->pName)) {
								if (pSymSet->pClassSet->ppClasses[i]->pName->length > longestLength) {
									longestLength = pSymSet->pClassSet->ppClasses[i]->pName->length;
									longestIndex = i;
								}
							}
						}
						if (longestIndex != -1) {
							pSymSet->pClassSet->ppClasses[longestIndex]->add_func(pFunc);
						}
						else
							bRetVal = false;
					}
					else {
						pSymSet->pFuncSet->add_func(pFunc);
					}
				}
				else {
					if (pThis->pBaseElem->type == CDE_CLASSDECL) {
						SClass* pClass = pSymSet->pClassSet->ppClasses[pThis->symLinkIndex];
						pClass->add_func(pFunc);
					}
					else if (pThis->pBaseElem->type == CDE_TYPEDEF) {
						SStruct* pStruct = pSymSet->pStructSet->ppStructs[pThis->symLinkIndex];
						pStruct->add_func(pFunc);
					}
					else
						bRetVal = false;
				}
				// depending of element type detected, add the element to pThis
				if (bFnDecl)
					pThis->add_single(CDE_S_FWDDECL_FN, pPage->ppLines[index]);
				else {
				  int classIndex = -1;
				  if( pClassName != NULL ) {
					bool bFound = false;
					for( int i=0; i<pSymSet->pClassSet->numClasses && !bFound; i++ )
					  if( tl_equals(pClassName, pSymSet->pClassSet->ppClasses[i]->pName) ) {
						classIndex = i;
						pThis->add_section(CDE_CLASS_FNDEFN, index, *pnLength, pPage, bInlineBrace, 2, i);
						bFound = true;
					  }
					wxASSERT( bFound );
				  }
				  else
					pThis->add_section(CDE_FNDEFN, index, *pnLength, pPage, bInlineBrace, 0, -1);
				  // add the vars in the func's paramaters varset to the newly added section's varset
				  for (int i = 0; i < pFunc->pVarSet->numVars; i++)
					pThis->ppElements[pThis->numElements - 1]->pSec->pVarSet->add_var(var_clone(pFunc->pVarSet->ppVars[i]));  
				}
				int fileOffset = ce_file_offset(pThis->ppElements[pThis->numElements - 1]);
				SLocation* pLocation = new_location(pThis->ppElements[pThis->numElements - 1], fileOffset);
				// update the location of pFunc
				pFunc->pLocation = pLocation;
				bRetVal = true;
			}
			else {
				bRetVal = false;
			}
			tl_free(pClass);
			pClass = NULL;  
			tl_free(pFuncName);
			pFuncName = NULL;  
			tl_free(pReturnType);
			pReturnType = NULL;  
			tl_free(pLine);
			pLine = NULL;  
		}
		else {
			if( !bVarDecl )
				bRetVal = false;
		}
	}
	else 
		bRetVal = false;
	tl_free(pClassName);
  pClassName = NULL;  
	tl_free(pVarName);
  pVarName = NULL;  
	tl_free(pVarType);
  pVarType = NULL;  

	tl_free(pLineT);
  pLineT = NULL;  
	return( bRetVal );
}
// parses a typedef struct or a class decleration
// looks for comments. blanks, function defns and var defns and #ifdefs
// loads the SCodeSection's pStruct structure with
// the struct's (or class decl's) fn defs (SSymFunc) and vars (SVar)
// this info is used by the code editor 
bool parse_struct( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet, bool bInlineBrace, int symLinkType, int symLinkIndex ) {
  bool bRetVal = true;
  int idx = index;
  STxtLine *pLine;

  // first create and set the summary line for this section
  STxtLine *pLineSum = tl_clone( pPage->ppLines[idx] );
  // append close brace only
  if( bInlineBrace ) {
	char szAppend[5] = {'.','.','.',125,0};
	tl_insert( pLineSum, szAppend, pLineSum->length );
  }
  // append open brace and close brace
  else {
	char szAppend[7] = {' ',123,'.','.','.',125,0};
	tl_insert( pLineSum, szAppend, pLineSum->length );
  }
  pThis->pBaseElem->pLine = pLineSum;

  // load the first line and start from the second line
  pThis->add_single( CDE_S_CODELINE, pPage->ppLines[idx] );
  idx++;
  int temp = pThis->get_length();

  // check for comment lines
  // blank lines
  // fn definitions
  // var defns
  // enum
  while( idx-index < length-1 ) {
    int secLength = 0;
    int lenT = 0;
    pLine = tl_clone( pPage->ppLines[idx] );
    tl_trim( pLine );
    if( find_comment( pThis, pPage, idx, &secLength, pSymSet ) ) {
      idx += secLength;
      lenT = pThis->get_length();
      if( lenT != idx-index ) {
      wxString msg;
      msg.Printf("parse error at %d", idx-secLength );
      wxLogMessage( msg );
      }
    }
    else if( tl_isempty( pLine ) ) {
      pThis->add_single( CDE_S_COMMENT, pPage->ppLines[idx] );
      idx++;
    }
    else if( find_var_decl( pPage, idx, pThis, &secLength, pSymSet ) ) {
      idx += secLength;
      lenT = pThis->get_length();
      if( lenT != idx-index ) {
      wxString msg;
      msg.Printf("parse error at %d", idx-secLength );
      wxLogMessage( msg );
      }
    }
    else if( find_enum( pThis, pPage, idx, &secLength, pSymSet ) ) {
      idx += secLength;
      lenT = pThis->get_length();
      if( lenT != idx-index ) {
      wxString msg;
      msg.Printf("parse error at %d", idx-secLength );
      wxLogMessage( msg );
      }
    }
    else if( find_ifdef( pThis, pPage, idx, &secLength ) ) {
      idx += secLength;
      lenT = pThis->get_length();
      if( lenT != idx-index ) {
      wxString msg;
      msg.Printf("parse error at %d", idx-secLength );
      wxLogMessage( msg );
      }
    }
    else {
      pThis->add_single(CDE_S_CODELINE, pPage->ppLines[idx]);
      idx++;
    }
    tl_free( pLine );
    pLine = NULL;  
  }
  // add the last line
  pThis->add_single(CDE_S_CODELINE, pPage->ppLines[index + length - 1]);
  return( bRetVal );
};
// parses the given page using C++ language conventions
// the parse starts at index in the page and continues for length lines from that index
// the provided symbol set is used in the parse
// the approach taken is to detect the type of line by its startstring
// or by detecting a variable declaration.
// this function is used to parse a sub-block
bool parse_global_scope( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet ) {
  bool bContinue = true;
  int idx = index;
  int numLines = 0;

	// first create and set the summary line for this section
  STxtLine *pLineSum = tl_clone( pPage->ppLines[idx] );
  char szAppend[7] = {' ',123,'.','.','.',125,0};
  tl_insert( pLineSum, szAppend, pLineSum->length );
  // remove SUBBLOCK:, add a CDE_S_SUBBLOCKSTART to pThis
  if( pThis->pBaseElem->type == CDE_SUBBLOCK ) {
	tl_remove( pLineSum, (char*)"// SUBBLOCK: " );
	// load the demarcator and start from the second line
	pThis->add_single( CDE_S_SUBBLOCKSTART, pPage->ppLines[idx] );
  }
  else
	pThis->add_single( CDE_S_CODELINE, pPage->ppLines[idx] );
  pThis->pBaseElem->pLine = pLineSum;

  idx++;

  // find a startstring
  // else find a var decl
  // else add a codeline
  while( idx-index<length && bContinue ) {
	int len = pThis->get_length();
	if( find_startstring_parse( pPage, idx, pThis, &numLines, pSymSet ) ) {
	  idx += numLines;
	  // signal error if accum length of this section is not idx-index
	  len = pThis->get_length();
	  if( len != (idx-index) ) {
		wxString msg;
		msg.Printf("parse error at %d", idx);
		wxLogMessage(msg);
	  }
	}
	else if( find_var_decl( pPage, idx, pThis, &numLines, pSymSet ) ) {
	  idx += numLines;
	  // signal error if accum length of this section is not idx-index
	  if( pThis->get_length() != (idx-index) ) {
		wxString msg;
		msg.Printf("parse error at %d", idx);
		wxLogMessage(msg);
	  }
	}
	// add a codeline
	else {
	  pThis->add_single( CDE_S_CODELINE, pPage->ppLines[idx] );
	  idx++;
	}
  }
  return( bContinue );
};
// parses a "block" in a codebase
// a block is a sectional structure defined above the cpp language level
// it may contain sub-blocks
bool parse_block( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet ) {
  int idx = index;
  int numLines = 0;
  bool bParsed = true;
  bool bEOSection = false;
  bool bExit = false;
  int blockLength = 0;

  // create and set the summary line for this section
  STxtLine *pLineSum = tl_clone( pPage->ppLines[idx] );
  char szAppend[7] = {' ',123,'.','.','.',125,0};
  tl_insert( pLineSum, szAppend, pLineSum->length );
  tl_remove( pLineSum, (char*)"// BLOCK: " );
  pThis->pBaseElem->pLine = pLineSum;
  pThis->add_single( CDE_S_BLOCKSTART, pPage->ppLines[idx] );
  idx++;
  int subBlockIndex = 0;
  // look for sub-block demarcators and parse a subblock
  // otherwise startstring
  // otherwise var-decl
  // otherwise fail and exit
  while( idx-index < length && !bExit) {
	// look for a sub-block demarcator
	// look for the next sub-block demarcator or end of section
	// add a CDE_SUBBLOCK to pThis for the lines in between
	if( tl_find( pPage->ppLines[idx], (char*)"// SUBBLOCK:" ) ==  0 ) {
	  subBlockIndex = 1;
	  bool bFound = false;
	  // look for the next sub-block demarcator or end of section
	  // add a CDE_SUBBLOCK to pThis for the lines in between
	  while( !bFound && idx-index+subBlockIndex<length ) {
		// add a CDE_SUBBLOCK to pThis for the lines in between 2 subblock demarcs
		if( tl_find( pPage->ppLines[idx+subBlockIndex], (char*)"// SUBBLOCK:" ) ==  0 ) {
		  bFound = true;
		  pThis->add_section( CDE_SUBBLOCK, idx, subBlockIndex, pPage, true, 0, -1 );
		  idx += subBlockIndex;
		  blockLength = pThis->get_length();
		}
		else {
		  subBlockIndex++;
		  // test for EOSection
		  // add a CDE_SUBBLOCK to pThis for the lines in between the last subblock and EOS
		  if( idx-index + subBlockIndex == length ) {
			bFound = true;
			pThis->add_section( CDE_SUBBLOCK, idx, subBlockIndex, pPage, true, 0, -1 );
			bEOSection = true;
			idx += subBlockIndex;
			blockLength = pThis->get_length();
		  }
		}
	  }
	}
	else if (find_startstring_parse(pPage, idx, pThis, &numLines, pSymSet)) {
		idx += numLines;
		blockLength = pThis->get_length();
	}

	else if (find_var_decl(pPage, idx, pThis, &numLines, pSymSet)) {
		idx += numLines;
		blockLength = pThis->get_length();
	}
	// fail and exit
	else {
	  bParsed = false;
	  bExit = true;
	}
  }
  // summarize the block if no sub-blocks were found
  if( subBlockIndex == 0 )
	pThis->bSummarized = true;

  blockLength = pThis->get_length();
  return( bParsed );
}
// parse out only comments in this section
// in other words it detects comments or codelines but no structure beyond that
// it used to parse enum, L4 sections etc
bool parse_not( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet, bool bInlineBrace ) {
  bool bFound = true;
  int idx = index;
  int secLength = 0;

  // create and set the summary line for this section
  STxtLine *pLineSum = tl_clone( pPage->ppLines[idx] );

  // append a close brace to the summaru line
  if( bInlineBrace ) {
	char szAppend[5] = {'.','.','.',125,0};
	tl_insert( pLineSum, szAppend, pLineSum->length );
  }
  // append an open and close brace to the summary line
  else {
	char szAppend[7] = {' ',123,'.','.','.',125,0};
	tl_insert( pLineSum, szAppend, pLineSum->length );
  }
  pThis->pBaseElem->pLine = pLineSum;
  // add a comment section or a codeline
  while( idx < index+length ) {
	if( find_comment( pThis, pPage, idx, &secLength, pSymSet ) )
	  idx += secLength;
	// add a CDE_S_CODELINE
	else {
	  pThis->add_single( CDE_S_CODELINE, pPage->ppLines[idx] );
	  idx++;
	}
  }
  return( bFound );
}
// parses the preamble to a codebase which is a block of comments
// the preamble is a comment block that starts with a specific demarcator
bool parse_preamble( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet ) {
  bool bRetVal = true;
  int idx = index;
  bool bExit = false;
  int secLength = 0;

  // first create and set the summary line for this section
  STxtLine *pLineSum = tl_clone( pPage->ppLines[index] );
  char szAppend[7] = {' ',123,'.','.','.',125,0};
  tl_insert( pLineSum, szAppend, pLineSum->length );
  pThis->pBaseElem->pLine = pLineSum;

  // look for a comment section
  // or a blank line
  // else fail and exit
  while( idx - index < length && !bExit ) {
	int len = pThis->get_length();
	if( find_comment( pThis, pPage, idx, &secLength, pSymSet ) ) {
	  idx += secLength;
	  len = pThis->get_length();
	  if( len != idx-index ) {
		wxString msg;
		msg.Printf( "parse error at %d", idx );
		wxLogMessage( msg );
	  }
	}
	else if( tl_isempty( pPage->ppLines[idx] ) )
	  pThis->add_single( CDE_S_BLANK, pPage->ppLines[idx++] );
	else
	  bExit = true;
  }
  return( !bExit );
}
// parses a Modal codebase
// this involves parsing its preamble and all its blocks
bool parse_codebase( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet ) {
  bool bParsed = true;
  STxtLine *pLine = pPage->ppLines[index];

  // find a preamble demarcator
  // then look for block demarctors
  // add a preamble when the first block demarcator is found
  // add blocks for subsequent block demarcators
  // add a last block between the last block demarcator and EOF
  if( tl_find( pLine, (char*)"// PREAMBLE:" ) ==  0 ) {
	int offsetPrev = 0;
	int offset = 1;
	bool bEOPage = false;
	int numBlocks = 0;
	// look for the block demarcators in the rest of the file
	// add the preamble at first block demarcator
	// a block between 2 block demarctors
	// last block is demarcated by EOF
	while( !bEOPage ) {
	  pLine = pPage->ppLines[index+offset];
	  // look for a block demarcator and add a block or preamble
	  // depending on whether it is the first block demarc or not
	  if( tl_find( pLine, (char*)"// BLOCK:" ) ==  0 ) {
		// if this it first block demarc
		// it's the end of the preamble add a CDE_PREAMBLE to pThis
		if( numBlocks == 0 ) {
		  bParsed = pThis->add_section( CDE_PREAMBLE, index+offsetPrev, offset-offsetPrev, pPage, true, 0, -1 );
		  if( !bParsed )
			wxLogMessage("error parsing the preamble");
		}
		// add a CDE_BLOCK to pThis
		else {
		  bParsed = pThis->add_section( CDE_BLOCK, index+offsetPrev, offset-offsetPrev, pPage, true, 0, -1 );
		  if( !bParsed ) {
			wxString strMsg;
			strMsg.Printf( "error parsing block %d", numBlocks );
			wxLogMessage( strMsg );
		  }
		}
		offsetPrev = offset;
		numBlocks++;
	  }
	  offset++;
	  // test for EOF, add the last block
	  if( offset == length ) {
		bParsed = pThis->add_section( CDE_BLOCK, index+offsetPrev, offset-offsetPrev, pPage, true, 0, -1 );
		if( !bParsed )
		  wxLogMessage("error parsing the last block");
		if (numBlocks != MODAL_NUMBLOCKS) {
		  wxLogMessage("detected an invalid number of blocks in your modal codefile");
		  bParsed = false;
		}
		bEOPage = true;
	  }
	}
  }
  // no preamble implies parse a non-modal codefile
  else
	bParsed = parse_nm_codebase( pThis, pPage, index, length, pSymSet );

  return( bParsed );
};
// parse a non Modal codefile
bool parse_nm_codebase( SCodeSection *pThis, STxtPage *pPage, int index, int length, SSymbolSet *pSymSet ) {
  bool bContinue = true;
  int idx = index;
  int numLines;

  // find a startstring
  // else find a var decl
  // else fail and exit
  while( idx-index<length && bContinue ) {
	if( find_startstring_parse( pPage, idx, pThis, &numLines, pSymSet ) ) {
	  idx += numLines;
	  // signal error if accum length of this section is not idx-index
	  if( pThis->get_length() != (idx-index) ) {
		int len = pThis->get_length();
		wxString msg;
		msg.Printf("parse error at %d", idx);
		wxLogMessage(msg);
	  }
	}
	else if( find_var_decl( pPage, idx, pThis, &numLines, pSymSet ) ) {
	  idx += numLines;
	  // signal error if accum length of this section is not idx-index
	  if( pThis->get_length() != (idx-index) ) {
		int len = pThis->get_length();
		wxString msg;
		msg.Printf("parse error at %d", idx);
		wxLogMessage(msg);
	  }
	}
	// fail and exit if neither worked
	else {
	  wxString msg;
	  msg.Printf("parse failure in parse_nm_codebase %d", idx );
	  wxLogMessage( msg );
	  bContinue = false;
	}
  }
  return( bContinue );
};
// SUBBLOCK: EDIT OPERATIONS ON A CODEBASE
// This sub-block defines structs and fns for the edit operations possible on a codebase
// It has been created to enable undo functionality

// the types of edit operations
enum {
  OP_EDITCHAR,
  OP_CUTSEL,
  OP_PASTESEL,
  OP_SUMMARIZE
};
// the edit character operation
typedef struct SOpEditChar {
  char cChar;
  int index;
  bool bInsDel;
  void serialize( wxFile &File, bool bToFrom ) {
	if( bToFrom ) {
	  File.Write( &(this->cChar), sizeof(char) );
	  File.Write( &(this->index), sizeof(int) );
	  File.Write( &(this->bInsDel), sizeof(bool) );
	}
	else {
	  File.Read( &(this->cChar), sizeof(char) );
	  File.Read( &(this->index), sizeof(int) );
	  File.Read( &(this->bInsDel), sizeof(bool) );
	}
  };
} SOpEditChar;
// the cut or paste selection operation
typedef struct SOpCutPasteSel {
  STxtPage CutPage;
  bool bCutPaste;
  void serialize( wxFile &File, bool bToFrom ) {
	if( bToFrom ) {
	  CutPage.serialize( File, true );
	  File.Write( &(this->bCutPaste), sizeof(bool) );
	}
	else {
	  CutPage.serialize( File, false );
	  File.Read( &(this->bCutPaste), sizeof(bool) );
	}
  };
} SCutSel;
// the summarize or unsummarize operation
typedef struct SOpSummarize {
  bool bSumUn;
  void serialize( wxFile &File, bool bToFrom ) {
	if( bToFrom ) {
	  File.Write( &(this->bSumUn), sizeof(bool) );
	}
	else {
	  File.Read( &(this->bSumUn), sizeof(bool) );
	}
  };
} SSummarize;
// union to hold the different types of edit operations
// it is contained by the edit operation struct as a way to extend the base representation
typedef union SOpExt {
  SOpEditChar EditChar;
  SOpCutPasteSel CutPasteSel;
  SOpSummarize Summarize;
} SOpExt;
// the edit operation
typedef struct SOperation {
  int type;
  SOpExt OpExt;
  int fileOffset;
  int caretY;
  int selStart;
  int selEnd;
  void serialize( wxFile &File, bool bToFrom ) {
	if( bToFrom ) {
	  File.Write( &(this->type), sizeof(int) );
	  File.Write( &(this->fileOffset), sizeof(int) );
	  File.Write( &(this->caretY), sizeof(int) );
	  File.Write( &(this->selStart), sizeof(int) );
	  File.Write( &(this->selEnd), sizeof(int) );
	  switch( this->type ) {
		case OP_EDITCHAR:
		  this->OpExt.EditChar.serialize( File, true );
		break;
		case OP_CUTSEL:
		case OP_PASTESEL:
		  this->OpExt.CutPasteSel.serialize( File, true );
		break;
		case OP_SUMMARIZE:
		  this->OpExt.Summarize.serialize( File, true );
		break;
		default:
		break;
	  }
	}
	else {
	  File.Read( &(this->type), sizeof(int) );
	  File.Read( &(this->fileOffset), sizeof(int) );
	  File.Read( &(this->caretY), sizeof(int) );
	  File.Read( &(this->selStart), sizeof(int) );
	  File.Read( &(this->selEnd), sizeof(int) );
	  switch( this->type ) {
		case OP_EDITCHAR:
		  this->OpExt.EditChar.serialize( File, false );
		break;
		case OP_CUTSEL:
		case OP_PASTESEL:
		  this->OpExt.CutPasteSel.serialize( File, false );
		break;
		case OP_SUMMARIZE:
		  this->OpExt.Summarize.serialize( File, false );
		break;
		default:
		break;
	  }

	}
  }
} SOperation;
// initializes an edit char operation
void op_edit_char_init( SOperation Op, int fileOffset, int caretY, char cChar, int key, int index, bool bInsDel ) {
  Op.type = OP_EDITCHAR;
  Op.fileOffset = fileOffset;
  Op.caretY = caretY;
  Op.selStart = -1;
  Op.selEnd = -1;
  Op.OpExt.EditChar.cChar = cChar;
  Op.OpExt.EditChar.index = index;
  Op.OpExt.EditChar.bInsDel = bInsDel;
}
// clones an edit char operation on the stack
SOpEditChar op_edit_char_clone( SOpEditChar OpEC ) {
  SOpEditChar RetVal;
  RetVal.cChar = OpEC.cChar;
  RetVal.index = OpEC.index;
  RetVal.bInsDel = OpEC.bInsDel;
  return( RetVal );
}
// initializes a cut or paste selection operation
void op_cutpaste_sel_init( SOperation Op, int fileOffset, int caretY, int selStart, int selEnd, STxtPage CutPage, bool bCutPaste ) {
  Op.type = (bCutPaste? OP_CUTSEL : OP_PASTESEL);
  Op.fileOffset = fileOffset;
  Op.caretY = caretY;
  Op.selStart = selStart;
  Op.selEnd = selEnd;
  Op.OpExt.CutPasteSel.CutPage = tp_clone( CutPage );
  Op.OpExt.CutPasteSel.bCutPaste = bCutPaste;
}
// clones a cut or paste selection op on the stack
SOpCutPasteSel op_cutpaste_sel_clone( SOpCutPasteSel OpCPS ) {
  SOpCutPasteSel RetVal;
  RetVal.CutPage = tp_clone( OpCPS.CutPage );
  RetVal.bCutPaste = OpCPS.bCutPaste;
  return( RetVal );
}
// init a summarize op
void op_summarize_init( SOperation Op, int fileOffset, int caretY, bool bSumUn ) {
  Op.type = OP_SUMMARIZE;
  Op.fileOffset = fileOffset;
  Op.caretY = caretY;
  Op.selStart = -1;
  Op.selEnd = -1;
  Op.OpExt.Summarize.bSumUn = bSumUn;
}
// clones a summarize operation on the stack
SOpSummarize op_summarize_clone( SSummarize OpSum ) {
  SOpSummarize RetVal;
  RetVal.bSumUn = OpSum.bSumUn;
  return( RetVal );
}
// clones an operation of any type on the stack
SOperation op_clone( SOperation Op ) {
  SOperation RetVal;
  RetVal.type = Op.type;
  RetVal.fileOffset = Op.fileOffset;
  RetVal.caretY = Op.caretY;
  RetVal.selStart = Op.selStart;
  RetVal.selEnd = Op.selEnd;
  switch( Op.type ) {
	case OP_EDITCHAR: {
	  RetVal.OpExt.EditChar = op_edit_char_clone( Op.OpExt.EditChar );
	}
	break;
	case OP_CUTSEL:
	case OP_PASTESEL: {
	  RetVal.OpExt.CutPasteSel = op_cutpaste_sel_clone( Op.OpExt.CutPasteSel );
	}
	break;
	case OP_SUMMARIZE: {
	  RetVal.OpExt.Summarize = op_summarize_clone( Op.OpExt.Summarize );
	}
	break;
	default:
	  wxLogMessage("invalid op type in op_clone");
	break;
  }
  return( RetVal );
}
// an ordered list of operations that functions like a LIFO queue
typedef struct SOpList {
  // inits with max number of ops it can hold
  void init( int maxOps ) {
	this->maxOps = maxOps;
	this->numOps = 0;
	this->pOps = (SOperation *) malloc( maxOps * sizeof( SOperation ) );
  };
  // adds an op to the end of this oplist
  void add( SOperation Op ) {
	this->pOps[this->numOps] = op_clone( Op );
	this->numOps += 1;
	wxASSERT_MSG(this->numOps == this->maxOps, "op list overflow");
  };
  // replaces the op at the end of this oplist
  void replace( SOperation Op ) {
	this->pOps[this->numOps-1] = op_clone( Op );
  };
  // removes an op from the end of this oplist
  void remove() {
	this->numOps -= 1;
	wxASSERT_MSG(this->numOps < 0, "op list underflow");
  };
  // returns the op at the end of this oplist
  SOperation get_last() {
	wxASSERT_MSG(this->numOps == 0, "op list empty in get_last call");
	return( this->pOps[this->numOps-1] );
  }
  void serialize( wxFile &File, bool bToFrom ) {
	if( bToFrom ) {
	  File.Write( &(this->numOps), sizeof(int) );
	  for( int i=0; i<this->numOps; i++ )
		this->pOps[i].serialize( File, true );
	}
	else {
	  File.Read( &(this->numOps), sizeof(int) );
	  if (this->numOps >= this->maxOps) {
		  this->maxOps = this->numOps * 2;
		  this->pOps = (SOperation*)realloc(this->pOps, this->maxOps * sizeof(SOperation));
	  }
	  for( int i=0; i<this->numOps; i++ )
		this->pOps[i].serialize( File, false );
	}
  };
  SOperation *pOps;
  int numOps;
  int maxOps;
} SOpList;
// frees an oplist
void oplist_free( SOpList OpList ) {
  if( OpList.pOps != NULL )
	free( OpList.pOps );
}

// SUBBLOCK: CODE BASE
// This sub-block defines the codebase struct and fns
void serialize_map_file_offsets(SSymbolSet* pSymSet, SCodeBase* pCodeBase);
void serialize_set_sym_sets(SCodeSection * pSec, SSymbolSet * pSymSet);

// the codebase, contains a symbol set and an oplist for it's editing ops
// it is parsed from a file into a nested sequence of code sections
typedef struct SCodeBase {
  // inits the codebase
  void init( SCodeSection *pBaseSec ) {
	this->OpList.init( MAX_OPS_CACHED );
	this->pSymSet = new_symbol_set();
	this->pBaseSec = pBaseSec;
	this->pBaseSec->pCodeBase = this;
	this->pBaseSec->set_symbol_set(pSymSet);
  };
  bool load_codefile(wxString strFileName) {
	  bool bRetVal = false;
	  wxTextFile* pFile = new wxTextFile(strFileName);
	  if (pFile->Exists()) {
		  pFile->Open();
		  if (pFile->IsOpened()) {
			  STxtPage* pPage = new_txt_page(pFile->GetLineCount());
			  for (int i = 0; i < pFile->GetLineCount(); i++) 
				  pPage->add_line(new_txt_line_wx(pFile->GetLine(i)), i);
			  if (this->pBaseSec->parse(pPage, 0, pFile->GetLineCount(), true)) 
				  bRetVal = true;
			  else 
				  bRetVal = false;
			  free_txt_page( pPage );
		  }
		  pFile->Close();
	  }
	  else {
		  bRetVal = false;
	  }
	  delete(pFile);
	  return(bRetVal);
  };
  // performs the last edit operation in the oplist of this codebase
  bool do_edit() {
	bool bRetVal = true;
	SOperation Op = this->OpList.get_last();
	switch( Op.type ) {
	  case OP_EDITCHAR: {
		//  char c = pMode->uniKey;
		  char c = 0;
		  char cU = 0;
		  if( c != WXK_NONE ) {
			if( c >= 32  ) {
		//      cU = to_upper( c, pSrcEdr->bShiftDown );
		//      tl_insert_char( pCurLine, cU, pSrcEdr->Caret.x );
			}
		  }
	  }
	  break;
	  case OP_CUTSEL: {

	  }
	  break;
	  case OP_PASTESEL: {

	  }
	  break;
	  case OP_SUMMARIZE: {

	  }
	  break;
	}
	return( bRetVal );
  };
  // undoes the last edit operation in the oplist of this codebase
  // removes the undone operation from the oplist
  bool undo_edit() {
	bool bRetVal = true;
	return( bRetVal );
  }
  // compiles and links the codebase into an executable if possible
  bool build() {
	bool bRetVal = true;
	return( bRetVal );
  };
  // debugs the built executable
  // if it's not been built, calls build first
  bool debug() {
	bool bRetVal = true;
	return( bRetVal );
  };
  void serialize( wxFile &File, bool bToFrom ) {
    if( bToFrom ) {
      ce_serialize_base( this->pBaseSec->pBaseElem, File, true);
      this->pBaseSec->serialize( File, bToFrom );
      this->OpList.serialize( File, true );
      this->pSymSet->serialize( File, bToFrom );
    }
    else {
      this->OpList.serialize( File, false );
      pSymSet->serialize( File, false );
      serialize_map_file_offsets( this->pSymSet, this );
      serialize_set_sym_sets( this->pBaseSec, this->pSymSet );
    }
  };
  SOpList OpList;
  SSymbolSet *pSymSet;
  SCodeSection *pBaseSec; // pointer to base code section of which this codebase is a sub-struct
} SCodeBase;

// new a codebase ptr on the heap
// caller has to free
SCodeBase *new_codebase() {
  STxtLine* pLine = new_txt_line((char*)"The CodeBase");
  SCodeElement *pElem = new_code_element( CDE_CODEBASE, NULL, 0, pLine );
  tl_free(pLine);
  pLine = NULL;  
  pElem->pSec = new_code_section( pElem, NULL, 0, -1 );
  pElem->pSec->pCodeBase = (SCodeBase *) malloc( sizeof( SCodeBase ) );
  pElem->pSec->pCodeBase->init( pElem->pSec );
  return( pElem->pSec->pCodeBase );
}
// load a code element from a File
SCodeElement *load_code_element( wxFile &File, SCodeSection *pContainer, int indexContainer ) {
  SCodeElement *pElem = (SCodeElement *) malloc( sizeof(SCodeElement) );
  if (pElem != NULL) {
	  pElem->pContainer = pContainer;
	  pElem->indexContainer = indexContainer;
	  ce_serialize_base(pElem, File, false);
	  if (!pElem->bSingle) {
		  pElem->pSec = (SCodeSection*)malloc(sizeof(SCodeSection));
		  if (pElem->pSec != NULL) {
			  pElem->pSec->init(pElem, NULL, 0, -1);
			  pElem->pSec->serialize(File, false);
			  if (pElem->type == CDE_CODEBASE) {
				  pElem->pSec->pCodeBase = (SCodeBase*)malloc(sizeof(SCodeBase));
				  if (pElem->pSec->pCodeBase != NULL) {
					  pElem->pSec->pCodeBase->init(pElem->pSec);
					  pElem->pSec->pCodeBase->serialize(File, false);
				  }
			  }
		  }
	  }
  }
  return( pElem );
}

// free a codebase code ptr
void free_codebase( SCodeBase *pCodeBase ) {
	if (pCodeBase != NULL) {
		free_symbol_set(pCodeBase->pSymSet);
		pCodeBase->pSymSet = NULL;
		free(pCodeBase->OpList.pOps);
		pCodeBase->OpList.pOps = NULL;
		free_code_section(pCodeBase->pBaseSec);
		pCodeBase->pBaseSec = NULL;
		free(pCodeBase);
	}
}
// ancillary used by serialize to store pointers as fileOffsets in the codebase
SCodeElement *elem_from_file_offset( int fileOffset, SCodeBase *pCodeBase ) {
  SCodeElement *pElem;
  SCodeSection *pSec = pCodeBase->pBaseSec;
  int offset = -1;
  while( offset != 0 ) {
	pElem = pSec->get_element_at( fileOffset, 0, &offset );
	if( offset != 0 ) {
	  pSec = pElem->pSec;
	  fileOffset = offset;
	}
  }
  return( pElem );
}

// the symbols in a codebase are pointers to elements in the codebase
// since a pointer cant be serialized
// we serialize the file offset location of the pointed to element instead.
// when we load a serialized symbol with a file offset location,
// this location has to be converted back to a pointer to an element in the codebase.
// this functions does just that for the entire symbol set
// it takes the unconverted symbol set and the codebase
// in which to find the pointed to elements as inputs
void serialize_map_file_offsets(SSymbolSet* pSymSet, SCodeBase* pCodeBase) {
	int fileOffset = 0;
	int dummy = 0;
	// map the file offsets for the class set
	for (int i = 0; i < pSymSet->pClassSet->numClasses; i++) {
		SClass* pClass = pSymSet->pClassSet->ppClasses[i];
		fileOffset = pClass->pLocation->fileOffset;
		if (fileOffset != -1)
			pClass->pLocation->pCodeBaseLoc = pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &dummy);
		if (pClass->pConstr != NULL) {
			fileOffset = pClass->pConstr->pLocation->fileOffset;
			if (fileOffset != -1)
				pClass->pConstr->pLocation->pCodeBaseLoc = pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &dummy);
		}
		if (pClass->pDestr != NULL) {
			fileOffset = pClass->pDestr->pLocation->fileOffset;
			if (fileOffset != -1)
				pClass->pDestr->pLocation->pCodeBaseLoc = pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &dummy);
		}
		for (int j = 0; j < pClass->pFuncSet->numFuncs; j++) {
			SSymFunc* pFunc = pClass->pFuncSet->ppFuncs[j];
			fileOffset = pFunc->pLocation->fileOffset;
			if( fileOffset != -1 )
				pFunc->pLocation->pCodeBaseLoc = pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &dummy);
		}
		for (int j = 0; j < pClass->pVarSet->numVars; j++) {
			SVar* pVar = pClass->pVarSet->ppVars[j];
			fileOffset = pVar->pLocation->fileOffset;
			if( fileOffset != -1 )
				pVar->pLocation->pCodeBaseLoc = pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &dummy);
		}
	}
	// map the file offsets for the struct set
	for (int i = 0; i < pSymSet->pStructSet->numStructs; i++) {
		SStruct* pStruct = pSymSet->pStructSet->ppStructs[i];
		fileOffset = pStruct->pLocation->fileOffset;
		if (fileOffset != -1)
			pStruct->pLocation->pCodeBaseLoc = pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &dummy);
		for (int j = 0; j < pStruct->pFuncSet->numFuncs; j++) {
			SSymFunc* pFunc = pStruct->pFuncSet->ppFuncs[j];
			fileOffset = pFunc->pLocation->fileOffset;
			if( fileOffset != -1 )
				pFunc->pLocation->pCodeBaseLoc = pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &dummy);
		}
		for (int j = 0; j < pStruct->pVarSet->numVars; j++) {
			SVar* pVar = pStruct->pVarSet->ppVars[j];
			fileOffset = pVar->pLocation->fileOffset;
			if( fileOffset != -1 )
				pVar->pLocation->pCodeBaseLoc = pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &dummy);
		}
	}
	// map the file offsets for the func set
	for (int i = 0; i < pSymSet->pFuncSet->numFuncs; i++) {
		SSymFunc* pFunc = pSymSet->pFuncSet->ppFuncs[i];
		fileOffset = pFunc->pLocation->fileOffset;
		if (fileOffset != -1)
			pFunc->pLocation->pCodeBaseLoc = pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &dummy);
		for (int j = 0; j < pFunc->pVarSet->numVars; j++) {
			SVar* pVar = pFunc->pVarSet->ppVars[j];
			fileOffset = pVar->pLocation->fileOffset;
			if( fileOffset != -1 )
				pVar->pLocation->pCodeBaseLoc = pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &dummy);
		}
	}
	return;
}

// see serialize_map_file_offsets for why this is needed
// sets the symbol sets for all the SCodeSection elements 
// in a codebase that has been loaded from a serialized file.
// these are all set to NULL in the loading from file process
// since their pointer locations are unknown at that point.
// serialize_map_file_offsets sets these ptr locations
// from the file offsets that are stored with each symbol.
// this function is therefore always called after serialize_map_file_offsets. 
void serialize_set_sym_sets(SCodeSection* pSec, SSymbolSet* pSymSet) {
	pSec->set_symbol_set(pSymSet);
	for (int i = 0; i < pSec->numElements; i++) 
		if (!pSec->ppElements[i]->bSingle)
			serialize_set_sym_sets(pSec->ppElements[i]->pSec, pSymSet );
}

// BLOCK: POP-UP MODES DEFINED BY THE TOOLKIT
// The definitions of the "modes of operation" of the UI
// provided by the Modal toolkit.
// A mode of operation of the UI
// is a defined behavior of the UI
// associated with the type of data being interacted with
// or the mode' function.
// This behavior is defined in terms of
// expected (kybd) inputs from the user
// and how the mode updates the displayed data in response.
// The expected user inputs are defined as a set of "intents".
// An intent is the user's intention to perform some operation on the UI
// such as moving the location of a caret or making a selection.
// An intent is like a mode of operation at an instant of time.
// SUBBLOCK: FWD_DECLS
// fwd decls needed for the mode def fns
void mode_on_load( SMode *pMode );
void mode_on_unload( SMode *pMode );
bool mode_key_up( SMode * pMode, wxKeyEvent &Event, ModalWindow *pWin );

bool msg_map( SMode * pMode, wxKeyEvent &Event, ModalWindow *pWin );
void msg_disp_state( SMode * pMode, ModalWindow *pWin, wxDC& DC );
void msg_disp_update( SMode *pBase, int phase, ModalWindow *pWin, wxDC& DC );

bool line_input_map( SMode * pMode, wxKeyEvent &Event, ModalWindow *pWin );
bool line_input_key_up( SMode * pMode, wxKeyEvent &Event, ModalWindow *pWin );
void line_input_disp_state( SMode * pMode, ModalWindow *pWin, wxDC& DC );
void line_input_disp_update( SMode *pBase, int phase, ModalWindow *pWin, wxDC& DC );

bool lev_adj_map( SMode * pMode, wxKeyEvent &event, ModalWindow *pWin );
void lev_adj_disp_state( SMode * pMode, ModalWindow *pWin, wxDC& DC );
void lev_adj_adjust( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC );

bool file_sel_map( SMode * pMode, wxKeyEvent &Event, ModalWindow *pWin );
void file_sel_disp_state( SMode *pMode, ModalWindow *pWin, wxDC& DC );
void file_sel_change_sel( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void file_sel_commit( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );

bool int_disp_map( SMode * pMode, wxKeyEvent &Event, ModalWindow *pWin );
void int_disp_disp_state( SMode *pMode, ModalWindow *pWin, wxDC& DC );
void int_disp_change_sel( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void int_disp_execute( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );

bool src_edr_map( SMode * pMode, wxKeyEvent &Event, ModalWindow *pWin );
bool src_edr_key_up( SMode * pMode, wxKeyEvent &Event, ModalWindow *pWin );
void src_edr_disp_state( SMode *pMode, ModalWindow *pWin, wxDC& DC );
void src_edr_disp_update( SMode *pMode, int phase, ModalWindow *pWin, wxDC& DC );
void src_edr_on_load( SMode *pMode, SModeManager *pManager);
void src_edr_on_unload( SMode *pMode, SModeManager *pManager);
bool src_edr_serialize( SMode *pBase, wxFile &File, bool bToFrom );
void src_edr_edit_char( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_update_caret( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_start_sel( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_update_sel( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_un_sel( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_cut_sel( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_paste_sel( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_undo( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_summarize( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_goto( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_control( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_scroll( SMode *pMode, bool bUp, ModalWindow *pWin );
void src_edr_export( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_load_new( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_build( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_debug( SMode *pMode, int phase, ModalWindow *pWin, wxDC &DC );
void src_edr_adjust_fontsize(SMode* pMode, int phase, ModalWindow* pWin, wxDC& DC);
void src_edr_input_codefile(SMode* pMode, int phase, ModalWindow* pWin, wxDC& DC);

// SUBBLOCK: SOME BASE MODE FUNCTIONS
// this sub-clock contains some of the base functions for SMode

// base fn called when a mode is loaded into the mode manager
void mode_on_load( SMode *pThis, SModeManager* pManager) {

};
// base fn called when a mode is unloaded from the mode manager
void mode_on_unload( SMode *pThis, SModeManager* pManager) {

};
// base mode keyup, sets shiftdown and ctrldown to false
bool mode_key_up( SMode * pMode, wxKeyEvent &event, ModalWindow *pWin ) {
  if( event.GetKeyCode() == WXK_SHIFT )
	pMode->bShiftDown = false;
  else if( event.GetKeyCode() == WXK_CONTROL )
	pMode->bCtrlDown = false;
  pWin->m_bUsrActn = false;
  return( true );
}
// SUBBLOCK: MESSAGE
// This sub-block contains the definitions for mode message
// This is a pop-up mode that displays a message

// a "pop-up" message mode
typedef struct SModeMsg {
  void init( SMode *pBase ) {
	pBase->fnDisp_state = msg_disp_state;
	pBase->fnKybd_map = msg_map;
	pBase->type = MODE_MESSAGE;
	pBase->bReset = true;
	this->pBase = pBase;
	this->load_intents( pBase );
	this->szMsg = NULL;
  };
  void set_msg( char *szMsg ) {
	this->szMsg = szMsg;
  };
  // loads the intent dispatch fns for this mode
  void load_intents( SMode *pBase ) {
	pBase->numIntents = 1;
	pBase->fnIntent_handler[0] = msg_disp_update;
  };
  SMode* pBase;
  char *szMsg;
} SModeMsg;
// allocs and inits a msg mode ptr on the heap and returns
// developer must set_msg before using
// caller has to free
SMode * new_msg( int scrnW, int scrnH, wxFont *pFont ) {
  SMode *pMode = (SMode *) malloc( sizeof( SMode) );
  if (pMode != NULL) {
	  pMode->init(scrnW, scrnH, pFont);
	  SModeMsg* pMsg = (SModeMsg*)malloc(sizeof(SModeMsg));
	  if (pMsg != NULL) {
		  pMode->sExt.pMsg = pMsg;
		  pMode->sExt.pMsg->init(pMode);
	  }
  }
  return( pMode );
}
void free_msg(SMode* pMode) {
	if (pMode != NULL) {
		if (pMode->sExt.pMsg != NULL)
			free(pMode->sExt.pMsg);
		free(pMode);
	}
}

// kybd map for mode msg
bool msg_map( SMode *pBase, wxKeyEvent &event, ModalWindow *pWin ) {
  SModeMsg *pMsg = pBase->sExt.pMsg;
  bool bRetVal = true;
  // only exits on escape
  if( pMsg->szMsg != NULL )
  {
	if( event.GetKeyCode() == WXK_ESCAPE ) {
	  pWin->m_pModeManager->pop();
	  pWin->Refresh( true );
	}
  }
  pWin->m_bUsrActn = false;
  return( bRetVal );
} // line_input_map
// display current state for mode msg
void msg_disp_state( SMode *pBase, ModalWindow *pWin, wxDC &DC ) {
  SModeMsg *pMsg = pBase->sExt.pMsg;
  // determines size of the message framing rect
  // determines size and location of message text
  // draws both
  if( pMsg->szMsg != NULL )
  {
	wxRect rectFrame;
	wxRect rectMsg;
	int xMsg;
	int yMsg;
	int widthMsg;
	int heightMsg;

	// determine the size of the message framing rect
	wxString strFrame( pMsg->szMsg );
	DC.GetTextExtent( strFrame, &(rectFrame.width), &(rectFrame.height) );
	rectFrame.height *= 5;
	rectFrame.width += 60;
	rectFrame.x = pBase->scrnW/2  - rectFrame.width/2;
	rectFrame.y = pBase->scrnH/2  - rectFrame.height/2;

	// determine the location of the text to be displayed
	wxString dispText( pMsg->szMsg );
	DC.GetTextExtent( dispText, &(widthMsg), &(heightMsg) );
	xMsg = pBase->scrnW/2 - widthMsg/2;
	yMsg = pBase->scrnH/2 - heightMsg/2;

	wxPen Pen = DC.GetPen();
	wxBrush Brush = DC.GetBrush();
	DC.SetPen( *wxTRANSPARENT_PEN );
	DC.SetBrush( wxBrush( wxColour( 208,208,200 ) ) );
	DC.DrawRectangle( rectFrame );
	DC.SetPen( Pen );
	DC.SetBrush( Brush );
	DC.DrawText( dispText, xMsg, yMsg );
  }
}
// display update for mode msg, never get's called
void msg_disp_update( SMode * pBase, int phase, ModalWindow *pWin, wxDC& DC ) {
  return;
}
// SUBBLOCK: LINE INPUT
// This sub-block contains the definitions for mode line input

// a "pop-up" line input mode
typedef struct SModeLineInp {
  void init( SMode *pBase ) {
	pBase->fnDisp_state = line_input_disp_state;
	pBase->fnKybd_map = line_input_map;
	pBase->type = MODE_LINE_INPUT;
	pBase->bReset = true;
	this->pBase = pBase;
	this->load_intents( pBase );
	this->pCaller = NULL;
	this->callerIntent = -1;
	this->pInput = new_txt_line( NULL );
	this->pMsg = NULL;
	this->bReset = true;
	this->indexCaret = 0;
	this->bInputRcvd = false;
  };
  void set_caller( SMode *pCaller, int callerIntent, char *szMsg ) {
	this->pCaller = pCaller;
	this->callerIntent = callerIntent;
	if( this->pMsg != NULL )
	  tl_free( this->pMsg );
	this->pMsg = new_txt_line( szMsg );
	this->bInputRcvd = false;
  }
  // loads the intent dispatch fns for this mode
  void load_intents( SMode *pBase ) {
	pBase->numIntents = 1;
	pBase->fnIntent_handler[0] = line_input_disp_update;
  };
  SMode* pBase;
  SMode *pCaller;
  int callerIntent;
  STxtLine *pMsg;
  STxtLine *pInput;
  bool bReset;
  int indexCaret;
  bool bInputRcvd;
} SModeLineInp;
// allocs and inits a line input mode ptr on the heap and returns
// the message and destination are set to NULL
// developer must set_msg_dest before using
// caller has to free
SMode * new_line_input( int scrnW, int scrnH, wxFont *pFont ) {
  SMode *pMode = (SMode *) malloc( sizeof( SMode) );
  if (pMode != NULL) {
	  pMode->init(scrnW, scrnH, pFont);
	  SModeLineInp* pLineInput = (SModeLineInp*)malloc(sizeof(SModeLineInp));
	  if (pLineInput != NULL) {
		  pMode->sExt.pLineInput = pLineInput;
		  pMode->sExt.pLineInput->init(pMode);
	  }
  }
  return( pMode );
}
void free_line_input( SMode *pMode ) {
	if (pMode != NULL) {
		if (pMode->sExt.pLineInput != NULL) {
			tl_free(pMode->sExt.pLineInput->pInput);
			pMode->sExt.pLineInput->pInput = NULL;
			tl_free(pMode->sExt.pLineInput->pMsg);
			pMode->sExt.pLineInput->pMsg = NULL;
			free(pMode->sExt.pLineInput);
			pMode->sExt.pLineInput = NULL;
		}
		free(pMode);
	}
}

// kybd map for mode line input
bool line_input_map( SMode *pBase, wxKeyEvent &event, ModalWindow *pWin ) {
  SModeLineInp *pLineInp = pBase->sExt.pLineInput;
  bool bRetVal = true;

  // proceses text input
  // updates szBufDest
  // calls line-input_disp_update
  if( pLineInp->pMsg != NULL && pLineInp->pInput != NULL )
  {
    pBase->key = event.GetKeyCode();
    pBase->uniKey = event.GetUnicodeKey();
    // case exit
    if( pBase->key == WXK_ESCAPE ) {
      pWin->m_pModeManager->pop();
      pWin->m_bUsrActn = false;
      pWin->Refresh( true );
    }
    // proceses text input
    // update szBufDest
    // call line-input_disp_update
    else {
      // single line length is 20 characters
      // Enter exits the process
      // getChar, add to g_pcSingleLine
      // if not char, handle backspace or arrow keys
      bool bExit = false;
      wxMemoryDC DC;
      if( pBase->pFont != NULL ) {
        pBase->load_font();
        DC.SetFont( *(pBase->pFont) );
      }
      // delete the character behind the caret
      if( pBase->key == WXK_BACK ) {
        if( pLineInp->indexCaret >= 1 ) {
          pLineInp->pInput->szBuf[pLineInp->indexCaret-1] = '|';
          pLineInp->pInput->szBuf[pLineInp->indexCaret] = 0;
          pLineInp->indexCaret--;
        }
        pLineInp->bInputRcvd = false;
      }
      else if( pBase->key == WXK_SHIFT )
      pBase->bShiftDown = true;
      // add the input character to szBufDest
      // if return,
      // pop the line-input from the mode manager
      // notify caller
      else {
        char c = pBase->uniKey;
        char cU;
        // add the input character to szBufDest
        // if return,
        // pop the line-input from the mode manager
        // notify caller
        if( c != WXK_NONE ) {
          // add the input character to szBufDest
          if( c >= 32  ) {
            cU = to_upper( c, pBase->bShiftDown );
            if( pLineInp->indexCaret < pLineInp->pInput->maxLength ) {
              pLineInp->pInput->szBuf[pLineInp->indexCaret] = cU;
              pLineInp->pInput->szBuf[pLineInp->indexCaret+1] = '|';
              pLineInp->pInput->szBuf[pLineInp->indexCaret+2] = 0;
              pLineInp->indexCaret++;
            }
          }
          // if return,
          // pop the line-input from the mode manager
          // notify caller
          else {
            // pop the line-input from the mode manager
            // notify caller
            if( c == WXK_RETURN ) {
              if( pLineInp->indexCaret < pLineInp->pInput->maxLength )
              pLineInp->pInput->szBuf[pLineInp->indexCaret] = 0;
              pLineInp->indexCaret = 0;
              pWin->m_pModeManager->pop();
              pLineInp->bInputRcvd = true;
              pLineInp->pCaller->fnIntent_handler[pLineInp->callerIntent]( pLineInp->pCaller, PH_NOTIFY, pWin, DC );
              bExit = true;
            }
          }
        }
      }
      if( !bExit )
        line_input_disp_update( pBase, PH_NOTIFY, pWin, DC );
    }
  }
  pWin->m_bUsrActn = false;
  return( bRetVal );
} // line_input_map
// display current state for mode line input
void line_input_disp_state( SMode *pBase, ModalWindow *pWin, wxDC &DC ) {
  SModeLineInp *pLineInp = pBase->sExt.pLineInput;
  // determine the width of the text input framing rect
  // determine the location of the text to be displayed
  // determine the location of the user message
  // draw framing rext, user message, user input text
  if( pLineInp->pMsg != NULL && pLineInp->pInput != NULL )
  {
	wxRect rectFrame;
	wxRect rectMsg;
	int widthTxt;
	int heightTxt;
	int widthMsg;
	int heightMsg;
	int xTxt;
	int yTxt;
	int xMsg;
	int yMsg;

	// determine the width of the text input framing rect
	// 40 max width characters
	wxString strFrame("A");
	DC.GetTextExtent( strFrame, &(rectFrame.width), &(rectFrame.height) );
	rectFrame.width *= 40;
	rectFrame.height += 10;
	rectFrame.x = pBase->scrnW/2  - rectFrame.width/2;
	rectFrame.y = pBase->scrnH/2  - rectFrame.height/2;

	wxString dispText( pLineInp->pInput->szBuf );
	// determine the location of the text to be displayed
	if( pLineInp->indexCaret > 0 ) {
	  DC.GetTextExtent( dispText, &(widthTxt), &(heightTxt) );
	  xTxt = pBase->scrnW/2 - widthTxt/2;
	  yTxt = pBase->scrnH/2 - heightTxt/2;
	}

	// determine the location of the user message
	DC.GetTextExtent( wxString( pLineInp->pMsg->szBuf ), &widthMsg, &heightMsg );
	xMsg = pBase->scrnW/2 - widthMsg/2;
	yMsg = pBase->scrnH/2 - rectFrame.height/2 - heightMsg;
	rectMsg.x = rectFrame.x;
	rectMsg.width = rectFrame.width;
	rectMsg.y = yMsg;
	rectMsg.height = heightMsg;

	// draw
	wxPen Pen = DC.GetPen();
	wxBrush Brush = DC.GetBrush();
	DC.SetBrush( *wxWHITE_BRUSH );
	DC.DrawRectangle( rectFrame );
	DC.SetPen( *wxTRANSPARENT_PEN );
	DC.SetBrush( wxBrush( wxColour( 208,208,200 ) ) );
	DC.DrawRectangle( rectMsg );
	DC.SetPen( Pen );
	DC.SetBrush( Brush );
	if( pLineInp->indexCaret > 0 )
	DC.DrawText( dispText, xTxt, yTxt );
	DC.DrawText( wxString( pLineInp->pMsg->szBuf ), xMsg, yMsg );
  }
}
// display update for mode line input
void line_input_disp_update( SMode * pBase, int phase, ModalWindow *pWin, wxDC& DC ) {
  SModeLineInp *pLineInp = pBase->sExt.pLineInput;
  // refresh the framing rect for this line input's input erea
  if( phase == PH_NOTIFY ) {
	wxRect rectFrame;
	wxString strFrame("A");
	DC.GetTextExtent( strFrame, &(rectFrame.width), &(rectFrame.height) );
	rectFrame.width *= pLineInp->pInput->maxLength;
	rectFrame.height += 10;
	rectFrame.x = pBase->scrnW/2  - rectFrame.width/2;
	rectFrame.y = pBase->scrnH/2  - rectFrame.height/2;
	pBase->intent = 0;
	pWin->RefreshRect( rectFrame, true );
	pWin->Update();
  }
  else
	line_input_disp_state( pBase, pWin, DC );
  return;
}
// SUBBLOCK: LEVEL ADJUSTER
// Definitions for mode level adjuster
// A pop-up mode for adjusting a level

// the intents for mode level adjuster
enum {
  LAI_ADJUST=0,
};
// the pop-up level adjuster mode
typedef struct SModeLevAdj {
  void init( SMode *pBase ) {
	pBase->fnDisp_state = lev_adj_disp_state;
	pBase->fnKybd_map = lev_adj_map;
	pBase->type = MODE_LEVEL_ADJUSTER;
	pBase->bReset = true;
	this->pBase = pBase;
	this->load_intents( pBase );
	this->szMsg = NULL;
	this->pCaller = NULL;
	this->callerIntent = 0;
	this->bInputRcvd = false;
  };
  void set_caller( SMode *pCaller, int callerIntent, char *szMsg ) {
	this->pCaller = pCaller;
	this->callerIntent = callerIntent;
	this->szMsg = szMsg;
	this->bInputRcvd = false;
	this->bActive = false;
  }
  // loads the intent dispatch fns for thsi mode
  void load_intents( SMode *pBase ) {
	pBase->numIntents = 1;
	pBase->fnIntent_handler[LAI_ADJUST] = lev_adj_adjust;
  };
  SMode* pBase;
  SMode *pCaller;
  int callerIntent;
  char * szMsg;
  bool bInputRcvd;
  wxRect Rect;
  bool bReset;
  bool bIncDec;
  bool bActive;
} SModeLevAdj;
// allocs and inits a ptr on the heap and returns it
// caller has to free
SMode * new_lev_adj( int scrnW, int scrnH, wxFont *pFont ) {
  SMode *pMode = (SMode *) malloc( sizeof( SMode) );
  if (pMode != NULL) {
	  pMode->init(scrnW, scrnH, pFont);
	  SModeLevAdj *pLevAdj = (SModeLevAdj *)malloc(sizeof(SModeLevAdj));
	  if (pLevAdj != NULL) {
		  pMode->sExt.pLevAdj = pLevAdj;
		  pMode->sExt.pLevAdj->init(pMode);
	  }
  }
  return( pMode );
}
void free_lev_adj(SMode* pMode) {
	free(pMode->sExt.pLevAdj);
	free(pMode);
}

// kybd map for the level adjuster
bool lev_adj_map( SMode *pBase, wxKeyEvent &event, ModalWindow *pWin ) {
  bool bRetVal = true;
  SModeLevAdj *pLevAdj = pBase->sExt.pLevAdj;
  wxClientDC DC(pWin);
  if( pBase->pFont != NULL ) {
    pBase->load_font();
    DC.SetFont( *(pBase->pFont) );
  }

  // Up Arrow or Right Arrow and Down Arrow or Left Arrow (Inc or dec) dispatch to LAI_SELECT
  // Esc, bInputRcvd = false, pop this off the mode manager
  pBase->key = event.GetKeyCode();
  pBase->uniKey = event.GetUnicodeKey();

  if (pBase->key == WXK_UP || pBase->key == WXK_DOWN || pBase->key == WXK_RIGHT || pBase->key == WXK_LEFT) {
	  if (!pLevAdj->bActive) {
		  pLevAdj->bActive = true;
		  pWin->m_bUsrActn = false;
		  pWin->Refresh(true);
		  pWin->Update();
	  }
	  pBase->intent = LAI_ADJUST;
	  pBase->fnIntent_handler[pBase->intent](pBase, PH_NOTIFY, pWin, DC);
  }
  else if (pBase->key == WXK_ESCAPE) {
	  pWin->m_pModeManager->pop();
	  pWin->m_bUsrActn = false;
	  pWin->Refresh( true );
  }
  return( bRetVal );
} 

// displays the current state of this level adjuster
void lev_adj_disp_state( SMode *pBase, ModalWindow *pWin, wxDC& DC ) {
  SModeLevAdj *pLevAdj = pBase->sExt.pLevAdj;
  // display the level adjusters intro message if it's not yet active
  // nothing otherwise
  if (!pLevAdj->bActive) {
	  wxRect rectFrame;
	  DC.GetTextExtent(wxString(pLevAdj->szMsg), &(rectFrame.width), &(rectFrame.height));
	  rectFrame.width += 80;
	  rectFrame.height *= 5;
	  rectFrame.x = pBase->scrnW / 2 - rectFrame.width / 2;
	  rectFrame.y = pBase->scrnH / 2 - rectFrame.height / 2;
	  wxPen Pen = DC.GetPen();
	  wxBrush Brush = DC.GetBrush();
	  DC.SetPen(*wxTRANSPARENT_PEN);
	  DC.SetBrush(wxBrush(wxColour(208, 208, 200)));
	  DC.DrawRectangle(rectFrame);
	  DC.SetPen(Pen);
	  DC.SetBrush(Brush);

	  // message
	  int width;
	  int height;
	  DC.GetTextExtent(wxString(pLevAdj->szMsg), &width, &height);
	  DC.DrawText(wxString(pLevAdj->szMsg),
		  pBase->scrnW / 2 - width / 2,
		  pBase->scrnH / 2 - height / 2);
  }
} 
// intent handler for level adjuster LAI_ADJUST
void lev_adj_adjust( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
	SModeLevAdj* pLevAdj = pBase->sExt.pLevAdj;
	if( phase == PH_NOTIFY ) {
		if (pBase->key == WXK_UP || pBase->key == WXK_RIGHT) {
			pLevAdj->bIncDec = true;
			pLevAdj->bInputRcvd = true;
			pLevAdj->pCaller->fnIntent_handler[pLevAdj->callerIntent](pLevAdj->pCaller, PH_NOTIFY, pWin, DC);
		}
		else if (pBase->key == WXK_DOWN || pBase->key == WXK_LEFT) {
			pLevAdj->bIncDec = false;
			pLevAdj->bInputRcvd = true;
			pLevAdj->pCaller->fnIntent_handler[pLevAdj->callerIntent](pLevAdj->pCaller, PH_NOTIFY, pWin, DC);
		}
	}
}
// SUBBLOCK: FILE SELECTOR
// The file selector mode is for selecting a file from the file system
// or creating a new one at a location in the file system.

// the different (user) intents for mode file selector
enum {
  // change the selection using arrows and PgUp PgDn
  FSI_CHANGE_SELECTION=0,
  // commit to the current selection using Spacebar or Return
  FSI_COMMIT
};
#define FILES_PER_DIR 100
struct SDirPanel;
// struct to hold a file directory name
typedef struct SFileEntry {
	bool bFileDir;
	STxtLine *pName;
} SFileEntry;
void free_file_entry(SFileEntry Entry) {
	if (Entry.pName != NULL) {
		tl_free(Entry.pName);
    Entry.pName = NULL;
  }
}
void dir_panel_add_entry(SDirPanel* pDirPanel, SFileEntry Entry);
// Modal's dir traverser
class ModalDirTraverser : public wxDirTraverser {
public:
	ModalDirTraverser(SDirPanel* pFiles) {
		m_pFiles = pFiles;
	}
	virtual wxDirTraverseResult OnFile(const wxString& filename)
	{
		SFileEntry Entry;
		Entry.bFileDir = true;
		Entry.pName = new_txt_line_wx( filename );
		dir_panel_add_entry(m_pFiles, Entry);
		return wxDIR_CONTINUE;
	}
	virtual wxDirTraverseResult OnDir(const wxString& dirname)
	{
		SFileEntry Entry;
		wxDir Dir(dirname);
		if( Dir.IsOpened() ) {
		  wxString strDir = Dir.GetNameWithSep();
		  Entry.bFileDir = false;
		  Entry.pName = new_txt_line_wx( strDir );
		  dir_panel_add_entry(m_pFiles, Entry);
		}
		return wxDIR_IGNORE;
	}
private:
	SDirPanel* m_pFiles;
};
// struct to hold a panel full of file or directory entries
typedef struct SDirPanel {
	void init(int maxEntries) {
		this->pEntries = (SFileEntry*)malloc(maxEntries * sizeof(SFileEntry));
		this->maxEntries = maxEntries;
		this->numEntries = 0;
		this->rectDisp.x = 0;
		this->rectDisp.y = 0;
		this->rectDisp.width = 0;
		this->rectDisp.height = 0;
		this->numDispEntries = 0;
		this->entryH = -1;
		this->startIndex = -1;
		this->selIndex = -1;
		this->scrnH = -1;
		this->pDirName = NULL;
		this->bActive = false;
		this->bRootDir = false;
		this->bReset = true;
	};
	void load_dir(STxtLine* pDirName) {
		if (this->pDirName != NULL) {
			tl_free(this->pDirName);
      this->pDirName = NULL;
    }
		this->erase();
		wxDir Dir(wxString(pDirName->szBuf));
		wxString strDirName = Dir.GetNameWithSep();
		// if it's the root dir, set a flag on this dirpanel
		wxFileName FileName(strDirName);
		int numDirs = FileName.GetDirCount();
		if (numDirs == 0)
			this->bRootDir = true;
		this->pDirName = new_txt_line_wx(strDirName);
		if (Dir.IsOpened()) {
			ModalDirTraverser traverser(this);
			int flags = wxDIR_DIRS | wxDIR_FILES;
			Dir.Traverse(traverser, wxEmptyString, flags);
		}
		this->startIndex = 0;
		this->selIndex = 0;
		this->bReset = true;
	};
	void erase() {
		for (int i = 0; i < this->numEntries; i++)
			free_file_entry(this->pEntries[i]);
		this->numEntries = 0;
	};
	void disp_init( int scrnH, int panelW, int entryH, int numEntries, int index ) {
		this->rectDisp.x = panelW / 2 + index * panelW;
		this->rectDisp.width = panelW;
		this->maxDispEntries = numEntries;
		if (numEntries < this->numEntries)
			this->numDispEntries = numEntries;
		else
			this->numDispEntries = this->numEntries;
		this->entryH = entryH;
		this->scrnH = scrnH;
		this->rectDisp.y = this->scrnH / 2 - (this->numDispEntries / 2) * this->entryH;
		this->rectDisp.height = this->numDispEntries * this->entryH;
	};
	void set_start_index(int index) {
		if (index >= 0 && index < this->numEntries)
			this->startIndex = index;
	};
	void inc_sel_index( int inc ) {
		wxASSERT(inc != 0);
		if (inc > 0) {
			if (this->selIndex + inc < this->numDispEntries)
				this->selIndex += inc;
			else {
			  if( this->numDispEntries < this->numEntries ) {
          this->startIndex += inc;
          this->selIndex += inc;
          if (this->selIndex >= this->numEntries) {
            this->selIndex = (this->numEntries - 1);
            this->startIndex = (this->numEntries - this->maxDispEntries);
          }
			  }
			}
		}
		else {
			if (this->selIndex + inc >= 0) {
				this->selIndex += inc;
				if (this->selIndex - this->startIndex < 0) {
					this->startIndex = this->selIndex;
				}
			}
			else {
				this->startIndex = 0;
				this->selIndex = 0;
			}
		}
	};
	void display(wxDC& DC) {
		// re-initialize display parameters
		if (this->bReset) {
			if (this->numEntries >= this->maxDispEntries)
				this->numDispEntries = this->maxDispEntries;
			else
				this->numDispEntries = this->numEntries;
			if (this->selIndex > this->numDispEntries)
				this->startIndex = this->selIndex;
			this->bReset = false;
		}
		// display this dir-panel if it is not empty
		if (this->numEntries > 0) {
			// display an empty rect around the sel index
			wxRect rect;
			rect.x = this->rectDisp.x + 20;
			rect.width = this->rectDisp.width - 40;
			rect.y = this->rectDisp.y + (this->selIndex-this->startIndex) * this->entryH;
			rect.height = this->entryH;
			wxBrush Brush = DC.GetBrush();
			wxPen Pen = DC.GetPen();
			if( this->bActive ) 
				DC.SetPen(*wxWHITE_PEN);
			DC.SetBrush(*wxTRANSPARENT_BRUSH);
			DC.DrawRectangle(rect);
			DC.SetBrush(Brush);
			DC.SetPen(Pen);

			wxColour Colour = DC.GetTextForeground();
			int width = 0;
			int height = 0;
			// display the entries in this dir panel
			for (int i = 0; i < this->numDispEntries && i < this->numEntries - this->startIndex; i++) {
				if (this->pEntries[this->startIndex + i].bFileDir) {
					wxString strFileName;
					if (!this->bRootDir) {
						wxFileName FileName(wxString(this->pEntries[this->startIndex + i].pName->szBuf));
						strFileName = FileName.GetFullName();
					}
					else
						strFileName = wxString(this->pEntries[this->startIndex + i].pName->szBuf);
					STxtLine *pLine = new_txt_line_wx(strFileName); 
					DC.GetTextExtent( wxString( pLine->szBuf ), &width, &height);
					while( width > this->rectDisp.width-80 ) {
					  char* szTemp = tl_cut_out(pLine, pLine->length / 2, pLine->length);
					  if (szTemp != NULL)
						free(szTemp);
					  tl_insert(pLine, (char*)" ...", pLine->length);
					  DC.GetTextExtent( wxString( pLine->szBuf ), &width, &height);
					}
					DC.SetTextForeground(wxColour(0, 0, 0));
					DC.DrawText( wxString( pLine->szBuf ),
						this->rectDisp.x + this->rectDisp.width / 2 - width / 2,
						this->rectDisp.y + this->entryH / 2 - height / 2 + i * this->entryH);
					tl_free( pLine );
          pLine = NULL;  
				}
				else {
					wxDir Dir(wxString(this->pEntries[this->startIndex + i].pName->szBuf));
					wxString strDir = Dir.GetNameWithSep();
					wxString strDirName;
					if (!this->bRootDir) {
						wxFileName FileName(strDir);
						wxArrayString strDirs = FileName.GetDirs();
						int numDirs = strDirs.GetCount();
						if (numDirs > 0) {
							strDirName = strDirs[numDirs - 1];
						}
					}
					else
						strDirName = strDir;
					STxtLine *pLine = new_txt_line_wx(strDirName); 
					DC.GetTextExtent( wxString( pLine->szBuf ), &width, &height);
					while( width > this->rectDisp.width-80 ) {
					  char* szTemp = tl_cut_out(pLine, pLine->length / 2, pLine->length);
					  if (szTemp != NULL)
						free(szTemp);
					  tl_insert(pLine, (char*)" ...", pLine->length);
					  DC.GetTextExtent( wxString( pLine->szBuf ), &width, &height);
					}
					int widthOutline = width + 20;
					int heightOutline = height + 10;
					DC.DrawRectangle( this->rectDisp.x + this->rectDisp.width / 2 - widthOutline / 2,
						this->rectDisp.y + this->entryH / 2 - heightOutline / 2 + i * this->entryH,
						widthOutline,
						heightOutline );
					DC.SetTextForeground(wxColour(0, 0, 64));
					DC.DrawText( wxString( pLine->szBuf ),
						this->rectDisp.x + this->rectDisp.width / 2 - width / 2,
						this->rectDisp.y + this->entryH / 2 - height / 2 + i * this->entryH);
					tl_free( pLine );
          pLine = NULL;  
				}
			}
			DC.SetTextForeground(Colour);
		}
	};
	wxRect get_rect() {
		return(this->rectDisp);
	};
	// sets the index of the this dir panel
	// to pDirName if it matches an entry in the panel
	// and if that entry is in visible range
	void set_index(STxtLine* pDirName) {
		bool bFound = false;
		int index = -1;
		for (int i = 0; i < this->numEntries && !bFound; i++) {
			if (tl_equals(pDirName, this->pEntries[i].pName)) {
				bFound = true;
				index = i;
			}
		}
		if (bFound) {
			this->selIndex = index;
			this->bReset = true;
		}
	}
	SDirPanel clone() {
		SDirPanel RetVal;
		RetVal.maxEntries = this->maxEntries;
		RetVal.numEntries = this->numEntries;
		RetVal.numDispEntries = this->numDispEntries;
		RetVal.entryH = this->entryH;
		RetVal.startIndex = this->startIndex;
		RetVal.selIndex = this->selIndex;
		RetVal.scrnH = this->scrnH;
		RetVal.bActive = this->bActive;
		RetVal.bRootDir = this->bRootDir;
		if( this->pDirName != NULL ) 
		  RetVal.pDirName = tl_clone(pDirName);
		else
		  RetVal.pDirName = NULL;
		RetVal.pEntries = (SFileEntry*)malloc(RetVal.maxEntries * sizeof(SFileEntry));
		for (int i = 0; i < RetVal.numEntries; i++) {
			RetVal.pEntries[i].bFileDir = this->pEntries[i].bFileDir;
			RetVal.pEntries[i].pName = tl_clone(this->pEntries[i].pName);
		}
		return(RetVal);
	};

	wxRect rectDisp;
	SFileEntry *pEntries;
	int maxEntries;
	int numEntries;
	int numDispEntries;
	int maxDispEntries;
	int entryH;
	int startIndex;
	int selIndex;
	int scrnH;
	bool bActive;
	bool bRootDir;
	bool bReset;
	STxtLine* pDirName;
} SDirPanel;
// adds an entry to the specified dir panel
void dir_panel_add_entry( SDirPanel *pDirPanel, SFileEntry Entry) {
	if (pDirPanel->numEntries < pDirPanel->maxEntries) {
		pDirPanel->pEntries[pDirPanel->numEntries] = Entry;
		pDirPanel->numEntries += 1;
	}
	else {
		pDirPanel->maxEntries *= 2;
		pDirPanel->pEntries = (SFileEntry *) realloc(pDirPanel->pEntries, pDirPanel->maxEntries*sizeof(SFileEntry) );
		pDirPanel->pEntries[pDirPanel->numEntries] = Entry;
		pDirPanel->numEntries += 1;
	}
}
// frees the entries allocated in the dir panel
void free_dir_panel(SDirPanel DirPanel) {
	if (DirPanel.pEntries != NULL) {
		for (int i = 0; i < DirPanel.numEntries; i++)
			free_file_entry(DirPanel.pEntries[i]);
		tl_free(DirPanel.pDirName);
    DirPanel.pDirName = NULL;  
		free(DirPanel.pEntries);
	}
}

// the file selector mode extension
// this mode ext uses the wxDirTraverser class
// to traverse a directory and find the files and dirs it contains
// it displays upto 5 panels of heirarchial directories.
// when initially loaded by a caller
// it loads the current dir in the center panel
// and 2 higher levels of dirs in the left panels
// if the user opens a sub dir, a right panel is added
// this operation can be done twice.
// Subsequently, all the panels are shifted left by 1 slot
// If the user navigates to a parent dir using Left Arrow
// the panels are shifted right by 1 slot
typedef struct SModeFileSel {
  // init with max files per directory
  void init( SMode *pBase, int filesPerDir ) {
	pBase->fnDisp_state = file_sel_disp_state;
	pBase->fnKybd_map = file_sel_map;
	pBase->type = MODE_FILE_SELECTOR;
	pBase->bReset = true;
	this->pBase = pBase;
	this->load_intents( pBase );
	this->pCaller = NULL;
	this->callerIntent = -1;
	this->bInputRcvd = false;
	this->panelHeight = 0;
	this->panelWidth = 0;
	this->numPanelEntries = 0;
	this->entryHeight = 0;
	this->selEntry = 0;
	for (int i = 0; i < 5; i++) 
		this->aDirPanels[i].init(filesPerDir);
	this->activePanel = -1;
	this->pStartDir = NULL;
	this->pFilePath = NULL;
  };
  // loads the intent dispatch fns for this mode
  void load_intents( SMode *pBase ) {
	pBase->numIntents = 2;
	pBase->fnIntent_handler[FSI_CHANGE_SELECTION] = file_sel_change_sel;
	pBase->fnIntent_handler[FSI_COMMIT] = file_sel_commit;
  };
  void set_caller( SMode *pCaller, int callerIntent, STxtLine *pStartDir ) {
	this->pCaller = pCaller;
	this->callerIntent = callerIntent;
	this->bInputRcvd = false;
	if (this->pStartDir != NULL) {
		tl_free(this->pStartDir);
    this->pStartDir = NULL;  
  }
	// load the current dir in the center(2) panel
	// the parent dir in the left of center(1) panel
	// and the parent's parent in the leftmost(0) panel;
	wxString strFileName( pStartDir->szBuf );
	wxFileName FileName( strFileName );
	STxtLine *pDirName = new_txt_line_wx(FileName.GetPath());
	for (int i = 0; i < 5; i++)
		this->aDirPanels[i].erase();
	this->aDirPanels[2].load_dir( pDirName );
	tl_free(pDirName);
	pDirName = NULL; 
	int numDirs = FileName.GetDirCount();
	if (numDirs > 0) {
		FileName.RemoveLastDir();
		pDirName = new_txt_line_wx(FileName.GetPath());
		this->aDirPanels[1].load_dir( pDirName );
		tl_free(pDirName);
		pDirName = NULL;  
		this->aDirPanels[1].set_index(this->aDirPanels[2].pDirName);
	}

	if (numDirs > 1) {
		FileName.RemoveLastDir();
		pDirName = new_txt_line_wx(FileName.GetPath());
		this->aDirPanels[0].load_dir( pDirName );
		tl_free(pDirName);
    pDirName = NULL;    
		this->aDirPanels[0].set_index(this->aDirPanels[1].pDirName);
	}
	set_active_panel( 2 );
	pBase->bReset = true;
  }
  void set_active_panel(int index) {
	  if (index >= 0 && index < 5) {
		  if( this->activePanel != -1 )
			  this->aDirPanels[this->activePanel].bActive = false;
		  this->activePanel = index;
		  this->aDirPanels[this->activePanel].bActive = true;
	  }
  }
  SMode *pBase;
  SMode *pCaller;
  int callerIntent;
  bool bInputRcvd;
  STxtLine *pStartDir;
  STxtLine *pFilePath;
  int panelWidth;
  int panelHeight;
  int entryHeight;
  int numPanelEntries;
  int selEntry;
  SDirPanel aDirPanels[5];
  int activePanel;
} SModeFileSel;
// allocs and inits a ptr on the heap and returns it
// caller has to free
SMode * new_file_sel( int scrnW, int scrnH, wxFont *pFont ) {
  SMode *pBase = (SMode *) malloc( sizeof( SMode) );
  if (pBase != NULL) {
	  pBase->init(scrnW, scrnH, pFont);
	  SModeFileSel* pFileSel = (SModeFileSel*)malloc(sizeof(SModeFileSel));
	  if (pFileSel != NULL) {
		  pBase->sExt.pFileSel = pFileSel;
		  pBase->sExt.pFileSel->init( pBase, FILES_PER_DIR );
	  }
  }
  return( pBase );
}
void free_file_sel(SMode* pMode) {
	for (int i = 0; i < 5; i++) 
		free_dir_panel(pMode->sExt.pFileSel->aDirPanels[i]);
	tl_free(pMode->sExt.pFileSel->pStartDir);
  pMode->sExt.pFileSel->pStartDir = NULL; 
	tl_free(pMode->sExt.pFileSel->pFilePath);
	pMode->sExt.pFileSel->pFilePath = NULL;
  free(pMode->sExt.pFileSel);
	free(pMode);
}

// mode :: kybd map
// called by the mode manager when the users inputs on the kybd
bool file_sel_map( SMode *pBase, wxKeyEvent &event, ModalWindow *pWin ) {
  bool bRetVal = true;
  SModeFileSel *pFileSel = pBase->sExt.pFileSel;

  wxClientDC DC( pWin ); // dummy
  if( pBase->pFont != NULL ) {
    pBase->load_font();
    DC.SetFont( *(pBase->pFont) );
  }

  pBase->key = event.GetKeyCode();
  pBase->uniKey = event.GetUnicodeKey();

  // case exit, pop the mode manager refresh the window
  if( pBase->key == WXK_ESCAPE ) {
	pWin->m_pModeManager->pop();
	pWin->m_bUsrActn = false;
	pWin->Refresh( true );
  }
  // if its and arrow or PgUp, PgDn, dispatch to CHANGE_SELECTION
  // if its retrun or space dispatch to COMMIT
  else {
	bool bChangeSel = false;
	bChangeSel = pBase->key == WXK_UP || pBase->key==WXK_DOWN || pBase->key==WXK_LEFT  || pBase->key==WXK_RIGHT;
	// dispatch to CHANGE_SELECTION
	if( bChangeSel ) {
	  pBase->intent = FSI_CHANGE_SELECTION;
	  pBase->fnIntent_handler[pBase->intent]( pBase, PH_NOTIFY, pWin, DC );
	}
	// dispatch to COMMIT
	else if( pBase->key == WXK_RETURN || pBase->key == WXK_SPACE ) {
	  pBase->intent = FSI_COMMIT;
	  pBase->fnIntent_handler[pBase->intent]( pBase, PH_NOTIFY, pWin, DC );
	}
  } // end case not exit
  pWin->m_bUsrActn = false;
  return( bRetVal );
}
// ancillary gets the display rect of this file selector
void file_sel_get_rect( SMode *pBase, wxRect *pRect, wxDC& DC ) {
  SModeFileSel *pFileSel = pBase->sExt.pFileSel;

}
// mode :: display current state
// called by the mode manager
void file_sel_disp_state( SMode *pBase, ModalWindow *pWin, wxDC& DC ) {
	// if the display params have not been inited, init them
	SModeFileSel* pFileSel = pBase->sExt.pFileSel;
	if (pBase->bReset) {
		pFileSel->panelWidth = pBase->scrnW / 6;
		pFileSel->panelHeight = (pBase->scrnH * 9) / 10;
		int entryHeight = 0;
		int dummy;
		DC.GetTextExtent("A", &dummy, &entryHeight);
		pFileSel->entryHeight = entryHeight * 3;
		pFileSel->numPanelEntries = pFileSel->panelHeight / pFileSel->entryHeight;
		for (int i = 0; i < 5; i++)
			pFileSel->aDirPanels[i].disp_init(
				pBase->scrnH,
				pFileSel->panelWidth,
				pFileSel->entryHeight,
				pFileSel->numPanelEntries,
				i);
		pBase->bReset = false;
	}

	// display each of the 5 panels 
	for (int i = 0; i < 5; i++) 
		pFileSel->aDirPanels[i].display(DC);
}
// intent handler for CHANGE_SELECTION
// user wants to change the selected using the arrows or PgUp PgDn
void file_sel_change_sel( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  if( phase == PH_NOTIFY ) {
	SModeFileSel * pFileSel = pBase->sExt.pFileSel;
	// change the current selection based on the arrows or PgUp PgDn
	switch( pBase->key ) {
	  // move up a line in the current display column
	  case WXK_UP: {
		  pFileSel->aDirPanels[pFileSel->activePanel].inc_sel_index(-1);
		  pWin->m_bUsrActn = false;
		  pWin->Refresh(true);
	  }
	  break;
	  // move down a line in the current display column
	  case WXK_DOWN: {
		  pFileSel->aDirPanels[pFileSel->activePanel].inc_sel_index(1);
		  pWin->m_bUsrActn = false;
		  pWin->Refresh(true);
	  }
	  break;
	  // goto the parent directory if it exists
	  case WXK_LEFT: {
		  // shift panels right, update panel 0 with parent dir if it exists
		  // set sel index of panel 0 with index of panel 1 dir 
		  if (pFileSel->activePanel == 0) {
			  // check panel 0 dir has a parent
			  wxFileName FileName(wxString(pFileSel->aDirPanels[0].pDirName->szBuf));
			  int numDirs = FileName.GetDirCount();
			  if (numDirs > 0) {
				  FileName.RemoveLastDir();
				  pFileSel->aDirPanels[pFileSel->activePanel].bActive = false;
				  for (int i = 4; i >= 1; i--)
					  pFileSel->aDirPanels[i] = pFileSel->aDirPanels[i - 1].clone();
				  STxtLine* pDirName = new_txt_line_wx(FileName.GetPath());
				  pFileSel->aDirPanels[0].load_dir(pDirName);
				  pFileSel->aDirPanels[0].set_index(pFileSel->aDirPanels[1].pDirName);
				  pFileSel->aDirPanels[0].bActive = true;
				  pBase->bReset = true;
				  tl_free(pDirName);
				  pDirName = NULL;    
			  }
		  }
		  // update active panel
		  else {
			  if (!pFileSel->aDirPanels[pFileSel->activePanel].bRootDir)
				  pFileSel->set_active_panel(pFileSel->activePanel - 1);
		  }
		  pWin->m_bUsrActn = false;
		  pWin->Refresh(true);
	  }
	  break;
	  // if current selection is a dir open it
	  case WXK_RIGHT: {
		  SDirPanel ActivePanel = pFileSel->aDirPanels[pFileSel->activePanel];
		  int selEntry = 0;
		  // if the current panel is right-most, shift panels left
		  // open a new panel at right-most location
		  if (pFileSel->activePanel == 4) {
			  selEntry = ActivePanel.selIndex;
			  if (ActivePanel.pEntries[selEntry].bFileDir == false) {
				  pFileSel->aDirPanels[4].bActive = false;
				  for (int i = 0; i < 4; i++) 
					  pFileSel->aDirPanels[i] = pFileSel->aDirPanels[i + 1].clone();  
				  STxtLine* pDirName = tl_clone(ActivePanel.pEntries[selEntry].pName);
				  pFileSel->aDirPanels[4].load_dir(pDirName);
				  tl_free(pDirName);
          pDirName = NULL;    
				  if( pFileSel->aDirPanels[4].numEntries > 0 )
            pFileSel->aDirPanels[4].bActive = true;
				  else
            pFileSel->aDirPanels[3].bActive = true;
			  }
			  pBase->bReset = true;
		  }
		  // if the next panel does not have the selected dir loaded, load it
		  // erase all panels after the next panel in this case
		  else {
			  selEntry = ActivePanel.selIndex;
			  if (ActivePanel.pEntries[selEntry].bFileDir == false) {
				  if (!tl_equals(ActivePanel.pEntries[selEntry].pName, pFileSel->aDirPanels[pFileSel->activePanel + 1].pDirName)) {
					  pFileSel->aDirPanels[pFileSel->activePanel + 1].load_dir(ActivePanel.pEntries[selEntry].pName);
					  if (pFileSel->activePanel < 3)
						  for (int i = pFileSel->activePanel + 2; i < 5; i++)
							  pFileSel->aDirPanels[i].erase();
				  }
				  if( pFileSel->aDirPanels[pFileSel->activePanel+1].numEntries > 0 )
					pFileSel->set_active_panel(pFileSel->activePanel + 1);
			  }
			  pBase->bReset = true;
		  }
		  pWin->m_bUsrActn = false;
		  pWin->Refresh(true);
	  }
	  break;
	  // signal error
	  default: {
		wxString msg;
		msg.Printf( "invalid key value in file_sel_update_sel %d", pBase->key );
		wxLogError( msg );
	  }
	  break;
	}
  }
  // PH_EXEC
  else
	file_sel_disp_state( pBase, pWin, DC );
}
// intent handler for COMMIT
// user wants to commit to the selected file
void file_sel_commit( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // pop this file selector off the mode stack
  // notify caller
  if( phase == PH_NOTIFY ) {
	SModeFileSel * pFileSel = pBase->sExt.pFileSel;
	SDirPanel DirPanel = pFileSel->aDirPanels[pFileSel->activePanel];
	if (DirPanel.pEntries[DirPanel.selIndex + DirPanel.startIndex].bFileDir) {
		if (pFileSel->pFilePath != NULL) {
			tl_free(pFileSel->pFilePath);
      pFileSel->pFilePath = NULL;    
    }
		pFileSel->pFilePath = tl_clone(DirPanel.pEntries[DirPanel.selIndex + DirPanel.startIndex].pName);
		pWin->m_pModeManager->pop();
		pFileSel->bInputRcvd = true;
		pFileSel->pCaller->fnIntent_handler[pFileSel->callerIntent](pFileSel->pCaller, PH_NOTIFY, pWin, DC);
	}
  }
}
// SUBBLOCK: INTENT DISPATCHER
// The intent dispatcher mode is used to add onscreen controls for a mode
// Onscreen controls differ from direct mapped controls
// in that they have an ocscreen representation for what they do
// and they are not accessible directly from the kybd but indirectly
// hence their spped of access is slower than direct-mapped controls
// They contain a set of additional (non-direct-mapped) intents
// from which one may be selected and executed.
// selection is done using the up and down arrow keys
// and execution by pressing space bar.
// once one of the available choices is executed,
// the mode is popped of the mode manager
// and the intent handler for the selected intent is called on the calling mode

// ancillary struct used by SModeIntDisp
typedef struct SIntent {
  STxtLine *ptlName;
  int index;
} SIntent;

SIntent * new_intent( char *szString, int index ) {
  SIntent *pIntent = (SIntent *) malloc( sizeof( SIntent ) );
  if (pIntent != NULL) {
	  pIntent->ptlName = new_txt_line(szString);
	  pIntent->index = index;
  }
  return( pIntent );
}
void free_intent(SIntent* pIntent) {
	if (pIntent != NULL) {
		tl_free(pIntent->ptlName);
		pIntent->ptlName = NULL;
		free(pIntent);
	}
}
// the different (user) intents for mode intent dispatcher
enum {
  // change the selection using up and down arrows
  IDI_CHANGE_SELECTION=0,
  // execute the current selection using Spacebar
  IDI_EXECUTE
};
// the intent dispatcher editor mode
typedef struct SModeIntDisp {
  // inits with caller mode, hexel drawer and number of intents that will be added
  void init( SMode *pBase, SMode *pCaller, int numIntents ) {
	pBase->fnDisp_state = int_disp_disp_state;
	pBase->fnKybd_map = int_disp_map;
	pBase->type = MODE_INTENT_DISPATCHER;
	pBase->bReset = true;
	this->pBase = pBase;
	this->pCaller = pCaller;
	this->load_intents( pBase );
	this->ppIntents = (SIntent **) malloc( numIntents * sizeof( SIntent *) );
	this->maxIntents = numIntents;
	this->numIntents = 0;
	this->curSel = 0;
  };
  // loads the intent dispatch fns for this mode
  void load_intents( SMode *pBase ) {
	pBase->numIntents = 2;
	pBase->fnIntent_handler[IDI_CHANGE_SELECTION] = int_disp_change_sel;
	pBase->fnIntent_handler[IDI_EXECUTE] = int_disp_execute;
  };
  // does nothing if no more handlers can be added
  void add_intent( SIntent *pIntent ) {
	if( this->numIntents < this->maxIntents ) {
	  this->ppIntents[this->numIntents] = pIntent;
	  this->numIntents += 1;
	}
  }
  SMode* pBase;
  SMode *pCaller;
  int numIntents;
  int maxIntents;
  int curSel;
  bool bReset;
  wxRect Rect;
  SIntent **ppIntents;
} SModeIntDisp;
// allocs and inits a ptr on the heap and returns it
// caller has to free
SMode * new_int_disp( SMode *pCaller, int numIntents, int scrnW, int scrnH, wxFont *pFont ) {
  SMode *pBase = (SMode *) malloc( sizeof( SMode) );
  if (pBase != NULL) {
	  pBase->init(scrnW, scrnH, pFont);
	  SModeIntDisp* pIntDisp = (SModeIntDisp*)malloc(sizeof(SModeIntDisp));
	  if (pIntDisp != NULL) {
		  pBase->sExt.pIntDisp = pIntDisp;
		  pBase->sExt.pIntDisp->init(pBase, pCaller, numIntents);
	  }
  }
  return( pBase );
}
// free using free_mode
void free_int_disp( SMode *pMode ) {
	if (pMode != NULL) {
		if (pMode->sExt.pIntDisp != NULL) {
			if (pMode->sExt.pIntDisp->ppIntents != NULL) {
				for (int i = 0; i < pMode->sExt.pIntDisp->numIntents; i++) 
					free_intent(pMode->sExt.pIntDisp->ppIntents[i]);
				free(pMode->sExt.pIntDisp->ppIntents);
			}
			free(pMode->sExt.pIntDisp);
		}
		free(pMode);
	}
}

// mode :: kybd map
// called by the mode manager when the users inputs on the kybd
bool int_disp_map( SMode *pBase, wxKeyEvent &event, ModalWindow *pWin ) {
  bool bRetVal = true;
  SModeIntDisp *pIntDisp = pBase->sExt.pIntDisp;

  wxClientDC DC( pWin ); // dummy
  if( pBase->pFont != NULL ) {
    pBase->load_font();
    DC.SetFont( *(pBase->pFont) );
  }

  pBase->key = event.GetKeyCode();
  pBase->uniKey = event.GetUnicodeKey();

  // case exit, pop this Intent Dispatcher off the mode stack
  // refresh the window
  if( pBase->key == WXK_ESCAPE ) {
	pWin->m_pModeManager->pop();
	pWin->m_bUsrActn = false;
	pWin->Refresh( true );
  }
  // if up or down arrow dispatch to CHANGE_SELECTION
  // if return or space dispatch to execute
  else {
	if( pBase->key == WXK_UP  || pBase->key == WXK_DOWN ) {
	  pBase->intent = IDI_CHANGE_SELECTION;
	  pBase->fnIntent_handler[pBase->intent]( pBase, PH_NOTIFY, pWin, DC );
	}
	else if( pBase->key == WXK_RETURN || pBase->key == WXK_SPACE ) {
	  pBase->intent = IDI_EXECUTE;
	  pBase->fnIntent_handler[pBase->intent]( pBase, PH_NOTIFY, pWin, DC );
	}
  }
  pWin->m_bUsrActn = false;
  return( bRetVal );
}
// ancillary sets the display rect of this intent dispatcher
void int_disp_set_rect( SMode *pBase, wxDC& DC ) {
  SModeIntDisp *pIntDisp = pBase->sExt.pIntDisp;
  int widthMax = 0;
  int heightLine = 0;;

  if( pBase->pFont != NULL ) {
    pBase->load_font();
    DC.SetFont( *(pBase->pFont) );
  }
  // get the width of the widest intent string and the height of the strings
  for( int i=0; i<pIntDisp->numIntents; i++ ) {
	DC.GetTextExtent( wxString( pIntDisp->ppIntents[i]->ptlName->szBuf ), &(pIntDisp->Rect.width), &(pIntDisp->Rect.height) );
	if( pIntDisp->Rect.width > widthMax )
	  widthMax = pIntDisp->Rect.width;
  }
  heightLine = pIntDisp->Rect.height;
  pIntDisp->Rect.width = (int) (widthMax * 1.4);
  pIntDisp->Rect.height = heightLine * (pIntDisp->numIntents * 2 + 2);
  pIntDisp->Rect.x = pBase->scrnW/2 - pIntDisp->Rect.width/2;
  pIntDisp->Rect.y = pBase->scrnH/2 - pIntDisp->Rect.height/2;
}
// mode :: display current state
// called by the mode manager
void int_disp_disp_state( SMode *pBase, ModalWindow *pWin, wxDC& DC ) {
  // draw a stack of intent handler control strings
  // at the center of the screen
  // highlight the current selection
  SModeIntDisp *pIntDisp = pBase->sExt.pIntDisp;
  if(pBase->bReset) {
    if( pBase->pFont != NULL ) {
      pBase->load_font();
      DC.SetFont( *(pBase->pFont) );
    }
	  int_disp_set_rect(pBase, DC);
	  pBase->bReset = false;
  }
  // draw a bg-color bg rect
  wxPen Pen = DC.GetPen();
  wxBrush Brush = DC.GetBrush();
  DC.SetPen( *wxTRANSPARENT_PEN );
  DC.SetBrush( wxBrush( wxColour( 208, 208, 200 ) ) );
  DC.DrawRectangle( pIntDisp->Rect );
  DC.SetPen( Pen );
  DC.SetBrush( Brush );

  // draw a whitehighlight rect
  wxRect rectH;
  int heightLine = 0;
  DC.GetTextExtent( wxString( pIntDisp->ppIntents[pIntDisp->curSel]->ptlName->szBuf ), &(rectH.width), &heightLine );
  int linesStartY = ((pIntDisp->numIntents * 2 - 1) * heightLine)/2;
  rectH.width = (int) (rectH.width + 10);
  rectH.height = (int) (heightLine + 10);
  rectH.x = pBase->scrnW/2 - rectH.width/2;
  rectH.y = pBase->scrnH/2 - linesStartY + pIntDisp->curSel*2*heightLine - 5;
  Pen = DC.GetPen();
  Brush = DC.GetBrush();
  DC.SetPen( *wxTRANSPARENT_PEN );
  DC.SetBrush( *wxWHITE_BRUSH );
  DC.DrawRectangle( rectH );
  DC.SetPen( Pen );
  DC.SetBrush( Brush );

  wxRect rectLine;
  // draw the intent lines
  for( int i=0; i<pIntDisp->numIntents; i++ ) {
	DC.GetTextExtent( wxString( pIntDisp->ppIntents[i]->ptlName->szBuf), &rectLine.width, &rectLine.height );
	rectLine.x = pBase->scrnW/2 - rectLine.width/2;
	rectLine.y = pBase->scrnH/2 - linesStartY + i*2*rectLine.height;
	DC.DrawText( wxString( pIntDisp->ppIntents[i]->ptlName->szBuf ), rectLine.x, rectLine.y );
  }
}
// intent handler for CHANGE_SELECTION
// user wants to change the selection using up and down arrows
void int_disp_change_sel( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // change the current selection based on up or down arrow
  // determine the refresh rect for redrawing the entire mode
  // Refresh that rect in the window
  if( phase == PH_NOTIFY ) {
	SModeIntDisp * pIntDisp = pBase->sExt.pIntDisp;
	bool bUpdated = true;
	// change the current selection based on up or down arrow
	if( pBase->key == WXK_UP )
	  pIntDisp->curSel = (pIntDisp->curSel - 1 + pIntDisp->numIntents)%pIntDisp->numIntents;
	else if( pBase->key == WXK_DOWN )
	  pIntDisp->curSel = (pIntDisp->curSel + 1)%pIntDisp->numIntents;
	else
	  bUpdated = false;
	// determine the refresh rect for redrawing the entire mode
	// Refresh that rect in the window
	if( bUpdated ) {
	  pWin->m_bUsrActn = true;
	  pWin->RefreshRect( pIntDisp->Rect, true );
	}
  }
  else
	int_disp_disp_state( pBase, pWin, DC );
}
// intent handler for EXECUTE
// user wants to execute the selection handler using space-bar
void int_disp_execute( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // pop this intent dispatcher off the mode stack
  // notify the caller
  // do not issue a refresh
  if( phase == PH_NOTIFY ) {
	SModeIntDisp * pIntDisp = pBase->sExt.pIntDisp;
	int intent = pIntDisp->ppIntents[pIntDisp->curSel]->index;
	pWin->m_pModeManager->pop();
	pIntDisp->pCaller->fnIntent_handler[intent]( pIntDisp->pCaller, PH_NOTIFY, pWin, DC);
  }
}
// BLOCK: THIS APP'S PRIMARY MODE, THE SOURCE EDITOR 
// This block contains the definition for mode source editor
// a mode defined for editing a Modal source-code file
// We start our intent definitions for this mode with navigation related intents.
// Caret movement, (de)summarization of code section
// and the ability to open a browser to view a hyperlink in a comment
// and to jump to the definition of a symbol.
// Both these we club under gotos.
// Caret movement is done using the arrow keys and PgUp, PgDn
// Summarization is done using Ctrl-S
// Goto is using Ctrl-right arrow, Back is Ctrl-left arrow
// SUBBLOCK: BASE DEFINITIONS

// the different (user) intents for mode source editor
enum {
  // change the location of the caret using arrows or PgUp, PgDn
  SEI_UPDATE_CARET=0,
  // summarize or expand a section using Ctrl-S
  SEI_SUMMARIZE,
  // goto (or return from) the symbol at caret line using Ctrl-right/left
  SEI_GOTO,
  // edit a character using alphanumerics or Return or Backspace
  SEI_EDIT_CHAR,
  // start a selection using Shift-arrow
  SEI_START_SEL,
  // update a selection using Shift-arrow
  SEI_UPDATE_SEL,
  // unselect a selection by pressing an alphnumeric
  SEI_UN_SEL,
  // cut a selection using Ctrl-X
  SEI_CUT_SEL,
  // paste a selection using Ctrl-V
  SEI_PASTE_SEL,
  // undo the last edit operation using Ctrl-Z
  SEI_UNDO,
  // launch the mode level controls menu by pressing Ctrl
  SEI_CONTROL,
  // export the current codebase to a cpp file
  SEI_EXPORT,
  // save the current codebase and load a new one
  SEI_LOAD_NEW,
  // build the current codebase
  SEI_BUILD,
  // debug the current executable
  SEI_DEBUG,
  // agjust the fontsize
  SEI_ADJUST_FONTSIZE,
  // got a line in the codebase
  SEI_GOTO_LINE,
  // input the codefile to load
  SEI_INPUT_CODEFILE
};
// the source editor mode
typedef struct SModeSrcEdr {
  // inits with scrnW, scrnH
  // note that init does not set the codebase
  // this has to be done before the mode can be used
  void init( SMode *pBase ) {
    pBase->fnDisp_state = src_edr_disp_state;
    pBase->fnKybd_map = src_edr_map;
    pBase->fnKey_up = src_edr_key_up;
    pBase->fnSerialize = src_edr_serialize;
    pBase->fnOn_load = src_edr_on_load;
    pBase->type = MODE_SOURCE_EDITOR;
    pBase->bReset = true;
    this->pBase = pBase;
    this->load_intents( pBase );
    this->pLineInp = new_line_input( pBase->scrnW, pBase->scrnH, pBase->pFont );
    this->pMsg = new_msg( pBase->scrnW, pBase->scrnH, pBase->pFont );
    this->pFileSel = new_file_sel(pBase->scrnW, pBase->scrnH, pBase->pFont);
    this->pLevAdj = new_lev_adj(pBase->scrnW, pBase->scrnH, pBase->pFont);
    this->pCodeBase = NULL;
    this->fileOffset = 0;
    this->Caret.x = 0;
    this->Caret.y = 0;
    this->CaretPrev.x = 0;
    this->CaretPrev.y = 0;
    this->selStartX = -1;
    this->selEndX = -1;
    this->selStartY = -1;
    this->selEndY = -1;
    this->dispLines = -1;
    this->colMidStart = -1;
    this->colRightStart = -1;
    this->lineHeight = -1;
    this->txtHeight = -1;
    this->counterWidth = -1;
    this->pNavTrail = new_nav_trail();
    this->bSelectingX = false;
    this->bSelectingY = false;
	this->pMemDC = NULL;
  };
  void set_codebase( SCodeBase *pCodeBase ) {
    this->pCodeBase = pCodeBase;
  };

  // loads the intent dispatch fns for this mode
  void load_intents( SMode *pBase ) {
    pBase->numIntents = 20;
    pBase->fnIntent_handler[SEI_EDIT_CHAR] = src_edr_edit_char;
    pBase->fnIntent_handler[SEI_UPDATE_CARET] = src_edr_update_caret;
    pBase->fnIntent_handler[SEI_START_SEL] = src_edr_start_sel;
    pBase->fnIntent_handler[SEI_UPDATE_SEL] = src_edr_update_sel;
    pBase->fnIntent_handler[SEI_UN_SEL] = src_edr_un_sel;
    pBase->fnIntent_handler[SEI_CUT_SEL] = src_edr_cut_sel;
    pBase->fnIntent_handler[SEI_PASTE_SEL] = src_edr_update_sel;
    pBase->fnIntent_handler[SEI_UNDO] = src_edr_undo;
    pBase->fnIntent_handler[SEI_SUMMARIZE] = src_edr_summarize;
    pBase->fnIntent_handler[SEI_GOTO] = src_edr_goto;
    pBase->fnIntent_handler[SEI_CONTROL] = src_edr_control;
    pBase->fnIntent_handler[SEI_EXPORT] = src_edr_export;
    pBase->fnIntent_handler[SEI_LOAD_NEW] = src_edr_load_new;
    pBase->fnIntent_handler[SEI_BUILD] = src_edr_build;
    pBase->fnIntent_handler[SEI_DEBUG] = src_edr_debug;
    pBase->fnIntent_handler[SEI_ADJUST_FONTSIZE] = src_edr_adjust_fontsize;
    pBase->fnIntent_handler[SEI_INPUT_CODEFILE] = src_edr_input_codefile;
    pBase->sExt.pSrcEdr->pIntentDispatcher = new_int_disp( pBase, 5, pBase->scrnW, pBase->scrnH, pBase->pFont );
    // load the intent dispatcher for indirect-mapped intents
    SModeIntDisp *pIntDisp = this->pIntentDispatcher->sExt.pIntDisp;
    pIntDisp->add_intent( new_intent((char*)"Export source file", SEI_EXPORT ) );
    pIntDisp->add_intent( new_intent((char*)"Save and load new file", SEI_LOAD_NEW ) );
    pIntDisp->add_intent( new_intent((char*)"Build", SEI_BUILD ) );
    pIntDisp->add_intent( new_intent((char*)"Debug", SEI_DEBUG ) );
    pIntDisp->add_intent( new_intent((char*)"Adjust Fontsize", SEI_ADJUST_FONTSIZE ) );
  };
  SMode *pBase;
  SCodeBase *pCodeBase; // the CodeBase to be edited
  int fileOffset; // the file offset of the start of the screen
  wxPoint Caret;
  wxPoint CaretPrev; // caret location on the screen, limited to 0-dispLines
  int selStartX;
  int selEndX; // character range selection
  int selStartY;
  int selEndY; // line range selection
  int dispLines;
  int colMidStart;
  int colRightStart;
  int lineHeight;
  int txtHeight; 
  int counterWidth; // display params
  bool bSelectingX;
  bool bSelectingY;
  bool bCutBufLoaded; // selecting state
  SNavTrail *pNavTrail;
  SMode *pIntentDispatcher; // to handle non-direct-mapped intents
  SMode *pLineInp; // for getting line input from the user
  SMode *pMsg; // for displaying a message for the user
  SMode *pFileSel;
  SMode* pLevAdj;
  wxMemoryDC* pMemDC;
} SModeSrcEdr;
// allocs and inits a ptr on the heap and returns it
// caller has to free
SMode * new_src_edr( int scrnW, int scrnH, wxFont *pFont ) {
  SMode *pBase = (SMode *) malloc( sizeof( SMode) );
  if (pBase != NULL) {
	  pBase->init(scrnW, scrnH, pFont);
	  SModeSrcEdr* pSrcEdr = (SModeSrcEdr*)malloc(sizeof(SModeSrcEdr));
	  if (pSrcEdr != NULL) {
		  pBase->sExt.pSrcEdr = pSrcEdr;
		  pBase->sExt.pSrcEdr->init(pBase);
	  }
  }
  return( pBase );
}
// loads a SrcEdr extension from File
SModeSrcEdr * load_src_edr( SMode *pBase, wxFile &File ) {
  SModeSrcEdr *pSrcEdr = (SModeSrcEdr *) malloc( sizeof( SModeSrcEdr) );
  pSrcEdr->init( pBase );
  pBase->fnSerialize( pBase, File, false );
  return( pSrcEdr );
}

void free_src_edr(SMode* pMode) {
	free_line_input(pMode->sExt.pSrcEdr->pLineInp);
	free_msg(pMode->sExt.pSrcEdr->pMsg);
	free_int_disp(pMode->sExt.pSrcEdr->pIntentDispatcher);
	free_file_sel(pMode->sExt.pSrcEdr->pFileSel);
	free_lev_adj(pMode->sExt.pSrcEdr->pLevAdj);
	if (pMode->sExt.pSrcEdr->pCodeBase != NULL) {
		free_codebase(pMode->sExt.pSrcEdr->pCodeBase);
		pMode->sExt.pSrcEdr->pCodeBase = NULL;
	}
	free_nav_trail(pMode->sExt.pSrcEdr->pNavTrail);
	if (pMode->sExt.pSrcEdr->pMemDC != NULL)
		delete(pMode->sExt.pSrcEdr->pMemDC);
	free(pMode->sExt.pSrcEdr);
	free(pMode);
}
// SUBBLOCK: MODE IMPLEMENTATION FNS

// mode :: kybd map
// called by the mode manager when the users inputs on the kybd
bool src_edr_map( SMode *pBase, wxKeyEvent &event, ModalWindow *pWin ) {
  bool bRetVal = true;
  SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
  wxClientDC DC( pWin ); // dummy
  if( pBase->pFont != NULL ) {
    pBase->load_font();
    DC.SetFont( *(pBase->pFont) );
  }

  pBase->key = event.GetKeyCode();
  pBase->uniKey = event.GetUnicodeKey();

  if( pBase->key == WXK_ESCAPE )
	pWin->m_pOwner->Close(true);
    // map user kybd input to a user intent
	// non control down inputs
	// arrows or pgup/dn dispatch to UPDATE_CARET
	// ctrl set intent only, dispatch will happen on key-up
	// shift set bShiftDwon
	// control down inputs
	// Ctrl-S dispatch to SUMMARIZE
	// Ctrl-Right/Left dispatch to GOTO
  else {
	// control not down
	// arrows or pgup/dn dispatch to UPDATE_CARET
	// ctrl set intent only, dispatch will happen on key-up
	// shift set bShiftDwon
	if( !pBase->bCtrlDown ) {
	  // dispatch to UPDATE_CARET
	  if( pBase->key == WXK_UP || pBase->key == WXK_DOWN || pBase->key == WXK_RIGHT || pBase->key == WXK_LEFT || pBase->key == WXK_PAGEUP || pBase->key == WXK_PAGEDOWN ) {
		pBase->intent = SEI_UPDATE_CARET;
		pBase->fnIntent_handler[pBase->intent]( pBase, PH_NOTIFY, pWin, DC);
	  }
	  // store control down for next key procssing
	  // set intent = SEI_CONTROL
	  // do not dispatch, that will happen on keyup
	  else if( pBase->key == WXK_CONTROL ) {
		pBase->bCtrlDown = true;
		pBase->intent = SEI_CONTROL;
	  }
	  else if( pBase->key == WXK_SHIFT )
		pBase->bShiftDown = true;
	}
	// control down
	else {
	  // dispatch to SUMMARIZE
	  if( pBase->uniKey == 'S' ) {
		pBase->intent = SEI_SUMMARIZE;
		pBase->fnIntent_handler[pBase->intent]( pBase, PH_NOTIFY, pWin, DC);
	  }
	  // dispatch to GOTO
	  else if( pBase->key == WXK_RIGHT || pBase->key == WXK_LEFT ) {
		pBase->intent = SEI_GOTO;
		pBase->fnIntent_handler[pBase->intent]( pBase, PH_NOTIFY, pWin, DC);
	  }
	}
  } // end case not escape
  pWin->m_bUsrActn = false;
  return( bRetVal );
}
// mode :: key_up
// called by the mode manager when the users releases a key
bool src_edr_key_up( SMode *pBase, wxKeyEvent &event, ModalWindow *pWin ) {
	SModeSrcEdr* pSrcEdr = pBase->sExt.pSrcEdr;
  wxClientDC DC( pWin ); // dummy
  if( pBase->pFont != NULL ) {
    pBase->load_font();
    DC.SetFont( *(pBase->pFont) );
  }
	if( event.GetKeyCode() == WXK_SHIFT ) {
	pBase->bShiftDown = false;
  }
  // if intent == SEI_CONTROL, dispatch
  else if( event.GetKeyCode() == WXK_CONTROL ) {
	pBase->bCtrlDown = false;
	if( pBase->intent == SEI_CONTROL ) {
	  pBase->fnIntent_handler[pBase->intent]( pBase, PH_NOTIFY, pWin, DC);
	}
  }
  pWin->m_bUsrActn = false;
  return( true );
}
// mode :: on_load
// called by the mode manager when the mode is pushed onto it
// initializes the mode
// determines if pCodeBase has been set, if not, pushes a LineInput 
// to get the path of the codebase.
void src_edr_on_load(SMode* pBase, SModeManager *pManager ) {
  SModeSrcEdr* pSrcEdr = pBase->sExt.pSrcEdr;
  if (pSrcEdr->pCodeBase == NULL) {
	  pSrcEdr->pLineInp->sExt.pLineInput->set_caller(pBase, SEI_INPUT_CODEFILE, (char*)"enter the path of ModalXW.cpp");
	  pManager->push(pSrcEdr->pLineInp);
  }
  return;
}
// ancillary function used by src_edr_disp_state
wxColour get_element_colour( SCodeElement *pElem ) {
  int type = pElem->type;
  wxColour Colour;

  if( type == CDE_S_BLOCKSTART || type == CDE_BLOCK )
	Colour = wxColour( 128, 0, 0 );
  else if( type == CDE_S_SUBBLOCKSTART || type == CDE_SUBBLOCK )
	Colour = wxColour( 192, 128, 128 );
  else if( !pElem->bSingle && type != CDE_ENUM && type != CDE_COMMENT)
	Colour = wxColour( 0, 0, 96 );
  else if( type == CDE_S_COMMENT || type == CDE_S_FWDDECL_FN || type == CDE_S_FWDDECL_STR || type == CDE_ENUM || type == CDE_COMMENT )
	Colour = wxColour( 128, 128, 128 );
  else
	Colour = wxColour( 0, 0, 0 );

  return( Colour );
}
// mode :: display current state
// called by the mode manager
void src_edr_disp_state( SMode *pBase, ModalWindow *pWin, wxDC& DC ) {
  // Display the current state of the codebase
  // using a 3 column display format
  // with a 3-4-3 width division of the screen width
  // if a reset is needed, recompute display params
  if (pBase->sExt.pSrcEdr->pCodeBase != NULL) {
		SModeSrcEdr* pSrcEdr = pBase->sExt.pSrcEdr;
    pBase->load_font();
		DC.SetFont(*(pBase->pFont));
    SCodeElement *pElem = NULL;  
		STxtLine* pLine = NULL;


		// if a reset is needed recompute display parameters
		// determine the number of displayable lines for the screen size
		// determine line display parms such as txtHeight, lineHeight
		// width of the line counter
		// and set the locations of the 3 columns in the display
		if (pBase->bReset) {
			wxString strTemp("9999");
			int widthTxt;
			int heightTxt;
			DC.GetTextExtent(strTemp, &widthTxt, &heightTxt);
			pSrcEdr->txtHeight = heightTxt;
			pSrcEdr->lineHeight = heightTxt + 6;
			pSrcEdr->dispLines = (pBase->scrnH - 4) / pSrcEdr->lineHeight - 1;
			pSrcEdr->counterWidth = widthTxt + 10;

			pSrcEdr->colMidStart = (3 * pBase->scrnW) / 10;
			pSrcEdr->colRightStart = (7 * pBase->scrnW) / 10;

			pBase->bReset = false;
		}
		int firstLineOffset = pSrcEdr->lineHeight - pSrcEdr->txtHeight;

		// display the 3 columnar sections
		bool bEOF = false;
		int type;
		bool bEditable = true;
		int lineOffset;
		int skip;
		int x;
		wxString strCtr;
		int dispIndex = 0;
		pLine = NULL;

		pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, pSrcEdr->dispLines, &lineOffset);
		// check if the current file offset is overflowing the file length and trim it back
		// update caret.y to be at the same element as it was before the trim
		if (pSrcEdr->fileOffset + lineOffset + ce_length(pElem) > pSrcEdr->pCodeBase->pBaseSec->get_length()) {
			pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, pSrcEdr->Caret.y, &lineOffset);
			pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->pCodeBase->pBaseSec->get_length(), -pSrcEdr->dispLines, &lineOffset);
			pSrcEdr->fileOffset = pSrcEdr->pCodeBase->pBaseSec->get_length() + lineOffset;
			bool bFound = false;
			int i = 0;
			// find the new location of the element that was at Caret.y before the trim
			for (i = 0; i < pSrcEdr->dispLines && !bFound; i++)
				if (pElem == pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, i, &lineOffset))
					bFound = true;
			pSrcEdr->Caret.y = i - 1;
		}

		// first display the right columnar section
		x = pSrcEdr->colRightStart + 10;
		dispIndex = 0;
		// determine the offset of the first line of the right section
		// which is dispLines steps after fileOffset
		pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, pSrcEdr->dispLines, &skip);
		// if the first elem of the right sec is not beyond the end of the file display it
		// retrieve succesive elements (which may or may not summarized)
		// from the codebase based on the fileoffset of the right section
		// process the elemnt for the display, add a line counter
		if (pSrcEdr->fileOffset + skip + ce_length(pElem) < pSrcEdr->pCodeBase->pBaseSec->get_length()) {
			// retrieve and display (depending on type) the element at dispIndex
			// check if we've reached EOF in which case set bEOF
			while (dispIndex < pSrcEdr->dispLines && !bEOF) {
				pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset + skip, dispIndex, &lineOffset);

				// check if we've reached EOF in which case set bEOF
				if (pSrcEdr->fileOffset + skip + lineOffset + ce_length(pElem) >= pSrcEdr->pCodeBase->pBaseSec->get_length())
					bEOF = true;
				pLine = tl_clone(pElem->pLine);

				// if it's a block or sub-block start, edit it for display
				if (pElem->type == CDE_S_BLOCKSTART || pElem->type == CDE_S_SUBBLOCKSTART) {
					tl_remove(pLine, (char*)"// BLOCK: ");
					tl_remove(pLine, (char*)"// SUBBLOCK: ");
				}

				int lineDispWidth = tl_caret_loc(pLine, pLine->length, DC, pWin);
				// if the display length of the line is greater than the section's dispwidth, clip the line
				if (lineDispWidth > pBase->scrnW - pSrcEdr->colRightStart - pSrcEdr->counterWidth) {
					char* szTemp = tl_cut_out(pLine, pLine->length / 2, pLine->length);
					if (szTemp != NULL)
						free(szTemp);
					tl_insert(pLine, (char*)" ...", pLine->length);
				}

				// display the line
				type = pElem->type;
				bEditable = pElem->bSingle;
				wxColour ColourElem = get_element_colour(pElem);
				// display the element in its designated colour
				if (type != CDE_S_BLANK) {
					wxColour Colour = DC.GetTextForeground();
					DC.SetTextForeground(ColourElem);
					DC.DrawText(wxString(pLine->szBuf), x + pSrcEdr->counterWidth, dispIndex * pSrcEdr->lineHeight + firstLineOffset);
					DC.SetTextForeground(Colour);
				}

				// display the counter
				strCtr.Printf("%4d", pSrcEdr->fileOffset + skip + lineOffset + 1);
				wxColour Colour = DC.GetTextForeground();
				DC.SetTextForeground(wxColour(128, 128, 160));
				DC.DrawText(strCtr, x, dispIndex * pSrcEdr->lineHeight + firstLineOffset);
				DC.SetTextForeground(Colour);
				dispIndex++;
				tl_free(pLine);
				pLine = NULL;    
			}
		}

		// display the left section
		bEOF = false;
		x = 10;
		dispIndex = 0;

		// display the left section if there's something to display
		// determine the offset (skip back) of the first line of the left section
		// which is dispLines before fileOffset
		// display lines till we reach fileOffset
		if (pSrcEdr->fileOffset > 0) {
			// skip back
			pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, -pSrcEdr->dispLines, &skip);
			// display the lines in the left section
			// till we reach fileOffset
			if (pSrcEdr->fileOffset + skip >= 0) {
				dispIndex = 0;
				while (dispIndex < pSrcEdr->dispLines && !bEOF) {
					pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset + skip, dispIndex, &lineOffset);
					// check if we've displayed the last element in which case set bEOF to exit
					if (skip + lineOffset + ce_length(pElem) > 0)
						bEOF = true;
					// display the element depnding on its type
					else {
						pLine = tl_clone(pElem->pLine);

						// if it's a block or sub-block start, edit it for display
						if (pElem->type == CDE_S_BLOCKSTART || pElem->type == CDE_S_SUBBLOCKSTART) {
							tl_remove(pLine, (char*)"// BLOCK: ");
							tl_remove(pLine, (char*)"// SUBBLOCK: ");
						}

						int lineDispWidth = tl_caret_loc(pLine, pLine->length, DC, pWin);
						// if the display length of the line is greater than the section's dispwidth, clip the line
						if (lineDispWidth > pSrcEdr->colMidStart - pSrcEdr->counterWidth) {
							char* szTemp = tl_cut_out(pLine, pLine->length / 2, pLine->length);
							if (szTemp != NULL)
								free(szTemp);
							tl_insert(pLine, (char*)" ...", pLine->length);
						}

						// display the line
						type = pElem->type;
						bEditable = pElem->bSingle;
						wxColour ColourElem = get_element_colour(pElem);
						// display the element in its designated colour
						if (type != CDE_S_BLANK) {
							wxColour Colour = DC.GetTextForeground();
							DC.SetTextForeground(ColourElem);
							DC.DrawText(wxString(pLine->szBuf), x + pSrcEdr->counterWidth, dispIndex * pSrcEdr->lineHeight + firstLineOffset);
							DC.SetTextForeground(Colour);
						}

						// display the counter
						strCtr.Printf("%4d", pSrcEdr->fileOffset + skip + lineOffset + 1);
						wxColour Colour = DC.GetTextForeground();
						DC.SetTextForeground(wxColour(128, 128, 160));
						DC.DrawText(strCtr, x, dispIndex * pSrcEdr->lineHeight + firstLineOffset);
						DC.SetTextForeground(Colour);
						dispIndex++;
						tl_free(pLine);
						pLine = NULL;    
					} // end case there's something to display in the left section
				}
			}
		}

		// display the center section

		// clear out the bg in case the left section has overflowed into this one
		wxRect rect;
		rect.x = pSrcEdr->colMidStart;
		rect.y = 0;
		rect.width = pSrcEdr->colRightStart - rect.x;
		rect.height = pBase->scrnH;
		wxPen Pen = DC.GetPen();
		wxBrush Brush = DC.GetBrush();
		DC.SetPen(*wxTRANSPARENT_PEN);
		DC.SetBrush(wxBrush(wxColour(208, 208, 200)));
		DC.DrawRectangle(rect);
		DC.SetPen(Pen);
		DC.SetBrush(Brush);

		// if bSelecting, display the selection region in the bg
		// TODO
		if (pSrcEdr->bSelectingX) {

		}
		else if (pSrcEdr->bSelectingY) {

		}

		bEOF = false;
		x = pSrcEdr->colMidStart + 10;
		dispIndex = 0;
		// retrieve succesive elements from the codebase, 
		// draw based on type, draw the counter, check for EOF
		while (dispIndex < pSrcEdr->dispLines && !bEOF) {
			pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, dispIndex, &lineOffset);
			pLine = tl_clone(pElem->pLine);

			// if it's a block or sub-block start, edit it for display
			if (pElem->type == CDE_S_BLOCKSTART || pElem->type == CDE_S_SUBBLOCKSTART) {
				tl_remove(pLine, (char*)"// BLOCK: ");
				tl_remove(pLine, (char*)"// SUBBLOCK: ");
			}

			int lineDispWidth = tl_caret_loc(pLine, pLine->length, DC, pWin);
			// if the display length of the line is greater than the section's dispwidth, clip the line
			// center section elements are allowed to overflow into the right display column
			if (lineDispWidth > pBase->scrnW - pSrcEdr->colMidStart - pSrcEdr->counterWidth) {
				char* szTemp = tl_cut_out(pLine, pLine->length / 2, pLine->length);
				if (szTemp != NULL)
					free(szTemp);
				tl_insert(pLine, (char*)" ...", pLine->length);
			}

			type = pElem->type;
			bEditable = pElem->bSingle;
			wxColour ColourElem = get_element_colour(pElem);
			// display the element in its colour with a bg rect in bg color
			// the bg rect is for cases where the element overflows into the right display column
			if (type != CDE_S_BLANK) {
				wxRect rectBG;
				DC.GetTextExtent(wxString(pLine->szBuf), &rectBG.width, &rectBG.height);
				rectBG.x = x + pSrcEdr->counterWidth;
				rectBG.y = dispIndex * pSrcEdr->lineHeight + firstLineOffset;
				if (rectBG.width > pSrcEdr->colRightStart - pSrcEdr->colMidStart)
					rectBG.width = pBase->scrnW - rectBG.x - 1;
				wxPen Pen = DC.GetPen();
				wxBrush Brush = DC.GetBrush();
				DC.SetPen(*wxTRANSPARENT_PEN);
				DC.SetBrush(wxBrush(wxColour(208, 208, 200)));
				DC.DrawRectangle(rectBG);
				DC.SetPen(Pen);
				DC.SetBrush(Brush);

				wxColour Colour = DC.GetTextForeground();
				DC.SetTextForeground(ColourElem);
				DC.DrawText(wxString(pLine->szBuf), x + pSrcEdr->counterWidth, dispIndex * pSrcEdr->lineHeight + firstLineOffset);
				DC.SetTextForeground(Colour);
			}

			// display the counter
			strCtr.Printf("%4d", pSrcEdr->fileOffset + lineOffset + 1);
			wxColour Colour = DC.GetTextForeground();
			DC.SetTextForeground(wxColour(128, 128, 160));
			DC.DrawText(strCtr, x, dispIndex * pSrcEdr->lineHeight + firstLineOffset);
			DC.SetTextForeground(Colour);

			if (pSrcEdr->fileOffset + lineOffset + ce_length(pElem) >= pSrcEdr->pCodeBase->pBaseSec->get_length())
				bEOF = true;
			else
				dispIndex++;
			tl_free(pLine);
			pLine = NULL;    
		}

		// display the caret
		Pen = DC.GetPen();
		DC.SetPen(wxPen(wxColour(255, 0, 0)));
		pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, pSrcEdr->Caret.y, &lineOffset);
		int caretLoc;
		pLine = pElem->pLine;
		caretLoc = tl_caret_loc(pLine, pSrcEdr->Caret.x, DC, pWin);
		caretLoc += pSrcEdr->colMidStart + pSrcEdr->counterWidth + 10;
		DC.DrawLine(caretLoc,
			pSrcEdr->Caret.y * pSrcEdr->lineHeight,
			caretLoc,
			(pSrcEdr->Caret.y + 1) * pSrcEdr->lineHeight + firstLineOffset);
		DC.SetPen(Pen);
	}
  return;
}
// write to or load from File the state of this source editor
bool src_edr_serialize( SMode *pBase, wxFile &File, bool bToFrom ) {
  bool bRetVal = true;
  SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
  if( bToFrom ) {
	if( pSrcEdr->pCodeBase == NULL )
	  bRetVal = false;
	else {
	  pSrcEdr->pCodeBase->serialize( File, true );
	  File.Write( &(pSrcEdr->fileOffset), sizeof(int) );
	  File.Write( &(pSrcEdr->Caret.x), sizeof(int) );
	  File.Write( &(pSrcEdr->Caret.y), sizeof(int) );
	  File.Write( &(pSrcEdr->CaretPrev.x), sizeof(int) );
	  File.Write( &(pSrcEdr->CaretPrev.y), sizeof(int) );
	  File.Write( &(pSrcEdr->selStartX), sizeof(int) );
	  File.Write( &(pSrcEdr->selEndX), sizeof(int) );
	  File.Write( &(pSrcEdr->selStartY), sizeof(int) );
	  File.Write( &(pSrcEdr->selEndY), sizeof(int) );
	  File.Write( &(pSrcEdr->bSelectingX), sizeof(bool) );
	  File.Write( &(pSrcEdr->bSelectingY), sizeof(bool) );
	  File.Write( &(pSrcEdr->bCutBufLoaded), sizeof(bool) );
    pSrcEdr->pNavTrail->serialize(File, bToFrom);
	}
  }
  else {
    SCodeElement* pElem = load_code_element(File, NULL, 0);
    pSrcEdr->pCodeBase = pElem->pSec->pCodeBase;
    File.Read( &(pSrcEdr->fileOffset), sizeof(int) );
    File.Read( &(pSrcEdr->Caret.x), sizeof(int) );
    File.Read( &(pSrcEdr->Caret.y), sizeof(int) );
    File.Read( &(pSrcEdr->CaretPrev.x), sizeof(int) );
    File.Read( &(pSrcEdr->CaretPrev.y), sizeof(int) );
    File.Read( &(pSrcEdr->selStartX), sizeof(int) );
    File.Read( &(pSrcEdr->selEndX), sizeof(int) );
    File.Read( &(pSrcEdr->selStartY), sizeof(int) );
    File.Read( &(pSrcEdr->selEndY), sizeof(int) );
    File.Read( &(pSrcEdr->bSelectingX), sizeof(bool) );
    File.Read( &(pSrcEdr->bSelectingY), sizeof(bool) );
    File.Read( &(pSrcEdr->bCutBufLoaded), sizeof(bool) );
    pSrcEdr->pNavTrail->serialize(File, bToFrom);
  }
  return( bRetVal );
}
// SUBBLOCK: INTENT HANDLERS

// intent handler for UPDATE_CARET
// the user wants to navigate the codebase
// using the arrow keys, pgup and pgdn
void src_edr_update_caret( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
	static int caretLoc;
	static int caretLocPrev;
  SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
  SCodeElement *pElem = NULL;
  int chunkSize;

  // update the caret location
  // scroll the page if needed
  // determines update rect and generates a refresh
  if( phase == PH_NOTIFY ) {
	int key = pBase->key;
	bool bHandled = false;
	pSrcEdr->CaretPrev = pSrcEdr->Caret;
	switch( key ) {
	  // move caret up a line, scroll if we've reached the top
	  case WXK_UP: {
      if( pSrcEdr->Caret.y > 0 )
        pSrcEdr->Caret.y -= 1;
      // scroll up if Caret.y == 0
      else {
        pSrcEdr->Caret.y = 0;
        src_edr_scroll( pBase, true, pWin );
        bHandled = true;
      }
	  }
	  break;
	  // scroll up
	  case WXK_PAGEUP: {
      src_edr_scroll( pBase, true, pWin );
      bHandled = true;
	  }
	  break;
	  // move caret down a line, scroll down if we've reached the bottom
	  case WXK_DOWN: {
      if( pSrcEdr->Caret.y < pSrcEdr->dispLines-1 )
        pSrcEdr->Caret.y += 1;
      // scroll down if Caret.y is at the bottom of the page
      else {
        src_edr_scroll( pBase, false, pWin );
        bHandled = true;
      }
	  }
	  break;
	  // scroll down
	  case WXK_PAGEDOWN: {
		src_edr_scroll( pBase, false, pWin );
		bHandled = true;
	  }
	  break;
	  // move caret right a character
	  // if we've reached the end of the line
	  // goto beginning of next line
	  case WXK_RIGHT: {
		pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at( pSrcEdr->fileOffset, pSrcEdr->Caret.y, &chunkSize );
		if( pSrcEdr->Caret.x < pElem->pLine->length )
		  pSrcEdr->Caret.x += 1;
		// goto beginning of next line
		else {
		  pSrcEdr->Caret.x = 0;
		  pSrcEdr->Caret.y += 1;
		  // scroll down if we've reached the last line
		  if( pSrcEdr->Caret.y == pSrcEdr->dispLines ) {
			pSrcEdr->Caret.y = pSrcEdr->dispLines - 1;
			src_edr_scroll( pBase, false, pWin );
			bHandled = true;
		  }
		}
	  }
	  break;
	  // move caret left a character
	  // if we've reached the beginning of the line
	  // goto end of previous line
	  case WXK_LEFT: {
		if( pSrcEdr->Caret.x > 0 )
		  pSrcEdr->Caret.x -= 1;
		// goto end of previous line
		else {
		  pSrcEdr->pCodeBase->pBaseSec->get_element_at( pSrcEdr->fileOffset, pSrcEdr->Caret.y, &chunkSize );
		  if( pSrcEdr->fileOffset + chunkSize == 0 )
			bHandled = true;
		  // goto end of previous line
		  else {
			pSrcEdr->Caret.y -= 1;
			// scroll up if we're at the first line in the page
			if( pSrcEdr->Caret.y < 0 ) {
			  pSrcEdr->Caret.y = 0;
			  src_edr_scroll( pBase, true, pWin );
			  bHandled = true;
			}
			// goto end of previous line
			else {
			  pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at( pSrcEdr->fileOffset, pSrcEdr->CaretPrev.y, &chunkSize );
			  pSrcEdr->Caret.x = pElem->pLine->length;
			}
		  }
		}
	  } // end case LEFT
	  break;
	  default:
		wxLogError("invalid key in src_edr_update_caret");
	  break;
	}
	// if not scrolling refresh the rects for caretPrev and caret
	if( !bHandled ) {
	  wxRect rect;
	  pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at( pSrcEdr->fileOffset, pSrcEdr->Caret.y, &chunkSize );
	  caretLoc = tl_caret_loc( pElem->pLine, pSrcEdr->Caret.x, DC, pWin );
	  pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at( pSrcEdr->fileOffset, pSrcEdr->CaretPrev.y, &chunkSize );
	  caretLocPrev = tl_caret_loc( pElem->pLine, pSrcEdr->CaretPrev.x, DC, pWin );

	  rect.x = pSrcEdr->colMidStart + pSrcEdr->counterWidth + 10 + caretLoc;
	  rect.y = pSrcEdr->Caret.y * pSrcEdr->lineHeight;
	  rect.width = 1;
	  rect.height = pSrcEdr->lineHeight * 2 - pSrcEdr->txtHeight;
	  pWin->Refresh( true, &rect );
	  rect.x = pSrcEdr->colMidStart + pSrcEdr->counterWidth + 10 + caretLocPrev;
	  rect.y = pSrcEdr->CaretPrev.y * pSrcEdr->lineHeight;
	  pWin->Refresh( true, &rect );
	  pWin->Update();
	}
  }
  // called by the OS to paint
  // draw a background color line at caretPrev and a fg color line at Caret
  else { // PH_EXEC
	wxPen Pen = DC.GetPen();
	DC.SetPen( wxPen( wxColour( 208, 208, 200 ) ) );
	DC.DrawLine(
	  caretLocPrev + pSrcEdr->colMidStart + 10 + pSrcEdr->counterWidth,
	  pSrcEdr->CaretPrev.y * pSrcEdr->lineHeight,
	  caretLocPrev + pSrcEdr->colMidStart + 10 + pSrcEdr->counterWidth,
	  pSrcEdr->CaretPrev.y*pSrcEdr->lineHeight + 2*pSrcEdr->lineHeight - pSrcEdr->txtHeight );
	DC.SetPen( wxPen( wxColour( 255, 0, 0 ) ) );
	DC.DrawLine(
	  caretLoc + pSrcEdr->colMidStart + 10 + pSrcEdr->counterWidth,
	  pSrcEdr->Caret.y * pSrcEdr->lineHeight,
	  caretLoc + pSrcEdr->colMidStart + 10 + pSrcEdr->counterWidth,
	  pSrcEdr->Caret.y*pSrcEdr->lineHeight + 2*pSrcEdr->lineHeight - pSrcEdr->txtHeight );
	DC.SetPen( Pen );
  }
}
// intent handler for SUMMARIZE
// user wants to summarize or expand a section using Ctrl-S
// to summarize or unsummarize a section
// the caret location has to be first line of the section
// if the caret location is any other line,
// it causes a collapse of the the section that contains that line
void src_edr_summarize( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // if the code element at the current caret location is a single
  // if it is the starting element of it's parent section
  // summarize the parent section
  // else, its a section, implies it is summarized, desummarize it
  // refresh the window
  if( phase == PH_NOTIFY ) {
	SModeSrcEdr * pSrcEdr = pBase->sExt.pSrcEdr;
	int lineOffset;
	SCodeElement * pElem;
	pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at( pSrcEdr->fileOffset, pSrcEdr->Caret.y, &lineOffset );
	// if it's not the starting element, collapse  the element
	if( pElem->bSingle ) {
	  // if starting element of its container summarize the container
	  if( pElem->indexContainer == 0 )
		pElem->pContainer->bSummarized = true;
	}
	else
	  pElem->pSec->bSummarized = false;

	pWin->m_bUsrActn = false;
	pWin->Refresh( true );
  }
}
// ancillary
// verifies that the var type is a class or struct.
// if it is, returns the class or sturct symbol ptr in ppCLass or ppStruct
// pSymSet is an input used for the search
bool verify_var_isstruct( STxtLine *pVarType, SSymbolSet *pSymSet, SClass **ppClass, SStruct** ppStruct ) {
  bool bFound = false;
  if( pVarType != NULL ) {
    for( int i=0; i<pSymSet->pStructSet->numStructs &&!bFound; i++ ) 
      if( tl_equals( pVarType, pSymSet->pStructSet->ppStructs[i]->pName) ) {
        *ppStruct = pSymSet->pStructSet->ppStructs[i];
        bFound = true;
      }
    for( int i=0; i<pSymSet->pClassSet->numClasses &&!bFound; i++ ) 
      if( tl_equals( pVarType, pSymSet->pClassSet->ppClasses[i]->pName) ) {
        *ppClass = pSymSet->pClassSet->ppClasses[i];
        bFound = true;
      }
  }
  return( bFound );
}
// ancillary
// finds a fn or var dereferenced from a class or struct
// if found, returns true and its location in *ppLocation  
// whether var of func in *pbVarFunc
bool find_deref_varfunc( STxtLine *pLine, SVarSet *pVarSetContext, SClass* pClass, SStruct *pStruct, SSymbolSet *pSymSet, SLocation** ppLocation, bool *pbVarFunc) {
  // we find the first "word before deref"
  // and verify it's a class or struct
  // we remove the found word and deref from pLine 
  // then call this function again with a new context (the found class or struct)
  // if we don't find word before deref, then we look for a terminal var or func.  
  // we maintain a static callCount in this fn
  // to ensure that has been called at least once before a terminal condition occurs
  static int callCount = 0; 
  bool bRetVal = false;
  bool bFound = false;
  // pLine will be in the format
  // <Class/Struct Var Name><deref sym><Class/Struct member var name><deref sym>...<fn name>(...);/<var name>;
  // where deref sym is -> or .
  // We parse left to right
  int numDerefChars = 0;
  tl_trim( pLine );
  STxtLine *pTemp = tl_before_first_deref( pLine );
  // if pLine starts with a this->, just remove it , get the next word and keep going
  if( pTemp != NULL ) 
    if( tl_equals_sz( pTemp, (char *) "this") ) {
      tl_free( pTemp );
      pTemp = tl_before_first_deref( pLine );
      callCount++;
    }
  // if we got a word
  // it might contain the class/struct varname 
  // check if it exists in the current context
  // and keep going
  // else if it's a terminal (var or func)
  // and the callCount > 0 which means there was something before it
  // return true, else return flase
  if( pTemp != NULL ) {
    STxtLine *pVarType = NULL;
    SVar *pVar = pVarSetContext->get_var( pTemp );
    // remove any *s and &s
    if( pVar != NULL ) {
      pVarType = tl_clone( pVar->pTypeName );
      tl_remove( pVarType, (char*) "*" );
      tl_trim(pVarType);
      tl_remove(pVarType, (char*) "*" );
      tl_trim(pVarType);
      tl_remove(pVarType, (char*) "&" );
      tl_trim(pVarType);
    }      
    // find struct or class symbol for the returned var type
    // find vars or funcs in the symbol L to R till EOL
    // (for each var found verify deref sym)
    // or till a func is found
    SClass *pClassThis = NULL;
    SStruct *pStructThis = NULL;
    bFound = verify_var_isstruct( pVarType, pSymSet, &pClassThis, &pStructThis );
    tl_free( pVarType );
    pVarType = NULL;    
    // find vars or funcs in the symbol L to R till EOL
    // (for each var found verify deref sym)
    // or till a terminal var/func is found or not
    if( bFound ) {
      bool bContinue = true;
      bool bVarFunc = true;
      // extract the next deref
      SVarSet *pVarSetContext = NULL;
      if( pStructThis != NULL )
        pVarSetContext = pStructThis->pVarSet;
      else
        pVarSetContext = pClassThis->pVarSet;
      callCount++;
      bFound = find_deref_varfunc( pLine, pVarSetContext, pClassThis, pStructThis, pSymSet, ppLocation, &bVarFunc);
      callCount--;
    }
  }
  // deref not found, check for terminal var of func
  else {
    if( callCount > 0 ) {
      char cSeparator;
      STxtLine *pLast = tl_extract_word( pLine, &cSeparator );
      if( pLast != NULL ) {
        // terminal var
        // find the var in the current class or struct
        // and set its location in the return
        SVarSet *pVarSet;
        if( pStruct != NULL )
          pVarSet = pStruct->pVarSet;
        else
          pVarSet = pClass->pVarSet;
        SVar* pVar = pVarSet->get_var( pLast );  
        if( pVar != NULL ) {
          bFound = true;
          *pbVarFunc = true;
          *ppLocation = pVar->pLocation;
        }
        else {
          // find the func in the current class or struct
          // and set its location in the return
          SFuncSet *pFuncSet;
          if( pStruct != NULL )
            pFuncSet = pStruct->pFuncSet;
          else
            pFuncSet = pClass->pFuncSet;
          SSymFunc* pFunc = pFuncSet->get_func( pLast );  
          if( pFunc != NULL ) {
            bFound = true;
            *pbVarFunc = false;
            *ppLocation = pFunc->pLocation;
          }
          else
            bFound = false;
        }
      }
	  tl_free(pLast);
    }
  }
  tl_free(pTemp);
  return( bFound );
}
// ancillary
// gets the code element associated with the users request
// request is in terms of the code line at caret.y, and caret.x
// the symbol set has info about element location
SLocation* get_requested_element(SCodeElement *pElem, int caretX, SSymbolSet *pSymSet)
{
  SLocation* pRetVal = NULL;
  STxtLine *pLine = tl_clone( pElem->pLine ); 
  STxtLine *pWord = tl_get_word_at(pLine, caretX);
  if( pWord != NULL ) {
    bool bFound = false;
    // check for struct or class
    for( int i=0; i<pSymSet->pStructSet->numStructs &&!bFound; i++ )
      if( tl_equals( pWord, pSymSet->pStructSet->ppStructs[i]->pName ) ) {
        pRetVal = pSymSet->pStructSet->ppStructs[i]->pLocation;
        bFound = true;
      }
    for( int i=0; i<pSymSet->pClassSet->numClasses &&!bFound; i++ )
      if( tl_equals( pWord, pSymSet->pClassSet->ppClasses[i]->pName ) ) {
        pRetVal = pSymSet->pClassSet->ppClasses[i]->pLocation;
        bFound = true;
        // check for new <ClassName> format constructor
        int indexNew = tl_find( pLine, (char*) "new" );
        if( indexNew != -1 ) 
          pRetVal = pSymSet->pClassSet->ppClasses[i]->pConstr->pLocation;
      }
    // check for fn contained in a class
    if( !bFound ) {
      if( pElem->pContainer->pBaseElem->type == CDE_CLASSDECL ) {
        SClass *pClass = pSymSet->pClassSet->ppClasses[pElem->pContainer->symLinkIndex];
        wxASSERT( pClass != NULL );
        for( int i=0; i<pClass->pFuncSet->numFuncs &&!bFound; i++ )
          if( tl_equals( pWord, pClass->pFuncSet->ppFuncs[i]->pName ) ) {
            pRetVal = pClass->pFuncSet->ppFuncs[i]->pLocation;
            bFound = true;
          }
      }
    }
    // check for fn contained in a class/struct calling a sibling fn
    if( !bFound ) {
      int type = pElem->pContainer->pBaseElem->pContainer->pBaseElem->type; 
      SFuncSet *pFuncSet = NULL;
      if( type == CDE_CLASSDECL ) 
        pFuncSet = pSymSet->pClassSet->ppClasses[ pElem->pContainer->pBaseElem->pContainer->symLinkIndex]->pFuncSet;
      if( type == CDE_TYPEDEF ) 
        pFuncSet = pSymSet->pStructSet->ppStructs[ pElem->pContainer->pBaseElem->pContainer->symLinkIndex]->pFuncSet;
      if( pFuncSet != NULL ) {
        STxtLine *pTemp = tl_before_first_deref( pWord );
        if( pTemp == NULL || tl_equals_sz(pTemp, (char*) "this") ) {
          for( int i=0; i<pFuncSet->numFuncs; i++ ) 
            if( tl_equals( pWord, pFuncSet->ppFuncs[i]->pName ) ) {
              pRetVal = pFuncSet->ppFuncs[i]->pLocation;
              bFound = true;          
            }
        }
        tl_free( pTemp );
      }
    }
    // check for fn dereferenced from a class/struct var
    if( !bFound ) {
      bool bVarFunc = true;
      SVarSet *pVarSetContext = new_var_set();
      // create the var set for the cur context.
      // there are 2 possibilities here.
      // 1. pElem is contained in a fn or sub-func(L1,L2 etc) contained in a class or struct
      // and has the form this->...->var/func
      // 2. pElem is conatined in a fn or subfn conatined in a standalone fn
      // in both cases
      // add the vars for this code element and it container
      // recusivley till the container is a CDE_FNDEFN or a CDE_CLASS_FNDEFN
      // then add the vars for the CDE_FNDEFN or CDE_CLASS_FNDEFN
	  // if its a CDE_CLASS_DEFN, add the varset of its class
      // first case 1 or case 2 has to be detrmined.
      int nCase = 0;
      int type = -1;
      SCodeSection *pThis = pElem->pContainer;
      type = pThis->pBaseElem->type;
      while( type != CDE_FNDEFN && type != CDE_CLASS_FNDEFN ) {
        pThis = pThis->pBaseElem->pContainer;
        type = pThis->pBaseElem->type;
      }
      // we know it's a fndefn.
      // check if it's contained in a class or struct
      pThis = pThis->pBaseElem->pContainer;
      type = pThis->pBaseElem->type;
      if( type == CDE_TYPEDEF || type == CDE_CLASSDECL ) 
        nCase = 1;
      else
        nCase = 2;
        
      if( nCase == 1 ) {
        pThis = pElem->pContainer;
        while( pThis->pBaseElem->type != CDE_TYPEDEF && pThis->pBaseElem->type != CDE_CLASSDECL ) {
          for( int i=0; i<pThis->pVarSet->numVars; i++ )
            pVarSetContext->add_var(pThis->pVarSet->ppVars[i]);
          pThis = pThis->pBaseElem->pContainer;
        }
        SVarSet *pVarSet = NULL;
        if( pThis->pBaseElem->type == CDE_TYPEDEF ) 
          pVarSet = pSymSet->pStructSet->ppStructs[pThis->symLinkIndex]->pVarSet; 
        else
          pVarSet = pSymSet->pClassSet->ppClasses[pThis->symLinkIndex]->pVarSet;         
        for( int i=0; i<pVarSet->numVars; i++ )
          pVarSetContext->add_var(pVarSet->ppVars[i]);        
      }
      else {
        pThis = pElem->pContainer;
        while( pThis->pBaseElem->type != CDE_FNDEFN && pThis->pBaseElem->type != CDE_CLASS_FNDEFN ) {
          for( int i=0; i<pThis->pVarSet->numVars; i++ )
            pVarSetContext->add_var(pThis->pVarSet->ppVars[i]);
          pThis = pThis->pBaseElem->pContainer;
        }
        for( int i=0; i<pThis->pVarSet->numVars; i++ )
          pVarSetContext->add_var(pThis->pVarSet->ppVars[i]);
        if (pThis->pBaseElem->type == CDE_CLASS_FNDEFN) {
          SClass* pClass = pSymSet->pClassSet->ppClasses[pThis->symLinkIndex];
          for (int i = 0; i < pClass->pVarSet->numVars; i++)
            pVarSetContext->add_var(pClass->pVarSet->ppVars[i]);
        }
      }
      bFound = find_deref_varfunc( pWord, pVarSetContext, NULL, NULL, pSymSet, &pRetVal, &bVarFunc );  
	  free(pVarSetContext->ppVars);
      free( pVarSetContext );
    }
    // check for fn (not contained in a class/struct)
    if( !bFound ) 
      for( int i=0; i<pSymSet->pFuncSet->numFuncs &&!bFound; i++ )
        if( tl_equals( pWord, pSymSet->pFuncSet->ppFuncs[i]->pName ) ) {
          pRetVal = pSymSet->pFuncSet->ppFuncs[i]->pLocation;
          bFound = true;
        }
  }
  tl_free( pLine );
  pLine = NULL;    
  tl_free( pWord );
  pWord = NULL;    
  return(pRetVal);
}
// intent handler for GOTO
// user wants to goto a hyperlink on a comment line
// or to a symbol on a codeline using Ctrl-RArrow
// or return from a prior goto usint Ctrl-LArrow
void src_edr_goto( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // process goto or backto
  if( phase == PH_NOTIFY ) {
	SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
	// it's a goto
	// check the line at Caret.y
	// for an http
	// else for a symbol
	// if neither is found, pop-up a goto line-Input
	if( pBase->key == WXK_RIGHT ) {
	  // get the line at the caret location
	  // if it has for http
	  // parse out the URL
	  // launch the default browser
	  int dummy = 0;
	  SCodeElement *pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, pSrcEdr->Caret.y, &dummy );
	  STxtLine *pLine = tl_clone( pElem->pLine );
	  tl_trim( pLine );

	  // look for an http
	  if( tl_find( pLine, (char*)"http" ) != -1 ) {
		  tl_remove( pLine, (char*)"// See ");
		  pBase->bCtrlDown = false;
		  wxLaunchDefaultBrowser(	wxString( pLine->szBuf ) );
		  tl_free(pLine);
		  pLine = NULL;
	  }
	  // look for a symbol else popup a line input
	  else {
		  tl_free(pLine);
		  pLine = NULL;
		  SLocation* pLocation = NULL;
		  pLocation = get_requested_element(pElem, pSrcEdr->Caret.x, pSrcEdr->pCodeBase->pSymSet);
		  // if a symbol was found, set fileoffset, collapse current, expand goto and refresh
		  if (pLocation != NULL) {
			  if (pLocation->pCodeBaseLoc != NULL) {
				  int fileOffset = 0;
				  fileOffset = pLocation->fileOffset;
				  int lineOffset = 0;
				  SCodeElement* pElem = NULL;
				  // get current element (at caret.y) and collapse it
				  pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, pSrcEdr->Caret.y, &lineOffset);
				  // add the current location to the nav trail
				  SLocation* pCurLoc = new_location(pElem, pSrcEdr->fileOffset + lineOffset);
				  pSrcEdr->pNavTrail->add_step(pCurLoc, pSrcEdr->Caret.y);
				  pElem = ce_collapse(pElem);
				  lineOffset = -1; // just to make it !=0
				  // get the elem at fileOffset, if it's summarized, unsummarize it, repeat
				  // until you get an unsummarized element
				  while (lineOffset != 0) {
					  pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &lineOffset);
					  if (!pElem->bSingle && lineOffset != 0)
						  pElem->pSec->bSummarized = false;
				  }
				  if (!pElem->bSingle)
					  pElem->pSec->bSummarized = false;

				  // expand the goto element
				  ce_expand(pElem);
				  // get the element again in it's expanded state
				  pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(fileOffset, 0, &lineOffset);
				  // place the element at the center by walking back dispLines/2 steps;
				  pSrcEdr->pCodeBase->pBaseSec->get_element_at(fileOffset, -pSrcEdr->dispLines / 2, &lineOffset);
				  pSrcEdr->fileOffset = fileOffset + lineOffset;
				  pSrcEdr->Caret.x = 0;

				  // in some cases, the element may not be at the center and under the caret
				  // account for such cases
				  SCodeElement* pElemTemp = NULL;
				  pElemTemp = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, pSrcEdr->dispLines / 2, &lineOffset);
				  if (tl_equals(pElemTemp->pLine, pElem->pLine))
					  pSrcEdr->Caret.y = pSrcEdr->dispLines / 2;
				  // find the location of pElem and set Caret.y
				  else {
					  bool bFound = false;
					  for (int i = 0; i <= pSrcEdr->dispLines / 2 && !bFound; i++) {
						  pElemTemp = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, i, &lineOffset);
						  if (tl_equals(pElemTemp->pLine, pElem->pLine)) {
							  bFound = true;
							  pSrcEdr->Caret.y = i;
						  }
					  }
					  wxASSERT(bFound);
				  }
				  pWin->m_bUsrActn = false;
				  pWin->Refresh(true);
			  }
			  // if a symbol was not found pop up a line input for goto
			  else {
				  // if it's not popped up, pop-up the line input for the destination of goto
				  if (!pSrcEdr->pLineInp->sExt.pLineInput->bInputRcvd) {
					  pBase->bCtrlDown = false; // so this mode is not confused on return
					  pSrcEdr->pLineInp->sExt.pLineInput->set_caller(pBase, SEI_GOTO, (char*)"goto?");
					  pWin->m_pModeManager->push(pSrcEdr->pLineInp);
				  }
				  // callback from the line-input pop-up with the goto data
				  // if it's a valid fileOffset number, collapse current, expand goto number refresh
				  else {
					  wxString strLineNum = wxString(pSrcEdr->pLineInp->sExt.pLineInput->pInput->szBuf);
					  long lVal;
					  strLineNum.ToLong(&lVal);
					  // if it's a valid fileOffset number, collapse current, expand goto number refresh
					  if (lVal >= 0 && lVal < ce_length(pSrcEdr->pCodeBase->pBaseSec->pBaseElem) - 1) {
						  int lineOffset = 0;
						  SCodeElement* pElem;
						  // get current element (at caret.y) and collapse it
						  pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, pSrcEdr->Caret.y, &lineOffset);
						  // add the current location to the nav trail
						  SLocation* pCurLoc = new_location(pElem, pSrcEdr->fileOffset + lineOffset);
						  pSrcEdr->pNavTrail->add_step(pCurLoc, pSrcEdr->Caret.y);
						  pElem = ce_collapse(pElem);
						  lineOffset = -1; // just to make it !=0
						  // get the elem at lLval, if it's summarized, unsummarize it, repeat
						  // until you get an unsummarized element
						  while (lineOffset != 0) {
							  pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at((int)lVal, 0, &lineOffset);
							  if (!pElem->bSingle && lineOffset != 0)
								  pElem->pSec->bSummarized = false;
						  }
						  if (!pElem->bSingle)
							  pElem->pSec->bSummarized = false;

						  // expand the goto element
						  ce_expand(pElem);
						  // place the element at the center by walking back dispLines/2 steps;
						  pSrcEdr->fileOffset = (int)lVal + lineOffset;
						  pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, -pSrcEdr->dispLines / 2, &lineOffset);
						  pSrcEdr->fileOffset += lineOffset;
						  pSrcEdr->Caret.y = pSrcEdr->dispLines / 2;
						  pSrcEdr->pLineInp->sExt.pLineInput->bInputRcvd = false;
					  }
					  // check if it's a some other valid goto input
					  else {
						  // TODO extend the possible inputs for the goto line-input
					  }
				  }
				  pWin->m_bUsrActn = false;
				  pWin->Refresh(true);
			  }
		  }
	  }
	} // end case goto
	// it's a back to
	// get the back to location from the nav trail
	// collapse the current location
	// expand the backTo location
	// set fileOffset and caretY for the back to location
	// refresh
	else {
	  // collapse the current location
	  // expand the backTo location
	  // set fileOffset and caretY for the back to location
	  // reset the backTo location
	  // refresh
    int caretLoc = -1; 
    SLocation *pBackToLoc = pSrcEdr->pNavTrail->remove_step( &caretLoc ); 
	  if( pBackToLoc != NULL ) {
      SCodeElement *pElem = NULL;
      int lineOffset = 0;
      // collapse the current location
      pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at( pSrcEdr->fileOffset, pSrcEdr->Caret.y, &lineOffset );
      pElem = ce_collapse( pElem );
      lineOffset = -1;
      // expand the backTo location
      while( lineOffset != 0 ) {
        pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at( pBackToLoc->fileOffset, 0, &lineOffset );
        if( lineOffset != 0 )
          pElem->pSec->bSummarized = false;
      }
      // set fileOffset and caretY for the back to location
      pSrcEdr->pCodeBase->pBaseSec->get_element_at( pBackToLoc->fileOffset, -caretLoc, &lineOffset );
      pSrcEdr->fileOffset = pBackToLoc->fileOffset + lineOffset;
      pSrcEdr->Caret.y = caretLoc;
      free_location( pBackToLoc );
      pBackToLoc = NULL;
      // refresh
      pWin->m_bUsrActn = false;
      pWin->Refresh( true );
	  }
	}
  }
}
// intent handler for EDIT_CHAR, not wired yet
// user wants to edit chracter by character
// by inputting an alphanumeric, return, spacebar or backspace
// with the conventional mappings for these operations
void src_edr_edit_char( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
	static int refreshLines;
	static int startLine;
  int chunkSize;
  bool bEditable = true;
  SCodeElement *pElem;
  // if the current element belongs to an editable section
  // check for backspace, delete the character before caret.x
  // check for return, add a new line at the current caret.x
  // if it's an alphanumberic, add the char to the current line
  // determine update rect based on mods made and generate a refresh
  if( phase == PH_NOTIFY ) {
	SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
	SOperation Op;
	Op.type = 0;
	pSrcEdr->CaretPrev = pSrcEdr->Caret;
	SCodeElement *pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at( pSrcEdr->fileOffset, pSrcEdr->Caret.y, &chunkSize );
	// if it's a section it's editable if not summarized
	if( !(pElem->bSingle) )
	  bEditable = !(pElem->pSec->bSummarized);
	// check for backspace, delete the character before caret.x
	// check for return, add a new line at the current caret.x
	// if it's an alphanumberic, add the char to the current line
	// determine update rect based on mods made and generate a refresh
	if( bEditable ) {
	  // delete the character before the caret location
	  // if at the beginning of line cut the line and merge with previous line if possible
	  if( pBase->key == WXK_BACK ) {
		if( pSrcEdr->Caret.x > 0 ) {
		  // create a charedit delete operation
		  op_edit_char_init( Op, pSrcEdr->fileOffset, pSrcEdr->Caret.y, pBase->uniKey, pBase->key, pSrcEdr->Caret.x-1, false );
		  // add the operation to the codebase's oplist for a possible undo later,
		  pSrcEdr->pCodeBase->OpList.add( Op );
		  // execute the operation
		  pSrcEdr->pCodeBase->do_edit();
		  // update the caret
		  pSrcEdr->Caret.x -= 1;
		}
		else {
		  // cut the line and merge with previous line if possible
		}
	  }
	  // create a new line with the line segment after current caret location
	  else if( pBase->key == WXK_RETURN ) {
		// create a charedit insert operation
		op_edit_char_init( Op, pSrcEdr->fileOffset, pSrcEdr->Caret.y, pBase->uniKey, pBase->key, pSrcEdr->Caret.x, true);
		// add the operation to the codebase's oplist,
		pSrcEdr->pCodeBase->OpList.add( Op );
		// create a new line with the line segment after current caret location
		pSrcEdr->Caret.y += 1;
		pSrcEdr->Caret.x = 0;
		pWin->m_bUsrActn = false;
		pWin->Refresh( true );
	  }
	  // insert the entered char at the caret location
	  else {
		// create a charedit insert operation
		op_edit_char_init( Op, pSrcEdr->fileOffset, pSrcEdr->Caret.y, pBase->uniKey, pBase->key, pSrcEdr->Caret.x, true);
		// add the operation to the codebase's oplist,
		pSrcEdr->pCodeBase->OpList.add( Op );
		// insert the entered char at the caret location
		pSrcEdr->Caret.x += 1;
	  }
	  // determine startLine and number of refresh line for the refresh
	  if( pSrcEdr->Caret.y == pSrcEdr->CaretPrev.y ) {
		refreshLines = 1;
		startLine = pSrcEdr->Caret.y;
	  }
	  else if( pSrcEdr->Caret.y > pSrcEdr->CaretPrev.y ) {
		refreshLines = 2;
		startLine = pSrcEdr->CaretPrev.y;
	  }
	  else {
		refreshLines = 2;
		startLine = pSrcEdr->Caret.y;
	  }
	  // create the refresh rect and call refresh
	  wxRect rect;
	  rect.x = pSrcEdr->colMidStart + pSrcEdr->counterWidth;
	  rect.y = startLine * pSrcEdr->lineHeight;
	  rect.width = pSrcEdr->colRightStart - pSrcEdr->colMidStart;
	  rect.height = refreshLines * pSrcEdr->lineHeight;
	  pWin->Refresh( true, &rect );
	} // end if editable
  } // end PH_NOTIFY
  // called by the OS to paint
  // paints the areas to be updated
  else { // PH_EXEC
	SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
	pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at( pSrcEdr->fileOffset, pSrcEdr->Caret.y, &chunkSize );
	// draw the update line/s
	wxBrush Brush( wxColour( 208,208,200 ) );
	wxPen Pen = DC.GetPen();
	DC.SetPen( *wxTRANSPARENT_PEN );
	wxRect rect;
	rect.x = pSrcEdr->colMidStart + pSrcEdr->counterWidth;
	rect.y = startLine * pSrcEdr->lineHeight;
	rect.width = pSrcEdr->colRightStart - pSrcEdr->colMidStart;
	rect.height = refreshLines * pSrcEdr->lineHeight;
	DC.SetBrush( Brush );
	DC.DrawRectangle( rect );
	DC.SetPen( Pen );
	if( pElem->type != CDE_S_BLANK )
	  DC.DrawText( wxString( pElem->pLine->szBuf ), rect.x, pSrcEdr->Caret.y * pSrcEdr->lineHeight );

	// redraw the previous line if a new line has been created
	if( pSrcEdr->Caret.y != pSrcEdr->CaretPrev.y ) {
	  SCodeElement *pElemPrev = pSrcEdr->pCodeBase->pBaseSec->get_element_at( pSrcEdr->fileOffset, pSrcEdr->CaretPrev.y, &chunkSize );
	  if( pElemPrev->type != CDE_S_BLANK ) {
		DC.DrawText( wxString( pElemPrev->pLine->szBuf ), rect.x, pSrcEdr->CaretPrev.y * pSrcEdr->lineHeight );
	  }
	}
	// draw the caret
	int caretLoc = pSrcEdr->colMidStart+tl_caret_loc( pElem->pLine, pSrcEdr->Caret.x, DC, pWin );
	DC.DrawLine( caretLoc, (pSrcEdr->Caret.y)*pSrcEdr->lineHeight, caretLoc, (pSrcEdr->Caret.y + 1)*pSrcEdr->lineHeight );
  }
}
// intent handler for START_SEL
// user want to start a selection
// by inputting Shift-Arrow key
void src_edr_start_sel( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {

}
// intent handler for UPDATE_SEL
// user is in the process of updating a selection
// by inputting Shift-Arrow key
void src_edr_update_sel( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {

 }
// intent handler for ERASE_SEL
// users wants to unselect the current selection
// by pressing the Backspace key
void src_edr_un_sel( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {

}
// intent handler for CUT_SEL
// user want to cut the current selection by Ctrl-C or Ctrl-X
void src_edr_cut_sel( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // create an edit operation for a cut selection
  // from the current selection
  // and execute it on the codebase
  // update the caret location
  // refresh the window
  if( phase == PH_NOTIFY ) {
	SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
	pSrcEdr->CaretPrev = pSrcEdr->Caret;

	SOperation Op;
	STxtPage CutPage;

 //   op_cutpaste_sel_init( Op, pSrcEdr->fileOffset, pSrcEdr->Caret.y, pSrcEdr->selStart, pSrcEdr->selEnd, CutPage, true );
 //   oplist_add( pSrcEdr->pCodeBase->OpList, Op );

		  // add the operation to the codebase's oplist,
		  // do the edit on the codebase


	pWin->m_bUsrActn = false;
	pWin->Refresh( true );
  }
  else
    wxLogError("PH_EXEC in src_edr_del_sel");

}
// intent handler for PASTE_SEL
// user want to paste the current slection by Ctrl-V
void src_edr_paste_sel( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {

}
// intent handler for UNDO
// user wants to undo the last edit using Ctrl-Z
void src_edr_undo( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {

}
// intent handler for CONTROL
// user wants to do something with the file being edited
// launches an intent dispatcher of available action
void src_edr_control( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // push the intent dispatcher onto the modemanager
  // refresh the window
  if( phase == PH_NOTIFY ) {
	pWin->m_pModeManager->push( pBase->sExt.pSrcEdr->pIntentDispatcher );
	pWin->Refresh( true );
	pWin->m_bUsrActn = false;
  }
}
// intent handler for EXPORT
// user wants to export the current codebase to a .cpp file
// launches a file browser
void src_edr_export( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // pop-up a not yet supported message
  if( phase == PH_NOTIFY )
  {
	SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
	pSrcEdr->pMsg->sExt.pMsg->set_msg((char*)".cpp export is under development" );
	pWin->m_pModeManager->push( pSrcEdr->pMsg );
	pWin->m_bUsrActn = false;
	pWin->Refresh( true );
  }
}
// intent handler for LOAD_NEW
// user wants to save current codebase and load a new file
// launches a file browser
void src_edr_load_new( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // pop-up a not yet supported message
  if( phase == PH_NOTIFY )
  {
	SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
	pSrcEdr->pMsg->sExt.pMsg->set_msg((char*)"loading a new file is under development" );
	pWin->m_pModeManager->push( pSrcEdr->pMsg );
	pWin->m_bUsrActn = false;
	pWin->Refresh( true );
  }
}
// intent handler for BUILD
// user wants to build the current codebase
// launches a compiler and linker
void src_edr_build( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // pop-up a not yet supported message
  if( phase == PH_NOTIFY )
  {
	SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
	pSrcEdr->pMsg->sExt.pMsg->set_msg((char*)"building the current codebase is under development" );
	pWin->m_pModeManager->push( pSrcEdr->pMsg );
	pWin->m_bUsrActn = false;
	pWin->Refresh( true );
  }

}
// intent handler for DEBUG
// user wants to debug the codebase
// build the codebase, if succesful
// launches a debugger
void src_edr_debug( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // pop-up a not yet supported message
  if( phase == PH_NOTIFY )
  {
	SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
	pSrcEdr->pMsg->sExt.pMsg->set_msg( (char *) "debugging the current codebase is under development" );
	pWin->m_pModeManager->push( pSrcEdr->pMsg );
	pWin->m_bUsrActn = false;
	pWin->Refresh( true );
  }
}
// intent handler for ADJUST_FONTSIZE
// user wants to adjust the fontsize for their display
// launches a SModeLevAdj pop-up for font-size adjustments
void src_edr_adjust_fontsize( SMode *pBase, int phase, ModalWindow *pWin, wxDC &DC ) {
  // launches a pop-up for font-size adjustments
  if( phase == PH_NOTIFY )
  {
    SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
    if( !pSrcEdr->pLevAdj->sExt.pLevAdj->bInputRcvd ) {
      pBase->bCtrlDown = false; // so this mode is not confused on return
      pSrcEdr->pLevAdj->sExt.pLevAdj->set_caller( pBase, SEI_ADJUST_FONTSIZE, (char*) "arrows to change font size, esc to exit" );
      pWin->m_pModeManager->push( pSrcEdr->pLevAdj );
    }
    // callback from the set-sel pop-up with the font-size update data
    // set fontsize on pBase, reset pSrcEdr, Refresh
    else {
      if( pSrcEdr->pLevAdj->sExt.pLevAdj->bIncDec ) 
       pBase->adjust_font_scale( 1.05 );
      else
        pBase->adjust_font_scale(1.00/1.05);
      pBase->bReset = true;
      pSrcEdr->pLevAdj->sExt.pLevAdj->bInputRcvd = false;
    }
    pWin->m_bUsrActn = false;
    pWin->Refresh( true );
  }
}
// intent handler for INPUT_CODEFILE
// user wants to load a codefile into the src edr
// gets the codefile path from pLineInput
// parses the codebase and if successful set the codebase
// else, asks the user to input the codefile again
void src_edr_input_codefile(SMode* pBase, int phase, ModalWindow* pWin, wxDC& DC) {
	SModeSrcEdr* pSrcEdr = pBase->sExt.pSrcEdr;
	wxString strCodeFilePath;
	if (pSrcEdr->pLineInp->sExt.pLineInput->bInputRcvd) {
		strCodeFilePath = wxString(pSrcEdr->pLineInp->sExt.pLineInput->pInput->szBuf);
		pSrcEdr->pLineInp->sExt.pLineInput->bInputRcvd = false;
	}
	else if (pSrcEdr->pFileSel->sExt.pFileSel->bInputRcvd) {
		strCodeFilePath = wxString( pSrcEdr->pFileSel->sExt.pFileSel->pFilePath->szBuf );
		pSrcEdr->pFileSel->sExt.pFileSel->bInputRcvd = false;
	}
	if( strCodeFilePath.Matches("*.cpp") ) {
	  SCodeBase* pCodeBase = new_codebase();
	  if (pCodeBase->load_codefile(strCodeFilePath))
		  pSrcEdr->set_codebase(pCodeBase);
	  else {
		  free_codebase(pCodeBase);
		  pBase->bCtrlDown = false; // so this mode is not confused on return
      wxDir Dir(wxStandardPaths::Get().GetDocumentsDir());
		  wxString strDir = Dir.GetNameWithSep();
		  STxtLine* pCodeFilePath = new_txt_line_wx(strDir);
		  pSrcEdr->pFileSel->sExt.pFileSel->set_caller(pBase, SEI_INPUT_CODEFILE, pCodeFilePath );
		  tl_free(pCodeFilePath);
      pCodeFilePath = NULL;    
		  pWin->m_pModeManager->push(pSrcEdr->pFileSel);
	  }
	}
	else {
	  pBase->bCtrlDown = false; // so this mode is not confused on return
	  wxDir Dir(wxStandardPaths::Get().GetDocumentsDir());
	  wxString strDir = Dir.GetNameWithSep();
	  STxtLine* pCodeFilePath = new_txt_line_wx(strDir);
	  pSrcEdr->pFileSel->sExt.pFileSel->set_caller(pBase, SEI_INPUT_CODEFILE, pCodeFilePath );
	  tl_free(pCodeFilePath);
    pCodeFilePath = NULL;    
	  pWin->m_pModeManager->push(pSrcEdr->pFileSel);
	}
	pWin->m_bUsrActn = false;
	pWin->Refresh(true);
	return;
}

// ancillary fn for scrolling used by src editor implementation
void src_edr_scroll( SMode *pBase, bool bUp, ModalWindow *pWin ) {
  SModeSrcEdr *pSrcEdr = pBase->sExt.pSrcEdr;
  SCodeElement *pElem = NULL;
  pSrcEdr->CaretPrev = pSrcEdr->Caret;
  int skip;
  // if the scroll was caused by an arrow up at the top of the page
  // scroll up a line
  // if scroll was caused by a page up
  // scroll up a full page
  if( bUp ) {
    pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, -pSrcEdr->dispLines, &skip );
    // if it's not the first page
    // if the scroll was caused by an arrow up at the top of the page
    // scroll up a line
    // if scroll was caused by a page up
    // scroll up a full page
    if( pSrcEdr->fileOffset >= -skip && skip < 0 ) {
      // scroll up a line
      if( pSrcEdr->Caret.y == 0 ) {
      pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, -1, &skip );
      pSrcEdr->fileOffset += skip;
      }
      // scroll up a full page
      else {
      pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, -pSrcEdr->dispLines, &skip );
      pSrcEdr->fileOffset += skip;
      }
    }
    // it is the first page
    else
      pSrcEdr->fileOffset = 0;
  }
  // scroll down
  else {
    // get the last element in the current display panel
    pElem = pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, pSrcEdr->dispLines-1, &skip );
    // if it's not the last page
    // if the scroll was caused by an arrow down at the bottom of the page
    // scroll down a line
    // if scroll was caused by a page down
    // scroll down a full page
    if( pSrcEdr->fileOffset + skip + ce_length(pElem) < pSrcEdr->pCodeBase->pBaseSec->get_length() ) {
      if( pSrcEdr->Caret.y == pSrcEdr->dispLines - 1 ) {
      pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, 1, &skip );
      pSrcEdr->fileOffset += skip;
      }
      else {
      pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->fileOffset, pSrcEdr->dispLines, &skip );
      pSrcEdr->fileOffset += skip;
      }
    }
    // it is the last page, set fileOffset to EOF - dispLines
    else {
      pSrcEdr->pCodeBase->pBaseSec->get_element_at(pSrcEdr->pCodeBase->pBaseSec->get_length(), -pSrcEdr->dispLines, &skip );
      pSrcEdr->fileOffset = pSrcEdr->pCodeBase->pBaseSec->get_length() + skip;
    }
  }
  pWin->m_bUsrActn = false;
  pWin->Refresh( true );
  pWin->Update();
}


// free's a mode of any type
// if you extend UModeExt, you must add your new ModeExt type to be freed here
void free_mode(SMode* pThis) {
	switch (pThis->type) {
	case MODE_INTENT_DISPATCHER:
		free_int_disp(pThis);
		break;
	case MODE_MESSAGE:
		free_msg(pThis);
		break;
	case MODE_LINE_INPUT:
		free_line_input(pThis);
		break;
	case MODE_LEVEL_ADJUSTER:
		free_lev_adj(pThis);
		break;
	case MODE_FILE_SELECTOR:
		free_file_sel(pThis);
		break;
	case MODE_SOURCE_EDITOR:
		free_src_edr(pThis);
		break;
	case MODE_BASE:
		free(pThis);
		break;
	default:
		wxLogError("invalid mode type in free_mode");
		break;
	}
};

// loads a serialized mode from a file
SMode *load_mode( int scrnW, int scrnH, wxFile &File ) {
  SMode *pMode = (SMode *) malloc( sizeof(SMode) );
  if (pMode != NULL) {
	  pMode->init(scrnW, scrnH, NULL);
	  pMode->serialize(File, false);
	  switch (pMode->type) {
		  case MODE_SOURCE_EDITOR: {
			  pMode->sExt.pSrcEdr = (SModeSrcEdr*)malloc(sizeof(SModeSrcEdr));
			  if (pMode->sExt.pSrcEdr != NULL) {
				  pMode->sExt.pSrcEdr->init(pMode);
				  pMode->fnSerialize(pMode, File, false);
			  }
		  }
		  break;
		  default:
			  break;
	  }
  }
  return( pMode );
}
// BLOCK: INITIALIZING AND EXITING MODAL
// This block contains the functions that initialize Modal
// and close Modal when the app exits

// Ancillary fn.
// loads the default UI state
// with the given system parameters
// called by modal_init()
// returns an initialized mode manager
SModeManager * load_UI_state( int scrnW, int scrnH, wxFont *pFont ) {
  SModeManager *pModeManager = NULL;
  // load the codefile, the src editor and load the src editor into the mode manager
  SMode *pMode = new_src_edr( scrnW, scrnH, pFont );
  pModeManager = new_mode_manager( scrnW, scrnH, pFont );
  pModeManager->push( pMode );
  return( pModeManager );
}
// initilize this Modal app
// it is called by the constructor of ModalWindow
SModeManager * modal_init( int scrnWidth, int scrnHeight ) {
  SModeManager *pModeManager = NULL;

  // we assume all users have the Helvetica font on their systems
  // we check if they dont and use a portable wxWidgets font instead
  // A note on fontsize.
  // A fonsize expressed in points is portable
  // since points are screen units and resolution independent
  // However, on some systems, such as OSX systems with external monitors
  // the system has no way of determining screen PPI (pixels per inch)
  // which is needed to detrmine the pixel font size based on the point based size.
  // For such situations, we provide a font scaling User Intent Handler.
  wxFont* pFont = new wxFont(wxFontInfo(12.0).FaceName("Helvetica"));
  bool bIsOK = pFont->IsOk();
  if (!bIsOK) 
	  pFont = new wxFont(wxFontInfo(12.0).Family(wxFONTFAMILY_SWISS));

  wxFile File;
  if( wxFile::Exists("./State.hxp") ) {
    File.Open("./State.hxp");
    pModeManager = new_mode_manager( scrnWidth, scrnHeight, pFont );
    pModeManager->serialize( File, false );
    File.Close();
  }
  else 
	  pModeManager = load_UI_state(scrnWidth, scrnHeight, pFont);
  
  return( pModeManager );
}
// exit a Modal app
// last function to be called before app exits
// serializes the mode manager to a file
void modal_exit( SModeManager *pModeManager ) {
  wxFile File;
 
  File.Create("./State.hxp", true);
  // serialize state to the file
  if( File.IsOpened() ) {
    if (pModeManager->serialize(File, true))
      File.Close();
    // if serialisation failed, close and remove file
    else {
      File.Close();
      wxRemoveFile("./State.hxp");
    }
  }
  free_mode_manager( pModeManager );
  return;
}
