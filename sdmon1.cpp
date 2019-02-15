#include <iostream>
#include <dic.hxx>
#include <unistd.h>
#include <vector>
#include <math.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <iomanip>

using namespace std;

const int PRINTOUTPERIOD=100;
const int IAMP_DEADBAND=10;

std::vector<std::pair<int,int> > iamp;
std::vector<int> ireel;

typedef struct sddim_s {
  int ii[4];
  int tindex;
  int ireel;
  int iamp;
  int ivolt;
  int ii2[2];
  char controller[64];
  char tubeinfo[64];
  char lastcommand[64];
  char str4[128];
};

sddim_s *sddim;
char blankline[]="                                                             ";

std::string decodeState(int dstate){
  if(dstate < 0){ return(" undefined");}
  int s = dstate & 0x07;
  std::string sstate;
  if(0 == s){ sstate = "IDLE";}
  else if(1==s){ sstate = "EXTENDING";}
  else if(2==s){ sstate = "RETRACTING";}
  else if(3==s){ sstate = "SPOOLING";}
  else if(4==s){ sstate = "INDEX UP";}
  else if(5==s){ sstate = "INDEX DN";}
  else if(6==s){ sstate = "INITING";}
  else if(7==s){ sstate = "UNINITIALIZED";}
  return sstate;
}


class ErrorHandler : public DimErrorHandler
{
	void errorHandler(int severity, int code, char *msg)
	{
		int index = 0;
		char **services;
		if(code){}
		cout << severity << " " << msg << endl;
	}
public:
	ErrorHandler() {DimClient::addErrorHandler(this);}
};


class StrService : public DimInfo
{

	void infoHandler()
	{
		int index = 0;
		char **services;
//		cout << "Dns Node = " << DimClient::getDnsNode() << endl;
		cout << "Received STRVAL : " << getString() << endl;
		services = DimClient::getServerServices();
		cout<< "from "<< DimClient::getServerName() << " services:" << endl;
		while(services[index])
		{
			cout << services[index] << endl;
			index++;
		}
	}
public :
	StrService() : DimInfo("TEST/STRVAL","not available") {};
};



class SDDimService : public DimInfo {

  void infoHandler() {
    sddim = (sddim_s *)getData();

    if (sddim->tindex != prev.tindex) {
      //     cout << endl << "at index " << sddim->tindex << endl;
      ireel.clear();
      iamp.clear();
      copy_sddim(&prev);
      lastPrintedIreel=-1;
      print_title(sddim->tindex);
    }

      print_sddim(0);
    if (sddim->ireel > prev.ireel) {
      ireel.push_back(sddim->ireel);
      iamp.push_back(std::make_pair(sddim->ireel,sddim->iamp));
      copy_sddim(&prev);
      if (sddim->ireel > lastPrintedIreel+PRINTOUTPERIOD || abs(sddim->iamp-prev.iamp) > IAMP_DEADBAND ) {
	lastPrintedIreel=sddim->ireel;
	print_pairs();
      }
      startRetracting=false;
    } else {
      if (!startRetracting) {
	print_pairs();
	print_sddim(1);
      }
      startRetracting=true;
    }
    
  }
    
private:
  FILE *pipe;
public :
  static volatile sddim_s prev;  
  SDDimService(char *servicename) : DimInfo(servicename,-1) {prev.ireel=999;getServiceInfo(); lastPrintedIreel=-999; startRetracting=false; };
  //  SDDimService() : DimInfo("",-1) {prev.ireel=999;getServiceInfo();  };
  void copy_sddim(  volatile sddim_s *dest);
  void print_sddim(int newline=0);
  void print_pairs(void);
  void print_title( int index);
  void getServiceInfo();
  int lastPrintedIreel;
  void setPipe(FILE *p) { pipe=p;};
  bool startRetracting;
};

void SDDimService::getServiceInfo() {
    char **services;
    int i=0;
    cout << "Dns Node = " << DimClient::getDnsNode() << endl;
    cout << "Dns Port = " << DimClient::getDnsPort() << endl;
    services = DimClient::getServerServices();
    cout<< "from "<< DimClient::getServerName() << " services:" << endl;
    while(services[i]) {
	cout << services[i] << endl;
	i++;
      }
}

