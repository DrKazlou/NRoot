

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "N957Lib.h"
#include "N957oslib.h"
#include "N957types.h"

#include "NFrame.h"
#include "NFunc.h"

#include "NRoot.h"

#include "TROOT.h"
#include "TApplication.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TROOT.h"
#include "TStyle.h"



using namespace std;

	
	ReadoutConfig_t Rcfg;    	
	int32_t handle;



//---- Main program ------------------------------------------------------------

int main(int argc, char **argv)
{
	
   TApplication theApp("App", &argc, argv);

   if (gROOT->IsBatch()) {
      fprintf(stderr, "%s: cannot run in batch mode\n", argv[0]);
      return 1;
   }
   
	
	memset(&Rcfg, 0, sizeof(Rcfg));
		
		
	handle = 0; //N957_INVALID_HANDLE_VALUE
	
	Rcfg.fInit = false; // flag for initialisation
	Rcfg.fTimer = false; // flag for timer usage
	Rcfg.loop = -1; // ReadoutLoop flag
	Rcfg.DrawTime = 0.5; // time between histograms drawing in sec	
	Rcfg.port = 1;
	
	
	new MainFrame(gClient->GetRoot(), 1700, 850);
	
	N957ErrorCodes ret = N957Success;
	ret = DataAcquisition(handle);

	
	theApp.Run();

   return 0;
}
//#endif
