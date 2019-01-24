
//_____________________________________________________________________________
void bfc_daq_fzd(const string daqfile) {

  //fzd file from starsim, coherent J/psi by Starlight, 1000 events
  string fzdfile = "output.fzd";

  //chain options for daq file
  string chain1Opt = "in,magF,tpcDb,NoDefault,TpxRaw,-ittf,NoOutput";
                      // in,magF,tpcDb,NoDefault,TpxRaw,-ittf,NoOutput

  //options for fzd chain
  string chain2Opt = "gen_T,geomT,sim_T,TpcRS,-ittf,-tpc_daq,nodefault,fzin,ry2010c";
  // "gen_T,geomT,sim_T,TpcRS,-ittf,-tpc_daq,nodefault,fzin,NoOutput"; old one

  //options for mixer chain, from previous starsim with bfc chain, provided by Lidya
  string chain3Opt;
  chain3Opt += "DbV20101213 ";
  // chain3Opt += "DbV20170712_trg_Calibrations ";
  chain3Opt += "VFPPVnoCTB beamline ";
  chain3Opt += "fzin ";
  chain3Opt += "y2010c ";
  // chain3Opt += "AgML ";
  // chain3Opt += "usexgeom ";
  chain3Opt += "FieldOn ";
  chain3Opt += "MakeEvent CMuDST ";
   chain3Opt += "Sti ";
  chain3Opt += "NoSsdIt ";
  chain3Opt += "NoSvtIt ";
  chain3Opt += "TpcHitMover ";
  chain3Opt += "TpxClu ";
  chain3Opt += "Idst ";
  chain3Opt += "BAna ";
  // chain3Opt += "l0 ";
  chain3Opt += "Tree ";
  chain3Opt += "logger ";
  chain3Opt += "genvtx VFMCE ";
  chain3Opt += "tpcDB ";
  //chain3Opt += "bbcSim ";
  //chain3Opt += "btofsim ";
  //chain3Opt += "btofMatch ";
  chain3Opt += "eemcDB ";
  // chain3Opt += "tags ";
  // chain3Opt += "emcY2 ";
  // chain3Opt += "EEfs ";
  // chain3Opt += "geantout ";
  // chain3Opt += "evout ";
  chain3Opt += "-dstout ";
  chain3Opt += "IdTruth ";
  // chain3Opt += "big ";
  chain3Opt += "MiniMcMk ";
  chain3Opt += "clearmem ";
  chain3Opt += "TpcMixer in NoInput useInTracker";
  
  // TString prodP10ikAuAu200("DbV20101213 P2010a pmdReco btof BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D VFMCE TpxClu -VFMinuit -hitfilt");
  // prodP10ikAuAu200 += " VFPPVnoCTB beamLine TpxClu -VFMCE -VFMinuit -hitfilt"

  // chain3Opt = string(prodP10ikAuAu200.Data());

  chain3Opt = "DbV20101213 TpcRS TpxClu y2010a MakeEvent Sti VFMCE tpcDB TpcHitMover tags evout IdTruth TpcMixer NoInput useInTracker";
  chain3Opt = "DbV20101213 P2010a pmdReco BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D VFMCE TpxClu -VFMinuit -hitfilt,TpcMixer,GeantOut,MiniMcMk,McAna,-in,NoInput,useInTracker"; 
  chain3Opt = "DbV20101213 P2010a -VFMinuit VFPPVnoCTB beamline pmdReco  BEmcChkStat Corr4 OSpaceZ2 OGridLeak3D IdTruth McEvent McAss,TpcMixer,GeantOut,MiniMcMk,McAna,-in,NoInput,useInTracker,nodefault";

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


  //gSystem->Load("StEmcSimulatorMaker");
  //gSystem->Load("StEmcMixerMaker");
  //gSystem->Load("StEEmcSimulatorMaker");

  //StMcEventMaker* mcEventMaker = new StMcEventMaker();
  //chain3->AddAfter("emcRaw",mcEventMaker);


  Int_t iInit = Chain->Init();
  if (iInit >=  kStEOF) {Chain->FatalErr(iInit,"on init"); return;}

  StMaker *treeMk = Chain->GetMaker("outputStream");
  Chain->EventLoop(2, treeMk);
  gMessMgr->QAInfo() << "Run completed " << endm;















}//bfc_daq_fzd

