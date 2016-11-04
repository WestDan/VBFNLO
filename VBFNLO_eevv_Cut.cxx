#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <locale>
#include <map>
#include <utility>
#include <time.h>
#include "TFile.h"
#include "TH1F.h"
#include "TTree.h"
#include "TLorentzVector.h"
using namespace std;

static double Z_MASS = 91.1876;
time_t start, finish;

const vector<string> CUT = {"none", "zvv", "lepEta", "leadingLepPt", "subleadingLepPt", "missingET", "z", "numJet", "mjj", "deltaEtajj"};

typedef map<string, bool> MapType1_Bool;
typedef map<string, float> MapType1_Float;
typedef map<string, TH1F *> MapType1_TH1F;
typedef map<string, map<string, TH1F *> > MapType2_TH1F;

MapType2_TH1F HIST;
MapType1_Float VALUE;
MapType1_Bool FLAG;


/* This root script do the following:
 *
 * Read in root file (provided in function parameters) and tree
 * (default name " tree_NOMINAL" ) produced by xAOD codes.
 * Loop through all events in the tree
 *	do pairing for same flavour, opposited charged leptons
 *	extract valuable information
 *	do cut on them
 *  Output tree and root file ( name provided by parameters )
 *
 *
 *  Variables used:
 */

int find_space(string in, int pos=0)
{
    // find the first space after(include) pos within a string
    int size = in.length();
    if( (pos < 0) || (pos >= size) )
    {
	cerr << "invalid position parameter for find_space: " << pos << endl;
	return -1;
    }
    while(pos < size)
    {
	char c = in.at(pos);
	if ( isspace(c) )
	    return pos;
	pos++;
    }
    return -1;
}

int find_pre_space(string in, int pos=0)
{
    // find the first space previor(include) pos within a string
    int size = in.length();
    if( (pos < 0) || (pos >= size) )
    {
	cerr << "invalid position parameter for find_space: " << pos << endl;
	return -1;
    }
    while(pos >= 0)
    {
	char c = in.at(pos);
	if ( isspace(c) )
	    return pos;
	pos--;
    }
    return -1;
}

int find_non_space(string in, int pos=0)
{
    // find the first non-space after(include) pos within a string
    int size = in.length();
    if( (pos < 0) || (pos >= size) )
    {
	cerr << "invalid position parameter for find_space: " << pos << endl;
	return -1;
    }
    while(pos < size)
    {
	char c = in.at(pos);
	if ( ! isspace(c) )
	    return pos;
	pos++;
    }
    return -1;
}


void parse_string(string in, vector<string> &out, string de = ",")
{
    // remove space around de
    // space are: "\s", "\t", "\n"
    // remove space and 'de' at beginning, if they are
    char c = in.at(0);
    while( isspace(c) || ( c == de ) )
    {
	in.erase(0,1);
	c = in.at(0);
    }

    // find de and remove space before and after it.
    // which means space before and after word is not allowed.
    
    int pos = 0;
    while ( (pos = in.find(de, pos+1)) != string::npos )
    {
	int pos_1 = find_space(in, pos+1);
	// remove space after de, it relies on that the first char isn't de
	while( (pos_1 - pos) == 1 )
	{
	    in.erase(pos_1, 1);
	    pos_1 = find_space(in,pos);
	}
	// remove space before de
	pos_1 = find_pre_space(in, pos);
	while( (pos - pos_1) == 1 )
	{
	    in.erase(pos_1, 1);
	    pos--;
	    pos_1 = find_pre_space(in,pos);
	}
    }

    // but the space in end part of last item is kept.
    int pos_pre = 0;
    while ( (pos = in.find(de, pos_pre)) != string::npos )
    {
	if( pos == pos_pre ) // continuous de
	{
	    pos_pre = pos + 1;
	    continue;
	}
	out.push_back( in.substr( pos_pre, pos - pos_pre) );
	pos_pre = pos+1;
    }
    if( pos_pre < in.length())
    {
	if ( find_non_space(in, pos_pre) >= 0 )
	    out.push_back(in.substr(pos_pre));
    }
}

