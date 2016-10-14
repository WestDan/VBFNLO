#!/usr/bin/python

##############
## Make Plots
##############

########################
# Libraries and Includes
########################

# Python System Modules
import os,sys,glob
import logging

import array
# Python Paths
ROOTLIB, PATH_OnePlot_MODULE1, PATH_OnePlot_MODULE2 = "", "", ""

if os.getenv("ROOTLIB"): ROOTLIB=os.getenv("ROOTLIB")
if os.getenv("PATH_OnePlot_MODULE1"): PATH_OnePlot_MODULE1=os.getenv("PATH_OnePlot_MODULE1")
if os.getenv("PATH_OnePlot_MODULE2"): PATH_OnePlot_MODULE2=os.getenv("PATH_OnePlot_MODULE2")

sys.path.append(ROOTLIB)
sys.path.append(PATH_OnePlot_MODULE1)
sys.path.append(PATH_OnePlot_MODULE2)

# Python Math Modules
import array
from math import sqrt,fabs,sin,pow

# ROOT Modules
from ROOT import TFile,TTree,TChain,TBranch,TH1F,TH2F
from ROOT import TCanvas,TPad,TLine
from ROOT import TLorentzVector
from ROOT import gROOT, gDirectory,gStyle
from ROOT import TPaveText, TLegend 
import ROOT

# user modules
from oneplot import oneplot
from module_syst import systPDFCT10, systEnvelope, syst1v1, systCombine
from module_style import atlas_style
    
###############################
# User defined drawing function
###############################

