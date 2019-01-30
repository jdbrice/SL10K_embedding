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

TRandom3 r;
TLorentzVector *lv1 ,*lv2;
int nPairs = 0;

// class StBFChain;
// StBFChain * chain = 0;



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

		int iEntry = r.Integer( nPairs );
		l1 = lv1[iEntry];
		l2 = lv2[iEntry];

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

		TLorentzVector W = l1 + l2;
		cout << "mass = " << W.M() << ", pT = " << W.Pt() << endl;

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
void starsim_gammagamma( Int_t nevents=1, Int_t rngSeed = 234234 )
{ 

	// Read in the STARLight events
	TFile *f = new TFile( "starlight.root" );
	TTree *t = (TTree*)f->Get("PairDst");
	// do this up here to make sure it doesnt mess up the active file below

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
 
	// Setup RNG seed and map all ROOT TRandom here
	r.SetSeed(rngSeed);
	gRandom->SetSeed(rngSeed);
	StarRandom::seed( rngSeed );
	StarRandom::capture();

 
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
	
	

	// copy out the tree info
	t->SetEstimate( t->GetEntries() + 1 );
	t->Draw( "d1_mPt : d1_mEta : d1_mPhi", "", "goff", t->GetEntries(), 0 );
	TLorentzVector l1, l2;

	lv1 = new TLorentzVector[500000];
	lv2 = new TLorentzVector[500000];

	for ( int i = 0; i < t->GetSelectedRows(); i++ ) {
		l1.SetPtEtaPhiM( t->GetVal(0)[i], t->GetVal(1)[i], t->GetVal(2)[i], 0.0005109989461 );
		lv1[i] = l1;
	}
	nPairs = t->GetSelectedRows();


	t->Draw( "d2_mPt : d2_mEta : d2_mPhi", "", "goff", t->GetEntries(), 0 );
	for ( int i = 0; i < t->GetSelectedRows(); i++ ) {
		l2.SetPtEtaPhiM( t->GetVal(0)[i], t->GetVal(1)[i], t->GetVal(2)[i], 0.0005109989461 );
		lv2[i] = l2;
	}

	//
	// Trigger on nevents
	//
	trig( nevents );

}
// ----------------------------------------------------------------------------