int InitHistVar(string vars, string cuts, int nbins, float xlow, float xhigh)
{
    vector<string> var_list;
    vector<string> cut_list;

    parse_string(vars, var_list, ",");
    // rewirte with RE
    if( cuts == "all" || cuts == "All" || cuts == "ALL" )
    	cut_list = CUT;    
    else
    	parse_string(cuts, cut_list, ",");  

    if( xlow > xhigh)
    {
	cerr << "Incorrect parameters for InitHistVar, xlow: " << xlow << " is larger than xhigh: " << xhigh << endl;
	return -1;
    }
    int nvar = var_list.size();
    int ncut = cut_list.size();
	for(int j=0; j<nvar; j++)
    {
	string var = var_list[j];
	for( int k=0; k<ncut; k++)
	{
	    string cut = cut_list[k];
	    string histname = cut + "_" + var;
	    HIST[cut][var] = new TH1F(histname.c_str(), histname.c_str(), nbins, xlow, xhigh );
	}
    }
    return 0;
}

void FillHistVar(float weight)
{
    MapType2_TH1F::iterator it_cut = HIST.begin();
    MapType2_TH1F::iterator it_cut_end = HIST.end();
    for ( ; it_cut != it_cut_end; it_cut++ )
    {
	string cut = it_cut->first;
	if(FLAG[cut])
	{
	    MapType1_TH1F::iterator it_var = HIST[cut].begin();
	    MapType1_TH1F::iterator it_var_end = HIST[cut].end();
	    for(; it_var != it_var_end; it_var++)
	    {
		string var = it_var->first;
		HIST[cut][var]->Fill(VALUE[var], weight);
	    }
	}
    }
}

void SaveHistVar()
{
    MapType2_TH1F::iterator it_cut = HIST.begin();
    MapType2_TH1F::iterator it_cut_end = HIST.end();
    for (; it_cut != it_cut_end; it_cut++)
    {
	string cut = it_cut->first;
	MapType1_TH1F::iterator it_var = HIST[cut].begin();
	MapType1_TH1F::iterator it_var_end = HIST[cut].end();
	for (; it_var != it_var_end; it_var++)
	{
	    string var = it_var->first;
	    HIST[cut][var]->Write();
	}
    }
}

