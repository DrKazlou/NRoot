#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <sys/types.h>
#include <unistd.h>

#include "NFunc.h"

#include <TSystem.h>
#include "TH1D.h"
#include "TROOT.h"
#include "TGMsgBox.h"

#include "N957Lib.h"
#include "N957oslib.h"
#include "N957types.h"

extern ReadoutConfig_t Rcfg;

long get_time()
{
    long time_ms;
    struct timeval t1;
    struct timezone tz;
    gettimeofday(&t1, &tz);
    time_ms = (t1.tv_sec) * 1000 + t1.tv_usec / 1000;

    return time_ms;
}

void DrawHisto(TCanvas *can){
	
	Rcfg.ampl->GetYaxis()->SetTitleOffset(1.1);
	Rcfg.ampl->GetYaxis()->SetTitle("Counts, N");
	
	Rcfg.ampl->GetXaxis()->SetTitle(" Channels, N");
	Rcfg.ampl->SetLineColor(kBlue);	
	
	Rcfg.ampl->Draw("HIST");
	
	
	can->Update();
}

void CalcRate(uint64_t &PrevRateTime){
	char CName[100];
	uint64_t CurrentTime, ElapsedTime, DAQTime;
	
	CurrentTime = get_time( );
    ElapsedTime = CurrentTime - PrevRateTime;
	DAQTime = (CurrentTime - Rcfg.StartTime) / 1000;
			
		if (Rcfg.fTimer && DAQTime > Rcfg.timer_val)
			Rcfg.loop = 0;
					        	
		if (ElapsedTime > 1000) { 
			sprintf(CName,"T: %li s",  DAQTime );
			Rcfg.TLabel->SetText(CName);
			gSystem->ProcessEvents(); 
				
           	if (Rcfg.TrgCnt != 0){
				sprintf(CName," TrgCnt. %.2f Hz ", (float)Rcfg.TrgCnt*1000.f/(float)ElapsedTime );
				Rcfg.StatusBar->SetText(CName, 0);
			}
			else{
				sprintf(CName, "No data...");
				Rcfg.StatusBar->SetText(CName, 0);
			}
			
			Rcfg.TrgCnt = 0;
	                       		
        PrevRateTime = CurrentTime;
	   	}
	
}

N957ErrorCodes InitialiseDigitizer(int32_t &handle, ReadoutConfig_t *Rcfg) {
	
	char CName[200];
	N957_UINT32 data32;
	N957ErrorCodes ret = N957Success;
	
	//Rcfg.ampl = new TH1D("h_ampl", "h_ampl", 8192, 0, 8192);
	
	 ret = N957_Init( Rcfg->port, &handle);
			
    printf("Init: ret %d  handle %d \n", ret, handle);
  		
	char fw_rev[10];
	
	ret = N957_GetFWRelease(handle, fw_rev, 10);
	
	printf("Board FW %s ret %d  handle %d \n", fw_rev, ret, handle);
	sprintf(CName, "Board FW %s \n ret %d  handle %d \n", fw_rev, ret, handle);
	
	new TGMsgBox(gClient->GetRoot(), Rcfg->main, "Info", CName, kMBIconAsterisk, kMBOk);
	
	ret = N957_SetLLD( handle, Rcfg->thr); // low level detect threshold
	
	printf("SetLLD[%i] ret %d  handle %d \n", Rcfg->thr, ret, handle);
		
	ret = N957_GetScaler( handle, &data32);
	printf("Scaler %i \n", data32);
	
	ret = N957_GetTimer( handle, &data32);
	printf("Timer %i \n", data32);
	
	ret = N957_GetLiveTime( handle, &data32);
	printf("LiveTime %i \n", data32);
	
		
	Rcfg->fInit = 	true;

	return ret;
}



N957ErrorCodes ReadoutLoop(int handle){
	char CName[100];
	N957ErrorCodes ret = N957Success;
		
	uint64_t PrevRateTime = get_time( ), PrevDrawTime;
	
	N957_UINT16 data_read, BLDIM = 32768; // BLDIM 65536
		
	N957_UINT16 *data_buff = NULL;
		
	data_buff = (N957_UINT16*)malloc( BLDIM * sizeof( N957_UINT16) );
	Rcfg.Nev = 0;
	Rcfg.TrgCnt = 0;
		
	
	while(Rcfg.loop == 1 && !ret) {
	
		data_read = BLDIM;
		ret = N957_ReadData(handle, data_buff, &data_read); // Read the buffer from the digitizer 
		if (ret) {
			printf("ERR_READ_DATA \n");
			sprintf(CName, "ERR_READ_DATA");
		}
		     		
        //Rcfg.Nev += data_read;
		
		// Calculate throughput and trigger rate (every second) 		
		CalcRate(PrevRateTime);
							
			
		for (int i=0; i<data_read; i++) {
			Rcfg.ampl->Fill(data_buff[i]);
			Rcfg.TrgCnt++;
			if (data_read <5){
				sprintf(CName, "Evt[%i]: %i", i, data_buff[i]);
				Rcfg.StatusBar->SetText(CName, i+1);
			}				
			
		}	
			
			
		
		gSystem->ProcessEvents(); 
		
		if ( (get_time( ) - PrevDrawTime) >= Rcfg.DrawTime*1000 && data_read!=0){ // sec*1000 = ms 
			DrawHisto(Rcfg.can);
			PrevDrawTime = get_time( );
		}
			
			

		
    } // end of while readout loop		
				
	if(ret){
		printf("Error: %s\n", CName);
		new TGMsgBox(gClient->GetRoot(), Rcfg.main, "Error", CName, kMBIconStop, kMBOk);
		return ret;
	}
	
	return ret;
}



N957ErrorCodes DataAcquisition(int32_t handle){
	N957ErrorCodes ret = N957Success;
	char CName[30];
	
	while(1) {
		gSystem->ProcessEvents(); 
		if (Rcfg.loop == 1){
			ret = N957_StartAcquire(handle, N957ControlModeAuto); //N957ControlModes
			
			if (ret) {
				sprintf(CName, "ERR_START_ACQUISITION %i \n", ret);
				new TGMsgBox(gClient->GetRoot(), Rcfg.main, "Error", CName, kMBIconStop, kMBOk);
				//ret = QuitMain(handle, buffer);
				return ret;
			}
			ret = ReadoutLoop(handle);
		}
		
		if (Rcfg.loop == 0){
			ret = N957_StopAcquire(handle);
			if (ret) {
				sprintf(CName, "ERR_STOP_ACQUISITION %i \n", ret);
				new TGMsgBox(gClient->GetRoot(), Rcfg.main, "Error", CName, kMBIconStop, kMBOk);
				//ret = QuitMain(handle, buffer);
				return ret;
			}
			
			printf("Acquisition stopped \n");
			Rcfg.loop = -1;
		}
	}
	return ret;
}