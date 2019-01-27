// macro to instantiate the Geant3 from within
// STAR  C++  framework and get the starsim prompt
// To use it do
//  root4star starsim.C

class St_geant_Maker;
St_geant_Maker *geant_maker = 0;

class StarGenEvent;
StarGenEvent   *event       = 0;

class StarPrimaryMaker;
StarPrimaryMaker *primary = 0;

class StarKinematics;
StarKinematics *kinematics = 0;

TF1 *ptDist  = 0;
TF1 *etaDist = 0;

TGenPhaseSpace eventPS;
TRandom3 r;

TF1 * fMass = 0;
TF1 * fPt2 = 0;
Double_t masses[2] = { 0.0005109989461, 0.0005109989461} ;


// ----------------------------------------------------------------------------
void geometry( TString tag, Bool_t agml=true )
{
  TString cmd = "DETP GEOM "; cmd += tag;
  if ( !geant_maker ) geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker -> LoadGeometry(cmd);
  //  if ( agml ) command("gexec $STAR_LIB/libxgeometry.so");
}
// ----------------------------------------------------------------------------
void command( TString cmd )
{
  if ( !geant_maker ) geant_maker = (St_geant_Maker *)chain->GetMaker("geant");
  geant_maker -> Do( cmd );
}
// ----------------------------------------------------------------------------
void trig( Int_t n=1 )
{
	TLorentzVector W;
	TLorentzVector l1, l2;

	for ( int i = 0; i < n; i++ ){
		// Clear the chain from the previous event
		chain->Clear();

		W.SetPtEtaPhiM( sqrt( fPt2->GetRandom() ) , r.Uniform(-1, 1), r.Uniform(-TMath::Pi(), TMath::Pi()), fMass->GetRandom() );

		eventPS.SetDecay(W, 2, masses);

        Double_t weight = eventPS.Generate();
      
        TLorentzVector l1    = *(eventPS.GetDecay(0));
        TLorentzVector l2    = *(eventPS.GetDecay(1));
		
		StarGenParticle *ele = kinematics->AddParticle( "e-" );
		
		ele->SetPx( l1.Px() );
		ele->SetPy( l1.Py() );
		ele->SetPz( l1.Pz() );
		ele->SetMass( 0.0005109989461 );

		StarGenParticle *pos = kinematics->AddParticle( "e+" );
		
		pos->SetPx( l2.Px() );
        pos->SetPy( l2.Py() );
        pos->SetPz( l2.Pz() );
        pos->SetMass( 0.0005109989461 );


		//kinematics->Kine( 1, "e-", 0.100, 1.0, -1.0, +1.0 );
		//kinematics->Kine( 1, "e+", 0.100, 1.0, -1.0, +1.0 );


		// Generate the event
		chain->Make();

		// Print the event
		//primary->event()->Print();
	}
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void Kinematics()
{
  
  //  gSystem->Load( "libStarGeneratorPoolPythia6_4_23.so" );
  gSystem->Load( "libKinematics.so");
  kinematics = new StarKinematics();
    
  primary->AddGenerator(kinematics);
}
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
void starsim_gammagamma( Int_t nevents=1 )
{ 

  gROOT->ProcessLine(".L bfc.C");
  {
    TString simple = "y2010 geant gstar";
    bfc(0, simple );
  }

  gSystem->Load( "libVMC.so");

  gSystem->Load( "StarGeneratorUtil.so" );
  gSystem->Load( "StarGeneratorEvent.so" );
  gSystem->Load( "StarGeneratorBase.so" );

  gSystem->Load( "libMathMore.so"   );  
  gSystem->Load( "xgeometry.so"     );
  
  //
  // Create the primary event generator and insert it
  // before the geant maker
  //
  //  StarPrimaryMaker *
  primary = new StarPrimaryMaker();
  {
    primary -> SetFileName( "kinematics.starsim.root");
    chain -> AddBefore( "geant", primary );
  }

  Kinematics();
  
  primary->SetVertex( 0.0, 0.0, 0.0 );
  primary->SetSigma( 0.01, 0.01, 35.0 );  

  //
  // Initialize primary event generator and all sub makers
  //
  primary -> Init();

  //
  // Setup geometry and set starsim to use agusread for input
  //
  geometry("y2010");
  command("gkine -4 0");
  command("gfile o pythia6.starsim.fzd");
  

  //
  // Setup PT and ETA distributions
  //

  Double_t pt0 = 3.0;
  ptDist = new TF1("ptDist","(x/[0])/(1+(x/[0])^2)^6",0.0,10.0);
  ptDist->SetParameter(0, pt0);
  ptDist->Draw();

  etaDist = new TF1("etaDist","-TMath::Erf(x+2.6)*TMath::Erf(x-2.6)",-0.8,+0.8);

   fMass = new TF1( "fmass", "pow(x, -3.5)" );
   fMass->SetRange( 0.4, 3.0 );

   fPt2 = new TF1("fpt2", "[0] * exp(  [1]*x )");
   fPt2->SetParameter( 0, 1412.5 );
   fPt2->SetParameter( 1, -689.31566 );
   fPt2->SetRange( 0, 0.01 );
	cout << "hello" << endl;
  //
  // Trigger on nevents
  //
  trig( nevents );

}
// ----------------------------------------------------------------------------