if __name__ == "__main__":

    ############################
    # Plot for Unfolding Results
    ############################
    
    files="eemm_185019_fid_hist.root"
    
    # open file
    fin = TFile(files)

    VAR_4 = [ [ ["l"], ["e", "pt", "eta", "phi"] ] ]
    VAR_2 = [ [ ["z"], ["pt", "eta", "phi", "m"] ] ] 
    VAR_1 = [ [ ["zz", "jj"], [ "m", "pt", "eta", "phi"] ] ]
    VAR_0 = ["delta_R_zz", "delta_R_jj", "delta_R_zz_jj", "mcweight", "delta_eta_jj", "centrality"]
    VAR_B = [("delta_R_ll_1", "delta_R_ll_2")] 
    VAR_J = ["pt", "eta", "phi"]

    TITLES = {"l_e":["E [GeV]","events / 10GeV "],
              "l_pt":["Pt [Gev]","events / 10GeV "],
              "l_eta":["#eta","events / 0.35 "],
              "l_phi":["#phi","events / 0.35 "],
              "z_pt":["Pt [Gev]","events / 10GeV "], 
              "z_eta":["#eta","events / 0.5 "],      
              "z_phi":["#phi","events / 0.35 "],      
              "z_m":["M [GeV]", "events / 6GeV"],
              "z_e":["E [GeV]","events / 20GeV "],   
              "leadingj_e-subleadingj_e":["E [GeV]","events / 20GeV "],   
              "leadingj_pt-subleadingj_pt":["Pt [Gev]","events / 10GeV "], 
              "leadingj_eta-subleadingj_eta":["#eta","events / 0.5 "],      
              "leadingj_phi-subleadingj_phi":["#phi","events / 0.35 "],      
              "zz_pt":["Pt [Gev]","events / 10GeV "], 
              "zz_eta":["#eta","events / 0.2 "],      
              "zz_phi":["#phi","events / 0.2 "],      
              "zz_m":["M [GeV]", "events / GeV"],
              "zz_e":["E [GeV]","events / 20GeV "],   
              "jj_pt":["Pt [Gev]","events / 10GeV "], 
              "jj_eta":["#eta","events / 0.2 "],      
              "jj_phi":["#phi","events / 0.2 "],      
              "jj_m":["M [GeV]", "events / 60GeV"],
              "jj_e":["E [GeV]","events / 20GeV "],   
              "delta_R_zz":["#{Delta}R_zz", "events/ 0.5"],
              "delta_R_jj":["#{Delta}R_jj", "events/ 0.5"],
              "delta_R_zz_jj":["#{Delta}R_zzjj", "events/ 0.5"],
              "delta_R_ll_1-delta_R_ll_2":["#{Delta}R_ll", "events/ 0.5"],
              "mcweight":["weight", "events/ 0.5"],
              "delta_eta_jj":["#Delta#eta_{jj}","events / 0.1"],
              "centrality":["centrality","events / 0.1 "]
              }
    LEGENDS = { 
                "deltaEta_jj":"#Delta#eta",
                "centrality":"centrality"
              }

    for entry in VAR_4:
        for par in entry[0]:
            for var in entry[1]:

                # load histograms
                histos = []
                names = []
                var_name = par + "_" + var
                for i in range(1,5):
                    hist_name = par + str(i) + "_" + var
                    names.append(hist_name)
                    histo_tmp = fin.Get(hist_name)
                    histos.append(histo_tmp)

                # Draw histograms
                xtitle = TITLES[var_name][0]
                ytitle = TITLES[var_name][1]
                legends = ["e1", "e2", "m1", "m2"]
                options = ["HIST", "HIST", "HIST", "HIST"]            
                opt_legends = ["L", "L", "L", "L"]                    
                marker_types = [26, 26, 32, 32]                       
                marker_sizes = [1, 1, 1, 1]                           
                line_sizes = [2, 2, 2, 2]                             
                marker_colors=line_colors=fill_colors=[4, 6, 2, 12] 
                fill_types = [0, 0, 0, 0]                             
                figname = var_name
        
                # call one plot
                theone = oneplot()
                theone._figtype=["png"]
                theone.initialize(list_histo=histos,names=names,legends=legends, opt_legends=opt_legends,
                                xtitle=xtitle,ytitle=ytitle,
                                figname=figname, 
                                options=options, marker_types=marker_types, marker_colors=marker_colors, marker_sizes=marker_sizes,
                                line_colors=line_colors, line_sizes=line_sizes,
                                fill_colors=fill_colors, fill_types=fill_types)
                theone.plot1DHistogram()
                theone.finish()

    for entry in VAR_2:
        for par in entry[0]:
            for var in entry[1]:

                # load histograms
                histos = []
                names = []
                var_name = par + "_" + var
                for i in range(1,3):
                    hist_name = par + str(i) + "_" + var
                    names.append(hist_name)
                    histo_tmp = fin.Get(hist_name)
                    histos.append(histo_tmp)

                # Draw histograms
                xtitle = TITLES[var_name][0]
                ytitle = TITLES[var_name][1]
                legends = ["z_ee", "z_mm"]
                options = ["HIST", "HIST"]            
                opt_legends = ["L", "L"]                    
                marker_types = [26, 32]                       
                marker_sizes = [1, 1]                           
                line_sizes = [2, 2]                             
                marker_colors=line_colors=fill_colors=[4, 6] 
                fill_types = [0, 0]                             
                figname = var_name
        
                # call one plot
                theone = oneplot()
                theone._figtype=["png"]
                theone.initialize(list_histo=histos,names=names,legends=legends, opt_legends=opt_legends,
                                xtitle=xtitle,ytitle=ytitle,
                                figname=figname, 
                                options=options, marker_types=marker_types, marker_colors=marker_colors, marker_sizes=marker_sizes,
                                line_colors=line_colors, line_sizes=line_sizes,
                                fill_colors=fill_colors, fill_types=fill_types)
                theone.plot1DHistogram()
                theone.finish()


    for entry in VAR_1:
        for par in entry[0]:
            for var in entry[1]:
                histo_name = par + "_" + var
                names = [histo_name]
                histos = []

                # load histograms
                histo_tmp = fin.Get(histo_name)
                histos.append(histo_tmp)

                # Draw histograms
                xtitle = TITLES[histo_name][0]
                ytitle = TITLES[histo_name][1]
                legends = [histo_name]
                options = ["HIST"]
                opt_legends = ["L"]
                marker_types = [26]
                marker_sizes = [1]
                line_sizes = [2]
                marker_colors=line_colors=fill_colors=[4]
                fill_types = [0]
                figname=histo_name
        
                # call one plot
                theone = oneplot()
                theone._figtype = ["png"]
                theone.initialize(list_histo=histos,names=names,legends=legends, opt_legends=opt_legends,
                                xtitle=xtitle,ytitle=ytitle,
                                figname=figname, 
                                options=options, marker_types=marker_types, marker_colors=marker_colors, marker_sizes=marker_sizes,
                                line_colors=line_colors, line_sizes=line_sizes,
                                fill_colors=fill_colors, fill_types=fill_types)
                newline = TLine(0.2,0.2,0.4,0.4)
                newline.Draw()
                theone.plot1DHistogram()
                theone.finish()

    for var in VAR_0:
        histo_name = var
        names = [var]
        histos = []

        # load histograms
        histo_tmp = fin.Get(histo_name)
        histos.append(histo_tmp)

        # Draw histograms
        xtitle = TITLES[histo_name][0]
        ytitle = TITLES[histo_name][1]
        legends = [var]
        options = ["HIST"]
        opt_legends = ["L"]
        marker_types = [26]
        marker_sizes = [1]
        line_sizes = [2]
        marker_colors=line_colors=fill_colors=[4]
        fill_types = [0]
        figname=histo_name

        # call one plot
        theone = oneplot()
        theone._figtype = ["png"]
        theone.initialize(list_histo=histos,names=names,legends=legends, opt_legends=opt_legends,
                        xtitle=xtitle,ytitle=ytitle,
                        figname=figname, 
                        options=options, marker_types=marker_types, marker_colors=marker_colors, marker_sizes=marker_sizes,
                        line_colors=line_colors, line_sizes=line_sizes,
                        fill_colors=fill_colors, fill_types=fill_types)
        newline = TLine(0.2,0.2,0.4,0.4)
        newline.Draw()
        theone.plot1DHistogram()
        theone.finish()

    for var in VAR_B:
        var1, var2 = var
        names = [var1, var2]
        histos = []

        histo_tmp = fin.Get(var1)
        histos.append(histo_tmp)
        histo_tmp = fin.Get(var2)
        histos.append(histo_tmp)

        xtitle = TITLES[var1+"-"+var2][0]
        ytitle = TITLES[var1+"-"+var2][1]
        var1.replace("1", "ee")
        var2.replace("2", "mm")
        legends = [var1, var2]
        options = ["HIST", "HIST"]            
        opt_legends = ["L", "L"]                    
        marker_types = [26, 32]                       
        marker_sizes = [1, 1]                           
        line_sizes = [2, 2]                             
        marker_colors=line_colors=fill_colors=[4, 6] 
        fill_types = [0, 0]                             
        figname = var1 + var2

        # call one plot
        theone = oneplot()
        theone._figtype=["png"]
        theone.initialize(list_histo=histos,names=names,legends=legends, opt_legends=opt_legends,
                        xtitle=xtitle,ytitle=ytitle,
                        figname=figname, 
                        options=options, marker_types=marker_types, marker_colors=marker_colors, marker_sizes=marker_sizes,
                        line_colors=line_colors, line_sizes=line_sizes,
                        fill_colors=fill_colors, fill_types=fill_types)
        theone.plot1DHistogram()
        theone.finish()
    
    for var in VAR_J:
        var1 = "leadingj_" + var
        var2 = "subleadingj_" + var
        names = [var1, var2]
        histos = []

        histo_tmp = fin.Get(var1)
        histos.append(histo_tmp)
        histo_tmp = fin.Get(var2)
        histos.append(histo_tmp)

        xtitle = TITLES[var1+"-"+var2][0]
        ytitle = TITLES[var1+"-"+var2][1]
        legends = ["j1", "j2"]
        options = ["HIST", "HIST"]            
        opt_legends = ["L", "L"]                    
        marker_types = [26, 32]                       
        marker_sizes = [1, 1]                           
        line_sizes = [2, 2]                             
        marker_colors=line_colors=fill_colors=[4, 6] 
        fill_types = [0, 0]                             
        figname = "jet_" + var

        # call one plot
        theone = oneplot()
        theone._figtype=["png"]
        theone.initialize(list_histo=histos,names=names,legends=legends, opt_legends=opt_legends,
                        xtitle=xtitle,ytitle=ytitle,
                        figname=figname, 
                        options=options, marker_types=marker_types, marker_colors=marker_colors, marker_sizes=marker_sizes,
                        line_colors=line_colors, line_sizes=line_sizes,
                        fill_colors=fill_colors, fill_types=fill_types)
        theone.plot1DHistogram()
        theone.finish()
    # plot test 1 (plot all truth distribution together)
    #names=["nom", "PSUp", "PSDown", "ScaleUp", "ScaleDown", "PDFUp", "PDFDown", "EWKUp", "EWKDown"]
    #legends=["nom", "PSUp", "PSDown", "ScaleUp", "ScaleDown", "PDFUp", "PDFDown", "EWKUp", "EWKDown"]
    #histos=[histo_mc, histo_mc_psup, histo_mc_psdn, histo_mc_scaleup, histo_mc_scaledn, histo_mc_pdfup, histo_mc_pdfdn, histo_mc_ewkup, histo_mc_ewkdn]
    #figname="compare_mctruth_{0}".format(keyname)
    #options=["HIST", "LPE", "LPE", "LPE", "LPE", "LPE", "LPE", "LPE", "LPE"]
    #marker_types=[1, 1, 1, 1, 1, 1, 1, 1, 1]
    #line_colors=[1, 2, 2, 3, 3, 4, 4, 5, 5]
    #line_sizes=[2, 2, 2, 2, 2, 2, 2, 2, 2]
    #theone = oneplot()
    #theone.initialize(list_histo=histos,names=names,legends=legends,
    #                ratio=0.2,xtitle="{0} [GeV]".format(xname),ytitle="#Delta#sigma / Bin [fb]",
    #                figname=figname, ratiotitle="Ratio",
    #                equalbin=True, options=options, line_colors=line_colors, line_sizes=line_sizes)
    #theone.plot1DHistogram()
    #theone.finish()
    
    fin.Close()
    
    # save the files into histograms
    #fout = TFile(figname+".root", "recreate")
    #histo_unfold_final.Write()
    #histo_mc_final.Write()



    #equalbin=True, fout.Close()
