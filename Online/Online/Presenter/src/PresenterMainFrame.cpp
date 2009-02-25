#include <algorithm>
#include <iostream>
#include <vector>
#include <TApplication.h>
#include <TCanvas.h>
#include <TColor.h>
#include <TGIcon.h>
#include <TGResourcePool.h>
#include <TG3DLine.h>
#include <TGButton.h>
#include <TGComboBox.h>
#include <TGDockableFrame.h>
#include <TGDoubleSlider.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGListTree.h>
#include <TGListView.h>
#include <TGMenu.h>
#include <TGMsgBox.h>
#include <TGPicture.h>
#include <TGNumberEntry.h>
#include <TGSplitter.h>
#include <TGStatusBar.h>
#include <TGTab.h>
#include <TGTextEdit.h>
#include <TGTextView.h>
//#include <TGText.h>
#include <TGToolBar.h>
#include <TDatime.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TImage.h>
//#include <TList.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TObject.h>
#include <TBox.h>
#include <TLine.h>
#include <TText.h>
#include <TKey.h>
#include <TPad.h>
#include <TPRegexp.h>
#include <TRootEmbeddedCanvas.h>
#include <TString.h>
#include <TObjString.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TThread.h>
#include <TTimer.h>
#include <TGFileDialog.h>
#include <TRootHelpDialog.h>
#include <TBenchmark.h>


#include "dim/dic.hxx"
#include "OnlineHistDB/OnlineHistDB.h"
#include "OMAlib/OMAlib.h"

#include "Gaucho/MonObject.h"

#include "DbRootHist.h"
#include "PresenterMainFrame.h"
#include "HistogramIdentifier.h"
#include "LoginDialog.h"
#include "PageSaveDialog.h"
#include "HistoPropDialog.h"
#include "SetDimDnsNodeDialog.h"
#include "Archive.h"
#include "ReferencePicker.h"
#include "IntervalPicker.h"
#include "icons.h"

using namespace pres;
using namespace std;
PresenterMainFrame* gPresenter = 0;

ClassImp(PresenterMainFrame)

PresenterMainFrame::PresenterMainFrame(const char* name,
                                       const std::string & savesetPath,
                                       const std::string & referencePath,
                                       const int & x,
                                       const int & y,
                                       const int & width,
                                       const int & height) :
  TGMainFrame(gClient->GetRoot(), width, height, kLHintsExpandX | kLHintsExpandY),
  currentTime(NULL),
  global_timePoint("20081126T160921"),
  global_pastDuration("00:05:00"),
  global_stepSize("00:15:00"),
  m_verbosity(Silent),
  m_historyMode(s_timeInterval),
  m_currentPartition(s_lhcbPartionName.Data()),
//  m_currentMonitoringNode(""),
//  m_currentMonitoringInstance(""),
  m_referencePath(referencePath),
  m_savesetPath(savesetPath),
  m_savesetFileName(""),
  m_archive(NULL),
  m_intervalPicker(NULL),
  m_presenterMode(History),
  m_databaseMode(LoggedOut),
  m_pageRefreshTimer(NULL),
  m_clockTimer(NULL),
  m_clearedHistos(false),
  m_historyTrendPlots(false),
  m_referencesOverlayed(false),
  m_fastHitMapDraw(false),
  m_refreshingPage(false),
  m_histogramDB(NULL),
  m_analysisLib(NULL),
  m_msgBoxReturnCode(0),
  m_menuDock(NULL),
  m_toolBarDock(NULL),
  m_databaseHistogramsDock(NULL),
  m_pageDock(NULL),
  m_mainCanvasInfoDock(NULL),
  m_drawPattern(NULL),
  m_dimBrowserDock(NULL),
  m_databasePagesDock(NULL),
  m_histoSvcBrowserGroupFrame(NULL),
  m_treeNode(NULL),
  m_taskNode(NULL),
  m_algorithmNode(NULL),
  m_histogramSetNode(NULL),
  m_histogramNode(NULL),
  m_pageNode(NULL),
  m_folderItems(NULL),
  m_folderItemsIt(NULL),
  m_folderItem(NULL),
  m_benchmark(NULL)
{

  m_benchmark = new TBenchmark();
  SetCleanup(kDeepCleanup);
  
  // Thermo palette for hitmaps
  gStyle->SetPalette(1);
  
//std::cout << "PresenterMainFrame() gStyle->GetOptStat: " << gStyle->GetOptStat;
  gStyle->SetOptStat("emrou"); // nemr

  // only one presenter session allowed
  if (gPresenter) { return; }

  if (!currentTime) { currentTime = new TDatime(); }
  
  if (!m_pageRefreshTimer) { 
    m_pageRefreshTimer = new TTimer(s_pageRefreshRate);
  }
  m_pageRefreshTimer->TurnOff();
  m_pageRefreshTimer->Connect("Timeout()", "PresenterMainFrame",
                              this, "refreshPage()");

  if (!m_clockTimer) { m_clockTimer = new TTimer(1000); }
  m_clockTimer->TurnOn();
  m_clockTimer->Connect("Timeout()", "PresenterMainFrame",
                        this, "refreshClock()");

  SetWindowName(name);
  Move(x, y);
  gPresenter = this;

  m_knownDimServices.reserve(s_estimatedDimServiceCount);
  m_candidateDimServices.reserve(s_estimatedDimServiceCount);
  m_histogramTypes.reserve(s_estimatedDimServiceCount);
  dbHistosOnPage.reserve(s_estimatedHistosOnPage);

  m_knownOnlinePartitionList = new TList();
  m_knownHistoryPartitionList = new TList();

  buildGUI();
  
  Resize(width, height);
  if (m_verbosity >= Verbose) {
    std::cout << "refRoot: "  << m_referencePath << std::endl
              << "savesetRoot: "  << m_savesetPath << std::endl;
  }

  m_dimBrowser = new DimBrowser();
  if (0 == m_dimBrowser->getServers()) {
    m_message = "Sorry, no DIM server found.";
    if (m_verbosity >= Verbose) {
      std::cout << m_message << std::endl;
    }
    m_mainStatusBar->SetText(m_message.c_str(), 2);
    new TGMsgBox(fClient->GetRoot(), this, "DIM Error", m_message.c_str(),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  } else {
//      refreshHistogramSvcList(s_withoutTree);      
    m_mainStatusBar->SetText("Ok.");
  }

  m_archive = new Archive(this, m_savesetPath, m_referencePath);
  if (0 != m_archive) { m_archive->setVerbosity(m_verbosity); }
  m_intervalPicker = new IntervalPicker(this);
  refreshPartitionSelectorPopupMenu();
  m_presenterMode = Online;  
}

PresenterMainFrame::~PresenterMainFrame()
{
  TGListTreeItem* node = m_histoSvcListTree->GetFirstItem();
  deleteTreeChildrenItemsUserData(node);
  m_histoSvcListTree->DeleteChildren(node);

  node = m_databaseHistogramTreeList->GetFirstItem();
  deleteTreeChildrenItemsUserData(node);
  m_databaseHistogramTreeList->DeleteChildren(node);

  node = m_pagesFromHistoDBListTree->GetFirstItem();
  deleteTreeChildrenItemsUserData(node);
  m_pagesFromHistoDBListTree->DeleteChildren(node);

  removeHistogramsFromPage();
  Cleanup();
  if (0 != m_histogramDB) { delete m_histogramDB; m_histogramDB = NULL; }
  if (0 != m_analysisLib) { delete m_analysisLib; m_analysisLib = NULL; }
  if (0 != m_dimBrowser) { delete m_dimBrowser; m_dimBrowser = NULL; }
  if (0 != m_archive) { delete m_archive; m_archive = NULL; }
  
  if (0 != m_intervalPicker) {
    m_intervalPicker->UnmapWindow();
    m_intervalPicker->CloseWindow();
    delete m_intervalPicker; m_intervalPicker = NULL;
  }
  if (0 != m_clockTimer) { delete m_clockTimer; m_clockTimer = NULL; }
  if (0 != m_pageRefreshTimer) {
    delete m_pageRefreshTimer;
    m_pageRefreshTimer = NULL;
  }
  if (0 != m_knownOnlinePartitionList) {
    m_knownOnlinePartitionList->Delete();
    delete m_knownOnlinePartitionList;
    m_knownOnlinePartitionList = NULL;
  }
  if (0 != m_knownHistoryPartitionList) {
    m_knownHistoryPartitionList->Delete();
    delete m_knownHistoryPartitionList;
    m_knownHistoryPartitionList = NULL;
  }
  if (0 != m_benchmark) { delete m_benchmark; m_benchmark = NULL; }  
  
  if (gPresenter == this) { gPresenter = 0; }
}
void PresenterMainFrame::buildGUI()
{
  SetCleanup(kDeepCleanup);
  TGPicturePool* picpool = gClient->GetResourcePool()->GetPicturePool();
  // set minimun size
  // SetWMSizeHints(400 + 200, 370+50, 2000, 1000, 1, 1);
  m_fileText = new TGHotString("&File");
//  m_filePrint = new TGHotString("Save/&Print Page to File...");
    m_fileNew = new TGHotString("&Create Page");
    m_fileSaveText = new TGHotString("&Save Page to Database...");
    m_fileLoginText = new TGHotString("&Login to Database...");
    m_fileLogoutText = new TGHotString("Log&out...");
    m_fileQuitText = new TGHotString("&Quit");

  m_editText = new TGHotString("&Edit");
    m_editAutoLayoutText = new TGHotString("Automatic Histogram &Layout");
    m_editHistogramPropertiesText = new TGHotString("&Histogram Properties...");
    m_editRemoveHistoText = new TGHotString("&Delete Histogram from Page");
    m_editPickReferenceHistogramText = new TGHotString("Set &Reference Histogram...");
    m_editSaveSelectedHistogramAsReferenceText = new TGHotString("&Save as Reference Histogram");
    m_editPagePropertiesText = new TGHotString("&Page Properties...");

  m_viewText = new TGHotString("&View");
    m_viewStartRefreshText = new TGHotString("Start Page &Refresh");
    m_viewStopRefreshText = new TGHotString("&Stop Page Refresh");
    m_viewToggleHistoryPlotsText = new TGHotString("&History plots");
    m_viewToggleReferenceOverlayText = new TGHotString("&Overlay Reference");
    m_viewToggleFastHitMapDrawText = new TGHotString("&Fast Hitmap Draw");
    m_viewInspectHistoText = new TGHotString("&Inspect Histogram");
    m_viewHistogramDescriptionText = new TGHotString("Histogram &Description");
    m_viewInspectPageText  = new TGHotString("Inspect &Page");
    m_viewClearHistosText = new TGHotString("&Clear Histograms");
    m_viewUnDockPageText = new TGHotString("&Undock Page");
    m_viewDockAllText = new TGHotString("&Dock All");

  m_toolText = new TGHotString("&Tools");
    m_toolMode = new TGHotString("&Mode");
    m_toolPageEditorOnline = new TGHotString("Page Editor Online");
    m_toolPageEditorOffline  = new TGHotString("Page Editor Offline");
    m_toolOnline = new TGHotString("&Online");
    m_toolOffline = new TGHotString("&History mode");
//    m_toolSetUtgidTaskText = new TGHotString("Set &UTGID for Task...");
    m_toolSetDimDnsText = new TGHotString("Set &DIM DNS...");

  m_helpText = new TGHotString("&Help");
    m_helpContentsText = new TGHotString("&Contents");
    m_helpAboutText = new TGHotString("&About");

  // list tree
  m_openedFolderIcon = picpool->GetPicture("folderOpen16",folderOpen16);
  m_closedFolderIcon = picpool->GetPicture("folder16",folder16);

  //gVirtualX CreatePictureFromData(Drawable_t id, char** data, Pixmap_t& pict, Pixmap_t& pict_mask, PictureAttributes_t& attr)

  m_databaseSourceIcon =  picpool->GetPicture("databaseSource16",
                                              databaseSource16);
  m_dimOnline16 =  picpool->GetPicture("dimOnline16",dimOnline16);
  m_iconROOT = gClient->GetPicture("root_t.xpm");

  m_iconH1D = gClient->GetPicture("h1_t.xpm");
  m_iconH2D = gClient->GetPicture("h2_t.xpm");
  m_iconProfile = gClient->GetPicture("profile_t.xpm");
  m_iconCounter = gClient->GetPicture("c_src_t.xpm");
  m_iconPage = gClient->GetPicture("bld_paste_into.png");
  m_iconQuestion = gClient->GetPicture("mdi_help.xpm");

  m_iconTask = gClient->GetPicture("tmacro_t.xpm");
  m_iconAlgorithm = gClient->GetPicture("expression_t.xpm");
  //m_iconSet = gClient->GetPicture("pack-empty.xpm");
  m_iconSet = gClient->GetPicture("pack-empty_t.xpm");
  m_iconLevel = gClient->GetPicture("bld_vbox.png");
  
  m_stockNewFormula =  picpool->GetPicture("stock_new_formula_xpm", stock_new_formula_xpm);

  // File menu
  m_fileMenu = new TGPopupMenu(fClient->GetRoot());
  m_fileMenu->AddEntry(m_fileNew, CLEAR_PAGE_COMMAND);
  m_fileMenu->AddSeparator();
  m_fileMenu->AddEntry(m_fileSaveText, SAVE_PAGE_TO_DB_COMMAND);
//  m_fileMenu->AddEntry(m_filePrint, SAVE_PAGE_TO_FILE_COMMAND);
  m_fileMenu->AddSeparator();
  m_fileMenu->AddEntry(m_fileLoginText, LOGIN_COMMAND);
  m_fileMenu->AddEntry(m_fileLogoutText, LOGOUT_COMMAND);
  m_fileMenu->AddSeparator();
  m_fileMenu->AddEntry(m_fileQuitText, EXIT_COMMAND);
  m_fileMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                      this, "handleCommand(Command)");
//m_fileMenu->DisableEntry(SAVE_PAGE_TO_FILE_COMMAND);

  // Edit menu
  m_editMenu = new TGPopupMenu(fClient->GetRoot());
  m_editMenu->AddEntry(m_editAutoLayoutText, AUTO_LAYOUT_COMMAND);
  m_editMenu->AddEntry(m_editHistogramPropertiesText, EDIT_HISTO_COMMAND);
  m_editMenu->AddEntry(m_editRemoveHistoText,
                       REMOVE_HISTO_FROM_CANVAS_COMMAND);
  m_editMenu->AddEntry(m_editPickReferenceHistogramText,
                       PICK_REFERENCE_HISTO_COMMAND);
  m_editMenu->AddEntry(m_editSaveSelectedHistogramAsReferenceText,
                       SAVE_AS_REFERENCE_HISTO_COMMAND); 
  m_editMenu->AddEntry(m_editPagePropertiesText,
                       EDIT_PAGE_PROPERTIES_COMMAND);
  m_editMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                      this, "handleCommand(Command)");
m_editMenu->DisableEntry(PICK_REFERENCE_HISTO_COMMAND);
m_editMenu->DisableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);
m_editMenu->DisableEntry(EDIT_PAGE_PROPERTIES_COMMAND);

  // View menu
  m_viewMenu = new TGPopupMenu(fClient->GetRoot());
  m_viewMenu->AddEntry(m_viewInspectHistoText, INSPECT_HISTO_COMMAND);
  m_viewMenu->AddEntry(m_viewHistogramDescriptionText, HISTOGRAM_DESCRIPTION_COMMAND);
  m_viewMenu->AddEntry(m_viewInspectPageText, INSPECT_PAGE_COMMAND);
  m_viewMenu->AddEntry(m_viewStartRefreshText, START_COMMAND);
  m_viewMenu->AddEntry(m_viewStopRefreshText, STOP_COMMAND);
  m_viewMenu->DisableEntry(STOP_COMMAND);
  m_viewMenu->AddEntry(m_viewToggleHistoryPlotsText,
                       HISTORY_PLOTS_COMMAND);
  m_viewMenu->UnCheckEntry(HISTORY_PLOTS_COMMAND);
  m_viewMenu->AddEntry(m_viewToggleReferenceOverlayText,
                       OVERLAY_REFERENCE_HISTO_COMMAND);
  m_viewMenu->AddEntry(m_viewToggleFastHitMapDrawText,
                       FAST_HITMAP_DRAW_COMMAND);                       
                       
  m_viewMenu->UnCheckEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
  m_viewMenu->UnCheckEntry(FAST_HITMAP_DRAW_COMMAND);  
  m_viewMenu->AddEntry(m_viewClearHistosText, CLEAR_HISTOS_COMMAND);
  m_viewMenu->UnCheckEntry(CLEAR_HISTOS_COMMAND);
  m_viewMenu->AddSeparator();
  m_viewMenu->AddEntry(m_viewUnDockPageText, UNDOCK_PAGE_COMMAND);
  m_viewMenu->AddEntry(m_viewDockAllText, DOCK_ALL_COMMAND);
  m_viewMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                      this, "handleCommand(Command)");
m_viewMenu->DisableEntry(HISTORY_PLOTS_COMMAND);
m_viewMenu->DisableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);

  // Mode menu
  m_toolMenu = new TGPopupMenu(fClient->GetRoot());
  m_toolMenu->AddEntry(m_toolPageEditorOnline, PAGE_EDITOR_ONLINE_MODE_COMMAND);
  m_toolMenu->CheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
  m_toolMenu->AddEntry(m_toolPageEditorOffline, PAGE_EDITOR_OFFLINE_MODE_COMMAND);
  m_toolMenu->UnCheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);  
  m_toolMenu->AddEntry(m_toolOnline, ONLINE_MODE_COMMAND);
  m_toolMenu->UnCheckEntry(ONLINE_MODE_COMMAND);
  m_toolMenu->AddEntry(m_toolOffline, OFFLINE_MODE_COMMAND);
  m_toolMenu->UnCheckEntry(OFFLINE_MODE_COMMAND);
//  m_toolMenu->AddSeparator();
//  m_toolMenu->AddEntry(m_toolSetUtgidTaskText, SET_UTGID_TASK_COMMAND);
//  m_toolMenu->AddSeparator();
//  m_toolMenu->AddEntry(m_toolSetDimDnsText, SET_DIM_DNS_COMMAND);
  m_toolMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                      this, "handleCommand(Command)");
//m_toolMenu->DisableEntry(ONLINE_MODE_COMMAND);
//m_toolMenu->DisableEntry(OFFLINE_MODE_COMMAND);
//m_toolMenu->DisableEntry(SET_UTGID_TASK_COMMAND);
//m_toolMenu->DisableEntry(SET_DIM_DNS_COMMAND);

  // Help menu
  m_helpMenu = new TGPopupMenu(fClient->GetRoot());
//  m_helpMenu->AddEntry(m_helpContentsText, HELP_CONTENTS_COMMAND);
  m_helpMenu->AddEntry(m_helpAboutText, HELP_ABOUT_COMMAND);
  m_helpMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                      this, "handleCommand(Command)");
// m_helpMenu->DisableEntry(HELP_CONTENTS_COMMAND);

  m_menuDock = new TGDockableFrame(this);
    m_menuDock->SetWindowName("LHCb Presenter Menu");

  m_toolBarDock = new TGDockableFrame(this);
    m_toolBarDock->SetWindowName("LHCb Presenter Toolbar");

  TGLayoutHints* menuBarLayout = new TGLayoutHints(kLHintsTop |
                                                   kLHintsExpandX);
  TGLayoutHints* menuBarItemLayout = new TGLayoutHints(kLHintsTop |
                                                       kLHintsLeft,
                                                       0, 4, 0, 0);
  TGLayoutHints* menuBarHelpLayout = new TGLayoutHints(kLHintsTop |
                                                       kLHintsRight);
  m_menuBar = new TGMenuBar(m_menuDock, 1, 1, kHorizontalFrame);
  m_menuBar->AddPopup(m_fileText, m_fileMenu, menuBarItemLayout);
  m_menuBar->AddPopup(m_editText, m_editMenu, menuBarItemLayout);
  m_menuBar->AddPopup(m_viewText, m_viewMenu, menuBarItemLayout);
  m_menuBar->AddPopup(m_toolText, m_toolMenu, menuBarItemLayout);
  m_menuBar->AddPopup(m_helpText, m_helpMenu, menuBarHelpLayout);

  m_menuDock->EnableUndock(false);
  m_menuDock->EnableHide(true);

  m_toolBarDock->EnableUndock(true);
  m_toolBarDock->EnableHide(true);

  m_menuDock->AddFrame(m_menuBar, menuBarLayout);
  m_menuDock->AddFrame(new TGHorizontal3DLine(this), menuBarLayout);

  // Toolbar
  m_toolBar = new TGToolBar(m_toolBarDock, 60, 20, kHorizontalFrame);
