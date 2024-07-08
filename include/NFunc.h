#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "DTconfig.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TGLabel.h"
#include "TGStatusBar.h"
#include "TGWindow.h"
#include "TGFrame.h"

#include "N957types.h"
#include "N957Lib.h"
#include "N957oslib.h"



typedef struct
{	
	bool fInit;
	bool fTimer;
	int timer_val;

	short int port;
		
	int loop;
	TH1D *ampl;
	
	int Nev, TrgCnt;

	uint64_t StartTime;
	double DrawTime;
	
	TGLabel  *TLabel;
	TGStatusBar *StatusBar;
	TCanvas *can;

	TGMainFrame *main;
	
	N957_BYTE thr = 30; // low level detect threshold
	N957_UINT16 BLDIM = 65535;  // [1..65536] 32768

	
} ReadoutConfig_t;

typedef struct{
	
	int xbins, xmin, xmax;
	int ybins, ymin, ymax;
	
}hist_settings_t;


long get_time();

void CalcRate(uint64_t &PrevRateTime);

void DrawHisto(TCanvas *can);

N957ErrorCodes InitialiseDigitizer(int32_t &handle, ReadoutConfig_t *Rcfg);

N957ErrorCodes ReadoutLoop(int32_t handle);

N957ErrorCodes DataAcquisition(int32_t handle);
