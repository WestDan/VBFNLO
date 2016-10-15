#include <iostream>
#include <string>
#include <time.h>
#include "TLorentzVector.h"
#include "TFile.h"
#include "TTree.h"
using namespace std;

// dealing with VBFNLO 4e channel
// extract raw pt, eta, phi, eta to form raw data for checking invanity.

static double Z_MASS = 91.1876; // unit in GeV 
time_t start, finish;

void raw(string input_file, string output_file) 
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

    TTree * tout = new TTree("vbfnlo", "vbfnlo");
    double px[10];
    double py[10];
    double pz[10];
    double E[10];
    double M[10];
    int pdgID[10];
    int status[10];
    int parent1_index[10];
    int parent2_index[10];
    double weight;
    tin->SetBranchAddress("mc_px", px);
    tin->SetBranchAddress("mc_py", py);
    tin->SetBranchAddress("mc_pz", pz);
    tin->SetBranchAddress("mc_E", E);
    tin->SetBranchAddress("mc_m", M);
    tin->SetBranchAddress("mc_pdgID", pdgID);
    tin->SetBranchAddress("mc_status", status);
    tin->SetBranchAddress("mc_parent1_index", parent1_index);
    tin->SetBranchAddress("mc_parent2_index", parent2_index);
    tin->SetBranchAddress("mc_evtweight", &weight);

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
    tout->Branch("l1_pt",  &l1_pt);
    tout->Branch("l1_eta", &l1_eta);
    tout->Branch("l1_phi", &l1_phi);
    tout->Branch("l1_e",   &l1_e);
    tout->Branch("l2_pt",  &l2_pt);
    tout->Branch("l2_eta", &l2_eta);
    tout->Branch("l2_phi", &l2_phi);
    tout->Branch("l2_e",   &l2_e);
    tout->Branch("l3_pt",  &l3_pt);
    tout->Branch("l3_eta", &l3_eta);
    tout->Branch("l3_phi", &l3_phi);
    tout->Branch("l3_e",   &l3_e);
    tout->Branch("l4_pt",  &l4_pt);
    tout->Branch("l4_eta", &l4_eta);
    tout->Branch("l4_phi", &l4_phi);
    tout->Branch("l4_e",   &l4_e);
    tout->Branch("leadingj_pt",  &leadingj_pt);
    tout->Branch("leadingj_eta", &leadingj_eta);
    tout->Branch("leadingj_phi", &leadingj_phi);
    tout->Branch("leadingj_e",   &leadingj_e);
    tout->Branch("subleadingj_pt",  &subleadingj_pt);
    tout->Branch("subleadingj_eta", &subleadingj_eta);
    tout->Branch("subleadingj_phi", &subleadingj_phi);
    tout->Branch("subleadingj_e",   &subleadingj_e);
    tout->Branch("z1_pt",  &z1_pt);
    tout->Branch("z1_eta", &z1_eta);
    tout->Branch("z1_phi", &z1_phi);
    tout->Branch("z1_e",   &z1_e);
    tout->Branch("z1_m",   &z1_m);
    tout->Branch("z2_pt",  &z2_pt);
    tout->Branch("z2_eta", &z2_eta);
    tout->Branch("z2_phi", &z2_phi);
    tout->Branch("z2_e",   &z2_e);
    tout->Branch("z2_m",   &z2_m);
    tout->Branch("zz_pt",  &zz_pt);
    tout->Branch("zz_eta", &zz_eta);
    tout->Branch("zz_phi", &zz_phi);
    tout->Branch("zz_e",   &zz_e);
    tout->Branch("zz_m",   &zz_m);
    tout->Branch("jj_pt",  &jj_pt);
    tout->Branch("jj_eta", &jj_eta);
    tout->Branch("jj_phi", &jj_phi);
    tout->Branch("jj_e",   &jj_e);
    tout->Branch("jj_m",   &jj_m);
    tout->Branch("delta_R_ll_1",   &delta_R_ll_1);
    tout->Branch("delta_R_ll_2",   &delta_R_ll_2);
    tout->Branch("delta_R_zz",   &delta_R_zz);
    tout->Branch("delta_R_jj",   &delta_R_jj);
    tout->Branch("delta_R_zz_jj",   &delta_R_zz_jj);
    tout->Branch("mcweight", &weight);
    tout->Branch("delta_eta_jj", &delta_eta_jj);
    tout->Branch("centrality", &centrality);

    for(int n=0; n<(int)tin->GetEntries(); n++)
    {
	TLorentzVector lep[4];
	for (int i=0; i<4; i++)
	{
	    lep[i].SetPtEtaPhiE(0, 0, 0, 0);
	}

	TLorentzVector jet[2];
	for (int i=0; i<2; i++)
	{
	    jet[i].SetPtEtaPhiE(0, 0, 0,0);
	}

	TLorentzVector z[2];
	for (int i=0; i<2; i++)
	{
	    z[i].SetPtEtaPhiE(0, 0, 0,0);
	}

	TLorentzVector zz; zz.SetPtEtaPhiE(0, 0, 0, 0);
	TLorentzVector jj; jj.SetPtEtaPhiE(0, 0, 0, 0);

	int	parent1_l[4]={-1, -1, -1, -1};
	int	parent2_l[4]={-1, -1, -1, -1};
	double  delta_R_ll[2] = {-9999., -9999.};

	delta_eta_jj=-9999E10;
	centrality=-9999E10;
	delta_R_ll_1 = -9999.0;
	delta_R_ll_2 = -9999.0;
	delta_R_jj   = -9999.0;
	delta_R_zz   = -9999.0;
	delta_R_zz_jj= -9999.0;

	tin->GetEntry(n);

	int index_l=0, index_j=0;
	for (int i=0; i<10; i++)
	{
	    if(status[i] != 1) continue;
	    else if (abs(pdgID[i]) == 0 || abs(pdgID[i]) == 1 || abs(pdgID[i]) == 2 || abs(pdgID[i])==3 || abs(pdgID[i]) == 4 || abs(pdgID[i]) == 5 ) 
	    {
                jet[index_j].SetPxPyPzE(px[i], py[i], pz[i], E[i]);
		index_j++;
	    }
	    else if ( abs(pdgID[i]) == 11) // electron
	    {
		lep[index_l].SetPxPyPzE(px[i], py[i], pz[i], E[i]);
		parent1_l[index_l] = parent1_index[i];
		parent2_l[index_l] = parent2_index[i];
		index_l++;
	    }
	    else if ( abs(pdgID[i]) == 13) // muon
	    {
		lep[index_l].SetPxPyPzE(px[i], py[i], pz[i], E[i]);
		parent1_l[index_l] = parent1_index[i];
		parent2_l[index_l] = parent2_index[i];
		index_l++;
	    }
	}

	if ( index_l == 4 )
	{
	    int index_z = 0;
	    for ( int i=0; i<4; i++)
	    {
		for (int j=i+1; j<4; j++)
		{
		    if(parent1_l[j] == parent1_l[i]) // do we need to compare their pID ???
		    {
			z[index_z] = lep[i] + lep[j];
			delta_R_ll[index_z] = sqrt( pow( (lep[i].Eta() - lep[j].Eta()), 2) + pow( (lep[i].Phi() - lep[j].Phi()), 2) );
			index_z++;
			break;
		    }
		}
	    }

	    if(index_z == 2)
	    {
		zz = z[0] + z[1];
		delta_R_zz = sqrt( pow( (z[0].Eta() - z[1].Eta()), 2) + pow( (z[0].Phi() - z[1].Phi()) , 2) );
	    }
	}
	else 
	{
	    cout << " event " << n << " doesn't have 4 leptons" << endl;
	    continue;
	}


	if( index_j == 2 )
	{
	    jj = jet[0] + jet[1];
	    delta_R_jj = sqrt( pow( ( jet[0].Eta() - jet[1].Eta()), 2) + pow( (jet[0].Phi() - jet[1].Phi()), 2) );

	    delta_eta_jj = abs( jet[0].Eta() - jet[1].Eta() );

	    if( zz.E() > 1 ) // GeV
	    {
		centrality = (zz.Rapidity() -( jet[0].Rapidity() + jet[1].Rapidity() )/2 ) / abs( jet[0].Rapidity() - jet[1].Rapidity() );
		delta_R_zz_jj = sqrt( pow((zz.Eta() - jj.Eta()), 2) + pow((zz.Phi() - jj.Phi()), 2) );
	    }
	}
	else 
	{
	    cout << "jet number less than 2" << endl;
	    continue;
	}


	l1_pt  = lep[0].Pt();
	l1_eta = lep[0].Eta();
	l1_phi = lep[0].Phi();
	l1_e   = lep[0].E();
	l2_pt  = lep[1].Pt();
	l2_eta = lep[1].Eta();
	l2_phi = lep[1].Phi();
	l2_e   = lep[1].E();
	l3_pt  = lep[2].Pt();
	l3_eta = lep[2].Eta();
	l3_phi = lep[2].Phi();
	l3_e   = lep[2].E();
	l4_pt  = lep[3].Pt();
	l4_eta = lep[3].Eta();
	l4_phi = lep[3].Phi();
	l4_e   = lep[3].E();
	leadingj_pt  = jet[0].Pt();
	leadingj_eta = jet[0].Eta();
	leadingj_phi = jet[0].Phi();
	leadingj_e   = jet[0].E();
	subleadingj_pt  = jet[1].Pt();
	subleadingj_eta = jet[1].Eta();
	subleadingj_phi = jet[1].Phi();
	subleadingj_e   = jet[1].E();
	z1_pt  = z[0].Pt();
	z1_eta = z[0].Eta();
	z1_phi = z[0].Phi();
	z1_e   = z[0].E();
	z1_m   = z[0].M();
	z2_pt  = z[1].Pt();
	z2_eta = z[1].Eta();
	z2_phi = z[1].Phi();
	z2_e   = z[1].E();
	z2_m   = z[1].M();
	zz_pt  = zz.Pt();
	zz_eta = zz.Eta();
	zz_phi = zz.Phi();
	zz_e   = zz.E();
	zz_m   = zz.M();
	jj_pt  = jj.Pt();
	jj_eta = jj.Eta();
	jj_phi = jj.Phi();
	jj_e   = jj.E();
	jj_m   = jj.M();
	delta_R_ll_1 = delta_R_ll[0];
	delta_R_ll_2 = delta_R_ll[1];

	tout->Fill();
    }
    fout->cd();
    tout->Write();

    time(&finish);
    double timedif = difftime(finish, start);

    cout << "time cost: " << timedif << endl;
}