//  m_toolbarElement = new ToolBarData_t();

  // New page
  const TGPicture* newPage16pic = picpool->GetPicture("newPage16",
                                                      newPage16);
  m_newPageButton = new TGPictureButton(m_toolBar, newPage16pic,
                                        CLEAR_PAGE_COMMAND);
  m_newPageButton->SetToolTipText("New page");
  m_newPageButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_newPageButton, 0);
  m_newPageButton->Connect("Clicked()", "PresenterMainFrame", this,
                           "removeHistogramsFromPage()");

  // Save page
  const TGPicture* savePageToDb16pic = picpool->GetPicture("savePageToDb16",
                                                           savePageToDb16);
  m_savePageToDatabaseButton = new TGPictureButton(m_toolBar,
                                                   savePageToDb16pic,
                                                   SAVE_PAGE_TO_DB_COMMAND);
  m_savePageToDatabaseButton->SetToolTipText("Save page to Database");
  m_savePageToDatabaseButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_savePageToDatabaseButton, 8);
  m_savePageToDatabaseButton->SetState(kButtonDisabled);
  m_savePageToDatabaseButton->Connect("Clicked()", "PresenterMainFrame",
                                      this, "savePageToHistogramDB()");

  // Auto n x n layout
  const TGPicture* automaticLayout16pic = picpool->GetPicture("automaticLayout16",
                                                              automaticLayout16);
  m_autoCanvasLayoutButton = new TGPictureButton(m_toolBar, automaticLayout16pic,
                                                 AUTO_LAYOUT_COMMAND);
  m_autoCanvasLayoutButton->SetToolTipText("Auto n x n histogram tiling layout");
  m_autoCanvasLayoutButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_autoCanvasLayoutButton, 8);
  m_autoCanvasLayoutButton->SetState(kButtonDisabled);
  m_autoCanvasLayoutButton->Connect("Clicked()", "PresenterMainFrame",
                                    this, "autoCanvasLayout()");

  // Login
  const TGPicture* connectpic = picpool->GetPicture("connect.xpm");
  m_loginButton = new TGPictureButton(m_toolBar, connectpic, LOGIN_COMMAND);
  m_loginButton->SetToolTipText("Login");
  m_loginButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_loginButton, 8);
  m_loginButton->Connect("Clicked()", "PresenterMainFrame", this,
                         "loginToHistogramDB()");
  // Logout
  const TGPicture* disconnectpic = picpool->GetPicture("disconnect.xpm");
  m_logoutButton = new TGPictureButton(m_toolBar, disconnectpic,
                                       LOGOUT_COMMAND);
  m_logoutButton->SetToolTipText("Logout");
  m_logoutButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_logoutButton, 0);
  m_logoutButton->SetState(kButtonDisabled);
  m_logoutButton->Connect("Clicked()", "PresenterMainFrame", this,
                          "logoutFromHistogramDB()");
  // Start
  const TGPicture* ed_executepic = picpool->GetPicture("ed_execute.png");
  m_startRefreshButton = new TGPictureButton(m_toolBar, ed_executepic,
                                             START_COMMAND);
  m_startRefreshButton->SetToolTipText("Start");
  m_startRefreshButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_startRefreshButton, 8);
  m_startRefreshButton->Connect("Clicked()", "PresenterMainFrame", this,
                                "startPageRefresh()");
  // Stop
  const TGPicture* ed_interruptpic = picpool->GetPicture("ed_interrupt.png");
  m_stopRefreshButton = new TGPictureButton(m_toolBar, ed_interruptpic,
                                            START_COMMAND);
  m_stopRefreshButton->SetToolTipText("Stop");
  m_stopRefreshButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_stopRefreshButton, 0);
  m_stopRefreshButton->SetState(kButtonDisabled);
  m_stopRefreshButton->Connect("Clicked()", "PresenterMainFrame", this,
                               "stopPageRefresh()");

  // Clear Histos
  const TGPicture* clear16pic = picpool->GetPicture("clear16", clear16);
  m_clearHistoButton = new TGPictureButton(m_toolBar, clear16pic,
                                           CLEAR_HISTOS_COMMAND);
  m_clearHistoButton->SetToolTipText("Clear histograms");
  m_clearHistoButton->AllowStayDown(true);
  m_toolBar->AddButton(this, m_clearHistoButton, 0);
  m_clearHistoButton->Connect("Clicked()", "PresenterMainFrame", this,
                              "clearHistos()");
  // SetToggleButton(Bool_t)                              

  //UTGID selector (partition)
  m_partitionSelectorPopupMenu = new TGPopupMenu(gClient->GetRoot());
  m_partitionSelectorPopupMenu->AddEntry("refresh partitions", M_RefreshKnownPartitions);
  // Create a split button, the menu is adopted.
  m_partitionSelectorQuickButton = new TGSplitButton(m_toolBar,
                                                new TGHotString("lhcb"),
                                                m_partitionSelectorPopupMenu);
  m_toolBar->AddButton(this, (TGPictureButton*)m_partitionSelectorQuickButton, 8);
  m_partitionSelectorQuickButton->Connect("ItemClicked(Int_t)", "PresenterMainFrame",
                                     this, "handleCommand(Command)");

  //History mode
  m_presetTimePopupMenu = new TGPopupMenu(gClient->GetRoot());
  m_presetTimePopupMenu->AddEntry("preset file", M_Last_File);
  m_presetTimePopupMenu->AddEntry("set file", M_File_Picker);
  m_presetTimePopupMenu->AddSeparator();  
  m_presetTimePopupMenu->AddEntry("preset interval", M_Last_Interval);
  m_presetTimePopupMenu->AddEntry("last 1 hour", M_LAST_1_HOURS);
  m_presetTimePopupMenu->AddEntry("last 8 hours", M_LAST_8_HOURS);
  m_presetTimePopupMenu->AddEntry("set interval", M_IntervalPicker);
  
  const TGPicture* arrow_left = picpool->GetPicture("arrow_left.xpm");
  m_previousIntervalButton = new TGPictureButton(m_toolBar, arrow_left,
                                             M_Previous_Interval);
  m_previousIntervalButton->SetToolTipText("Previous interval");
  m_previousIntervalButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_previousIntervalButton, 8);
  m_previousIntervalButton->Connect("Clicked()", "PresenterMainFrame", this,
                                "previousInterval()");

  const TGPicture* arrow_right = picpool->GetPicture("arrow_right.xpm");
  m_nextIntervalButton = new TGPictureButton(m_toolBar, arrow_right,
                                             M_Next_Interval);
  m_nextIntervalButton->SetToolTipText("Next interval");
  m_nextIntervalButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_nextIntervalButton, 0);
  m_nextIntervalButton->Connect("Clicked()", "PresenterMainFrame", this,
                                "nextInterval()");                                

  // Create a split button, the menu is adopted.
  m_historyIntervalQuickButton = new TGSplitButton(m_toolBar,
                                                   new TGHotString("Last &Options"),
                                                   m_presetTimePopupMenu);
  m_toolBar->AddButton(this, (TGPictureButton*)m_historyIntervalQuickButton, 8);
  m_historyIntervalQuickButton->Connect("ItemClicked(Int_t)", "PresenterMainFrame",
                                        this, "handleCommand(Command)");
  m_historyIntervalQuickButton->SetState(kButtonDisabled);

  // History plots
  m_historyPlotsButton= new TGPictureButton(m_toolBar, m_stockNewFormula,
                                             HISTORY_PLOTS_COMMAND);
  m_historyPlotsButton->SetToolTipText("Toggle history plot merge or trend mode");
  m_historyPlotsButton->AllowStayDown(true);
  m_toolBar->AddButton(this, m_historyPlotsButton, 8);
  m_historyPlotsButton->Connect("Clicked()", "PresenterMainFrame",
                                this, "toggleHistoryPlots()");
  m_historyPlotsButton->SetState(kButtonDisabled);
  // SetToggleButton(Bool_t)

  // Reference
  const TGPicture* f2_tpic = picpool->GetPicture("f2_t.xpm");
  m_overlayReferenceHistoButton = new TGPictureButton(m_toolBar, f2_tpic,
                                             OVERLAY_REFERENCE_HISTO_COMMAND);
  m_overlayReferenceHistoButton->SetToolTipText("Overlay with reference");
  m_overlayReferenceHistoButton->AllowStayDown(true);
  m_toolBar->AddButton(this, m_overlayReferenceHistoButton, 8);
  m_overlayReferenceHistoButton->Connect("Clicked()", "PresenterMainFrame",
                                         this, "toggleReferenceOverlay()");
  m_overlayReferenceHistoButton->SetState(kButtonDisabled);
  // SetToggleButton(Bool_t) 

  // Set reference
  const TGPicture* f1_tpic = picpool->GetPicture("f1_t.xpm");
  m_pickReferenceHistoButton = new TGPictureButton(m_toolBar, f1_tpic,
                                                  PICK_REFERENCE_HISTO_COMMAND);
  m_pickReferenceHistoButton->SetToolTipText("Pick reference histogram");
  m_pickReferenceHistoButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_pickReferenceHistoButton, 0);
  m_pickReferenceHistoButton->SetState(kButtonDisabled);
  m_pickReferenceHistoButton->Connect("Clicked()", "PresenterMainFrame",
                                      this, "pickReferenceHistogram()");
//  m_pickReferenceHistoButton->SetState(kButtonDisabled);

  // Clone Histo
  const TGPicture* inspectHistogram16pic = picpool->GetPicture("inspectHistogram16",
                                                               inspectHistogram16);
  m_inspectHistoButton = new TGPictureButton(m_toolBar, inspectHistogram16pic,
                                             INSPECT_HISTO_COMMAND);
  m_inspectHistoButton->SetToolTipText("Inspect histogram");
  m_inspectHistoButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_inspectHistoButton, 8);
  m_inspectHistoButton->Connect("Clicked()", "PresenterMainFrame",
                                this, "inspectHistogram()");

  // Histo Description
  const TGPicture* histogramDescription16pic = picpool->GetPicture("about.xpm");
  m_histogramDescriptionButton = new TGPictureButton(m_toolBar, histogramDescription16pic,
                                             HISTOGRAM_DESCRIPTION_COMMAND);
  m_histogramDescriptionButton->SetToolTipText("Histogram description");
  m_histogramDescriptionButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_histogramDescriptionButton, 0);
  m_histogramDescriptionButton->Connect("Clicked()", "PresenterMainFrame",
                                this, "histogramDescription()");
                                                                

  // Edit Histogram properties
  const TGPicture* editProperties16pic = picpool->GetPicture("editProperties16",
                                                             editProperties16);
  m_editHistoButton = new TGPictureButton(m_toolBar, editProperties16pic,
                                          EDIT_HISTO_COMMAND);
  m_editHistoButton->SetToolTipText("Edit histogram properties");
  m_editHistoButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_editHistoButton, 0);
  m_editHistoButton->Connect("Clicked()", "PresenterMainFrame",
                             this, "editHistogramProperties()");

  // Delete histo from Canvas
  const TGPicture* editDelete16pic = picpool->GetPicture("editDelete16",
                                                         editDelete16);
  m_deleteHistoFromCanvasButton = new TGPictureButton(m_toolBar,
                                                      editDelete16pic,
                                                      REMOVE_HISTO_FROM_CANVAS_COMMAND);
  m_deleteHistoFromCanvasButton->SetToolTipText("Remove selected histogram from page");
  m_deleteHistoFromCanvasButton->AllowStayDown(false);
  m_toolBar->AddButton(this, m_deleteHistoFromCanvasButton, 8);
  m_deleteHistoFromCanvasButton->Connect("Clicked()", "PresenterMainFrame",
                                         this, "deleteSelectedHistoFromCanvas()");

  m_toolBarDock->AddFrame(m_toolBar,
                          new TGLayoutHints(kLHintsTop |
                                            kLHintsExpandX,
                                            0, 0, 0, 0));

  m_mainHorizontalFrame = new TGHorizontalFrame(this, 544, 498,
                                                kHorizontalFrame |
                                                kLHintsExpandX |
                                                kLHintsExpandY);

    m_databasePagesDock = new TGDockableFrame(m_mainHorizontalFrame);
    m_databasePagesDock->SetWindowName("Database Page Browser");
    m_databasePagesDock->EnableUndock(true);
    m_databasePagesDock->EnableHide(false);
    m_databasePagesDock->Resize(220, 494);

    TGGroupFrame* databasePagesGroupFrame = new TGGroupFrame(m_databasePagesDock,
                                                             TGString("Database Page Browser"),
                                                             kVerticalFrame |
                                                             kFixedHeight);
    m_databasePagesDock->AddFrame(databasePagesGroupFrame,
                               new TGLayoutHints(kLHintsTop |
                                                 kLHintsExpandX |
                                                 kLHintsExpandY,
                                                 0, 0, 0, 0));

    TGCanvas* fCanvas652 = new TGCanvas(databasePagesGroupFrame, 220, 243);
    m_pagesFromHistoDBViewPort = fCanvas652->GetViewPort();

    // list tree
    m_pagesFromHistoDBListTree = new TGListTree(fCanvas652, kHorizontalFrame);
    m_pagesFromHistoDBListTree->AddRoot("Pages");
    m_pagesFromHistoDBListTree->SetCheckMode(TGListTree::kRecursive);
    (m_pagesFromHistoDBListTree->GetFirstItem())->SetPictures(m_databaseSourceIcon,
                                                              m_databaseSourceIcon);
    m_pagesFromHistoDBListTree->Connect(
      "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", "PresenterMainFrame",
      this, "clickedPageTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");

    m_pagesFromHistoDBViewPort->AddFrame(m_pagesFromHistoDBListTree);
    m_pagesFromHistoDBListTree->SetLayoutManager(
      new TGHorizontalLayout(m_pagesFromHistoDBListTree));
    m_pagesFromHistoDBListTree->MapSubwindows();
    fCanvas652->SetContainer(m_pagesFromHistoDBListTree);
    fCanvas652->MapSubwindows();
    databasePagesGroupFrame->AddFrame(fCanvas652,
                                      new TGLayoutHints(kLHintsRight |
                                                        kLHintsBottom |
                                                        kLHintsExpandX |
                                                        kLHintsExpandY,
                                                        0, 0, 0, 0));

    m_pagesContextMenu = new TGPopupMenu(fClient->GetRoot());
    m_pagesContextMenu->AddEntry("Load Page", M_LoadPage_COMMAND);
    m_pagesContextMenu->AddSeparator();
    m_pagesContextMenu->AddEntry("Delete Page", M_DeletePage_COMMAND);
    m_pagesContextMenu->AddEntry("Delete Folder", M_DeleteFolder_COMMAND);
    m_pagesContextMenu->AddSeparator();
    m_pagesContextMenu->AddEntry("Refresh", M_RefreshDBPagesListTree_COMMAND);
    m_pagesContextMenu->Connect("Activated(Int_t)", "PresenterMainFrame",
                                this, "handleCommand(Command)");

  TGVSplitter* leftVerticalSplitter = new TGVSplitter(m_mainHorizontalFrame,
                                                      2, 2);
  leftVerticalSplitter->SetFrame(m_databasePagesDock, true);

  m_pageDock = new TGDockableFrame(m_mainHorizontalFrame);
  m_pageDock->SetWindowName("LHCb Presenter Page");
  m_pageDock->EnableUndock(true);
  m_pageDock->EnableHide(false);
  m_pageDock->Resize(600, 494);
  m_pageDock->Connect("Docked()", "PresenterMainFrame",
                      this, "enablePageUndocking()");
  m_pageDock->Connect("Undocked()", "PresenterMainFrame",
                      this, "disablePageUndocking()");
  // centralFrame
  TGVerticalFrame* centralPageFrame = new TGVerticalFrame(m_pageDock,
                                                          600, 494,
                                                          kVerticalFrame);
  m_pageDock->AddFrame(centralPageFrame,
                       new TGLayoutHints(kLHintsLeft | kLHintsTop |
                                         kLHintsExpandX | kLHintsExpandY,
                                         0, 0, 0, 0));
  int partsTop[] = {75, 25};
  m_statusBarTop = new TGStatusBar(centralPageFrame, 50, 10, kVerticalFrame);
  m_statusBarTop->SetParts(partsTop, 2);
  m_statusBarTop->Draw3DCorner(false);
  centralPageFrame->AddFrame(m_statusBarTop, new TGLayoutHints(kLHintsTop |
                                                               kLHintsExpandX,
                                                               0, 0, 0, 0));

  // embedded canvas
  editorEmbCanvas = new TRootEmbeddedCanvas(0, centralPageFrame, 600, 494);
  int wm_editorEmbCanvas = editorEmbCanvas->GetCanvasWindowId();
  editorEmbCanvas->SetBit(kNoContextMenu);
  editorCanvas = new TCanvas("editor canvas", 600, 494, wm_editorEmbCanvas);
  editorCanvas->Connect("ProcessedEvent(Int_t, Int_t, Int_t, TObject*)",
                        "PresenterMainFrame", this,
                        "EventInfo(Int_t, Int_t, Int_t, TObject*)");
//  editorCanvas->SetGrid();
  editorCanvas->SetBit(kNoContextMenu);

//  editorCanvas->SetFixedAspectRatio(true);
  editorEmbCanvas->AdoptCanvas(editorCanvas);
  centralPageFrame->AddFrame(editorEmbCanvas, new TGLayoutHints(kLHintsRight |
                                                                kLHintsTop |
                                                                kLHintsExpandX |
                                                                kLHintsExpandY,
                                                                0, 0, 0, 0));
// Info dock
  m_mainCanvasInfoDock = new TGDockableFrame(centralPageFrame);
//  m_pageDock->SetWindowName("LHCb Presenter Page");
  m_mainCanvasInfoDock->SetWindowName("LHCb Presenter Information");

  m_mainCanvasInfoDock->EnableUndock(true);
  m_mainCanvasInfoDock->EnableHide(true);
  m_mainCanvasInfoDock->Resize(600, 90);


  TGGroupFrame* mainCanvasInfoGroupFrame = new TGGroupFrame(m_mainCanvasInfoDock,
                                                           TGString("Page Comments"),
                                                           kVerticalFrame |
                                                           kFixedHeight);
  m_mainCanvasInfoDock->AddFrame(mainCanvasInfoGroupFrame,
                             new TGLayoutHints(kLHintsTop |
                                               kLHintsExpandX|
                                               kLHintsExpandY,
                                               0, 0, 0, 0));

//  TGCanvas* fCanvas652 = new TGCanvas(mainCanvasInfoGroupFrame, 220, 243);
  

  centralPageFrame->AddFrame(m_mainCanvasInfoDock,
                             new TGLayoutHints(kLHintsRight |
                                               kLHintsTop |
                                               kLHintsExpandX,
                                               0, 0, 0, 0));





//    TGTab* fTab515 = new TGTab(m_mainCanvasInfoDock, 600, 90);
    // container of "Page Description"
//    TGCompositeFrame* fCompositeFrame518;
//    fCompositeFrame518 = fTab515->AddTab("Page Description");
//    fCompositeFrame518->SetLayoutManager(new TGVerticalLayout(fCompositeFrame518));
//    fCompositeFrame518->SetLayoutBroken(true);
//    TGTextEdit* fTextEdit523 = new TGTextEdit(fCompositeFrame518, 600, 80);

//   TGTextEdit *fTextEdit515 = new TGTextEdit(fMainFrame648,486,62);
//   fTextEdit515->LoadFile("TxtEdit515");
//   fMainFrame648->AddFrame(fTextEdit515, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX,2,2,2,2));
//   fTextEdit515->MoveResize(2,2,486,62);


    m_pageDescriptionView = new TGTextView(mainCanvasInfoGroupFrame, 600, 80);
    //fTextEdit523->LoadFile("TxtEdit523");
    mainCanvasInfoGroupFrame->AddFrame(m_pageDescriptionView, new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsExpandX | kLHintsExpandY,2,2,2,2));
//    fTextEdit523->Layout();
                                                                 
