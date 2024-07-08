#include "NFrame.h"
#include "NFunc.h"


#include <vector>
#include "TGWindow.h"


extern ReadoutConfig_t Rcfg;
extern int32_t handle;


		


MainFrame::MainFrame(const TGWindow *p, UInt_t w, UInt_t h)
{
	
	fMain = new TGMainFrame(p, w, h);
	Rcfg.main = fMain;
	

   // use hierarchical cleaning
   fMain->SetCleanup(kDeepCleanup);

   fMain->Connect("CloseWindow()", "MainFrame", this, "CloseWindow()");

   // Create menubar and popup menus. The hint objects are used to place
   // and group the different menu widgets with respect to eachother.
   fMenuDock = new TGDockableFrame(fMain);
   fMain->AddFrame(fMenuDock, new TGLayoutHints(kLHintsExpandX, 0, 0, 1, 0));
   fMenuDock->SetWindowName("NRoot Menu");

   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
	
	fMenuFile = new TGPopupMenu(gClient->GetRoot());
	fMenuFile->AddEntry("&Open...", M_FILE_OPEN);
	fMenuFile->AddEntry("Save h_ampl &txt", M_FILE_SAVE_AMPL_TXT);
	fMenuFile->AddEntry("Save &histo", M_FILE_SAVE_HISTO);
	
	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("E&xit", M_FILE_EXIT);
	
	fMenuFile->DisableEntry(M_FILE_OPEN);
	
   


   fMenuHelp = new TGPopupMenu(gClient->GetRoot());
   fMenuHelp->AddEntry("&Manual", M_MANUAL);
   fMenuHelp->AddEntry("&About", M_HELP_ABOUT);

   // HandleMenu() method.
	fMenuFile->Connect("Activated(Int_t)", "MainFrame", this, "HandleMenu(Int_t)");

	fMenuHelp->Connect("Activated(Int_t)", "MainFrame", this, "HandleMenu(Int_t)");

   fMenuBar = new TGMenuBar(fMenuDock, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   fMenuDock->AddFrame(fMenuBar, fMenuBarLayout);


	TGHorizontalFrame *hframe1 = new TGHorizontalFrame(fMain,200,40);
	TGVerticalFrame *vframe1 = new TGVerticalFrame(hframe1,200,40);		
    hframe1->AddFrame(vframe1, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));//
	

	
	TGGroupFrame *gframe_opt = new TGGroupFrame(hframe1, "Options", kVerticalFrame);
	gframe_opt->SetTitlePos(TGGroupFrame::kRight); // right aligned
	vframe1->AddFrame(gframe_opt, new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5));//

   // 2 column, n rows
   gframe_opt->SetLayoutManager(new TGMatrixLayout(gframe_opt, 0, 1, 10));

const char *numlabel[] = {
	 "Port \n[v1718_N]",
   "Threshold",
   "DrawTime",
	"Timer"
	};	
	
