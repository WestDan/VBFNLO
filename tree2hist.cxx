#include <iostream>
#include <string> 
#include <map>
#include <time.h>
#include <vector>
#include "TTree.h"
#include "TH1F.h"
#include "parse_string.cxx" 
time_t start, finish;
map<string, TH1F*> HIST;

int InitHistVar(string vars, int nbins, float xlow, float xhigh)
{
    vector<string> var_list;

    parse_string(vars, var_list, ",");

    if( xlow > xhigh)
    {
	cerr << "Incorrect parameters for InitHistVar, xlow: " << xlow << " is larger than xhigh: " << xhigh << endl;
	return -1;
    }
    int nvar = var_list.size();
    for(int j=0; j<nvar; j++)
    {
	string var = var_list[j];
	HIST[var] = new TH1F(var.c_str(), var.c_str(), nbins, xlow, xhigh );
    }
    return 0;
}

void SaveHistVar()
{
    map<string, TH1F*>::iterator it_var = HIST.begin();
    map<string, TH1F*>::iterator it_var_end = HIST.end();
    for (; it_var != it_var_end; it_var++)
    {
	string var = it_var->first;
	HIST[var]->Write();
    }
}

void Tree2Hist(string input_file, string output_file)
{
    time(&start);

    TFile * fin = new TFile(input_file.c_str(), "read");
    if( ! fin ) 
    {
	cout << "Error, file " << input_file << "doesn't exit" << endl;
	return ;
    }

    TTree * tin = (TTree *) fin->Get("vbfnlo");
    if ( ! tin ) 
    {
	cout << "tin vbfnlo doesn't exist in file " << input_file << endl;
	return ;
    }

    TFile * fout = new TFile(output_file.c_str(), "recreate");
    if(! fout)
    {
	cout << "Error! unable to create file: " << output_file << endl;
	return;
    }

    InitHistVar("l1_pt, l2_pt, l3_pt, l4_pt", 40, 0, 400);
    InitHistVar("l1_eta, l2_eta, l3_eta, l4_eta", 20, -3.5, 3.5);
    InitHistVar("l1_phi, l2_phi, l3_phi, l4_phi", 20, -3.5, 3.5);
    InitHistVar("l1_e, l2_e, l3_e, l4_e", 50, 0, 500);
    InitHistVar("leadingj_pt, subleadingj_pt, jj_pt", 30, 0, 300);
    InitHistVar("leadingj_eta,subleadingj_eta,jj_eta", 20, -5.0, 5.0);
    InitHistVar("leadingj_phi,subleadingj_phi,jj_phi", 20, -3.5, 3.5);
    InitHistVar("leadingj_e, subleadingj_e", 50, 0, 500);
    InitHistVar("z1_pt, z2_pt, zz_pt", 50, 0, 500);
    InitHistVar("z1_eta, z2_eta, zz_eta", 20, -5.0, 5.0);
    InitHistVar("z1_phi, z2_phi, zz_phi", 20, -3.5, 3.5);
    InitHistVar("z1_m, z2_m", 50, 0, 300);
    InitHistVar("zz_m", 50, 0, 500);
    InitHistVar("jj_m", 50, 0, 3000);
    InitHistVar("delta_R_ll_1, delta_R_ll_2, delta_R_zz, delta_R_jj, delta_R_zz_jj", 20, 0, 10);
    InitHistVar("mcweight", 10, 0, 5);
    InitHistVar("delta_eta_jj", 20, 0, 10);
    InitHistVar("centrality", 20, -1, 1);

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
    double weight;
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

    int entries = tin->GetEntries();
    for(int n=0; n<entries; n++)
    {
	l1_pt = -9999.; l2_pt = -9999.; l3_pt = -9999.; l4_pt = -9999.;
	l1_eta= -9999.; l2_eta= -9999.; l3_eta= -9999.; l4_eta= -9999.;
	l1_phi= -9999.; l2_phi= -9999.; l3_phi= -9999.; l4_phi= -9999.;
	l1_e  = -9999.; l2_e  = -9999.; l3_e  = -9999.; l4_e  = -9999.;
	z1_pt = -9999.; z2_pt = -9999.; leadingj_pt = -9999.; subleadingj_pt = -9999.;
	z1_eta= -9999.; z2_eta= -9999.; leadingj_eta= -9999.; subleadingj_eta= -9999.;
	z1_phi= -9999.; z2_phi= -9999.; leadingj_phi= -9999.; subleadingj_phi= -9999.;
	z1_e  = -9999.; z2_e  = -9999.; leadingj_e  = -9999.; subleadingj_e  = -9999.;
	z1_m  = -9999.; z2_m  = -9999.;
	jj_pt = -9999.; zz_pt = -9999.; 
	jj_eta= -9999.; zz_eta= -9999.; 
	jj_phi= -9999.; zz_phi= -9999.; 
	jj_e  = -9999.; zz_e  = -9999.; 
	jj_m  = -9999.; zz_m  = -9999.; 
	delta_R_ll_1 = -9999.; delta_R_ll_2 = -9999.;
	delta_R_jj = -9999.;  delta_R_zz = -9999.; delta_R_zz_jj = -9999.;
	weight = 1.0;
	delta_eta_jj = -9999.;
	centrality = -9999.;

	tin->GetEntry(n);
	HIST["l1_pt"]->Fill(l1_pt, weight);
	HIST["l1_eta"]->Fill(l1_eta, weight);
	HIST["l1_phi"]->Fill(l1_phi, weight);
	HIST["l1_e"]->Fill(l1_e, weight);
	HIST["l2_pt"]->Fill(l2_pt, weight);
	HIST["l2_eta"]->Fill(l2_eta, weight);
	HIST["l2_phi"]->Fill(l2_phi, weight);
	HIST["l2_e"]->Fill(l2_e, weight);
	HIST["l3_pt"]->Fill(l3_pt, weight);
	HIST["l3_eta"]->Fill(l3_eta, weight);
	HIST["l3_phi"]->Fill(l3_phi, weight);
	HIST["l3_e"]->Fill(l3_e, weight);
	HIST["l4_pt"]->Fill(l4_pt, weight);
	HIST["l4_eta"]->Fill(l4_eta, weight);
	HIST["l4_phi"]->Fill(l4_phi, weight);
	HIST["l4_e"]->Fill(l4_e, weight);
	HIST["leadingj_pt"]->Fill(leadingj_pt, weight);
	HIST["leadingj_eta"]->Fill(leadingj_eta, weight);
	HIST["leadingj_phi"]->Fill(leadingj_phi, weight);
//	HIST["leadingj_e"]->Fill(leadingj_e, weight);
	HIST["subleadingj_pt"]->Fill(subleadingj_pt, weight);
	HIST["subleadingj_eta"]->Fill(subleadingj_eta, weight);
	HIST["subleadingj_phi"]->Fill(subleadingj_phi, weight);
//	HIST["subleadingj_e"]->Fill(subleadingj_e, weight);
	HIST["z1_pt"]->Fill(z1_pt, weight);
	HIST["z1_eta"]->Fill(z1_eta, weight);
	HIST["z1_phi"]->Fill(z1_phi, weight);
//	HIST["z1_e"]->Fill(z1_e, weight);
	HIST["z1_m"]->Fill(z1_m, weight);
	HIST["z2_pt"]->Fill(z2_pt, weight);
	HIST["z2_eta"]->Fill(z2_eta, weight);
	HIST["z2_phi"]->Fill(z2_phi, weight);
//	HIST["z2_e"]->Fill(z2_e, weight);
	HIST["z2_m"]->Fill(z2_m, weight);
	HIST["zz_pt"]->Fill(zz_pt, weight);
	HIST["zz_eta"]->Fill(zz_eta, weight);
	HIST["zz_phi"]->Fill(zz_phi, weight);
//	HIST["zz_e"]->Fill(zz_e, weight);
	HIST["zz_m"]->Fill(zz_m, weight);
	HIST["jj_pt"]->Fill(jj_pt, weight);
	HIST["jj_eta"]->Fill(jj_eta, weight);
	HIST["jj_phi"]->Fill(jj_phi, weight);
//	HIST["jj_e"]->Fill(jj_e, weight);
	HIST["jj_m"]->Fill(jj_m, weight);
	HIST["delta_R_ll_1"]->Fill(delta_R_ll_1, weight);
	HIST["delta_R_ll_2"]->Fill(delta_R_ll_2, weight);
	HIST["delta_R_zz"]->Fill(delta_R_zz, weight);
	HIST["delta_R_jj"]->Fill(delta_R_jj, weight);
	HIST["delta_R_zz_jj"]->Fill(delta_R_zz_jj, weight);
	HIST["mcweight"]->Fill(weight);
	HIST["delta_eta_jj"]->Fill(delta_eta_jj, weight);
	HIST["centrality"]->Fill(centrality, weight);
    }
    fout->cd();
    SaveHistVar();
}