//    fTextEdit523->MoveResize(2, 2, 600, 80);
    // container of "Tab2"
//    TGCompositeFrame* fCompositeFrame520;
//    fCompositeFrame520 = fTab515->AddTab("Message Log");
//    //fCompositeFrame520->SetLayoutManager(new TGVerticalLayout(fCompositeFrame520));
//    fCompositeFrame520->SetLayoutBroken(true);
//    TGTextEdit* fTextEdit542 = new TGTextEdit(fCompositeFrame520, 400, 95);
//    //fTextEdit542->LoadFile("TxtEdit542");
//    fCompositeFrame520->AddFrame(fTextEdit542,
//                                 new TGLayoutHints(kLHintsLeft |
//                                                   kLHintsBottom |
//                                                   kLHintsExpandX,
//                                                   2, 2, 2, 2));
//    //fTextEdit542->MoveResize(0, 0, 290, 77);
//    fTab515->SetTab(1);
//    fTab515->Resize(fTab515->GetDefaultSize());
//    m_mainCanvasInfoDock->AddFrame(fTab515, new TGLayoutHints(kLHintsRight |
//                                                              kLHintsTop |
//                                                              kLHintsExpandX,
//                                                              2, 2, 2, 2));
    //fTab515->MoveResize(64, 40, 296, 104);

  m_rightMiscFrame = new TGVerticalFrame(m_mainHorizontalFrame, 220, 494,
                                         kVerticalFrame | kFixedWidth |
                                         kLHintsExpandX);
//                                         | kLHintsExpandX); // kFixedWidth

  m_rightVerticalSplitter = new TGVSplitter(m_mainHorizontalFrame, 2, 2);
    m_rightVerticalSplitter->SetFrame(m_rightMiscFrame, false);

    m_dimBrowserDock = new TGDockableFrame(m_rightMiscFrame);
    m_dimBrowserDock->SetWindowName("DIM Histogram Browser");
    m_dimBrowserDock->EnableUndock(true);
    m_dimBrowserDock->EnableHide(false);
    m_dimBrowserDock->Resize(220, 100);
    m_rightMiscFrame->AddFrame(m_dimBrowserDock,
                               new TGLayoutHints(kLHintsRight |
                                                 kLHintsTop |
                                                 kLHintsExpandX |
                                                 kLHintsExpandY,
                                                 0, 0, 0, 0));

    m_histoSvcBrowserGroupFrame = new TGGroupFrame(m_dimBrowserDock,
                                            TGString("DIM Histogram Browser"),
                                            kVerticalFrame | kFixedHeight);
    m_dimBrowserDock->AddFrame(m_histoSvcBrowserGroupFrame,
                               new TGLayoutHints(kLHintsTop |
                                                 kLHintsExpandX |
                                                 kLHintsExpandY,
                                                 0, 0, 0, 0));

    m_dimSvcListTreeContainterCanvas = new TGCanvas(m_histoSvcBrowserGroupFrame,
                                                    220, 100);
    TGViewPort* fViewPort664 = m_dimSvcListTreeContainterCanvas->GetViewPort();
    m_histoSvcListTree = new TGListTree(m_dimSvcListTreeContainterCanvas,
                                      kHorizontalFrame);
  //  m_histoSvcListTree->SetAutoCheckBoxPic(false); ROOT v5.16(?)...
    m_histoSvcListTree->AddRoot("DIM");
    (m_histoSvcListTree->GetFirstItem())->SetPictures(m_dimOnline16,
                                                    m_dimOnline16);
    m_histoSvcListTree->SetCheckMode(TGListTree::kRecursive);                                                    
    m_histoSvcListTree->Connect(
      "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", "PresenterMainFrame",
      this, "clickedHistoSvcTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");

    fViewPort664->AddFrame(m_histoSvcListTree);
    m_histoSvcListTree->SetLayoutManager(new TGHorizontalLayout(
                                         m_histoSvcListTree));
    m_histoSvcListTree->MapSubwindows();
    m_dimSvcListTreeContainterCanvas->SetContainer(m_histoSvcListTree);
    m_dimSvcListTreeContainterCanvas->MapSubwindows();
    m_histoSvcBrowserGroupFrame->AddFrame(m_dimSvcListTreeContainterCanvas,
      new TGLayoutHints(kLHintsRight | kLHintsTop |
                        kLHintsExpandX | kLHintsExpandY,
                        0, 0, 0, 0));

    m_histoSvcTreeContextMenu = new TGPopupMenu(fClient->GetRoot());
    m_histoSvcTreeContextMenu->AddEntry("Add checked to Database",
                               M_AddHistoToDB_COMMAND);
    m_histoSvcTreeContextMenu->AddEntry("Add checked to Page",
                               M_AddHistoToPage_COMMAND);
    m_histoSvcTreeContextMenu->AddSeparator();
    m_histoSvcTreeContextMenu->AddEntry("Resubscribe to checked",
                               M_SetDimSourceFromTree);
    m_histoSvcTreeContextMenu->AddSeparator();
    m_histoSvcTreeContextMenu->AddEntry("Collsapse all children",
                               M_DimCollapseAllChildren_COMMAND);
    m_histoSvcTreeContextMenu->AddSeparator();
    m_histoSvcTreeContextMenu->AddEntry("Refresh", M_RefreshHistoSvcListTree);
    m_histoSvcTreeContextMenu->Connect("Activated(Int_t)","PresenterMainFrame", this,
                              "handleCommand(Command)");

    m_databaseHistogramsDock = new TGDockableFrame(m_rightMiscFrame);
    m_databaseHistogramsDock->SetWindowName("Histograms in Database");
    m_databaseHistogramsDock->EnableUndock(true);
    m_databaseHistogramsDock->EnableHide(false);
    m_databaseHistogramsDock->Resize(220, 100);
    m_rightMiscFrame->AddFrame(m_databaseHistogramsDock,
                               new TGLayoutHints(kLHintsRight |
                                                 kLHintsBottom |
                                                 kLHintsExpandX |
                                                 kLHintsExpandY |
                                                 kFixedWidth,
                                                 0, 0, 0, 0));
      // databaseHistogramFrame
  TGVerticalFrame* leftDatabaseHistogramFrame = new TGVerticalFrame(m_databaseHistogramsDock,
                                                                    220, 100,
                                                                    kVerticalFrame |
                                                                    kFixedWidth);
//TGGroupFrame* leftDatabaseHistogramFrame = new TGGroupFrame(m_databaseHistogramsDock, kVerticalFrame | kFixedWidth);
  TGGroupFrame* databaseHistogramGroupFrame = new TGGroupFrame(leftDatabaseHistogramFrame,
                                                               TGString("Histograms in Database"),
                                                               kVerticalFrame |
                                                               kFixedWidth);
//TGGroupFrame(m_databaseHistogramsDock, TGString("Histogram Database"), kVerticalFrame | kFixedWidth);

      leftDatabaseHistogramFrame->AddFrame(databaseHistogramGroupFrame,
                                           new TGLayoutHints(kLHintsTop |
                                                             kLHintsExpandX |
                                                             kLHintsExpandY,
                                                             0, 0, 0, 0));

      m_databaseHistogramsDock->AddFrame(leftDatabaseHistogramFrame,
                                         new TGLayoutHints(kLHintsTop |
                                                           kLHintsExpandX |
                                                           kLHintsExpandY,
                                                           0, 0, 0, 0));
        // Database histogram filter box
        m_histoDBFilterComboBox = new TGComboBox(databaseHistogramGroupFrame, -1,
                                                 kHorizontalFrame | kDoubleBorder |
                                                 kSunkenFrame | kOwnBackground);
        m_histoDBFilterComboBox->AddEntry("Folder/Page", FoldersAndPages); // #0
        m_histoDBFilterComboBox->AddEntry("Tasks/Algorithm", Tasks); // #1
        m_histoDBFilterComboBox->AddEntry("Subsystems", Subsystems); // #2
        m_histoDBFilterComboBox->AddEntry("With auto analysis",
                                          HistogramsWithAnalysis); //#3
        m_histoDBFilterComboBox->AddEntry("Produced by analysis",
                                          AnalysisHistograms); //#4
        m_histoDBFilterComboBox->AddEntry("Full list", AllHistograms); // #5
        // @enum FilterCriteria
        m_histoDBFilterComboBox->Resize(149, 22);
        m_histoDBFilterComboBox->Select(Tasks);
        m_histoDBFilterComboBox->Connect("Selected(Int_t)", "PresenterMainFrame",
                                         this, "refreshHistoDBListTree()");
        databaseHistogramGroupFrame->AddFrame(m_histoDBFilterComboBox,
                                              new TGLayoutHints(kLHintsLeft |
                                                                kLHintsTop |
                                                                kLHintsExpandX,
                                                                0, 0, 0, 0));
        // canvas widget
        m_histoDBCanvas = new TGCanvas(databaseHistogramGroupFrame, 124, 460);

        // canvas viewport
        m_histoDBCanvasViewPort = m_histoDBCanvas->GetViewPort();

        // list tree
        m_databaseHistogramTreeList = new TGListTree(m_histoDBCanvas,
                                                     kHorizontalFrame);
        m_databaseHistogramTreeList->AddRoot("Histograms");
        m_databaseHistogramTreeList->SetCheckMode(TGListTree::kRecursive);
        m_databaseHistogramTreeList->Connect(
          "Clicked(TGListTreeItem*, Int_t, Int_t, Int_t)", "PresenterMainFrame",
          this, "clickedHistoDBTreeItem(TGListTreeItem*, Int_t, Int_t, Int_t)");

        m_histoDBContextMenu = new TGPopupMenu(fClient->GetRoot());
        m_histoDBContextMenu->AddEntry("Add checked histogram(s) to Page",
                                       M_AddDBHistoToPage_COMMAND);
        m_histoDBContextMenu->AddEntry("Set properties of checked histogram(s)",
                                       M_SetHistoPropertiesInDB_COMMAND);
        m_histoDBContextMenu->AddEntry("Delete checked histogram(s)",
                                       M_DeleteDBHisto_COMMAND);
        m_histoDBContextMenu->AddSeparator();
        m_histoDBContextMenu->AddEntry("Collsapse all children",
                                       M_DBHistoCollapseAllChildren_COMMAND);
        m_histoDBContextMenu->AddSeparator();
        m_histoDBContextMenu->AddEntry("Refresh",
          M_RefreshHistoDBListTree_COMMAND);
        m_histoDBContextMenu->Connect("Activated(Int_t)","PresenterMainFrame",
                                      this, "handleCommand(Command)");