void SDDimService::print_title( int index) {
  string st(sddim->tubeinfo);
  std::replace( st.begin(), st.end(), '_', ' '); 
  fprintf(pipe,"set title \"%s index:%3d \"\n", st.c_str(), index );
  fflush(pipe);
  cout << "\n index "<< index << " new tube " << st << "\n";

  int startMGT=strtol(st.substr(st.find("=")+1,5).c_str(),NULL,10);
  int endMGT=strtol(st.substr(st.find_last_of("=")+1).c_str(),NULL,10);

  fprintf(pipe,"$startMGT << EOD \n %d 0 \n %d 100 \nEOD\n",startMGT,startMGT);
  fprintf(pipe,"$endMGT << EOD \n %d 0 \n %d 100 \nEOD\n",endMGT,endMGT);
}


void SDDimService::print_pairs(void) {

  fprintf(pipe,"$data << EOD \n");

  for(std::vector<std::pair<int,int> >::iterator it = iamp.begin(); it != iamp.end(); ++it) {
    //      std::cout << it->first << "\t" << it->second << endl;
    fprintf(pipe,"%6d %6d \n", it->first,it->second);
  }
  fprintf(pipe,"EOD \n");
  fprintf(pipe,"plot $startMGT with lines lt 3, $endMGT with lines lt 3, $data with lines lc 1\n");
  fflush(pipe);
}


void SDDimService::copy_sddim( volatile sddim_s *dest) {
  memcpy( (void *) dest, sddim, sizeof (sddim_s));	
}

void  SDDimService::print_sddim( int newline) {
  //  cout << "cont: " << sddim->controller << "\t";
  //  cout << "tube: " << sddim->tubeinfo << "\t";
  //  cout << "cmd: " << sddim->lastcommand << endl;
  if (newline) {
    // cout << "ind: " << sddim->tindex<< "\t";
    // cout << "ireel: " << sddim->ireel<< "\t";
    // cout << "ivolt: " << sddim->ivolt << "\t";
    // cout << "iamp: " << sddim->iamp;
    cout << endl;
  }
  else {
    //    cout << blankline << '\r';
    cout << '\r';
    cout  <<"ind: " << setw(4) << sddim->tindex<< "\t";
    cout << "ireel: " << setw(6) << sddim->ireel<< "\t";
    cout << "ivolt: " << setw(4) << sddim->ivolt << "\t";
    cout << "iamp: " << setw(5) << sddim->iamp << "\t";
    cout  << setw(10) << decodeState(sddim->ii[3]) ;
  }
}


volatile sddim_s SDDimService::prev={0,0,0,-9,-999,-1,-1,0,0,0,"","","",""};
const string st_STATUS(".STATUS");
const string st_PREFIX("HCAL.SD");

int main(int argc, char ** argv) {
  DimClient::setDnsNode("hcalmon");
  //  DimClient::setDnsPort("2505");
  
  DimBrowser br;
  int type, n, pid;
  char *server, *ptr, *ptr1;
  fputs("\e[?25l", stdout);

  if (argc == 1) {
	
  n = br.getServices("*");
  cout << "found totally " << n << " services, now searching for SDMaganer services " << endl;
  
  while((type = br.getNextService(ptr, ptr1))!= 0) {
    string servicename(ptr);
    
    if (type == 1 && servicename.find(st_STATUS)!=std::string::npos && servicename.find(st_PREFIX)!=std::string::npos  )
      cout <<  ptr <<  endl;
  }
  cout << " Run " << argv[0] << " " << " service " << endl;
  return 0;
  } 
  
  // this is for simulated data: 
  //  std::vector<std::pair<double, double> > xy;
  //  std::vector<std::pair<double, double> > xy_ref;
  //  FILE *pipe = popen("/nfshome0/kaminsky/gnuplot/bin/gnuplot -persist", "w");
  FILE *pipe = popen("/nfshome0/kaminsky/gnuplot/bin/gnuplot ", "w");
  fprintf(pipe,"set term X11 size 1200,400\n");
  fprintf(pipe,"set xrange [0:12000]\n");
  fprintf(pipe,"set yrange [0:100]\n");

  // sddim_s *sddim;
	
      ErrorHandler errHandler;
      //   DimClient::addErrorHandler(errHandler);
      SDDimService sds(argv[1]);
      sds.setPipe(pipe);
      

      //  DimInfo servint("SDmanager/HCAL.SD.SD_HEP_14_15.STATUS",-1); 
  //  DimInfo servint("DIS_DNS",-1); 
	
	while(1)
	  pause();

	// {
	// 	usleep(10000);
	// 	sddim = (sddim_s*)servint.getData();
	// 	cout << "vals: " << sddim->str1 << endl;
	// 	//		DimClient::sendCommand("TEST/CMND","UPDATE_STRVAL");
	// }

  fclose(pipe);



}