int cut( string input_file, string output_file)
{
    time(&start);
    TFile *fin = new TFile(input_file.c_str(), "read");
    if(!fin) {
	cerr << "Error, input root file " << input_file << " doesn't exit" << endl;
	return 1;
    }

    TTree * tin = (TTree *) fin->Get("vbfnlo"); // default tree name;
    if (! tin)
    {
	cerr << "vbfnlo doesn't exit in input file" << endl;
	return 2;
    }

    TFile * fout = new TFile(output_file.c_str(), "update");
    if(! fout)
    {
	cerr << "Unable to create output file" << output_file << endl;
	return 3;
    }

// create histograms
    InitHistVar("l1_pt, l2_pt, l3_pt, l4_pt", "all", 35, 0, 350);
    InitHistVar("l1_eta, l2_eta, l3_eta, l4_eta", "all", 20, -3.5, 3.5);
    InitHistVar("l1_phi, l2_phi, l3_phi, l4_phi", "all", 20, -3.5,3.5);
    InitHistVar("l1_e, l2_e, l3_e, l4_e", "all", 50, 0, 500);
    InitHistVar("leadingj_pt, subleadingj_pt", "all", 30, 0, 300);
    InitHistVar("leadingj_eta, subleadingj_eta", "all", 20, -5.0, 5.0);
    InitHistVar("leadingj_phi, subleadingj_phi", "all", 20, -3.5, 3.5);
    InitHistVar("z1_pt, z2_pt, zz_pt", "all", 50, 0, 500);
    InitHistVar("z1_eta, z2_eta, zz_eta", "all", 20, -5.0, 5.0);
    InitHistVar("z1_phi, z2_phi, zz_phi", "all", 20, -3.5, 3.5);
    InitHistVar("z1_m, z2_m", "all", 50, 0, 200);
    InitHistVar("zz_m", "all", 50, 0, 500);
    InitHistVar("jj_pt", "all", 50, 0, 500);
    InitHistVar("jj_eta", "all", 28, -7.0, 7.0);
    InitHistVar("jj_phi", "all", 20, -3.5, 3.5);
    InitHistVar("jj_m", "all", 50, 0, 2000);
    InitHistVar("delta_R_ll_1, delta_R_ll_2, delta_R_zz, delta_R_jj, delta_R_zz_jj", "all", 20, 0, 10);
    InitHistVar("mcweight", "all", 10, 0, 5);
    InitHistVar("delta_eta_jj", "all", 40, 0, 10);
    InitHistVar("centrality", "all", 20, -1, 1);

    int m_none   = 0;
    int m_zvv    = 0;
    int m_lepEta = 0;
    int m_leadingLepPt = 0;
    int m_subleadingLepPt = 0;
    int m_missingET = 0;
    int m_z	 = 0;
    int m_numJet = 0;
    int m_mjj    = 0;
    int m_deltaEtajj = 0;


    double weight;
    float l1_pt, l1_eta, l1_phi, l1_e;
    float l2_pt, l2_eta, l2_phi, l2_e;
    float l3_pt, l3_eta, l3_phi, l3_e;
    float l4_pt, l4_eta, l4_phi, l4_e;
    float leadingj_pt, leadingj_eta, leadingj_phi, leadingj_e;
    float subleadingj_pt, subleadingj_eta, subleadingj_phi, subleadingj_e;
    float z1_pt, z1_eta, z1_phi, z1_e, z1_m;
    float z2_pt, z2_eta, z2_phi, z2_e, z2_m;
    float zz_pt, zz_eta, zz_phi, zz_e, zz_m;
    float jj_pt, jj_eta, jj_phi, jj_e, jj_m;
    float delta_eta_jj, centrality;
    float delta_R_ll_1, delta_R_ll_2, delta_R_zz, delta_R_jj, delta_R_zz_jj;


    tin->SetBranchAddress("l1_pt",  &l1_pt);
    tin->SetBranchAddress("l1_eta", &l1_eta);
    tin->SetBranchAddress("l1_phi", &l1_phi);
    tin->SetBranchAddress("l1_e",   &l1_e);
    tin->SetBranchAddress("l2_pt",  &l2_pt);
    tin->SetBranchAddress("l2_eta", &l2_eta);
    tin->SetBranchAddress("l2_phi", &l2_phi);
    tin->SetBranchAddress("l2_e",   &l2_e);
    tin->SetBranchAddress("l3_pt",  &l3_pt);
    tin->SetBranchAddress("l3_eta", &l3_eta);
    tin->SetBranchAddress("l3_phi", &l3_phi);
    tin->SetBranchAddress("l3_e",   &l3_e);
    tin->SetBranchAddress("l4_pt",  &l4_pt);
    tin->SetBranchAddress("l4_eta", &l4_eta);
    tin->SetBranchAddress("l4_phi", &l4_phi);
    tin->SetBranchAddress("l4_e",   &l4_e);
    tin->SetBranchAddress("leadingj_pt",  &leadingj_pt);
    tin->SetBranchAddress("leadingj_eta", &leadingj_eta);
    tin->SetBranchAddress("leadingj_phi", &leadingj_phi);
    tin->SetBranchAddress("leadingj_e",   &leadingj_e);
    tin->SetBranchAddress("subleadingj_pt",  &subleadingj_pt);
    tin->SetBranchAddress("subleadingj_eta", &subleadingj_eta);
    tin->SetBranchAddress("subleadingj_phi", &subleadingj_phi);
    tin->SetBranchAddress("subleadingj_e",   &subleadingj_e);
    tin->SetBranchAddress("z1_pt",  &z1_pt);
    tin->SetBranchAddress("z1_eta", &z1_eta);
    tin->SetBranchAddress("z1_phi", &z1_phi);
    tin->SetBranchAddress("z1_e",   &z1_e);
    tin->SetBranchAddress("z1_m",   &z1_m);
    tin->SetBranchAddress("z2_pt",  &z2_pt);
    tin->SetBranchAddress("z2_eta", &z2_eta);
    tin->SetBranchAddress("z2_phi", &z2_phi);
    tin->SetBranchAddress("z2_e",   &z2_e);
    tin->SetBranchAddress("z2_m",   &z2_m);
    tin->SetBranchAddress("zz_pt",  &zz_pt);
    tin->SetBranchAddress("zz_eta", &zz_eta);
    tin->SetBranchAddress("zz_phi", &zz_phi);
    tin->SetBranchAddress("zz_e",   &zz_e);
    tin->SetBranchAddress("zz_m",   &zz_m);
    tin->SetBranchAddress("jj_pt",  &jj_pt);
    tin->SetBranchAddress("jj_eta", &jj_eta);
    tin->SetBranchAddress("jj_phi", &jj_phi);
    tin->SetBranchAddress("jj_e",   &jj_e);
    tin->SetBranchAddress("jj_m",   &jj_m);
    tin->SetBranchAddress("delta_R_ll_1",   &delta_R_ll_1);
    tin->SetBranchAddress("delta_R_ll_2",   &delta_R_ll_2);
    tin->SetBranchAddress("delta_R_zz",   &delta_R_zz);
    tin->SetBranchAddress("delta_R_jj",   &delta_R_jj);
    tin->SetBranchAddress("delta_R_zz_jj",   &delta_R_zz_jj);
    tin->SetBranchAddress("mcweight", &weight);
    tin->SetBranchAddress("delta_eta_jj", &delta_eta_jj);
    tin->SetBranchAddress("centrality", &centrality);

    for(int i=0; i<tin->GetEntries(); i++)
    {
	if ( i%50000 == 0)  cout << "Processing " << i << " events." << endl; // monitoring
	int ncut = CUT.size();
	for ( int j=0; j<ncut; j++)
	{
	    FLAG[CUT[j]] = false;
	}
	// clear vector
	weight = -9999.; 
	l1_pt = -9999.; l1_eta = -9999.; l1_phi = -9999.; l1_e = -9999.;
	l2_pt = -9999.; l2_eta = -9999.; l2_phi = -9999.; l2_e = -9999.;
	l3_pt = -9999.; l3_eta = -9999.; l3_phi = -9999.; l3_e = -9999.;
	l4_pt = -9999.; l4_eta = -9999.; l4_phi = -9999.; l4_e = -9999.;
	z1_pt = -9999.; z1_eta = -9999.; z1_phi = -9999.; z1_e = -9999.; z1_m = -9999.;
	z2_pt = -9999.; z2_eta = -9999.; z2_phi = -9999.; z2_e = -9999.; z2_m = -9999.;
	zz_pt = -9999.; zz_eta = -9999.; zz_phi = -9999.; zz_e = -9999.; zz_m = -9999.;
	jj_pt = -9999.; jj_eta = -9999.; jj_phi = -9999.; jj_e = -9999.; jj_m = -9999.;
	leadingj_pt = -9999.; leadingj_eta = -9999.; leadingj_phi = -9999.; leadingj_e = -9999.;
	subleadingj_pt = -9999.; subleadingj_eta = -9999.; subleadingj_phi = -9999.; subleadingj_e = -9999.;
	delta_eta_jj = -9999.;
	centrality = -9999.;
	delta_R_ll_1  = -9999.;
	delta_R_ll_2  = -9999.;
	delta_R_zz    = -9999.;
	delta_R_jj    = -9999.;
	delta_R_zz_jj = -9999.;

	float zz_rap  = -9999.;
	tin->GetEntry(i);

	VALUE["l1_pt"]  = l1_pt;
	VALUE["l1_eta"] = l1_eta;
	VALUE["l1_phi"] = l1_phi;
	VALUE["l1_e"]   = l1_e;
	VALUE["l2_pt"]  = l2_pt;
	VALUE["l2_eta"] = l2_eta;
	VALUE["l2_phi"] = l2_phi;
	VALUE["l2_e"]   = l2_e;
	VALUE["l3_pt"]  = l3_pt;
	VALUE["l3_eta"] = l3_eta;
	VALUE["l3_phi"] = l3_phi;
	VALUE["l3_e"]   = l3_e;
	VALUE["l4_pt"]  = l4_pt;
	VALUE["l4_eta"] = l4_eta;
	VALUE["l4_phi"] = l4_phi;
	VALUE["l4_e"]   = l4_e;
	VALUE["z1_pt"]  = z1_pt;
	VALUE["z1_eta"] = z1_eta;
	VALUE["z1_phi"] = z1_phi;
	VALUE["z1_e"]   = z1_e;
	VALUE["z1_m"]   = z1_m;
	VALUE["z2_pt"]  = z2_pt;
	VALUE["z2_eta"] = z2_eta;
	VALUE["z2_phi"] = z2_phi;
	VALUE["z2_e"]   = z2_e;
	VALUE["z2_m"]   = z2_m;
	VALUE["zz_pt"]  = zz_pt;
	VALUE["zz_eta"] = zz_eta;
	VALUE["zz_phi"] = zz_phi;
	VALUE["zz_e"]   = zz_e;
	VALUE["zz_m"]   = zz_m;
	VALUE["jj_pt"]  = jj_pt;
	VALUE["jj_eta"] = jj_eta;
	VALUE["jj_phi"] = jj_phi;
	VALUE["jj_e"]   = jj_e;
	VALUE["jj_m"]   = jj_m;
	VALUE["leadingj_pt"]  = leadingj_pt;
	VALUE["leadingj_eta"] = leadingj_eta;
	VALUE["leadingj_phi"] = leadingj_phi;
	VALUE["leadingj_e"]   = leadingj_e;
	VALUE["subleadingj_pt"]  = subleadingj_pt;
	VALUE["subleadingj_eta"] = subleadingj_eta;
	VALUE["subleadingj_phi"] = subleadingj_phi;
	VALUE["subleadingj_e"]   = subleadingj_e;
	VALUE["delta_eta_jj"] = delta_eta_jj;
	VALUE["centrality"]   = centrality;
	VALUE["delta_R_jj"]   = delta_R_jj;
	VALUE["delta_R_zz"]   = delta_R_zz;
	VALUE["delta_R_ll_1"] = delta_R_ll_1;
	VALUE["delta_R_ll_2"] = delta_R_ll_2;
	VALUE["delta_R_zz_jj"]= delta_R_zz_jj;

	// cut
	FLAG["none"] = true;
	m_none++;

	if (FLAG["none"] && z2_m > 60 )
	{
	    FLAG["zvv"] = true;
	    m_zvv++;
	}
	if(FLAG["none"] && fabs(l1_eta) < 2.5 && fabs(l2_eta) < 2.5)
	{   
	    FLAG["lepEta"] = true;   
	    m_lepEta++;
	}

	if(FLAG["lepEta"] && l1_pt > 30)
	{   
	    FLAG["leadingLepPt"] = true;   
	    m_leadingLepPt++;
	}

	if(FLAG["leadingLepPt"] && l2_pt > 20)
	{   
	    FLAG["subleadingLepPt"] = true;   
	    m_subleadingLepPt++;
	}

	if(FLAG["subleadingLepPt"] && z2_pt > 90)
	{
	    FLAG["missingET"] = true;
	    m_missingET++;
	}

	if( FLAG["missingET"] && fabs(z1_m - Z_MASS) < 15 )
	{   
	    FLAG["z"] = true;   
	    m_z++;
	}
	
	if( FLAG["z"] && subleadingj_pt > 25 )
	{   
	    FLAG["numJet"] = true;   
	    m_numJet++;
	}

	if ( FLAG["numJet"] && jj_m > 500 )
	{
	    FLAG["mjj"] = true;
	    m_mjj++;
	}

	if( FLAG["mjj"] && delta_eta_jj > 3)
	{
	    FLAG["deltaEtajj"] = true;
	    m_deltaEtajj++;
	}

	FillHistVar(weight);
    }
    fout->cd();
    SaveHistVar();

    time(&finish);
    double timedif = difftime(finish, start);

    cout << "time cost: " << timedif << endl;

    cout << "none cut: " << m_none << "\t\t" << "zvv cut:  " << m_zvv << "\n"
	 << "lepEta events: " << m_lepEta << "\n"
	 << "leadingLepPt :" << m_leadingLepPt << "\n"
	 << "subleadingLepPt: " << m_subleadingLepPt << "\n"
	 << "missingET: " << m_missingET << "\n" 
	 << "z: " << m_z << "\n"
	 << "numJet: " << m_numJet << "\n"
	 << "mjj: " << m_mjj << "\n"
	 << "deltaEtajj: " << m_deltaEtajj << "\n"
	 << endl;
    return 0;
}