//m_histoDBContextMenu->DisableEntry(M_SetHistoPropertiesInDB_COMMAND);

        m_histoDBCanvasViewPort->AddFrame(m_databaseHistogramTreeList);
        m_databaseHistogramTreeList->SetLayoutManager(
          new TGHorizontalLayout(m_databaseHistogramTreeList));
        m_databaseHistogramTreeList->MapSubwindows();
        m_histoDBCanvas->SetContainer(m_databaseHistogramTreeList);
        m_histoDBCanvas->MapSubwindows();
        databaseHistogramGroupFrame->AddFrame(m_histoDBCanvas,
                                              new TGLayoutHints(kLHintsLeft |
                                                                kLHintsTop |
                                                                kLHintsExpandX |
                                                                kLHintsExpandY,
                                                                0, 0, 0, 0));

  m_mainHorizontalFrame->AddFrame(m_databasePagesDock,
                                  new TGLayoutHints(kLHintsLeft |
                                                    kLHintsExpandY,
                                                    0, 0, 0, 0));

  m_mainHorizontalFrame->AddFrame(leftVerticalSplitter,
                                  new TGLayoutHints(kLHintsLeft |
                                                    kLHintsExpandY));
  m_mainHorizontalFrame->AddFrame(m_pageDock,
                                  new TGLayoutHints(kLHintsLeft |
                                                    kLHintsExpandX |
                                                    kLHintsExpandY,
                                                    0, 0, 0, 0));
  m_mainHorizontalFrame->AddFrame(m_rightMiscFrame,
                                  new TGLayoutHints(kLHintsRight |
                                                    kLHintsExpandY,
                                                    0, 0, 0, 0));
  m_mainHorizontalFrame->AddFrame(m_rightVerticalSplitter,
                                  new TGLayoutHints(kLHintsRight |
                                                    kLHintsExpandY));
  int parts[] = {35, 25, 40};
  m_mainStatusBar = new TGStatusBar(this, 50, 10, kVerticalFrame);
    m_mainStatusBar->SetParts(parts, 3);
    m_mainStatusBar->SetText("Launching Presenter...", 2);

  AddFrame(m_menuDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
  AddFrame(m_toolBarDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
  AddFrame(m_mainHorizontalFrame, new TGLayoutHints(kLHintsExpandX |
                                                    kLHintsExpandY));
  AddFrame(m_mainStatusBar, new TGLayoutHints(kLHintsBottom | kLHintsExpandX,
                                              0, 0, 0, 0));

  m_menuDock->MapWindow();
  m_toolBarDock->MapWindow();
  m_databaseHistogramsDock->MapWindow();
  m_pageDock->MapWindow();
  m_dimBrowserDock->MapWindow();
  m_databasePagesDock->MapWindow();
//  m_mainCanvasInfoDock->MapWindow();

  SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputPrimaryApplicationModal);
  MapSubwindows();
  MapWindow();
  m_rightMiscFrame->UnmapWindow();
  m_rightVerticalSplitter->UnmapWindow();
  Resize();
  DoRedraw();
}
void PresenterMainFrame::CloseWindow()
{
  if (0 != m_pageRefreshTimer) { m_pageRefreshTimer->Stop(); }
  if (0 != m_clockTimer) { m_clockTimer->Stop(); }
  gApplication->Terminate();
}
void PresenterMainFrame::dockAllFrames()
{
  // TODO: FindObject IsA TGDockableFrame...
  m_pageDock->DockContainer();
//  m_mainCanvasInfoDock->DockContainer();
  m_toolBarDock->DockContainer();
  m_menuDock->DockContainer();
  m_databaseHistogramsDock->DockContainer();
  m_dimBrowserDock->DockContainer();
  m_databasePagesDock->DockContainer();
}
void PresenterMainFrame::handleCommand(Command cmd)
{
  // ROOT GUI workaround: slot mechanism differs on different signals from
  // different widgets... void* 4 messages :-(
  if (X_ENIGMA_COMMAND == cmd) {
    TGButton* btn = static_cast<TGButton*>(gTQSender);
    cmd = static_cast<Command>(btn->WidgetId());
  }  
  if (500 <= cmd < 550) {
    setHistogramDimSource(s_withoutTree);
  }
  switch (cmd) {
    case EXIT_COMMAND:
      CloseWindow();
      break;
    case START_COMMAND:
      startPageRefresh();
      break;
    case STOP_COMMAND:
      stopPageRefresh();
      break;
    case CLEAR_HISTOS_COMMAND:
      clearHistos();
      break;
    case CLEAR_PAGE_COMMAND:
      removeHistogramsFromPage();
      break;
    case SAVE_PAGE_TO_DB_COMMAND:
      savePageToHistogramDB();
      break;
//    case SAVE_PAGE_TO_FILE_COMMAND:
//      savePageToFile();
//      break;
    case M_Previous_Interval:
      previousInterval();
      break;
    case M_Next_Interval:
      nextInterval();
      break;
    case LOGOUT_COMMAND:
      logoutFromHistogramDB();
      break;
    case LOGIN_COMMAND:
      loginToHistogramDB();
      break;
    case INSPECT_HISTO_COMMAND:
      inspectHistogram();
      break;
    case HISTOGRAM_DESCRIPTION_COMMAND:
      histogramDescription();
      break;      
    case INSPECT_PAGE_COMMAND:
      inspectPage();
      break;
    case AUTO_LAYOUT_COMMAND:
      autoCanvasLayout();
      break;
    case EDIT_HISTO_COMMAND:
      editHistogramProperties();
      break;
    case REMOVE_HISTO_FROM_CANVAS_COMMAND:
      deleteSelectedHistoFromCanvas();
      break;
    case UNDOCK_PAGE_COMMAND:
      m_pageDock->UndockContainer();
      break;
    case OFFLINE_MODE_COMMAND:
      setPresenterMode(History);
      break;
    case ONLINE_MODE_COMMAND:
      setPresenterMode(Online);
      break;
    case PAGE_EDITOR_ONLINE_MODE_COMMAND:
      setPresenterMode(EditorOnline);
      break;
    case PAGE_EDITOR_OFFLINE_MODE_COMMAND:
      setPresenterMode(EditorOffline);
      break;      
    case DOCK_ALL_COMMAND:
      dockAllFrames();
      break;
    case M_AddHistoToDB_COMMAND:
      addDimSvcToHistoDB();
      break;
    case M_AddHistoToPage_COMMAND:
      addDimSvcToPage();
      break;
    case M_SetDimSourceFromTree:
      setHistogramDimSource(s_withTree);
      break;
    case M_SetDimSourceFromQuickButton:
      setHistogramDimSource(s_withoutTree);
      break;      
    case M_DimCollapseAllChildren_COMMAND:
      dimCollapseAllChildren();
      break;
    case M_RefreshHistoSvcListTree:
      refreshHistogramSvcList(s_withTree);
      refreshPartitionSelectorPopupMenu();
      break;
    case M_RefreshKnownPartitions:
      refreshPartitionSelectorPopupMenu();
      break;
    case M_AddDBHistoToPage_COMMAND:
        addDbHistoToPage();
      break;
    case M_DBHistoCollapseAllChildren_COMMAND:
      dbHistoCollapseAllChildren();
      break;
    case M_DeleteDBHisto_COMMAND:
      deleteSelectedHistoFromDB();
      break;
    case M_SetHistoPropertiesInDB_COMMAND:
      setHistogramPropertiesInDB();
      break;
    case M_RefreshHistoDBListTree_COMMAND:
      refreshHistoDBListTree();
      m_histogramDB->refresh();
      break;
    case M_RefreshDBPagesListTree_COMMAND:
      refreshPagesDBListTree();
      m_histogramDB->refresh();
      break;
    case M_IntervalPicker:
      m_intervalPicker->MapWindow();
      m_previousIntervalButton->SetState(kButtonEngaged);
      m_previousIntervalButton->SetState(kButtonUp);
      m_nextIntervalButton->SetState(kButtonEngaged);
      m_nextIntervalButton->SetState(kButtonUp);
      break;
    case M_Last_Interval:
      m_previousIntervalButton->SetState(kButtonEngaged);
      m_previousIntervalButton->SetState(kButtonUp);
      m_nextIntervalButton->SetState(kButtonEngaged);
      m_nextIntervalButton->SetState(kButtonUp);
      loadSelectedPageFromDB(global_timePoint, global_pastDuration);
      break;      
    case M_LoadPage_COMMAND:
    case M_LAST_1_HOURS:
    case M_LAST_8_HOURS:
    case M_Last_File: {
      if (Online == m_presenterMode ||
          (EditorOffline == m_presenterMode && !canWriteToHistogramDB())) {
        //Bug workaround: maybe fixed in some ROOT > 5.18.00
//        m_historyIntervalQuickButton->SetState(kButtonDisabled);
      }
      m_previousIntervalButton->SetState(kButtonDisabled);
      m_nextIntervalButton->SetState(kButtonDisabled);
       
      loadSelectedPageFromDB(s_startupFile, m_savesetFileName);
    }
      break;
    case M_File_Picker: {
        TGFileInfo fileInfo;
        const char* fileTypes[] = {"ROOT files", "*.root",
                                   "All files", "*",
                                   0 , 0};
        fileInfo.fFileTypes = fileTypes;
        fileInfo.fIniDir = StrDup(m_savesetPath.c_str());
        fileInfo.SetMultipleSelection(false);
// if (true == fileInfo.fMultipleSelection) { TList 1st element};
        new TGFileDialog(gClient->GetRoot(),this,kFDOpen,&fileInfo);
//        if(!fileInfo.fFilename) {        
          m_savesetFileName = std::string(fileInfo.fFilename);
          m_previousIntervalButton->SetState(kButtonDisabled);
          m_nextIntervalButton->SetState(kButtonDisabled);
// std::cout << "selected m_savesetFileName: " << m_savesetFileName << std::endl;                  
          loadSelectedPageFromDB(s_startupFile, m_savesetFileName);
//        }
    }
      break;      
    case M_DeletePage_COMMAND:
      deleteSelectedPageFromDB();
      break;
    case M_DeleteFolder_COMMAND:
      deleteSelectedFolderFromDB();
      break;
    case HELP_ABOUT_COMMAND:
      about();
      break;
    case PICK_REFERENCE_HISTO_COMMAND:
      pickReferenceHistogram();
      break;
    case HISTORY_PLOTS_COMMAND:
      toggleHistoryPlots();
      break;
    case OVERLAY_REFERENCE_HISTO_COMMAND:      
      toggleReferenceOverlay();
      break;
    case FAST_HITMAP_DRAW_COMMAND:      
      toggleFastHitMapDraw();
      break;            
    case SAVE_AS_REFERENCE_HISTO_COMMAND:      
      saveSelectedHistogramAsReference();
      break;
    default:
      if (m_verbosity >= Debug) {
        std::cout << "zut. TGButton WidgetId from gTQSender is corrupt"
          << std::endl;
      }
      break;
  }
}
void PresenterMainFrame::setVerbosity(const pres::MsgLevel & verbosity)
{
  m_verbosity = verbosity;
  if (0 != m_archive) { m_archive->setVerbosity(m_verbosity); }
}
void PresenterMainFrame::setTitleFontSize(int fontSize)
{
  // http://root.cern.ch/phpBB2/viewtopic.php?t=3991
  Float_t doesntWorkEither = fontSize;
  gStyle->SetTitleFontSize(doesntWorkEither);
}
void PresenterMainFrame::setDatabaseMode(const DatabaseMode & databaseMode) {
  switch (databaseMode) {
    case LoggedOut:
      logoutFromHistogramDB();
      break;
    case UserSelected:
      logoutFromHistogramDB();
      loginToHistogramDB();
      break;
    case ReadOnly:
      logoutFromHistogramDB();
      connectToHistogramDB(s_histReaderPw,
                           s_histReader.Data(),
                           s_histdb.Data());
      break;
    case ReadWrite:
      logoutFromHistogramDB();
      loginToHistogramDB();
      break;
    default:
      if (m_verbosity >= Debug) {
        std::cout << "something went wrong when setting database mode."
                  << std::endl;
      }
      break;
  }
}
void PresenterMainFrame::setReferencePath(const std::string & referencePath) {
  m_referencePath = referencePath;
  if (m_analysisLib) m_analysisLib->setRefRoot(m_referencePath);
  if (!m_referencePath.empty()) {
    if (0 != m_archive) {
      m_archive->setReferencePath(m_referencePath);
    }    
  }
}
void PresenterMainFrame::setSavesetPath(const std::string & savesetPath) {
  m_savesetPath = savesetPath;
  if (!m_savesetPath.empty()) {
    if (0 != m_archive) {
      m_archive->setSavesetPath(m_savesetPath);
    }
  }
}
void PresenterMainFrame::setPresenterMode(const PresenterMode & presenterMode)
{
  switch (presenterMode) {
    case Online:
      m_presenterMode = Online;
      break;
    case History:
      m_presenterMode = History;
      break;
    case EditorOnline:
      m_presenterMode = EditorOnline;
      if (ReadWrite != m_databaseMode) { setDatabaseMode(ReadWrite); }
      break;
    case EditorOffline:
      m_presenterMode = EditorOffline;
      if (ReadWrite != m_databaseMode) { setDatabaseMode(ReadWrite); }
      break;      
    default:    
      if (m_verbosity >= Debug) {
        std::cout << "something went wrong when setting presenter mode."
          << std::endl;
      }
      break;
  }
  reconfigureGUI();
}
void PresenterMainFrame::enableAutoCanvasLayoutBtn()
{
//  m_autoCanvasLayoutButton->SetState(kButtonEngaged);
  m_autoCanvasLayoutButton->SetState(kButtonUp);
}
void PresenterMainFrame::disableAutoCanvasLayoutBtn()
{
  m_autoCanvasLayoutButton->SetState(kButtonDisabled);
}
bool PresenterMainFrame::isConnectedToHistogramDB()
{
  if (LoggedOut == m_databaseMode && 0 == m_histogramDB) {
    return false;
  } else if (ReadOnly == m_databaseMode && 0 != m_histogramDB) {
    return true;
  } else if (ReadWrite == m_databaseMode && 0 != m_histogramDB) {
    return true;
  } else {
    return false;
  }
}
bool PresenterMainFrame::canWriteToHistogramDB()
{
  if (ReadWrite == m_databaseMode && 0 != m_histogramDB) {
    return true;
  } else {
    return false;
  }
}
void PresenterMainFrame::savePageToHistogramDB()
{
  if (ReadWrite == m_databaseMode) {
// TODO: map/unmap WaitForUnmap(TGWindow* w)    
    fClient->WaitFor(new PageSaveDialog(this, 493, 339, m_verbosity));
//  m_savePageToDatabaseButton->SetState(kButtonDisabled);

    refreshPagesDBListTree();
    refreshHistoDBListTree();
//    statusBar()
  }
}
// below is either HISTO DIM sevices, or a ROOT filename
void PresenterMainFrame::setStartupHistograms(const std::vector<std::string> & histogramList)
{
  std::vector<std::string>::const_iterator histogramListIt;
  histogramListIt = histogramList.begin();
  while (histogramListIt != histogramList.end()) {
    TString inputParamTS = TString(*histogramListIt);
    if (inputParamTS.EndsWith(s_rootFileExtension.c_str())) {
      m_savesetFileName = *histogramListIt;
//      m_rightMiscFrame->HideFrame(m_dimBrowserDock);
//      m_rightMiscFrame-> ShowFrame();
      
    } else {
      HistogramIdentifier histogram = HistogramIdentifier(*histogramListIt);
      if (histogram.isDimFormat()) {      
        addHistoToPage(*histogramListIt);
      }
    }
    histogramListIt++;
  }
  autoCanvasLayout();
}
bool PresenterMainFrame::connectToHistogramDB(const std::string & dbPassword,
                                              const std::string & dbUsername,
                                              const std::string & dbName)
{
//std::cout << dbPassword << " " <<   dbUsername << " " << dbName <<  std::endl;
  if ("" != dbPassword &&
      "" != dbUsername &&
      "" != dbUsername) {
    try {
      if (0 != m_histogramDB && isConnectedToHistogramDB()) {
        delete m_histogramDB;
        m_histogramDB = NULL;
      }

      m_histogramDB = new OnlineHistDB(dbPassword, dbUsername, dbName);

      // allocate vectors:
      m_histogramTypes.reserve(m_histogramDB->nHistograms());
      m_localDatabaseIDS.reserve(m_histogramDB->nHistograms());
      m_localDatabasePages.reserve(m_histogramDB->nPages());
      m_localDatabaseFolders.reserve(m_histogramDB->nPageFolders());

      m_histogramDB->setRefRoot(m_referencePath);
      m_histogramDB->setSavesetsRoot(m_savesetPath);
      if (0 != m_analysisLib) { delete m_analysisLib; m_analysisLib = NULL; }
      m_analysisLib = new OMAlib(m_histogramDB);
      m_analysisLib->setRefRoot(m_referencePath);
      if (0 != m_archive) { m_archive->setAnalysisLib(m_analysisLib); }

      m_dbName = dbName;
      m_mainStatusBar->SetText(dbUsername.c_str(), 2);

      m_message = "Successfully connected to OnlineHistDB.";
      m_mainStatusBar->SetText(m_message.c_str(), 2);

      if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }

//    statusBar()->message(tr("Successfully connected to OnlineHistDB."));
      m_histogramDB->setExcLevel(AllExceptions);
      m_histogramDB->setDebug(m_verbosity);

// TODO: move m_histogramDB setPaths fcts to Archive.
      if (!m_savesetPath.empty()) {
        if (0 != m_archive) { m_archive->setSavesetPath(m_savesetPath); }
      }
      if (!m_referencePath.empty()) {
        if (0 != m_archive) { m_archive->setReferencePath(m_referencePath); }
      }

      if (m_histogramDB->canwrite()) {
        m_databaseMode = ReadWrite;
      } else {
        m_databaseMode = ReadOnly;
      }
    } catch (std::string sqlException) {
      // TODO: add error logging backend - MsgStream?
      m_mainStatusBar->SetText(sqlException.c_str(), 2);
      if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }

      new TGMsgBox(fClient->GetRoot(), this, "Login Error",
                   Form("Could not connect to the OnlineHistDB server:\n\n%s\n"
                        "This program cannot function correctly",
                        sqlException.c_str()),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);

      //  TODO: disable/hideHistogramDatabaseTools();
      if (0 != m_histogramDB) { delete m_histogramDB; m_histogramDB = NULL; }
      m_databaseMode = LoggedOut;
    }
  } else {
    new TGMsgBox(fClient->GetRoot(), this, "Login Error",
                   Form("Could not connect to the OnlineHistDB server:\n"
                        "Please enter valid credentials."),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }
  return isConnectedToHistogramDB();
}
void PresenterMainFrame::loginToHistogramDB()
{
// TODO: map/unmap WaitForUnmap(TGWindow* w)
  fClient->WaitFor(new LoginDialog(this, 350, 310, m_databaseMode));

  //  histoDBFilterComboBox->HideFrame();
//  reconfigureGUI();
  //  TODO: refactor refresh* methods to use only 1 DB readout...
//  refreshPagesDBListTree();
//  refreshHistoDBListTree();

  reconfigureGUI();
  removeHistogramsFromPage();
}
void PresenterMainFrame::logoutFromHistogramDB()
{
  if (0 != m_histogramDB && isConnectedToHistogramDB()) {
    new TGMsgBox(fClient->GetRoot(), this, "Logout from Database",
                 "Do you really want to logout from Database?",
                 kMBIconQuestion, kMBYes | kMBNo, &m_msgBoxReturnCode);
    switch (m_msgBoxReturnCode) {
      case kMBNo:
        return;
    }
  }
  delete m_histogramDB;
  m_histogramDB = NULL;
  if (0 != m_analysisLib) { delete m_analysisLib; m_analysisLib = NULL; }

  m_databaseMode = LoggedOut;

//  removeHistogramsFromPage();
  m_message = "Disconnected from histogramDB";
  m_mainStatusBar->SetText(m_message.c_str(), 2);
  if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }

  reconfigureGUI();

  //  TODO: refactor refresh* methods to use only 1 DB readout...
  removeHistogramsFromPage();
}
void PresenterMainFrame::startPageRefresh()
{
  m_message = "Refresh started.";
  m_mainStatusBar->SetText(m_message.c_str(), 2);
  if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }

  m_stopRefreshButton->SetState(kButtonUp);
  m_viewMenu->EnableEntry(STOP_COMMAND);
  m_startRefreshButton->SetState(kButtonDisabled);
  m_viewMenu->DisableEntry(START_COMMAND);

  enablePageRefresh();
  m_pageRefreshTimer->TurnOn();
  m_refreshingPage = true;
}
void PresenterMainFrame::stopPageRefresh()
{
  disablePageRefresh();
  if (0 != m_pageRefreshTimer) {
    m_message = "Refresh stopped.";
    m_mainStatusBar->SetText(m_message.c_str(), 2);
    if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }
    if (History != m_presenterMode) {
      m_startRefreshButton->SetState(kButtonUp);
      m_viewMenu->EnableEntry(START_COMMAND);
    }
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(STOP_COMMAND);

    m_pageRefreshTimer->TurnOff();
    m_refreshingPage = false;
  }
}
void PresenterMainFrame::clearHistos()
{
  if (m_clearedHistos) {
    m_message = "Reverted to integral.";
    m_clearHistoButton->SetState(kButtonUp);
    m_viewMenu->UnCheckEntry(CLEAR_HISTOS_COMMAND);
    m_clearedHistos = false;
    disableHistogramClearing();
  } else {
    m_message = "Histograms cleared.";
    m_mainStatusBar->SetText(m_message.c_str(), 2);
    m_clearHistoButton->SetState(kButtonDown);
    m_clearedHistos = true;
    m_viewMenu->CheckEntry(CLEAR_HISTOS_COMMAND);
    enableHistogramClearing();
  }
  m_mainStatusBar->SetText(m_message.c_str(), 2);
  if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }
}
OMAlib* PresenterMainFrame::analysisLib()
{
  return m_analysisLib;
}
OnlineHistDB* PresenterMainFrame::histogramDB()
{
  if (!isConnectedToHistogramDB()) { loginToHistogramDB(); }
  return m_histogramDB;
}
void PresenterMainFrame::listFromHistogramDB(TGListTree* listView,
  const FilterCriteria & filterCriteria, bool histograms)
{
  if (isConnectedToHistogramDB() && 0 != listView) {
    try {
      listView->UnmapWindow();
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());

      m_treeNode = listView->GetFirstItem();
//      m_treeNode->SetPictures(m_databaseSourceIcon, m_databaseSourceIcon);
      deleteTreeChildrenItemsUserData(m_treeNode);
      listView->DeleteChildren(m_treeNode);
      listView->RenameItem(m_treeNode, TString(m_dbName));
      m_localDatabaseFolders.clear();
      m_localDatabaseIDS.clear();
      m_histogramTypes.clear();
      switch (filterCriteria) {
        case FoldersAndPages:
          m_histogramDB->getPageFolderNames(m_localDatabaseFolders, "_ALL_");
          break;
        case Subsystems:
          m_histogramDB->getSubsystems(m_localDatabaseFolders);
          break;
        case Tasks:
          m_histogramDB->getTasks(m_localDatabaseFolders);
          break;
        case HistogramsWithAnalysis:
          m_histogramDB->getHistogramsWithAnalysis(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
          break;
        case AnalysisHistograms:
          m_histogramDB->getAnalysisHistograms(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
          break;
        case AllHistograms:
          m_histogramDB->getAllHistograms(NULL,
            &m_localDatabaseIDS, &m_histogramTypes);
          break;
        default:
          break;
      }
      std::sort(m_localDatabaseFolders.begin(),
                m_localDatabaseFolders.end());

      if (filterCriteria == FoldersAndPages ||
          filterCriteria == Subsystems ||
          filterCriteria == Tasks) {
        for (m_folderIt = m_localDatabaseFolders.begin();
             m_folderIt != m_localDatabaseFolders.end(); ++m_folderIt) {
          m_folderItems = TString(*m_folderIt).Tokenize(s_slash);
          m_folderItemsIt = m_folderItems->MakeIterator();
          m_treeNode = listView->GetFirstItem();
          while (m_folderItem = m_folderItemsIt->Next()) {
            if (listView->FindChildByName(m_treeNode,
                                          m_folderItem->GetName()) &&
                NULL == m_treeNode->GetUserData()) {
              m_treeNode = listView->FindChildByName(m_treeNode,
                                                     m_folderItem->GetName());
            } else {
              if (filterCriteria != Tasks) {
                m_treeNode = listView->AddItem(m_treeNode,
                                               m_folderItem->GetName());
//                m_treeNode->SetPictures(m_openedFolder, m_closedFolderIcon);
              }
            }
            m_treeNode->SetUserData(0);
            if (m_folderItem == m_folderItems->Last() &&
                filterCriteria == FoldersAndPages) {
              m_localDatabasePages.clear();
              m_histogramDB->getPageNamesByFolder(*m_folderIt,
                                                  m_localDatabasePages);
              std::sort(m_localDatabasePages.begin(),
                        m_localDatabasePages.end());
              for (m_pageIt = m_localDatabasePages.begin();
                   m_pageIt != m_localDatabasePages.end(); ++m_pageIt) {
                std::string pageName = std::string(*m_pageIt).erase(0,
                                                   (*m_folderIt).length()+1);
                m_pageNode = listView->AddItem(m_treeNode, pageName.c_str());
                // TODO: memlost on new, root items lost?
                m_pageNode->SetUserData(new TString(*m_pageIt));
                listView->CheckItem(m_pageNode, false);
                setTreeNodeIcon(m_pageNode, s_PAGE);
                if (histograms) {
                  m_localDatabaseIDS.clear();
                  m_histogramTypes.clear();
                  m_histogramDB->getHistogramsByPage(*m_pageIt, NULL,
                                                     &m_localDatabaseIDS,
                                                     &m_histogramTypes);
                  fillTreeNodeWithHistograms(listView, m_pageNode,
                                             &m_localDatabaseIDS,
                                             &m_histogramTypes);
                }
              }
            } else if (m_folderItem == m_folderItems->Last() &&
                       filterCriteria != FoldersAndPages) {
              if (histograms) {
                m_localDatabaseIDS.clear();
                m_histogramTypes.clear();
                if (filterCriteria == Tasks) {
                  m_histogramDB->getHistogramsByTask(*m_folderIt, NULL,
                                                     &m_localDatabaseIDS,
                                                     &m_histogramTypes);
                } else if (filterCriteria == Subsystems) {
                  m_histogramDB->getHistogramsBySubsystem(*m_folderIt, NULL,
                                                          &m_localDatabaseIDS,
                                                          &m_histogramTypes);
                }
                fillTreeNodeWithHistograms(listView, m_treeNode,
                                           &m_localDatabaseIDS,
                                           &m_histogramTypes);
              }
            }
          }
          m_folderItems->Delete();
          delete m_folderItems;
          m_folderItems = NULL;
          delete m_folderItemsIt;
          m_folderItemsIt = NULL;
        }
      } else if (filterCriteria == HistogramsWithAnalysis ||
                 filterCriteria == AnalysisHistograms ||
                 filterCriteria == AllHistograms) {
          fillTreeNodeWithHistograms(listView, listView->GetFirstItem(),
                                     &m_localDatabaseIDS, &m_histogramTypes);
      }
      fClient->NeedRedraw(listView);
//      (listView->GetCanvas())->Update();
      Resize(); DoRedraw();
      m_message = "Histograms read from Database.";
//      listView->MapSubwindows();
      listView->MapWindow();
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetDefaultCursor());

    } catch (std::string sqlException) {
        // TODO: add error logging backend
        m_mainStatusBar->SetText(sqlException.c_str());
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }

        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
            Form("OnlineHistDB server:\n\n%s\n",
                 sqlException.c_str()),
            kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
  } else if (!isConnectedToHistogramDB()) {
    m_message = "Histograms not read from Database.";
  }
  m_mainStatusBar->SetText(m_message.c_str(), 2);
  if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }
  sortTreeChildrenItems(listView, listView->GetFirstItem());
  fClient->NeedRedraw(listView);
}
void PresenterMainFrame::fillTreeNodeWithHistograms(TGListTree* listView,
  TGListTreeItem* node, std::vector<std::string>*  histogramList,
  std::vector<std::string>*  histogramTypes)
{
  listView->UnmapWindow();
  
  m_histogramType = histogramTypes->begin();
  for (m_histogramIt = histogramList->begin();
       m_histogramIt != histogramList->end();
       ++m_histogramIt, ++m_histogramType) {
    HistogramIdentifier histogramIdentifier =
      HistogramIdentifier(*m_histogramIt);
    m_taskNode = node;
    // Taskname
    if (listView->FindChildByName(m_taskNode,
        histogramIdentifier.histogramUTGID().c_str())) {
      m_taskNode = listView->FindChildByName(m_taskNode,
        histogramIdentifier.histogramUTGID().c_str());
    } else {
      m_taskNode = listView->AddItem(m_taskNode,
        histogramIdentifier.histogramUTGID().c_str());
      setTreeNodeIcon(m_taskNode, s_TASK);
    }
    listView->SetCheckBox(m_taskNode, true);
    listView->CheckItem(m_taskNode, false);
    m_taskNode->SetUserData(0);
    // Algorithm name
    m_algorithmNode = m_taskNode;
    if (listView->FindChildByName(m_algorithmNode,
                              histogramIdentifier.algorithmName().c_str())) {
      m_algorithmNode = listView->FindChildByName(m_algorithmNode,
                                histogramIdentifier.algorithmName().c_str());
    } else {
      m_algorithmNode = listView->AddItem(m_algorithmNode,
                                histogramIdentifier.algorithmName().c_str());
      setTreeNodeIcon(m_algorithmNode, s_ALGORITHM);
    }
    listView->SetCheckBox(m_algorithmNode, true);
    listView->CheckItem(m_algorithmNode, false);    
    m_algorithmNode->SetUserData(0);
    // Histogramset name
    m_histogramSetNode = m_algorithmNode;
    if (histogramIdentifier.isFromHistogramSet()) {
      if (listView->FindChildByName(m_histogramSetNode,
        histogramIdentifier.histogramSetName().c_str())) {
          m_histogramSetNode = listView->FindChildByName(m_histogramSetNode,
                            histogramIdentifier.histogramSetName().c_str());
      } else {
        m_histogramSetNode = listView->AddItem(m_histogramSetNode,
          histogramIdentifier.histogramSetName().c_str());
        setTreeNodeIcon(m_histogramSetNode, s_SET);
      }
      listView->SetCheckBox(m_histogramSetNode, true);
      listView->CheckItem(m_histogramSetNode, false);  
      m_histogramSetNode->SetUserData(0);
    }
    // Histogram name
    m_histogramNode = m_histogramSetNode;
    m_histogramIdItems = TString(histogramIdentifier.histogramName()).
                                 Tokenize(s_slash);
    m_histogramIdItemsIt = m_histogramIdItems->MakeIterator();

    while (m_histogramIdItem = m_histogramIdItemsIt->Next()) {
      if (m_histogramIdItem != m_histogramIdItems->Last()) {
        if (listView->FindChildByName(m_histogramNode,
                                      m_histogramIdItem->GetName())) {
            m_histogramNode = listView->FindChildByName(m_histogramNode,
                                              m_histogramIdItem->GetName());
        } else {
          m_histogramNode = listView->AddItem(m_histogramNode,
                                              m_histogramIdItem->GetName());
        }
        listView->SetCheckBox(m_histogramNode, true);
        listView->CheckItem(m_histogramNode, false);
        m_histogramNode->SetUserData(0);
        setTreeNodeIcon(m_histogramNode, s_LEVEL);
      }
      if (m_histogramIdItem == m_histogramIdItems->Last()) {
        m_histogramNode = listView->AddItem(m_histogramNode,
                                            m_histogramIdItem->GetName());
        listView->SetCheckBox(m_histogramNode, true);
        listView->CheckItem(m_histogramNode, false);
        setTreeNodeIcon(m_histogramNode, *m_histogramType);
        m_histogramNode->SetUserData(new TString(*m_histogramIt));
      }
    }
    m_histogramIdItems->Delete();
    delete m_histogramIdItems;
    m_histogramIdItems = NULL;
    delete m_histogramIdItemsIt;
    m_histogramIdItemsIt = NULL;
  }
//  sortTreeChildrenItems(listView, node);
  listView->MapWindow();
}
std::string PresenterMainFrame::histogramDBName()
{
  return m_dbName;
}
// TODO: move all below to an eventual static helper class
// TODO: void PresenterMainFrame::WaitCursor()

