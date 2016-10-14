#!/usr/bin/python

# / / /
# \ \ \
# / / /
# Y.Wu <Yusheng.Wu@cern.ch>

# A script to extract the info. from a LHE file
# Convert into a ROOT tree

import array
import os,sys,glob
from math import sqrt,fabs,sin,pow
from array import array
ROOTSYS = '/home/zhangweibin/bin/root'
sys.path.append(ROOTSYS)

from ROOT import TFile,TTree,TChain,TBranch,TH1F
from ROOT import TCanvas,TPad
from ROOT import TLorentzVector
from ROOT import gROOT, gDirectory,gStyle
from ROOT import TPaveText, TLegend
import ROOT

def convert(lhefile="pwgevents.lhe", treename="truth", outfile="output.root", run=10000, **kw):

    # Parameters:
    # lhefile -> specify the LHE file to be converted
    # treename -> specify the tree name 
    # run -> specify the RunNumber for this event sample
    
    # Create the output root file
    # outname="output_tree_{0}.root".format(run)
    f=TFile(outfile,"recreate")

    # print-out
    print("")
    print("Convert LHE file to ROOT file, with treename {0} and runnumber {1}".format(treename,run))
    print("LHE file: {0}".format(lhefile))
    print("Output file: {0}".format(outfile))
    print("")
    
    # create the tree
    t=TTree(treename, treename)
    
    # create tree branches for needed variables: int or double
    dict={}
    var_int = ["Run", "Event", "mc_nparticle", "mc_procssid"]
    var_float = ["mc_evtweight", "mc_scale","mc_alphaQED","mc_alphaQCD"]
    
    for index in range(16): # the 16 reweighting factors stored for each events
        var_float.append("mc_evtweight_{0}".format(index+1))
    for var in var_int:
        dict[var]=array("i",[0])
        t.Branch(var, dict[var], var+"/I")
    for var in var_float:
        dict[var]=array("d",[0.])
        t.Branch(var, dict[var], var+"/D")
    
    # create tree branches for needed branches: 1-D arrays
    var_array_int = ["mc_index", "mc_pdgID", "mc_status","mc_parent1_index",
        "mc_parent2_index", "mc_color1", "mc_color2"]
    var_array_float = ["mc_px", "mc_py", "mc_pz", "mc_E", "mc_m", "mc_lifetime", "mc_spin"]
    max_array_length=10
    for var in var_array_int:
        dict[var]=array("i", max_array_length*[0])
        t.Branch(var, dict[var], "{0}[{1}]/I".format(var, "mc_nparticle"))
    for var in var_array_float:
        dict[var]=array("d", max_array_length*[0])
        t.Branch(var, dict[var], "{0}[{1}]/D".format(var, "mc_nparticle"))
    
    # read the LHE file and fill the tree
    event=0
    fin=open("{0}".format(lhefile),"r")
    particle_index=0
    weight_index=0
    for line in fin.readlines():
    
        # start of an event
        if "<event>" in line:
            # index
            event+=1
            particle_index=0
            weight_index=0
            # print-out
            sys.stdout.write("processing events: {0:<8d}\r".format(event))
            sys.stdout.flush()
            # intialize all the variables
            # default value -999 or -999.
            for var in var_int: 
                dict[var][0]=-999
            for var in var_float: 
                dict[var][0]=-999.
            for var in var_array_int:
                for element in range(len(dict[var])): 
                    dict[var][element]=-999
            for var in var_array_float:
                for element in range(len(dict[var])): 
                    dict[var][element]=-999.            
            # assign RunNumber and EventNumber
            dict["Run"][0]=run
            dict["Event"][0]=event
       
        # skip if event information not started
        if event==0: continue
       
        # store the weights
        # <wgt id='mg_reweight_1'> +1.4577405e-04 </wgt>
        if "<wgt" in line and "</wgt>" in line:
            weight_index+=1
            weight_value=float(line.split(">")[1].split("<")[0])
            dict["mc_evtweight_{0}".format(weight_index)][0]=weight_value
                       
        # skip the blank lines and lines with xml tags
        if len(line.split())!=0 and not "<" in line and not ">" in line: 
            items=line.split()
            nitems=len(items)
            # read the event line, like
            # 8      0 +3.7741000e-04 1.12197800e+01 7.54677100e-03 1.74194000e-01
            # nparticles, id of the process, event weight, scale of events, alpha_QED, alpha_QCD
            if nitems==6:
                dict["mc_nparticle"][0]=int(items[0])
                dict["mc_procssid"][0]=int(items[1])
                dict["mc_evtweight"][0]=float(items[2])
                dict["mc_scale"][0]=float(items[3])
                dict["mc_alphaQED"][0]=float(items[4])
                dict["mc_alphaQCD"][0]=float(items[5])
            # read the particle lines, like
            # -3 -1    0    0    0  501 -0.0000000e+00 +0.0000000e+00 +1.2500000e+02 1.25000000e+02 0.00000000e+00 0.0000e+00 1.0000e+00
            # Particle-id, status code, first parent, second parent, color line, anti-color line, lab frame Px, Py, Pz, E, M, 
            # C*tlifetime (flight distance), cosine of angle between particle 1 and decaying particle in lab frame
            if nitems==13:
                dict["mc_index"][particle_index]=particle_index
                dict["mc_pdgID"][particle_index]=int(items[0])
                dict["mc_status"][particle_index]=int(items[1])
                dict["mc_parent1_index"][particle_index]=int(items[2])
                dict["mc_parent2_index"][particle_index]=int(items[3])
                dict["mc_color1"][particle_index]=int(items[4])
                dict["mc_color2"][particle_index]=int(items[5])
                dict["mc_px"][particle_index]=float(items[6])
                dict["mc_py"][particle_index]=float(items[7])
                dict["mc_pz"][particle_index]=float(items[8])
                dict["mc_E"][particle_index]=float(items[9])
                dict["mc_m"][particle_index]=float(items[10])
                dict["mc_lifetime"][particle_index]=float(items[11])
                dict["mc_spin"][particle_index]=float(items[12])
                particle_index+=1
           
        # fill the tree at the end of an event
        if "</event>" in line:
            t.Fill()             
            
    # close
    t.Write()
    f.Close()
    print("")
    print("Total processed events: {0}".format(event))
    
# main function
if __name__ == "__main__": 

    theargv=[]
    if sys.argv[-1]=="-b": theargv=sys.argv[:-1]
    else: theargv=sys.argv

    if len(theargv)==1:
        print("")
        print("A script to convert LHE file into root trees")
        print("")
        print("Warning: Please note this script may only work for some specific LHE format,")
        print(" if encountering errors, one need to adjust the code to meet their own format")
        print("")
        print("Usage:")
        print("     ./convert.py -h  --> print help message")
        print("     ./convert.py lhe-file  --> start conversion by providing the LHE file")
        print("     ./convert.py lhe-file tree-name --> start conversion by providing the LHE file and specify tree name")
        print("     ./convert.py lhe-file tree-name out-file --> in addition, provide the outfile name for outputing tree")
        print("     ./convert.py lhe-file tree-name out-file run-number --> in addition, provide the outfile name and run number for outputing tree")
        print("")
        
    else:
        if len(theargv)==2: convert(lhefile=theargv[1])
        if len(theargv)==3: convert(lhefile=theargv[1], treename=theargv[2])
        if len(theargv)==4: 
            convert(lhefile=theargv[1], treename=theargv[2], outfile=theargv[3])
        if len(theargv)==5: 
            convert(lhefile=theargv[1], treename=theargv[2], outfile=theargv[3], run=int(theargv[4]))