const Double_t numinit[] = {
   //20, (double)WDcfg.Threshold[0], (double)WDcfg.Threshold[1], Rcfg.DrawTime, 0, 300
   1, 30, Rcfg.DrawTime, 300
};	

	
int iStyle[]	= { 0, 0, 2, 0}; 	
	
   for (int i = 0; i < 4; i++) {
      fF[i] = new TGHorizontalFrame(gframe_opt, 200, 30);
      gframe_opt->AddFrame(fF[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
      fNumericEntries[i] = new TGNumberEntry(fF[i], numinit[i], 8, i + 30, (TGNumberFormat::EStyle) iStyle[i]); //numinit[i], 7, i + 20, (TGNumberFormat::EStyle) iStyle[i]
	  fNumericEntries[i]->Connect("ValueSet(Long_t)", "MainFrame", this, "DoSetVal()");
      fF[i]->AddFrame(fNumericEntries[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
      fLabel[i] = new TGLabel(fF[i], numlabel[i]);
      fF[i]->AddFrame(fLabel[i], new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	  if ( i == 0)		  
	  	fNumericEntries[i]->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 5); // low level detect threshold [0:99] limit 0:500 mV
	  if ( i == 1)		  
	  	fNumericEntries[i]->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, 99); // low level detect threshold [0:99] limit 0:500 mV
	  if ( i == 3 ) {
		  fNumericEntries[i]->SetState(kFALSE);
		  fCTime = new TGCheckButton(fF[i], new TGHotString(""), 20);	
		  fCTime->Connect("Clicked()", "MainFrame", this, "DoCheckBox()");
		  fF[i]->AddFrame(fCTime, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	  } 
   }
	
	Rcfg.port = fNumericEntries[0]->GetNumber( );
	Rcfg.thr = fNumericEntries[1]->GetNumber( );
	
	
   
   gframe_opt->Resize();
   
   
	 
    fInitButton = new TGTextButton(vframe1, " In&it ", 1);
    fInitButton->SetFont(sFont); 
    fInitButton->Resize(60, 30);
    fInitButton->Connect("Clicked()","MainFrame",this,"InitButton()");
    vframe1->AddFrame( fInitButton, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 4, 4, 4, 4));	
	
   fClearButton = new TGTextButton(vframe1, " Cle&ar ", 1);
   fClearButton->SetFont(sFont);
   fClearButton->Resize(60, 30);
   fClearButton->SetState (kButtonDisabled);
   fClearButton->Connect("Clicked()","MainFrame",this,"ClearHisto()");
   vframe1->AddFrame(fClearButton, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 4, 4, 4, 4));	
	
	Rcfg.TLabel = new TGLabel(vframe1, "          Timer          ");
	Rcfg.TLabel->SetTextFont(sFont);
	Rcfg.TLabel->Resize(200, 30);
	
	vframe1->AddFrame(Rcfg.TLabel, new TGLayoutHints(kLHintsCenterY | kLHintsLeft, 2, 2, 2, 2));
	
	vframe1->Resize();
	
	//fEcanvas1 = new TRootEmbeddedCanvas("Ecanvas1", hframe1, WDcfg.windx - 300, WDcfg.windy - 120); // 1600 780
	fEcanvas1 = new TRootEmbeddedCanvas("Ecanvas1", hframe1, 1500, 730); // 1600 880
	hframe1->AddFrame(fEcanvas1, new TGLayoutHints(kLHintsCenterX, 10, 5, 25, 0));//kLHintsExpandX |   kLHintsExpandY
	hframe1->Resize();

	
    fMain->AddFrame(hframe1, new TGLayoutHints(kLHintsCenterX, 2, 2 , 2, 2) );
	
	Rcfg.can = fEcanvas1->GetCanvas();
		
	// status bar
	Int_t parts[] = {13, 13, 13, 13, 22, 26};
	Rcfg.StatusBar = new TGStatusBar(fMain, 100, 20, kHorizontalFrame); //kHorizontalFrame //kSunkenFrame
	Rcfg.StatusBar->SetParts(parts,6);
	fMain->AddFrame(Rcfg.StatusBar, new TGLayoutHints(kLHintsBottom | kLHintsLeft | kLHintsExpandX, 0, 0, 2, 0));
	
	
	TGHorizontalFrame *hframe2 = new TGHorizontalFrame(fMain, 200, 40);
 	fStartButton = new TGTextButton(hframe2," Sta&rt ", 1);
	fStartButton->SetFont(sFont);
    fStartButton->Resize(60, 30);
	fStartButton->SetState (kButtonDisabled);
  	fStartButton->Connect("Clicked()","MainFrame", this, "StartButton()");
  	hframe2->AddFrame(fStartButton, new TGLayoutHints(kLHintsCenterY |  kLHintsExpandX, 4, 4, 4, 4));

	fStopButton = new TGTextButton(hframe2,"  S&top  ", 1);
    fStopButton->SetFont(sFont);
    fStopButton->Resize(60, 30);
	fStopButton->SetState (kButtonDisabled);
	fStopButton->Connect("Clicked()","MainFrame",this,"StopButton()");	
    hframe2->AddFrame(fStopButton, new TGLayoutHints(kLHintsCenterY | kLHintsLeft | kLHintsExpandX, 4, 4, 4, 4));
	
	hframe2->Resize();
	

	
   fMain->AddFrame(hframe2, new TGLayoutHints(kLHintsCenterX,       2, 2, 20, 2));
   fMain->SetWindowName("NRoot");
   fMain->MapSubwindows( );

	fMain->Resize( );
	fMain->MapWindow( );
   //fMain->Print();
   Connect("Created()", "MainFrame", this, "Welcome()");
   Created( );
}

MainFrame::~MainFrame()
{
   // Delete all created widgets.

   delete fMenuFile;

   delete fMain;
}

void MainFrame::CloseWindow()
{
	N957ErrorCodes ret = N957Success;
	ret = N957_SwClear(handle);
	
   gApplication->Terminate();
}

void MainFrame::DoCheckBox(){
	
	TGButton *btn = (TGButton *) gTQSender;
	Int_t id = btn->WidgetId();
		
	
	 //Timer checkbox
	if (id == 20 ) {
	   fNumericEntries[3]->SetState( fCTime->GetState() == kButtonDown ? kTRUE : kFALSE );
	   Rcfg.fTimer = fCTime->GetState( ) == kButtonDown ? true : false;
	}  
			
	
	
}

void MainFrame::DoSetVal( ){
	TGNumberEntry *ne = (TGNumberEntry *) gTQSender;
	Int_t id = ne->WidgetId();	
		
	printf("handle in MainFrame %i  id = %i\n", handle, id );
	
	N957ErrorCodes ret = N957Success;
			
	if (id == 30)
		Rcfg.port = (N957_BYTE)fNumericEntries[0]->GetNumber( );
	
	
	if (id == 31){
		Rcfg.thr = (N957_BYTE)fNumericEntries[1]->GetNumber( );
		
		if (Rcfg.fInit){
			fStartButton->SetState (kButtonDown);
			ret = N957_SetLLD(handle, Rcfg.thr);
			printf("Setting threshold[%i] take some time \n", Rcfg.thr );
			fStartButton->SetState (kButtonUp);
		
	
			if (ret) {
				sprintf(CName, "SET_LLD_THRESHOLD[%i]_FAILURE \n handle handle = %i\n  ret = %i \n", Rcfg.thr, handle, ret);
				new TGMsgBox(gClient->GetRoot(), fMain, "Error", CName, kMBIconStop, kMBOk);
			}	
		}
					
	}	
	
	
	Rcfg.DrawTime = fNumericEntries[2]->GetNumber( );
	Rcfg.timer_val = fNumericEntries[3]->GetNumber( );
	
	
	
}


void MainFrame::InitButton()
{
	N957ErrorCodes ret = N957Success;
	
	fNumericEntries[0]->SetState(kFALSE);
	Rcfg.ampl = new TH1D("h_ampl", "h_ampl", 8192, 0, 8192);	
	
	ret = InitialiseDigitizer(handle, &Rcfg);
	
	if (ret) {
			sprintf(CName, "INITIALISE_DIGITIZER_FAILURE \n ret = %i \n", ret);
			new TGMsgBox(gClient->GetRoot(), fMain, "Error", CName, kMBIconStop, kMBOk);
		}	
	
	printf("ampl hist bins %i \n",Rcfg.ampl->GetNbinsX( ) );
	
	//enable buttons after DIGI initialisation
	fClearButton->SetState (kButtonUp);
	fStartButton->SetState (kButtonUp);
	fStopButton->SetState (kButtonUp);
	
	
	fInitButton->SetState (kButtonDisabled);
}

void MainFrame::ClearHisto()
{
	Rcfg.ampl->Reset("ICESM");

		
	printf("ClearHisto \n");
	
	Rcfg.StartTime = get_time( );
	
}

void MainFrame::StartButton()
{	
	
 	bool fStart = true;
	Rcfg.StartTime = get_time();
	printf("Start button \n");

	
	//Store traces if choosen
	
	
	Rcfg.loop = 1;
		
	//ret = N957_StartAcquire(handle);
}

void MainFrame::StopButton()
{	
	printf("Stop button \n");
	
	
	Rcfg.loop = 0;
	//ret = N957_StopAcquire(handle);
	
	
}




void MainFrame::HandleMenu(Int_t id)
{
   // Handle menu items.
	const char *filetypes[] = { "All files",     "*",
                            "ROOT files",    "*.root",
                            "ROOT macros",   "*.C",
                            "Text files",    "*.[tT][xX][tT]",
                            0,               0 };	
	switch (id) {
		
		case M_FILE_OPEN:
        	{
          	static TString dir(".");
          	TGFileInfo fi;
          	fi.fFileTypes = filetypes;
          	fi.fIniDir    = StrDup(dir);
          	//printf("fIniDir = %s\n", fi.fIniDir);
          	new TGFileDialog(gClient->GetRoot(), fMain, kFDOpen, &fi);
          	printf("Open file: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
         	dir = fi.fIniDir;
        	}
        	break;
			
      	case M_FILE_SAVE_AMPL_TXT:
        	{
           	static TString dir(".");
        	TGFileInfo fi;
            fi.fFileTypes = filetypes;
           	fi.fIniDir    = StrDup(dir);
            //printf("fIniDir = %s\n", fi.fIniDir);
            new TGFileDialog(gClient->GetRoot(), fMain, kFDSave, &fi);
			
			FILE *outtxt = fopen(fi.fFilename,"write");	
			for (int i = 0; i<Rcfg.ampl->GetNbinsX( ); i++) // Histo.ampl_set.xbins
				fprintf(outtxt, "%i %6.0f \n", i, Rcfg.ampl->GetBinContent(i) );
				
            printf("File saved as: %s (dir: %s)\n", fi.fFilename, fi.fIniDir);
            }
         	break;

   		case M_FILE_SAVE_HISTO:
		{			 
		 	static TString dir(".");
            TGFileInfo fi;
            fi.fFileTypes = filetypes;
            fi.fIniDir    = StrDup(dir);
            new TGFileDialog(gClient->GetRoot(), fMain, kFDSave, &fi);
                      
		 	TFile *outfile = new TFile(fi.fFilename,"RECREATE");
			Rcfg.can->Write("can");
			Rcfg.ampl->Write(Rcfg.ampl->GetTitle());
									
			outfile->Write(); 
         	printf("File saved - %s \n",fi.fFilename);			 
		 }
         break;
	
   	case M_FILE_EXIT:
        CloseWindow();   
        break;
		
	case M_MANUAL:
		new TGMsgBox(gClient->GetRoot(), fMain, "Manual", "After a while it will be added \n but noone knows value of while \n because it's a loop", kMBIconAsterisk, kMBOk);
        break;
	
	case M_HELP_ABOUT:
		new TGMsgBox(gClient->GetRoot(), fMain, "About program", "Handmade spectra and waveform reader \n for CAEN DT5720", kMBIconAsterisk, kMBOk);
        break;
 
      default:
         printf("Menu item %d selected\n", id);
         break;
   }
}