void PresenterMainFrame::startBenchmark(const std::string &timer)
  { m_benchmark->Start(timer.c_str()) ; }
 
void PresenterMainFrame::stopBenchmark(const std::string &timer)
  { m_benchmark->Stop(timer.c_str());}
 
void PresenterMainFrame::printBenchmark(const std::string &timer)
{
  std::cout << timer <<";"<< (m_benchmark->GetRealTime(timer.c_str())) << std::endl ;
  m_benchmark->Reset(); 
}


void PresenterMainFrame::listRootHistogramsFrom(TDirectory* rootFile,
                                                std::vector<std::string> & histogramList,
                                                std::vector<std::string> & histogramTypes,
                                                std::string& taskName) {
  
   TDirectory* tmpdir = gDirectory;
   TIter next(rootFile->GetListOfKeys());
   TKey* key;
   
   std::stringstream histogramIDStream;

   while ((key = (TKey*)next())) {
      if (key->IsFolder()) {
         rootFile->cd(key->GetName());
         TDirectory *subdirectory = gDirectory;
         std::string pathName = taskName + s_slash + key->GetName();
         listRootHistogramsFrom(subdirectory, histogramList, histogramTypes,
                                pathName);
         tmpdir->cd();
         continue;
      }
      if (std::string(key->GetClassName()) == "TH1D") {
        histogramTypes.push_back(s_pfixMonH1D);
        histogramIDStream << s_pfixMonH1D;
      } else if (std::string(key->GetClassName()) == "TH2D") {
        histogramTypes.push_back(s_pfixMonH2D);
        histogramIDStream << s_pfixMonH2D;
      } else if (std::string(key->GetClassName()) == "TProfile") {
        histogramTypes.push_back(s_pfixMonProfile);
        histogramIDStream << s_pfixMonProfile;
      }
      histogramIDStream << s_slash << taskName << s_slash << key->GetName();
      histogramList.push_back(histogramIDStream.str());
      histogramIDStream.str("");
   }
}


