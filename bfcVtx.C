
//_____________________________________________________________________________
void bfcVtx(const string daqfile, int nEvents = 5) {

  //fzd file from starsim, coherent J/psi by Starlight, 1000 events
  string fzdfile = "pythia6.starsim.fzd";

  //chain options for daq file
  string chain1Opt = "in,magF,tpcDb,NoDefault,TpxRaw,-ittf,NoOutput";
                      // in,magF,tpcDb,NoDefault,TpxRaw,-ittf,NoOutput

  //options for fzd chain
  string chain2Opt = "gen_T,geomT,sim_T,TpcRS,-ittf,-tpc_daq,nodefault,fzin,ry2010c";
                  // "gen_T,geomT,sim_T,TpcRS,-ittf,-tpc_daq,nodefault,fzin,NoOutput"; official embedding w/o geom tag

  //options for mixer chain, from previous starsim with bfc chain, provided by Lidya
  string chain3Opt;

  chain3Opt = "DbV20101213 P2010a -VFMinuit VFPPVnoCTB beamline pmdReco  BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D IdTruth McEvent McAss,TpcMixer,GeantOut,MiniMcMk,McAna,-in,NoInput,useInTracker,nodefault";
  // This is the official production chain + the embedding options sep by comman at the end (from the official embedding macro)  


  //load the bfc
  gROOT->LoadMacro("bfc.C");
  if (gClassTable->GetID("StBFChain") < 0) Load();

  //main chain
  StChain *Chain = new StChain("Embedding");

  //chain for daq file
  bfc(-1, chain1Opt.c_str(), daqfile.c_str());
  StBFChain *chain1 = chain;
  chain1->SetName("One"); 
  Chain->cd();

  //chain for fzd file
  bfc(-1, chain2Opt.c_str(), fzdfile.c_str());
  StBFChain *chain2 = chain;
  chain2->SetName("Two"); 
  Chain->cd();

  //mixer chain
  bfc(-1, chain3Opt.c_str(), 0, "output.root");
  StBFChain *chain3 = chain;
  chain3->SetName("Three"); 
  Chain->cd();
  StTpcMixerMaker  *mixer = (StTpcMixerMaker *) chain3->Maker("TpcMixer");
  mixer->SetInput("Input1","TpxRaw/.data/Event");

  mixer->SetInput("Input2","TpcRS/Event");
  Chain->cd();

  Int_t iInit = Chain->Init();
  if (iInit >=  kStEOF) {Chain->FatalErr(iInit,"on init"); return;}

  StMaker *treeMk = Chain->GetMaker("outputStream");
  Chain->EventLoop(nEvents, treeMk);
  gMessMgr->QAInfo() << "Run completed " << endm;


}//bfc_daq_fzd