void PresenterMainFrame::setTreeNodeIcon(TGListTreeItem* node,
                                         const std::string & type)
{
  const TGPicture*  m_icon;
  if (s_H1D == type || 
      s_pfixMonH1D == type) {
    m_icon = m_iconH1D;
  } else if (s_H2D == type ||
      s_pfixMonH2D == type) {
    m_icon = m_iconH2D;
  } else if (s_P1D == type ||
             s_pfixMonProfile == type ||
             s_HPD == type ||
             s_P2D == type ) {
    m_icon = m_iconProfile;
  } else if (s_CNT == type) {
    m_icon = m_iconCounter;
  } else if (s_PAGE == type) {
    m_icon = m_iconPage;
  } else if (s_TASK == type) {
    m_icon = m_iconTask;
  } else if (s_ALGORITHM == type) {
    m_icon = m_iconAlgorithm;
  } else if (s_SET == type) {
    m_icon = m_iconSet;
  } else if (s_LEVEL == type) {
    m_icon = m_iconLevel;
  } else {
    m_icon = m_iconQuestion;
  }
  if (0 != node) { node->SetPictures(m_icon, m_icon); }
}
void PresenterMainFrame::checkedTreeItems(TGListTree* selected,
                                          TGListTree* treeList)
{
  if (!selected) return;
  TGListTreeItem* node;
  node = treeList->GetFirstItem();
  if (node->IsChecked() && node->GetUserData()) {
    selected->AddItem(0, 0, node->GetUserData());
  }
  while (0 != node) {
    if (node->GetFirstChild()) {
      checkedTreeItemsChildren(node->GetFirstChild(), selected);
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::checkedTreeItemsChildren(TGListTreeItem* node,
                                                  TGListTree* selected)
{
  while (0 != node) {
    if (node->IsChecked() && node->GetUserData()) {
      selected->AddItem(0, 0, node->GetUserData());
    }
    if (node->GetFirstChild()) {
      checkedTreeItemsChildren(node->GetFirstChild(), selected);
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::collapseTreeChildrenItems(TGListTree* treeList,
                                                   TGListTreeItem* node)
{
  if (0 != node && 0 !=  treeList) {
    treeList->CloseItem(node);
  }
  while (0 != node) {
    if (node->GetFirstChild()) {
      collapseTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::collapseTreeChildrenItemsChildren(
  TGListTree* treeList, TGListTreeItem* node)
{
  while (0 != node) {
    if (node && treeList) {
      treeList->CloseItem(node);
    }
    if (node->GetFirstChild()) {
      collapseTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::sortTreeChildrenItems(TGListTree* treeList,
                                               TGListTreeItem* node)
{
  if (0 != node && 0 != treeList) {
    treeList->SortChildren(node);
  }
  while (0 != node) {
    if (0 != (node->GetFirstChild())) {
      sortTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::sortTreeChildrenItemsChildren(TGListTree* treeList,
                                                       TGListTreeItem* node)
{
  while (0 != node) {
    if (0 != node && 0 != treeList) {
      treeList->SortChildren(node);
    }
    if (0 != (node->GetFirstChild())) {
      sortTreeChildrenItemsChildren(treeList, node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::deleteTreeChildrenItemsUserData(TGListTreeItem* node)
{
  if (0 != node && 0 != (node->GetUserData())) {
    delete (TString*)(node->GetUserData());
    node->SetUserData(NULL);
  }
  if (0 != node && 0 != (node->GetFirstChild())) {
    deleteTreeChildrenItemsUserDataChildren(node->GetFirstChild());
  }
}
void PresenterMainFrame::deleteTreeChildrenItemsUserDataChildren(TGListTreeItem* node)
{
  while (0 != node) {
    if (0 != (node->GetUserData())) {
      delete (TString*)(node->GetUserData());
      node->SetUserData(NULL);
    }
    if (0 != (node->GetFirstChild())) {
      deleteTreeChildrenItemsUserDataChildren(node->GetFirstChild());
    }
    node = node->GetNextSibling();
  }
}
void PresenterMainFrame::setStatusBarText(const char* text, int slice)
{
  m_mainStatusBar->SetText(text, slice);
}
void PresenterMainFrame::editHistogramProperties()
{
  bool referenceOverlay(false);
  if (m_referencesOverlayed) {
    disableReferenceOverlay();
    referenceOverlay = true;
  }
  DbRootHist* selectedDbHisto = selectedDbRootHistogram();
  if (0 != selectedDbHisto) {
    if (0 != selectedDbHisto) {
      TH1* selectedHisto = selectedDbHisto->rootHistogram;
      if (0 != selectedHisto) {
        selectedHisto->DrawPanel();
      }
    }
  }
  if (referenceOverlay) { enableReferenceOverlay(); }  
}
void PresenterMainFrame::inspectHistogram()
{
  // TODO: draw rather ClonePad
  bool referenceOverlay(false);
  if (m_referencesOverlayed) {
    disableReferenceOverlay();
    referenceOverlay = true;
  }

  DbRootHist* selectedDbHisto = selectedDbRootHistogram();
  if (0 != selectedDbHisto) {
    if (0 != selectedDbHisto) {
      TH1* selectedHisto = selectedDbHisto->rootHistogram;
      if (0 != selectedHisto) {
        TCanvas* zoomCanvas =  new TCanvas();
        zoomCanvas->cd();
    //    zoomCanvas->SetTitle(selectedHisto->GetTitle());
        selectedHisto->DrawCopy();
        zoomCanvas->ToggleEventStatus();
        zoomCanvas->ToggleAutoExec();
        zoomCanvas->ToggleToolBar();
        zoomCanvas->ToggleEditor();
      }
    }
  }
  if (referenceOverlay) { enableReferenceOverlay(); } 
}
void PresenterMainFrame::histogramDescription()
{
  if (isConnectedToHistogramDB()) {
    
    m_currentPageHistogramHelp.clear();
    std::stringstream currentPageHistogramHelp;
    
    DbRootHist* histogram = selectedDbRootHistogram();
    if (0 != histogram) {
      
      currentPageHistogramHelp << "Title:\t\t" << (histogram->onlineHistogram())->htitle() << std::endl << std::endl
                               << "Name:\t\t" <<  (histogram->onlineHistogram())->hname() << std::endl
                               << "Type:\t\t" <<  (histogram->onlineHistogram())->hstype() << std::endl
                               << "Task:\t\t" <<  (histogram->onlineHistogram())->task() << std::endl
                               << "Algorithm:\t" <<  (histogram->onlineHistogram())->algorithm() << std::endl
                               << "ROOT name:\t" <<  (histogram->onlineHistogram())->rootName()  << std::endl
                               << "DIM in DB:\t" <<  (histogram->onlineHistogram())->dimServiceName() << std::endl << std::endl
                               << "Description:" << std::endl << (histogram->onlineHistogram())->descr() << std::endl
                               << std::endl;

      m_currentPageHistogramHelp = currentPageHistogramHelp.str().c_str();
            
      TRootHelpDialog *hd = new TRootHelpDialog(this, "Histogram Description", 600, 400);
      hd->SetText(Form("%s", m_currentPageHistogramHelp.c_str()));
      hd->Popup();
    }
  } else {
    new TGMsgBox(fClient->GetRoot(), this, "Description Unavailable",
                 "Description Unavailable, not logged in!",
                 kMBIconExclamation, kMBClose, &m_msgBoxReturnCode);
  } 
}
void PresenterMainFrame::inspectPage()
{
  TCanvas* inspectCanvas =  new TCanvas();
  inspectCanvas->cd();
  inspectCanvas->SetTitle("Inspect Page");
  editorCanvas->DrawClonePad();
  inspectCanvas->ToggleEventStatus();
  inspectCanvas->ToggleAutoExec();
  inspectCanvas->ToggleToolBar();
  inspectCanvas->ToggleEditor();
}
void PresenterMainFrame::reconfigureGUI()
{
  removeHistogramsFromPage();
  
  if (isConnectedToHistogramDB()) {
    refreshPagesDBListTree();
    enableAutoCanvasLayoutBtn();
    if (canWriteToHistogramDB() && 
        ((EditorOnline == m_presenterMode) ||
         (EditorOffline == m_presenterMode))) {
      showDBTools(ReadWrite);
    } else {
      showDBTools(ReadOnly);
    }
  } else {
    hideDBTools();
  }

  if (Online == m_presenterMode) {
    if (m_historyTrendPlots) { toggleHistoryPlots(); }
    
    m_toolMenu->CheckEntry(ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(OFFLINE_MODE_COMMAND);
    m_rightMiscFrame->UnmapWindow();
    m_rightVerticalSplitter->UnmapWindow();

    m_startRefreshButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(START_COMMAND);
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(STOP_COMMAND);
    m_clearHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(CLEAR_HISTOS_COMMAND);

    m_previousIntervalButton->SetState(kButtonDisabled);
    m_nextIntervalButton->SetState(kButtonDisabled);
    
    m_historyPlotsButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(HISTORY_PLOTS_COMMAND);

    m_overlayReferenceHistoButton->SetState(kButtonEngaged);
    m_overlayReferenceHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    
    m_pickReferenceHistoButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(PICK_REFERENCE_HISTO_COMMAND);
    
    m_viewMenu->DisableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);

    m_historyIntervalQuickButton->SetState(kButtonDisabled);
//    m_partitionSelectorQuickButton->SetState(kButtonEngaged);    
//    m_partitionSelectorQuickButton->SetState(kButtonUp);

    // hide refreshHistoDBListTree
//    if (!isConnectedToHistogramDB()) { refreshHistogramSvcList(); }
//    refreshHistogramSvcList();
  } else if (History == m_presenterMode) {
    disablePageRefresh();

    m_toolMenu->CheckEntry(OFFLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);    
    m_rightMiscFrame->UnmapWindow();
    m_rightVerticalSplitter->UnmapWindow();

    m_startRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(START_COMMAND);
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(STOP_COMMAND);
    m_clearHistoButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(CLEAR_HISTOS_COMMAND);

    m_previousIntervalButton->SetState(kButtonEngaged);
    m_previousIntervalButton->SetState(kButtonUp);
    m_nextIntervalButton->SetState(kButtonEngaged);
    m_nextIntervalButton->SetState(kButtonUp);

    m_historyIntervalQuickButton->SetState(kButtonEngaged);
    m_historyIntervalQuickButton->SetState(kButtonUp);
//    m_partitionSelectorQuickButton->SetState(kButtonDisabled);

//    m_historyPlotsButton->SetState(kButtonEngaged);
    m_historyPlotsButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(HISTORY_PLOTS_COMMAND);

//    m_overlayReferenceHistoButton->SetState(kButtonEngaged);
    m_overlayReferenceHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    
    m_pickReferenceHistoButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(PICK_REFERENCE_HISTO_COMMAND);
    
    m_viewMenu->DisableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);

    // hide refreshHistoDBListTree
  } else if (EditorOnline == m_presenterMode) {
    stopPageRefresh();
    if (m_clearedHistos) { clearHistos(); }
    if (m_referencesOverlayed) { toggleReferenceOverlay(); }
    if (m_historyTrendPlots) { toggleHistoryPlots(); }
    // enable play/stop/reset!
    m_toolMenu->CheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);    
    m_toolMenu->UnCheckEntry(ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(OFFLINE_MODE_COMMAND);
    
    m_previousIntervalButton->SetState(kButtonDisabled);
    m_nextIntervalButton->SetState(kButtonDisabled);
    m_rightMiscFrame->MapWindow();
    m_rightVerticalSplitter->MapWindow();
    
    refreshHistogramSvcList(s_withTree);
    
    if (isConnectedToHistogramDB()) { refreshHistoDBListTree(); }
//    m_clearHistoButton->SetState(kButtonUp);
//    m_viewMenu->EnableEntry(CLEAR_HISTOS_COMMAND);

//    m_startRefreshButton->SetState(kButtonDisabled);
//    m_viewMenu->DisableEntry(START_COMMAND);
//    m_stopRefreshButton->SetState(kButtonDisabled);
//    m_viewMenu->DisableEntry(STOP_COMMAND);
//    m_clearHistoButton->SetState(kButtonDisabled);
//    m_viewMenu->DisableEntry(CLEAR_HISTOS_COMMAND);

      m_startRefreshButton->SetState(kButtonUp);
      m_viewMenu->EnableEntry(START_COMMAND);
      m_stopRefreshButton->SetState(kButtonDisabled);
      m_viewMenu->DisableEntry(STOP_COMMAND);
      m_clearHistoButton->SetState(kButtonUp);
      m_viewMenu->EnableEntry(CLEAR_HISTOS_COMMAND);

      m_histoSvcTreeContextMenu->EnableEntry(M_SetDimSourceFromTree);

      m_historyIntervalQuickButton->SetState(kButtonEngaged);
      m_historyIntervalQuickButton->SetState(kButtonUp);
//    m_partitionSelectorQuickButton->SetState(kButtonEngaged);
//    m_partitionSelectorQuickButton->SetState(kButtonUp);

      m_historyPlotsButton->SetState(kButtonDisabled);
      m_viewMenu->DisableEntry(HISTORY_PLOTS_COMMAND);
    
//    m_overlayReferenceHistoButton->SetState(kButtonDisabled);
//    m_viewMenu->DisableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
//    m_overlayReferenceHistoButton->SetState(kButtonEngaged);
      m_overlayReferenceHistoButton->SetState(kButtonUp);
      m_viewMenu->EnableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    
    // show refreshHistoDBListTree
  } else if (EditorOffline == m_presenterMode) {
    stopPageRefresh();
    if (m_clearedHistos) { clearHistos(); }
    if (m_referencesOverlayed) { toggleReferenceOverlay(); }
    if (m_historyTrendPlots) { toggleHistoryPlots(); }
    // enable play/stop/reset!
    m_toolMenu->CheckEntry(PAGE_EDITOR_OFFLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(PAGE_EDITOR_ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(ONLINE_MODE_COMMAND);
    m_toolMenu->UnCheckEntry(OFFLINE_MODE_COMMAND);
    
    if (isConnectedToHistogramDB()) { refreshHistoDBListTree(); }
    
    m_previousIntervalButton->SetState(kButtonDisabled);
    m_nextIntervalButton->SetState(kButtonDisabled);
    m_rightMiscFrame->MapWindow();
    m_rightVerticalSplitter->MapWindow();    
    m_startRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(START_COMMAND);
    m_stopRefreshButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(STOP_COMMAND);
    m_clearHistoButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(CLEAR_HISTOS_COMMAND);
    
    m_histoSvcTreeContextMenu->DisableEntry(M_SetDimSourceFromTree);
    
    m_historyIntervalQuickButton->SetState(kButtonEngaged);
    m_historyIntervalQuickButton->SetState(kButtonUp);
//    m_partitionSelectorQuickButton->SetState(kButtonEngaged);
//    m_partitionSelectorQuickButton->SetState(kButtonUp);

    m_historyPlotsButton->SetState(kButtonDisabled);
    m_viewMenu->DisableEntry(HISTORY_PLOTS_COMMAND);    
    
    m_overlayReferenceHistoButton->SetState(kButtonUp);
    m_viewMenu->EnableEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    
    refreshHistogramSvcList(s_withTree);
  }

//  refreshPartitionSelectorPopupMenu();
  fClient->NeedRedraw(this);
  Resize();DoRedraw(); // wtf would trigger a redraw???
//  DoRedraw();
}
void PresenterMainFrame::hideDBTools()
{
  m_histoDBFilterComboBox->SetEnabled(false);

  m_histoDBCanvasViewPort->HideFrame(m_databaseHistogramTreeList);
  m_pagesFromHistoDBViewPort->HideFrame(m_pagesFromHistoDBListTree);
//  m_databaseHistogramTreeList->SetToolTipText(
//      std::string("Please login to DB for Histogram list").c_str(),
//      1, 1, 250);
  m_histoSvcTreeContextMenu->DisableEntry(M_AddHistoToDB_COMMAND);
  m_histoSvcTreeContextMenu->EnableEntry(M_AddHistoToPage_COMMAND);
  m_loginButton->SetState(kButtonUp);
  m_logoutButton->SetState(kButtonDisabled);
  m_savePageToDatabaseButton->SetState(kButtonDisabled);
  m_pickReferenceHistoButton->SetState(kButtonDisabled);

  m_fileMenu->EnableEntry(LOGIN_COMMAND);
  m_fileMenu->DisableEntry(LOGOUT_COMMAND);
  m_fileMenu->DisableEntry(SAVE_PAGE_TO_DB_COMMAND);
}
void PresenterMainFrame::showDBTools(DatabaseMode databasePermissions)
{
  m_logoutButton->SetState(kButtonUp);
  m_loginButton->SetState(kButtonDisabled);

  m_fileMenu->DisableEntry(LOGIN_COMMAND);
  m_fileMenu->EnableEntry(LOGOUT_COMMAND);

  m_histoDBFilterComboBox->SetEnabled(true);
//  m_databaseHistogramTreeList->SetToolTipText(
//    std::string("Please select Histogram from list").c_str(), 1, 1, 20);
  m_databaseHistogramTreeList->SetBackgroundColor(GetWhitePixel());
  m_histoDBCanvasViewPort->ShowFrame(m_databaseHistogramTreeList);
  m_pagesFromHistoDBViewPort->ShowFrame(m_pagesFromHistoDBListTree);

  if (ReadWrite == databasePermissions) {
    m_histoSvcTreeContextMenu->EnableEntry(M_AddHistoToDB_COMMAND);
//    m_savePageToDatabaseButton->SetState(kButtonEngaged);
    m_savePageToDatabaseButton->SetState(kButtonUp);
//    m_pickReferenceHistoButton->SetState(kButtonEngaged);
    m_pickReferenceHistoButton->SetState(kButtonUp);
    m_fileMenu->EnableEntry(SAVE_PAGE_TO_DB_COMMAND);
    m_pagesContextMenu->EnableEntry(M_DeletePage_COMMAND);
    m_pagesContextMenu->EnableEntry(M_DeleteFolder_COMMAND);
    m_histoDBContextMenu->EnableEntry(M_SetHistoPropertiesInDB_COMMAND);
    m_histoDBContextMenu->EnableEntry(M_DeleteDBHisto_COMMAND);
    
//    m_pickReferenceHistoButton->SetState(kButtonEngaged);
    m_pickReferenceHistoButton->SetState(kButtonUp);
    m_editMenu->EnableEntry(PICK_REFERENCE_HISTO_COMMAND);
    m_editMenu->EnableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);

  } else if (ReadOnly == databasePermissions) {
    m_savePageToDatabaseButton->SetState(kButtonDisabled);
    m_fileMenu->DisableEntry(SAVE_PAGE_TO_DB_COMMAND);
    m_histoSvcTreeContextMenu->DisableEntry(M_AddHistoToDB_COMMAND);
    m_pagesContextMenu->DisableEntry(M_DeletePage_COMMAND);
    m_pagesContextMenu->DisableEntry(M_DeleteFolder_COMMAND);
    m_histoDBContextMenu->DisableEntry(M_SetHistoPropertiesInDB_COMMAND);
    m_histoDBContextMenu->DisableEntry(M_DeleteDBHisto_COMMAND);

    m_pickReferenceHistoButton->SetState(kButtonDisabled);    
    m_editMenu->DisableEntry(PICK_REFERENCE_HISTO_COMMAND);    
    m_editMenu->DisableEntry(SAVE_AS_REFERENCE_HISTO_COMMAND);
  }
  m_histoSvcTreeContextMenu->DisableEntry(M_AddHistoToPage_COMMAND);

  enablePageLoading();
}
void PresenterMainFrame::refreshHistoDBListTree() {
  //TODO: add threading(?!)...
  if (m_verbosity >= Verbose) { std::cout << m_message << std::endl; }
  listFromHistogramDB(m_databaseHistogramTreeList,
    static_cast<FilterCriteria>(m_histoDBFilterComboBox->GetSelected()),
    s_withHistograms);
}
void PresenterMainFrame::refreshPagesDBListTree() {
  listFromHistogramDB(m_pagesFromHistoDBListTree, FoldersAndPages,
                      s_withoutHistograms);
}
void PresenterMainFrame::refreshHistogramSvcList(bool tree) {
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());

  m_knownDimServices.clear();
  m_histogramTypes.clear();

  if (Online == m_presenterMode || EditorOnline == m_presenterMode) {
    
    m_dimBrowserDock->SetWindowName("DIM Histogram Browser");
    m_histoSvcBrowserGroupFrame->SetTitle("DIM Histogram Browser");    
    
    m_candidateDimServices.clear();
    m_knownOnlinePartitionList->Delete();
    
    char *dimService; 
    char *dimFormat;
    int dimType;
  
  
  //TODO: put known Mon prefixes into a list and cycle through...
    std::string serviceType = s_pfixMonH1D;
    (serviceType.append(s_slash)).append("*");
    m_dimBrowser->getServices(serviceType.c_str());
    while(dimType = m_dimBrowser->getNextService(dimService, dimFormat)) {            
      m_candidateDimServices.push_back(std::string(dimService));
    }
    // s_pfixMonH2D
    serviceType = s_pfixMonH2D;
    (serviceType.append(s_slash)).append("*");
    m_dimBrowser->getServices(serviceType.c_str());
    while(dimType = m_dimBrowser->getNextService(dimService, dimFormat)) {            
      m_candidateDimServices.push_back(std::string(dimService));
    }
    // s_pfixMonProfile
    serviceType = s_pfixMonProfile;
    (serviceType.append(s_slash)).append("*");
    m_dimBrowser->getServices(serviceType.c_str());
    while(dimType = m_dimBrowser->getNextService(dimService, dimFormat)) {            
      m_candidateDimServices.push_back(std::string(dimService));
    }
    // "H1D", "H2D", "P1D", "P2D", "CNT", "SAM" -- is this uncle SAM?
    for (int it = 0; it < OnlineHistDBEnv_constants::NHTYPES; ++it) {
      std::string serviceType(OnlineHistDBEnv_constants::HistTypeName[it]);
      (serviceType.append(s_slash)).append("*");
      m_dimBrowser->getServices(serviceType.c_str());
      while(dimType = m_dimBrowser->getNextService(dimService, dimFormat)) {            
        m_candidateDimServices.push_back(std::string(dimService));
      }        
    }
    // HPD
    serviceType = s_HPD;
    (serviceType.append(s_slash)).append("*");
    m_dimBrowser->getServices(serviceType.c_str());
    while(dimType = m_dimBrowser->getNextService(dimService, dimFormat)) {            
      m_candidateDimServices.push_back(std::string(dimService));
    }
        
    if (0 != m_histoSvcListTree && 0 != m_dimBrowser) {
      const int nDimServers = m_dimBrowser->getServers();
      
      if (m_verbosity >= Verbose) {
        std::cout << "nDimServers:\t" << nDimServers << std::endl;
      }
  
      TString dimServerName;
      TString dimServerNodeName;
      TString dimServiceName;
      TString statusMessage;
  
      // If some servers found, discover what services are available
      if (nDimServers > 0) {
        const char* dimDnsServerNode = DimClient::getDnsNode();
  
        if (m_verbosity >= Verbose) {
          std::cout << std::endl << "DNS: " << dimDnsServerNode << std::endl;
        }
  
        setStatusBarText(dimDnsServerNode, 2);
        if (s_withTree == tree) {
          TGListTreeItem* node = m_histoSvcListTree->GetFirstItem();
          m_histoSvcListTree->RenameItem(node, dimDnsServerNode);
          
          (m_histoSvcListTree->GetFirstItem())->SetPictures(m_dimOnline16,
                                                            m_dimOnline16);
          
          
          deleteTreeChildrenItemsUserData(node);
          m_histoSvcListTree->DeleteChildren(node);          
        }
  
        m_candidateDimServicesIt = m_candidateDimServices.begin();
        while (m_candidateDimServicesIt != m_candidateDimServices.end()) {
          TString dimServiceTS = TString(*m_candidateDimServicesIt);
          if (!dimServiceTS.EndsWith(s_gauchocomment.c_str())) { // Beg&End not sym.
              HistogramIdentifier histogramService = HistogramIdentifier(*m_candidateDimServicesIt);
              if (histogramService.isDimFormat()) {
                if (s_withTree == tree) {
                  m_knownDimServices.push_back(*m_candidateDimServicesIt);
                  m_histogramTypes.push_back(histogramService.histogramType());                
                }
                if (!m_knownOnlinePartitionList->FindObject(histogramService.partitionName().c_str())) {
                  TObjString* partitionName = new TObjString(histogramService.partitionName().c_str());
                  if (!histogramService.isEFF() ||
                      (histogramService.isEFF() &&
                      false == partitionName->GetString().BeginsWith(s_hltNodePrefix.c_str()))) {                    
                    m_knownOnlinePartitionList->Add(partitionName);
                  }
                }
              if (m_verbosity >= Verbose) {
                std::cout << "\t\t\t|_ " << *m_candidateDimServicesIt << std::endl;
              }
            } else {
              new TGMsgBox(fClient->GetRoot(), this, "DIM Service name error",
                Form("The DIM servicename\n%s\n does not appear to follow the" \
                  "convention\nPlease use the following format:\n" \
                  "TYP/UTGID/Algorithmname/Histogramname\n where the UTGID " \
                  "normally has the following format:\n" \
                  "partition_node_taskname_instance", dimService),
                kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
            }
          }
          ++m_candidateDimServicesIt;
        }
//        refreshPartitionSelectorPopupMenu();       
      }
    } else {
      if (s_withTree == tree) {
        m_histoSvcListTree->RenameItem(m_histoSvcListTree->GetFirstItem(), "No DIM");
      }
      if (m_verbosity >= Verbose) {
        std::cout << "Sorry, no DIM server found." << std::endl;
      }
      new TGMsgBox(fClient->GetRoot(), this, "DIM Error",
                   "Sorry, no DIM server found", kMBIconExclamation,
                   kMBOk, &m_msgBoxReturnCode);
    }
  } else if (EditorOffline == m_presenterMode) {
    if (s_withTree == tree && !m_savesetFileName.empty()) {
      
      m_dimBrowserDock->SetWindowName("ROOT Histogram Browser");
      m_histoSvcBrowserGroupFrame->SetTitle("ROOT Histogram Browser");
            
      TGListTreeItem* node = m_histoSvcListTree->GetFirstItem();
      m_histoSvcListTree->RenameItem(node, m_savesetFileName.c_str());
      
      (m_histoSvcListTree->GetFirstItem())->SetPictures(m_iconROOT, m_iconROOT);
      
      deleteTreeChildrenItemsUserData(node);
      m_histoSvcListTree->DeleteChildren(node);
      
      TFile rootFile(m_savesetFileName.c_str());
      
      if (rootFile.IsZombie()) {
        new TGMsgBox(fClient->GetRoot(), this, "File Error",
                     "Sorry, file specified not found", kMBIconExclamation,
                     kMBOk, &m_msgBoxReturnCode);
      } else {        
        std::string taskName(m_archive->taskNameFromFile(std::string(rootFile.GetName())));
        listRootHistogramsFrom(dynamic_cast<TDirectory*>(&rootFile), m_knownDimServices, m_histogramTypes, taskName);
      }
      rootFile.Close();       
    }
  }
  if (s_withTree == tree) {
    fillTreeNodeWithHistograms(m_histoSvcListTree,
                               m_histoSvcListTree->GetFirstItem(),
                               &m_knownDimServices,
                               &m_histogramTypes);        
    sortTreeChildrenItems(m_histoSvcListTree, m_histoSvcListTree->GetFirstItem());
    fClient->NeedRedraw(m_histoSvcListTree);   
  }     
  Resize();DoRedraw(); // wtf would trigger a redraw???
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetDefaultCursor());
}
void PresenterMainFrame::refreshPartitionSelectorPopupMenu()
{
  bool updateList(false);
  TObject *obj = 0;  
  TIter nextUTGID(m_partitionSelectorPopupMenu->GetListOfEntries());
  while (obj = nextUTGID()) {
    if (obj) {
      m_partitionSelectorPopupMenu->DeleteEntry(dynamic_cast<TGMenuEntry*>(obj));
    }
  }
  m_partitionSelectorPopupMenu->AddEntry("refresh partitions", M_RefreshKnownPartitions);
  TIterator* partitionIt = 0;
  
  if (((History == m_presenterMode) ||
       (EditorOffline == m_presenterMode))
       && 0 != m_archive) {
    updateList = true;
    std::vector<std::string> m_historyPartitionList;
    std::vector<std::string>::const_iterator m_historyPartitionListIt;
    m_knownHistoryPartitionList->Delete();
    
    m_historyPartitionList = m_archive->listPartitions();

    m_historyPartitionListIt = m_historyPartitionList.begin();
    while (m_historyPartitionListIt != m_historyPartitionList.end()) {
      if (!m_knownHistoryPartitionList->FindObject((*m_historyPartitionListIt).c_str())) {
        TObjString* partitionName = new TObjString((*m_historyPartitionListIt).c_str());
        m_knownHistoryPartitionList->Add(partitionName);
      }
      m_historyPartitionListIt++;
    }
    partitionIt = m_knownHistoryPartitionList->MakeIterator();
    
  } else if (Online == m_presenterMode ||
             EditorOnline == m_presenterMode) {
    updateList = true;              
    refreshHistogramSvcList(s_withoutTree);
    partitionIt = m_knownOnlinePartitionList->MakeIterator();
  }
  if (updateList) {
    int id(500);        // just a 50 limit for reading partitions from
    int idLimit(550);   // directories to avoid deep nesting troubles
    while (obj = partitionIt->Next()) {
      if (obj) { 
        m_partitionSelectorPopupMenu->AddEntry(obj->GetName(), id);
        if (id < idLimit) {
          id++;
          if (0 == s_lhcbPartionName.CompareTo(obj->GetName(),
                                               TString::kIgnoreCase)) {
            m_currentPartition = obj->GetName();
            int entryId( m_partitionSelectorPopupMenu->GetEntry(m_currentPartition.c_str())->GetEntryId());
            m_partitionSelectorPopupMenu->DefaultEntry(entryId);
            m_partitionSelectorPopupMenu->Activate(entryId);
            if (m_verbosity >= Verbose) {
               std::cout << "Found default LHCb partition: " <<
                             m_currentPartition << std::endl;
            }
          }
        }
      }
    }
  }
  if (partitionIt) { delete partitionIt; partitionIt = 0; }

  if (m_verbosity >= Verbose) {
     std::cout << "Defaulting to partition: " << m_currentPartition <<
                   std::endl;
  }  
}
void PresenterMainFrame::clickedHistoSvcTreeItem(TGListTreeItem* node,
                                            EMouseButton btn,
                                            int x, int y) {
  if (0 != node && btn == kButton3) {
    m_histoSvcTreeContextMenu->PlaceMenu(x, y, 1, 1);
  }
}
void PresenterMainFrame::clickedHistoDBTreeItem(TGListTreeItem* node,
                                                EMouseButton btn,
                                                int x, int y) {
  if (0 != node && btn == kButton3) {
    m_histoDBContextMenu->PlaceMenu(x, y, 1, 1);
  }
}
void PresenterMainFrame::clickedPageTreeItem(TGListTreeItem* node,
                                             EMouseButton btn,
                                             int x, int y) {
  if (0 != node && btn == kButton3) {
    m_pagesContextMenu->PlaceMenu(x, y, 1, 1);
  } else if (0 != node &&
             NULL == node->GetFirstChild() &&
             ((EditorOnline != m_presenterMode) || (EditorOffline != m_presenterMode))) {
    loadSelectedPageFromDB(s_startupFile, m_savesetFileName);
  } else if (0 != node &&
             NULL == node->GetFirstChild() &&
             ((EditorOnline == m_presenterMode) || (EditorOffline == m_presenterMode))) {
    m_pagesContextMenu->PlaceMenu(x, y, 1, 1);
  }
}
void PresenterMainFrame::addDimSvcToHistoDB()
{
  disableAutoCanvasLayoutBtn();
  disableHistogramClearing();

  TGListTree* list = new TGListTree();
  checkedTreeItems(list, m_histoSvcListTree);

  TGListTreeItem* currentNode;
  currentNode = list->GetFirstItem();
  while (0 != currentNode) {
    try {
      if (0 != m_histogramDB) {
        m_histogramDB->declareHistByServiceName(
          std::string(*static_cast<TString*>(currentNode->GetUserData())));
      }
    } catch (std::string sqlException) {
      // TODO: add error logging backend
      if (m_verbosity >= Verbose) { std::cout << sqlException; }

      new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                   Form("OnlineHistDB server:\n\n%s\n",
                        sqlException.c_str()),
                   kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    }
    currentNode = currentNode->GetNextSibling();
  }
  try {
    if (0 != m_histogramDB) {
      m_histogramDB->commit();
      refreshHistoDBListTree();
    }
  } catch (std::string sqlException) {
    // TODO: add error logging backend
    if (m_verbosity >= Verbose) { std::cout << sqlException; }

    new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                 Form("OnlineHistDB server:\n\n%s\n",
                      sqlException.c_str()),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }
  list->Delete();
  delete list;
  list = NULL;

  m_histoSvcListTree->CheckAllChildren(m_histoSvcListTree->GetFirstItem(),
                                     s_uncheckTreeItems);
//  fClient->NeedRedraw(m_histoSvcListTree);
//  Resize();DoRedraw(); // wtf would trigger a redraw???
  enableAutoCanvasLayoutBtn();
}
void PresenterMainFrame::addHistoToPage(const std::string& histogramUrl)
{
  HistogramIdentifier histogramID = HistogramIdentifier(histogramUrl);
  if (histogramID.isDimFormat()) {
  }  
    int newHistoInstance = 0;
    // see if the histogram object exists already
    for (dbHistosOnPageIt = dbHistosOnPage.begin();
        dbHistosOnPageIt != dbHistosOnPage.end();
        ++dbHistosOnPageIt) {
      int existingHistoInstance = (*dbHistosOnPageIt)->instance();

      if (0 == (histogramID.histogramIdentifier()).compare((*dbHistosOnPageIt)->identifier()) &&
          (newHistoInstance < existingHistoInstance) ) {
        newHistoInstance = existingHistoInstance;
      }
    }
    //TODO: ? limit
    if (newHistoInstance > 998) { newHistoInstance = 0; }
    newHistoInstance++;

    OnlineHistDB*  histogramDB = NULL;
    OnlineHistogram* dimQuery = NULL;
    DimBrowser* dimBrowser = NULL;
    std::string currentPartition(m_currentPartition);
    
    if (isConnectedToHistogramDB()) {
      histogramDB = m_histogramDB;
      dimQuery = m_histogramDB->getHistogram(histogramID.histogramIdentifier());
    } else {
      histogramDB = NULL;
      dimQuery = NULL;
    }                   
    if ((History == m_presenterMode) || (EditorOffline == m_presenterMode)) {
      dimBrowser = NULL;
    } else if ((Online == m_presenterMode) || (EditorOnline == m_presenterMode)) {
      dimBrowser = m_dimBrowser;
      if (false == isConnectedToHistogramDB()) {
        currentPartition = histogramID.m_histogramUrl();
      }
    }

// TODO: merge this with loadSelectedPage()      
    DbRootHist* dbRootHist;
    dbRootHist = new DbRootHist(histogramID.histogramIdentifier(),
                                currentPartition,
                                2, newHistoInstance,
                                m_histogramDB, analysisLib(),
                                dimQuery,
                                m_verbosity,
                                dimBrowser);
    
     if ((0 != m_archive) && isConnectedToHistogramDB() &&
          (false == m_savesetFileName.empty()) &&
          ((History == m_presenterMode) || (EditorOffline == m_presenterMode))) {
          m_archive->fillHistogram(dbRootHist,
                                 s_startupFile,
                                 m_savesetFileName);
      }
                                              
  if (false == isConnectedToHistogramDB()) {
    // Set Properties
    TString paintDrawXLabel = bulkHistoOptions.m_xLabel;
    TString paintDrawYLabel = bulkHistoOptions.m_yLabel;
    int paintLineWidth = bulkHistoOptions.m_lineWidth;
    int paintFillColour = bulkHistoOptions.m_fillColour;
    int paintFillStyle = bulkHistoOptions.m_fillStyle;
    int paintLineStyle = bulkHistoOptions.m_lineStyle;
    int paintLineColour = bulkHistoOptions.m_lineColour;
    int paintStats = bulkHistoOptions.m_statsOption;
    if (dbRootHist && dbRootHist->rootHistogram) {
      dbRootHist->rootHistogram->SetXTitle(paintDrawXLabel.Data());
      dbRootHist->rootHistogram->SetYTitle(paintDrawYLabel.Data());
      dbRootHist->rootHistogram->SetLineWidth(paintLineWidth);
      dbRootHist->rootHistogram->SetFillColor(paintFillColour);
      dbRootHist->rootHistogram->SetFillStyle(paintFillStyle);
      dbRootHist->rootHistogram->SetLineStyle(paintLineStyle);
      dbRootHist->rootHistogram->SetLineColor(paintLineColour);
      dbRootHist->rootHistogram->SetStats(paintStats);

      TString paintDrawOption;
      if (s_H1D == dbRootHist->histogramType()) {
        m_drawOption = bulkHistoOptions.m_1DRootDrawOption;
        paintDrawOption = TString(m_drawOption + TString(" ") +
          bulkHistoOptions.m_genericRootDrawOption).Data();
      } else if (s_P1D == dbRootHist->histogramType() ||
          s_HPD == dbRootHist->histogramType()) {
        m_drawOption = bulkHistoOptions.m_1DRootDrawOption;
        paintDrawOption = TString(m_drawOption + TString(" ") +
          bulkHistoOptions.m_genericRootDrawOption + TString(" E ")).Data();
      } else if (s_H2D == dbRootHist->histogramType()) {
        m_drawOption = bulkHistoOptions.m_2DRootDrawOption;
        paintDrawOption = TString(m_drawOption + TString(" ") +
          bulkHistoOptions.m_genericRootDrawOption).Data();
      }
      dbRootHist->rootHistogram->SetOption(paintDrawOption.Data());                                           
    }
  }
  dbHistosOnPage.push_back(dbRootHist);
  paintHist(dbRootHist);
}
void PresenterMainFrame::addDimSvcToPage()
{
  disableAutoCanvasLayoutBtn();
  // TODO: add locking, exclusive, disable autolayout
  disableHistogramClearing();
  
  stopPageRefresh();
  if (m_clearedHistos) { clearHistos(); }
  if (m_referencesOverlayed) { toggleReferenceOverlay(); }

  fClient->WaitFor(dynamic_cast<TGWindow*>(
                   new HistoPropDialog(this, 646, 435, m_verbosity)));

  TGListTree* list = new TGListTree();
  checkedTreeItems(list, m_histoSvcListTree);

  TGListTreeItem* currentNode;
  currentNode = list->GetFirstItem();

  while (0 != currentNode) {
    addHistoToPage(std::string(*static_cast<TString*>(currentNode->GetUserData())));
    currentNode = currentNode->GetNextSibling();
  }
  list->Delete();
  delete list;
  list = NULL;
  m_histoSvcListTree->CheckAllChildren(m_histoSvcListTree->GetFirstItem(),
                                     s_uncheckTreeItems);
//  fClient->NeedRedraw(m_histoSvcListTree);
//  Resize();DoRedraw(); // wtf would trigger a redraw???

  editorCanvas->Update();

  enableAutoCanvasLayoutBtn();
}
void PresenterMainFrame::addDbHistoToPage()
{
  disableAutoCanvasLayoutBtn();
  try {
    if (0 != m_histogramDB) {
      TGListTree* list = new TGListTree();
      checkedTreeItems(list, m_databaseHistogramTreeList);
      TGListTreeItem* currentNode = list->GetFirstItem();
      while (0 != currentNode) {        
        addHistoToPage(std::string(*static_cast<TString*>(currentNode->GetUserData())));
        currentNode = currentNode->GetNextSibling();
      }
      list->Delete();
      delete list;
      list = NULL;
    }
  } catch (std::string sqlException) {
    // TODO: add error logging backend
    if (m_verbosity >= Verbose) { std::cout << sqlException; }

    new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                 Form("OnlineHistDB server:\n\n%s\n",
                       sqlException.c_str()),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }

  m_databaseHistogramTreeList->CheckAllChildren(m_databaseHistogramTreeList->GetFirstItem(),
                                                s_uncheckTreeItems);
//  fClient->NeedRedraw(m_databaseHistogramTreeList);
//  Resize();DoRedraw(); // wtf would trigger a redraw???
  editorCanvas->Update();
  enableAutoCanvasLayoutBtn();
}
void PresenterMainFrame::setHistogramDimSource(bool tree)
{
  bool resumeRefreshAfterLoading(false);
  if (m_refreshingPage) {
    stopPageRefresh();
    resumeRefreshAfterLoading = true;
  }
  if (m_clearedHistos) { clearHistos(); }
  
  if (tree == s_withoutTree) {
    m_currentPartition = m_partitionSelectorQuickButton->GetString();
    if (m_currentPartition == "refresh partitions") {
      m_currentPartition = s_lhcbPartionName.Data();      
    }    
  } else if (tree == s_withTree) {
    TGListTree* list = new TGListTree();
    checkedTreeItems(list, m_histoSvcListTree);
  
    TGListTreeItem* currentNode;
    currentNode = list->GetFirstItem();
    while (0 != currentNode) {
      std::string nodeHist = std::string(*static_cast<TString*>(currentNode->GetUserData()));
      dbHistosOnPageIt = dbHistosOnPage.begin();      
      while (dbHistosOnPageIt != dbHistosOnPage.end()) {
        if (HistogramIdentifier(nodeHist).histogramIdentifier() == (*dbHistosOnPageIt)->histogramIdentifier() &&
            HistogramIdentifier(nodeHist).histogramSetName() == (*dbHistosOnPageIt)->histogramSetName() &&
            HistogramIdentifier(nodeHist).histogramType() == (*dbHistosOnPageIt)->histogramType()) {
          (*dbHistosOnPageIt)->setDimServiceName(nodeHist);
        }
        ++dbHistosOnPageIt;
      }
      currentNode = currentNode->GetNextSibling();
    }
    list->Delete();
    delete list;
    list = NULL;
  
    m_histoSvcListTree->CheckAllChildren(m_histoSvcListTree->GetFirstItem(),
                                       s_uncheckTreeItems);
  }

//  fClient->NeedRedraw(m_histoSvcListTree);
//  Resize();DoRedraw(); // wtf would trigger a redraw???
  if (resumeRefreshAfterLoading) { startPageRefresh(); }
}
void PresenterMainFrame::dimCollapseAllChildren()
{
  if (0 != m_histoSvcListTree &&
      m_histoSvcListTree->GetSelected()) {
    collapseTreeChildrenItems(m_histoSvcListTree,
                              m_histoSvcListTree->GetSelected());
    fClient->NeedRedraw(m_histoSvcListTree);
//    Resize();DoRedraw(); // wtf would trigger a redraw???
  }
}
void PresenterMainFrame::dbHistoCollapseAllChildren()
{
  if (0 != m_databaseHistogramTreeList &&
      m_databaseHistogramTreeList->GetSelected()) {
    collapseTreeChildrenItems(m_databaseHistogramTreeList,
                              m_databaseHistogramTreeList->GetSelected());
    fClient->NeedRedraw(m_databaseHistogramTreeList);
//    Resize();DoRedraw(); // wtf would trigger a redraw???
  }
}
std::string PresenterMainFrame::convDimToHistoID(const std::string & dimSvcName)
{
  HistogramIdentifier histogram = HistogramIdentifier(dimSvcName);
  if (histogram.isDimFormat()) {
    return histogram.histogramIdentifier();
  } else {
    new TGMsgBox(fClient->GetRoot(), this, "DIM Service name error",
                 Form("The DIM servicename\n%s\n does not appear to follow the" \
                   "convention\nPlease use the following format:\n" \
                   "HnD/UTGID/Algorithmname/Histogramname\n where the UTGID " \
                   "normally has the following format:\n node00101_taskname_1 " \
                   "or simply taskname, without \"_\"", dimSvcName.c_str()),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
    return 0;
  }
}
void PresenterMainFrame::toggleHistoryPlots()
{
  if (m_historyTrendPlots) {
    m_historyPlotsButton->SetState(kButtonUp);
    m_viewMenu->UnCheckEntry(HISTORY_PLOTS_COMMAND);
    m_historyTrendPlots = false;
  } else {
    m_historyPlotsButton->SetState(kButtonDown);
    m_historyTrendPlots = true;
    m_viewMenu->CheckEntry(HISTORY_PLOTS_COMMAND);
  }
}
void PresenterMainFrame::toggleReferenceOverlay()
{
  if (m_referencesOverlayed) {
    m_overlayReferenceHistoButton->SetState(kButtonUp);
    m_viewMenu->UnCheckEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    m_referencesOverlayed = false;
    disableReferenceOverlay();
  } else {
    m_overlayReferenceHistoButton->SetState(kButtonDown);
    m_referencesOverlayed = true;
    m_viewMenu->CheckEntry(OVERLAY_REFERENCE_HISTO_COMMAND);
    enableReferenceOverlay();
  }
}
void PresenterMainFrame::toggleFastHitMapDraw()
{
  if (m_fastHitMapDraw) {
    m_viewMenu->UnCheckEntry(FAST_HITMAP_DRAW_COMMAND);
    m_fastHitMapDraw = false;
  } else {
    m_fastHitMapDraw = true;
    m_viewMenu->CheckEntry(FAST_HITMAP_DRAW_COMMAND);
  }
}
void PresenterMainFrame::pickReferenceHistogram()
{
  DbRootHist* histogram = selectedDbRootHistogram();
  
  if (0 != histogram) {
    TVirtualPad *padsav = gPad;
    fClient->WaitFor(new ReferencePicker(this, histogram));    
    padsav->cd();
  }
}
void PresenterMainFrame::saveSelectedHistogramAsReference()
{
  DbRootHist* histogram = selectedDbRootHistogram();
  
  if (0 != histogram && 0 != m_archive) {
    new TGMsgBox(fClient->GetRoot(), this, "Save as Reference Histogram",
                 "Are you sure to save selected histogram as a reference?",
                 kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
    switch (m_msgBoxReturnCode) {
      case kMBYes:
        m_archive->saveAsReferenceHistogram(histogram);
        break;
      default:
        break;
    }
  }
}
void PresenterMainFrame::setHistogramPropertiesInDB()
{
  try {
    if (0 != m_histogramDB) {
// TODO: map/unmap WaitForUnmap(TGWindow* w)   
      fClient->WaitFor(dynamic_cast<TGWindow*>(
                          new HistoPropDialog(this, 646, 435, m_verbosity)));

      TGListTree* list = new TGListTree();
      checkedTreeItems(list, m_databaseHistogramTreeList);

      TGListTreeItem* currentNode;
      currentNode = list->GetFirstItem();
      while (0 != currentNode) {
        OnlineHistogram* onlineHistogramToSet = m_histogramDB->
          getHistogram(std::string(*static_cast<TString*>(
                       currentNode->GetUserData())));

        std::string descriptionField = bulkHistoOptions.m_description.Data();

        std::string paintDrawXLabel = bulkHistoOptions.
          m_xLabel.Data();
        std::string paintDrawYLabel = bulkHistoOptions.
          m_yLabel.Data();
        std::string drawPatternTextEntry = bulkHistoOptions.
          m_drawPatternTextEntry.Data();          
          
        int paintLineWidth = bulkHistoOptions.m_lineWidth;
        int paintFillColour = bulkHistoOptions.m_fillColour;
        int paintFillStyle = bulkHistoOptions.m_fillStyle;
        int paintLineStyle = bulkHistoOptions.m_lineStyle;
        int paintLineColour = bulkHistoOptions.m_lineColour;
        int paintStats = bulkHistoOptions.m_statsOption;

        if (s_H1D == onlineHistogramToSet->hstype() ||
            s_P1D == onlineHistogramToSet->hstype() ||
            s_pfixMonH1D == onlineHistogramToSet->hstype() ||
            s_pfixMonProfile == onlineHistogramToSet->hstype() ||
            s_HPD == onlineHistogramToSet->hstype()) {
          m_drawOption = bulkHistoOptions.m_1DRootDrawOption;
        } else if (s_H2D == onlineHistogramToSet->hstype() ||
                   s_pfixMonH2F == onlineHistogramToSet->hstype() ||
                   s_pfixMonH2D == onlineHistogramToSet->hstype() ) {
          m_drawOption = bulkHistoOptions.m_2DRootDrawOption;
        }
        std::string paintDrawOption = TString(
          m_drawOption + TString(" ") +
          bulkHistoOptions.m_genericRootDrawOption).Data();

        onlineHistogramToSet->setDescr(descriptionField);

        onlineHistogramToSet->setDisplayOption("LABEL_X",
                                               &paintDrawXLabel);
        onlineHistogramToSet->setDisplayOption("LABEL_Y",
                                               &paintDrawYLabel);
        onlineHistogramToSet->setDisplayOption("FILLSTYLE",
                                               &paintFillStyle);
        onlineHistogramToSet->setDisplayOption("FILLCOLOR",
                                               &paintFillColour);
        onlineHistogramToSet->setDisplayOption("LINESTYLE",
                                               &paintLineStyle);
        onlineHistogramToSet->setDisplayOption("LINECOLOR",
                                               &paintLineColour);
        onlineHistogramToSet->setDisplayOption("LINEWIDTH",
                                               &paintLineWidth);
        onlineHistogramToSet->setDisplayOption("STATS",
                                               &paintStats);
        onlineHistogramToSet->setDisplayOption("DRAWOPTS",
                                               &paintDrawOption);

        onlineHistogramToSet->setDisplayOption("DRAWPATTERN",
                                               &drawPatternTextEntry);

        onlineHistogramToSet->saveDisplayOptions();

        currentNode = currentNode->GetNextSibling();
      }
      list->Delete();
      delete list;
      list = NULL;
      m_histogramDB->commit();
    }
  } catch (std::string sqlException) {
    // TODO: add error logging backend
    if (m_verbosity >= Verbose) { std::cout << sqlException; }

    new TGMsgBox(fClient->GetRoot(), this, "Database Error",
        Form("OnlineHistDB server:\n\n%s\n",
             sqlException.c_str()),
             kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }
//  refreshHistoDBListTree();
}
void PresenterMainFrame::deleteSelectedHistoFromDB() {
  new TGMsgBox(fClient->GetRoot(), this, "Delete histogram",
               "Are you sure to delete selected histogram from the database?",
               kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
  switch (m_msgBoxReturnCode) {
    case kMBNo:
      return;
  }
  try {
    if (0 != m_histogramDB) {
      TGListTree* list = new TGListTree();
      checkedTreeItems(list, m_databaseHistogramTreeList);

      TGListTreeItem* currentNode;
      currentNode = list->GetFirstItem();
      while (0 != currentNode) {
        std::string hist_id(*static_cast<TString*>(currentNode->GetUserData()));
        OnlineHistogram* histoToDelete = m_histogramDB->getHistogram(hist_id);
        if (histoToDelete) {
          // remove this histogram from current page if present
          dbHistosOnPageIt = dbHistosOnPage.begin();
          while (dbHistosOnPageIt != dbHistosOnPage.end() ) {
            if ( (*dbHistosOnPageIt)->identifier() == hist_id ) {
              delete *dbHistosOnPageIt;
              *dbHistosOnPageIt = NULL;
              dbHistosOnPageIt = dbHistosOnPage.erase(dbHistosOnPageIt);
            }
            else {
              dbHistosOnPageIt++;
            }
          }
          editorCanvas->Update();
          // remove from DB
          m_histogramDB->removeHistogram(histoToDelete, false);
        }
        currentNode = currentNode->GetNextSibling();
      }
      list->Delete();
      delete list;
      list = NULL;
      m_histogramDB->commit();
    }
  } catch (std::string sqlException) {
    // TODO: add error logging backend
    if (m_verbosity >= Verbose) { std::cout << sqlException; }
    new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                 Form("OnlineHistDB server:\n\n%s\n",
                      sqlException.c_str()),
                 kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
  }
  refreshHistoDBListTree();
}
void PresenterMainFrame::enablePageLoading() {
  m_pagesContextMenu->EnableEntry(M_LoadPage_COMMAND);
}
void PresenterMainFrame::loadSelectedPageFromDB(const std::string & timePoint,
                                                const std::string & pastDuration)
{
  if (isConnectedToHistogramDB()) {
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetWaitCursor());

    bool resumeRefreshAfterLoading(false);
    if (m_refreshingPage) {
      stopPageRefresh();
      resumeRefreshAfterLoading = true;
    }
    if (m_clearedHistos) { clearHistos(); }

    disableAutoCanvasLayoutBtn();
//    removeHistogramsFromPage();

    std::string rw_timePoint(timePoint);
    std::string rw_pastDuration(pastDuration);

    if (0 != m_archive &&
        ((History == m_presenterMode) || (EditorOffline == m_presenterMode))) {
      if ("last 8 hours" == m_historyIntervalQuickButton->GetString()) {
        rw_timePoint = s_Now;
        rw_pastDuration = std::string("08:00:00");
      } else if ("last 1 hour" == m_historyIntervalQuickButton->GetString()) {
        rw_timePoint = s_Now;
        rw_pastDuration = std::string("01:00:00");
      } else if ("preset file" == m_historyIntervalQuickButton->GetString()) {
        rw_timePoint = s_startupFile;
        rw_pastDuration = pastDuration;
      } else if ("set file" == m_historyIntervalQuickButton->GetString()) {
        rw_timePoint = s_startupFile;
        rw_pastDuration = pastDuration;
      } else {
        rw_timePoint = global_timePoint;
        rw_pastDuration = global_pastDuration;        
      }
      if (m_verbosity >= Verbose) {
        std::cout << "Navigation step size" << global_stepSize << std::endl;
      }
    }

    TGListTreeItem* node = m_pagesFromHistoDBListTree->GetSelected();
    if (0 != node && node->GetUserData()) {
      std::string path = std::string(*static_cast<TString*>(
                                     node->GetUserData()));
startBenchmark(path);                                     
      try {
        removeHistogramsFromPage();
        m_onlineHistosOnPage.clear();
        OnlineHistPage* page = m_histogramDB->getPage(path);
        if (m_verbosity >= Verbose) {
          std::cout << "Loading page: "  << path << std::endl;
        }
        if (m_verbosity >= Debug) {
          page->dump();
        }

        if (0 != page) {
          
//          if (false == page->patternFile().empty()) {
////            m_drawPattern;
//            TFile rootFile(page->patternFile().c_str()); 
//           if (rootFile.IsZombie()) {
//                std::cout << "Error opening page pattern file" << std::endl;
//            } else {
//               TIter next1(rootFile.GetListOfKeys());
//               TKey* key;    
//               while ((key = (TKey*)next1())) {
//                 if (key->ReadObj()->InheritsFrom(TCanvas::Class())) {
//               m_drawPattern = (TCanvas*)key->ReadObj();
//                 }
//               }        
//           TPad *padsav = (TPad*)gPad;
//        //   gPad =   editorCanvas->cd();
//           TObject *obj; //, *clone;
//        TPad* pad = editorCanvas;
//           dynamic_cast<TAttLine*>(m_drawPattern)->Copy((TAttLine&)*pad);
//           dynamic_cast<TAttFill*>(m_drawPattern)->Copy((TAttFill&)*pad);
//           dynamic_cast<TAttPad*>(m_drawPattern)->Copy((TAttPad&)*pad);
//        
//           TIter next(m_drawPattern->GetListOfPrimitives());
//           while ((obj=next())) {
//              pad->cd();
//              if (TBox::Class() == obj->IsA() ||
//                  TLine::Class() == obj->IsA() ||
//                  TText::Class() == obj->IsA()) {
//                  obj->Draw();
//        //      clone = obj->Clone();
//        //      pad->GetListOfPrimitives()->Add(clone,next.GetOption());
//              }
//           }
//           if (padsav) padsav->cd();
//          }
//          rootFile.Close();
//       } else {
//        editorCanvas->Clear();
//       }
////       if (m_verbosity >= Debug) {
//std::cout << "page pattern file: " << page->patternFile() << std::endl;
////       } 
          page->getHistogramList(&m_onlineHistosOnPage);
          m_onlineHistosOnPageIt = m_onlineHistosOnPage.begin();
          m_tasksNotRunning.clear();          
          while (m_onlineHistosOnPageIt != m_onlineHistosOnPage.end()) {
            DbRootHist* dbRootHist;
            if((History == m_presenterMode) || (EditorOffline == m_presenterMode)) { // no need for DIM
              dbRootHist = new DbRootHist((*m_onlineHistosOnPageIt)->histo->identifier(),
                                          std::string(""),
                                          2, (*m_onlineHistosOnPageIt)->instance,
                                          m_histogramDB, analysisLib(),
                                          (*m_onlineHistosOnPageIt)->histo,
                                          m_verbosity,
                                          NULL);
            } else if((Online == m_presenterMode) || (EditorOnline == m_presenterMode)) {
              bool taskNotRunning(false);             
              for (m_tasksNotRunningIt = m_tasksNotRunning.begin();
                   m_tasksNotRunningIt != m_tasksNotRunning.end(); m_tasksNotRunningIt++) {
                if (0 == ((*m_tasksNotRunningIt).compare((*m_onlineHistosOnPageIt)->histo->task()))) {
                  taskNotRunning = true;            
                }                          
              }
              DimBrowser* dimBrowser = NULL;
              std::string currentPartition("");
              if (taskNotRunning) {
                dimBrowser = NULL;
                currentPartition = "";
              } else {
                dimBrowser = m_dimBrowser;
                currentPartition = m_currentPartition;
              } 
              dbRootHist = new DbRootHist((*m_onlineHistosOnPageIt)->histo->identifier(),
                                          currentPartition,
                                          2, (*m_onlineHistosOnPageIt)->instance,
                                          m_histogramDB, analysisLib(),
                                          (*m_onlineHistosOnPageIt)->histo,
                                          m_verbosity,
                                          dimBrowser);
                                      
              if (dbRootHist->isEmptyHisto()) {
                m_tasksNotRunning.push_back((*m_onlineHistosOnPageIt)->histo->task());
              } 
            }

            if (0 != m_archive &&
                ((History == m_presenterMode) ||
                 (EditorOffline == m_presenterMode && canWriteToHistogramDB()))) {
              m_archive->fillHistogram(dbRootHist,
                                       rw_timePoint,
                                       rw_pastDuration);
//              if (dbRootHist && dbRootHist->isEmptyHisto()) {
                resumeRefreshAfterLoading = false;
//              }                                       
            }
            
            dbHistosOnPage.push_back(dbRootHist);
            
            if (m_verbosity >= Verbose) {
              std::cout << "db identifier "
                        << (*m_onlineHistosOnPageIt)->histo->identifier()
                        << std::endl << "\tdb task "
                        << (*m_onlineHistosOnPageIt)->histo->task() << std::endl    
                        << "\tdb dimServiceName "
                        << (*m_onlineHistosOnPageIt)->histo->dimServiceName()
                        << std::endl;
            }
            if (m_verbosity >= Debug) { (*m_onlineHistosOnPageIt)->histo->dump(); }            

            double xlow = (*m_onlineHistosOnPageIt)->xmin;
            double ylow = (*m_onlineHistosOnPageIt)->ymin;
            double xup  = (*m_onlineHistosOnPageIt)->xmax;
            double yup  = (*m_onlineHistosOnPageIt)->ymax;

           TPad* ovPad=NULL;
           if((*m_onlineHistosOnPageIt)->isOverlap()) { // histogram must be overdrawn on previous hist: look for the corresponding TPad
             OnlineHistoOnPage *mother = (*m_onlineHistosOnPageIt)->getOverlap();
             std::vector<DbRootHist*>::iterator prevh;
             for (prevh=dbHistosOnPage.begin() ; (*prevh) != dbRootHist ; prevh++) {
               if ( (*prevh)->onlineHistogram() == mother->histo) {
                 ovPad = (*prevh)->hostingPad;
                 break;
               }
             }
           }
           if (m_verbosity >= Debug) {
             if (ovPad) {
               std::cout << "drawing overlap on pad " << ovPad <<std::endl;
             } else {
               std::cout << "drawing pad X "<< xlow << " to "<< xup <<
                 "  Y " << ylow << " to " << yup <<std::endl;
             }
           }
          dbRootHist->draw(editorCanvas, xlow, ylow, xup, yup, m_fastHitMapDraw, ovPad);
          m_onlineHistosOnPageIt++;
          }
          
          m_pageDescriptionView->Clear();
          m_pageDescription = page->doc();
          m_pageDescriptionView->LoadBuffer(m_pageDescription.c_str());
          m_pageDescriptionView->DataChanged();    
  // workaround attempt for ROOT painter objects
  editorCanvas->Update();
  dbHistosOnPageIt = dbHistosOnPage.begin();
  while( dbHistosOnPageIt !=  dbHistosOnPage.end() ) {
     (*dbHistosOnPageIt)->setDrawOptionsFromDB((*dbHistosOnPageIt)->hostingPad);
     dbHistosOnPageIt++;
  }
  editorCanvas->Update();        
                
        }
        
        } catch (std::string sqlException) {
        // TODO: add error logging backend - MsgStream?
        setStatusBarText(sqlException.c_str(), 2);
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }

        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("Could not save the page to OnlineHistDB:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
stopBenchmark(path);
printBenchmark(path);      
    }
    
    if (resumeRefreshAfterLoading) { startPageRefresh(); }
gVirtualX->SetCursor(GetId(), gClient->GetResourcePool()->GetDefaultCursor());

  }
  if (m_referencesOverlayed) {
    enableReferenceOverlay();
  } else {
    disableReferenceOverlay();
  }
//  editorCanvas->Update();
  if ((EditorOnline == m_presenterMode) || (EditorOffline == m_presenterMode)) {
    enableAutoCanvasLayoutBtn();
  }  
}
void PresenterMainFrame::deleteSelectedPageFromDB()
{
  new TGMsgBox(fClient->GetRoot(), this, "Delete Page",
               "Are you sure to delete selected page from the database?",
               kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
  switch (m_msgBoxReturnCode) {
    case kMBNo:
      return;
  }

  if (canWriteToHistogramDB()) {
    TGListTreeItem* node = m_pagesFromHistoDBListTree->GetSelected();

    if (node && node->GetUserData()) {
      std::string path = std::string(*static_cast<TString*>(
                                     node->GetUserData()));
      try {
        OnlineHistPage* page = m_histogramDB->getPage(path);
        if (0 != page) {
          m_histogramDB->removePage(page);
          m_histogramDB->commit();
          refreshPagesDBListTree();
        }
      } catch (std::string sqlException) {
        // TODO: add error logging backend - MsgStream?
        setStatusBarText(sqlException.c_str(), 2);
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }

        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("Could not delete the page to OnlineHistDB:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
    }
  }
}
void PresenterMainFrame::deleteSelectedFolderFromDB()
{
// new TGMsgBox(fClient->GetRoot(), this, "Delete Folder",
//              "Are you sure to delete selected folder from the Database?",
//              kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
//  switch (m_msgBoxReturnCode) {
//    case kMBNo:
//      return;
//  }
  if (canWriteToHistogramDB()) {

    TGListTreeItem* node = m_pagesFromHistoDBListTree->GetSelected();

    if (0 != node) {
      char path[1024];
      m_pagesFromHistoDBListTree->GetPathnameFromItem(node, path);
      std::string folder = std::string(path);
      // Drop DB url
      folder = folder.erase(0, std::string(
        m_pagesFromHistoDBListTree->GetFirstItem()->GetText()).length()+1);
      if (m_verbosity >= Verbose) {
        std::cout << "Folder to delete: " << folder << std::endl;
      }

      try {
        m_histogramDB->removePageFolder(folder);
        m_histogramDB->commit();
        refreshPagesDBListTree();
      } catch (std::string sqlException) {
        // TODO: add error logging backend - MsgStream?
        setStatusBarText(sqlException.c_str(), 2);
        if (m_verbosity >= Verbose) { std::cout << sqlException << std::endl; }

        new TGMsgBox(fClient->GetRoot(), this, "Database Error",
                     Form("Could delete the page to OnlineHistDB:\n\n%s\n",
                          sqlException.c_str()),
                     kMBIconExclamation, kMBOk, &m_msgBoxReturnCode);
      }
    }
  }
}
void PresenterMainFrame::paintHist(DbRootHist* histogram)
{
  //  xlow [0, 1] is the position of the bottom left point of the pad
  //             expressed in the mother pad reference system
  //  ylow [0, 1] is the Y position of this point.
  //  xup  [0, 1] is the x position of the top right point of the pad
  //             expressed in the mother pad reference system
  //  yup  [0, 1] is the Y position of this point.

  double xlow = 0.1 + 0.01*m_histoPadOffset;
  double ylow = xlow;
  double xup = 0.5 + 0.01*m_histoPadOffset;
  double yup = xup;

  if (0.5 == xlow) { m_histoPadOffset = 0; }

  m_histoPadOffset++;
  histogram->draw(editorCanvas, xlow, ylow, xup, yup, m_fastHitMapDraw);
}
void PresenterMainFrame::autoCanvasLayout()
{
  double xmargin = 0.01;
  double ymargin = 0.01;
  int nx = (int) ceil(sqrt((double)dbHistosOnPage.size()));
  int ny = nx;

  if (nx <= 0) { nx = 1; }
  if (ny <= 0) { ny = 1; }
  int ix, iy;
  double x1, y1, x2, y2;
  double dx, dy;

  dbHistosOnPageIt = dbHistosOnPage.begin();

  if (xmargin > 0 && ymargin > 0) {
    dy = 1/double(ny);
    dx = 1/double(nx);
    for (iy = 0; iy < ny; ++iy) {
      if (dbHistosOnPage.end() == dbHistosOnPageIt)  { break; }
      y2 = 1 - iy*dy - ymargin;
      y1 = y2 - dy + 2*ymargin;
      if (y1 < 0) { y1 = 0; }
      if (y1 > y2) { continue; }
      for (ix = 0; ix < nx; ++ix) {
        if (dbHistosOnPage.end() == dbHistosOnPageIt)  { break; }
        x1 = ix*dx + xmargin;
        x2 = x1 + dx - 2*xmargin;
        if (x1 > x2) { continue; }
        if (0 != (*dbHistosOnPageIt) &&
            0 != ((*dbHistosOnPageIt)->hostingPad)) {
          ((*dbHistosOnPageIt)->hostingPad)->SetPad(x1, y1, x2, y2);
          ((*dbHistosOnPageIt)->hostingPad)->Modified();
        }
        ++dbHistosOnPageIt;
      }
    }
  }
  editorCanvas->Update();
}
void PresenterMainFrame::deleteSelectedHistoFromCanvas()
{
  bool resumeRefreshAfterLoading(false);
  if (m_refreshingPage) {
    stopPageRefresh();
    resumeRefreshAfterLoading = true;
  }
  bool referenceOverlay(false);
  if (m_referencesOverlayed) {
    disableReferenceOverlay();
    referenceOverlay = true;
  }
  if (m_clearedHistos) { clearHistos(); }
    
  DbRootHist* histogram = selectedDbRootHistogram();
  bool foundSelectedHisto(false);
  if (0 != histogram) {
    dbHistosOnPageIt = dbHistosOnPage.begin();
    while (dbHistosOnPageIt != dbHistosOnPage.end()) {
      if (0 == ((*dbHistosOnPageIt)->histoRootName()).
                compare(histogram->histoRootName())) {
        foundSelectedHisto = true;
        break;
      }
      if (false == foundSelectedHisto) { ++dbHistosOnPageIt; }
    }
    if (foundSelectedHisto) {
      new TGMsgBox(fClient->GetRoot(), this, "Remove Histogram",
                   "Are you sure to remove selected histogram from the page?",
                   kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
      switch (m_msgBoxReturnCode) {
        case kMBYes:
          delete (*dbHistosOnPageIt);
          (*dbHistosOnPageIt) = NULL;
          dbHistosOnPage.erase(dbHistosOnPageIt);
          editorCanvas->Update();
//          return;
          break;
        default:
          break;
      }
    }
  }
  if (resumeRefreshAfterLoading) { startPageRefresh(); }
  if (referenceOverlay) { enableReferenceOverlay(); } 
}
DbRootHist* PresenterMainFrame::selectedDbRootHistogram()
{
 DbRootHist* dbRootHist = 0;

  bool referenceOverlay(false);
  if (m_referencesOverlayed) {
    disableReferenceOverlay();
    referenceOverlay = true;
  }

  if (0 != gPad) {
    TIter nextPadElem(gPad->GetListOfPrimitives());
    TObject* histoCandidate;
    while (0 != (histoCandidate = nextPadElem())) {
      if (histoCandidate->InheritsFrom(TH1::Class())) {
        for (dbHistosOnPageIt = dbHistosOnPage.begin();
        dbHistosOnPageIt != dbHistosOnPage.end();
        dbHistosOnPageIt++) {
          if (0 == std::string((*dbHistosOnPageIt)->rootHistogram->GetName()).compare(histoCandidate->GetName())) {
            dbRootHist = *dbHistosOnPageIt;
          }
        }
      }   
    }
    
    if (0 == dbRootHist) {
    new TGMsgBox(fClient->GetRoot(), this, "Histogram Selection",
                 "Please select a histogram with the middle mouse button",
                 kMBIconStop, kMBOk, &m_msgBoxReturnCode);
    }
  }
  if (referenceOverlay) { enableReferenceOverlay(); }
  return dbRootHist;
}
void PresenterMainFrame::refreshClock()
{
  if (currentTime) {
    currentTime->Set();
    m_statusBarTop->SetText(currentTime->AsSQLString(), 1);
  }
}
void PresenterMainFrame::previousInterval()
{
  if(History == m_presenterMode &&
    s_timeInterval == m_historyMode &&
    ("preset interval" == m_historyIntervalQuickButton->GetString() ||
     "set interval" == m_historyIntervalQuickButton->GetString())    
    ) {
    global_timePoint = m_archive->substractIsoTimeDate(global_timePoint,
                                                       global_stepSize);
    loadSelectedPageFromDB(global_timePoint, global_pastDuration);                                                       
    if (m_verbosity >= Verbose) {
      std::cout << "after previousInterval global_timePoint " << global_timePoint << std::endl;
    }
  }
}
void PresenterMainFrame::nextInterval()
{
  if(History == m_presenterMode &&
     s_timeInterval == m_historyMode &&
     ("preset interval" == m_historyIntervalQuickButton->GetString() ||
      "set interval" == m_historyIntervalQuickButton->GetString())
     ) {
    global_timePoint = m_archive->addIsoTimeDate(global_timePoint,
                                                 global_stepSize);
    loadSelectedPageFromDB(global_timePoint, global_pastDuration);
    if (m_verbosity >= Verbose) {
      std::cout << "after nextInterval global_timePoint " << global_timePoint << std::endl;
    }
  }
}
void PresenterMainFrame::refreshPage()
{
  if (m_verbosity >= Verbose) {
    std::cout << "refreshing." << std::endl;
  }

  dbHistosOnPageIt = dbHistosOnPage.begin();
  while (dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*dbHistosOnPageIt)->fillHistogram();
    (*dbHistosOnPageIt)->normalizeReference();
    ++dbHistosOnPageIt;
  }
  editorCanvas->Update();
}
void PresenterMainFrame::enableHistogramClearing()
{
  dbHistosOnPageIt = dbHistosOnPage.begin();
  while (dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*dbHistosOnPageIt)->enableClear();
    ++dbHistosOnPageIt;
  }
}
void PresenterMainFrame::disableHistogramClearing()
{
  dbHistosOnPageIt = dbHistosOnPage.begin();
  while (dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*dbHistosOnPageIt)->disableClear();
    ++dbHistosOnPageIt;
  }
}
void PresenterMainFrame::enableReferenceOverlay()
{
  bool stopped(false);
  if (m_refreshingPage) {
    stopPageRefresh();
    stopped = true;
  }
  dbHistosOnPageIt = dbHistosOnPage.begin();
  while (dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*dbHistosOnPageIt)->referenceHistogram(Show);
    ++dbHistosOnPageIt;
  }
  editorCanvas->Update();
  if (stopped) { startPageRefresh(); }
}
void PresenterMainFrame::disableReferenceOverlay()
{
  dbHistosOnPageIt = dbHistosOnPage.begin();
  while (dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*dbHistosOnPageIt)->referenceHistogram(Hide);
    ++dbHistosOnPageIt;
  }
  editorCanvas->Update();
}
void PresenterMainFrame::enablePageRefresh()
{
  editorCanvas->SetEditable(false);
  dbHistosOnPageIt = dbHistosOnPage.begin();
  while (dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*dbHistosOnPageIt)->disableEdit();
    ++dbHistosOnPageIt;
  }
}
void PresenterMainFrame::disablePageRefresh()
{
  editorCanvas->SetEditable(true);
  dbHistosOnPageIt = dbHistosOnPage.begin();
  while (dbHistosOnPageIt != dbHistosOnPage.end()) {
    (*dbHistosOnPageIt)->enableEdit();
    ++dbHistosOnPageIt;
  }
}
void PresenterMainFrame::enablePageUndocking()
{
  m_viewMenu->EnableEntry(UNDOCK_PAGE_COMMAND);
}
void PresenterMainFrame::disablePageUndocking()
{
  m_viewMenu->DisableEntry(UNDOCK_PAGE_COMMAND);
}
void PresenterMainFrame::removeHistogramsFromPage()
{
//  new TGMsgBox(fClient->GetRoot(), this, "Clear Page",
//        "Do you really want to clear the current page?",
//        kMBIconQuestion, kMBYes|kMBNo, &m_msgBoxReturnCode);
//    switch (m_msgBoxReturnCode) {
//      case kMBNo:
//        return;
//  }
  disablePageRefresh();
  stopPageRefresh();
  editorCanvas->SetEditable(true);

  if (m_verbosity >= Verbose) { std::cout << "clearCanvas." << std::endl; }

  dbHistosOnPageIt = dbHistosOnPage.begin();
  while (dbHistosOnPageIt != dbHistosOnPage.end()) {
    if (*dbHistosOnPageIt) {
      delete *dbHistosOnPageIt;
      *dbHistosOnPageIt = NULL;
    }
    ++dbHistosOnPageIt;
  }
  dbHistosOnPage.clear();
  m_histoPadOffset = 0;

  editorCanvas->cd();
  editorCanvas->Clear();
  editorCanvas->Update();
}
void PresenterMainFrame::EventInfo(int /**/, int px, int py, TObject* selected)
{
  const char* text0;
  const char* text1;
  const char* text3;
  text0 = selected->GetTitle();
  setStatusBarText(text0, 0);
  text1 = selected->GetName();
  setStatusBarText(text1, 1);
  text3 = selected->GetObjectInfo(px, py);
  m_statusBarTop->SetText(text3, 0);
}
void PresenterMainFrame::about()
{
  std::stringstream config;
  config << std::string("Version: ") << s_presenterVersion << std::endl << std::endl <<
            std::string("Histogram Database version: ") << OnlineHistDBEnv_constants::version << std::endl <<
            std::string("Histogram Database schema: ") << OnlineHistDBEnv_constants::DBschema << std::endl <<
            std::string("Analysis Library version: ")  << OMAconstants::version << std::endl <<
            std::string("Connected to Histogram Database: ") << (isConnectedToHistogramDB()? std::string("yes") : std::string("no")) << std::endl;
  if (0 != m_histogramDB) {
     config << std::string("Write access to Histogram Database: ") << (canWriteToHistogramDB()? std::string("yes") : std::string("no")) << std::endl <<
     
               std::string("Reference histograms directory: ") << m_referencePath << std::endl <<
               std::string("Savesets directory: ") << m_savesetPath << std::endl <<
               
               std::string("DB Reference histograms directory: ") << dynamic_cast<OnlineHistDBEnv*>(m_histogramDB)->refRoot() << std::endl <<
               std::string("DB Savesets directory: ") << dynamic_cast<OnlineHistDBEnv*>(m_histogramDB)->savesetsRoot() << std::endl;
  }
  new TGMsgBox(fClient->GetRoot(), this, "LHCb Presenter",
               Form("%s", config.str().c_str()),
               kMBIconAsterisk, kMBOk, &m_msgBoxReturnCode);
}
