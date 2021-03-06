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
    
    files="211_eevv_cut.root"
    
    # open file
    fin = TFile(files)

    CUT = ["none", "lepEta", "leadingLepPt", "subleadingLepPt","missingET", "z", "numJet", "mjj", "deltaEtajj"]

    VAR_4 = [ [ ["l"], ["pt", "eta", "phi"] ] ]
    VAR_2 = [ [ ["z"], ["pt", "eta", "phi", "m"] ] ] 
    VAR_1 = [ [ ["zz", "jj"], ["m", "pt", "eta", "phi"] ] ]
    VAR_0 = ["delta_eta_jj", "centrality"]
    VAR_B = [("delta_R_ll_1", "delta_R_ll_2"), 
            ("leadingj_pt", "subleadingj_pt"), 
            ("leadingj_eta", "subleadingj_eta"),
            ("leadingj_phi", "subleadingj_phi")
            ]

    TITLES = {"l_e":["E [GeV]","events / 20GeV "],
              "l_pt":["Pt [Gev]","events / 10GeV "],
              "l_eta":["#eta","events / 0.2 "],
              "l_phi":["#phi","events / 0.2 "],
              "z_pt":["Pt [Gev]","events / 10GeV "], 
              "z_eta":["#eta","events / 0.2 "],      
              "z_phi":["#phi","events / 0.2 "],      
              "z_m":["M [GeV]", "events / GeV"],
              "z_e":["E [GeV]","events / 20GeV "],   
              "leadingj_e-subleadingj_e":["E [GeV]","events / 20GeV "],   
              "leadingj_pt-subleadingj_pt":["Pt [Gev]","events / 10GeV "], 
              "leadingj_eta-subleadingj_eta":["#eta","events / 0.2 "],      
              "leadingj_phi-subleadingj_phi":["#phi","events / 0.2 "],      
              "zz_pt":["Pt [Gev]","events / 10GeV "], 
              "zz_eta":["#eta","events / 0.2 "],      
              "zz_phi":["#phi","events / 0.2 "],      
              "zz_m":["M [GeV]", "events / GeV"],
              "zz_e":["E [GeV]","events / 20GeV "],   
              "jj_pt":["Pt [Gev]","events / 10GeV "], 
              "jj_eta":["#eta","events / 0.2 "],      
              "jj_phi":["#phi","events / 0.2 "],      
              "jj_m":["M [GeV]", "events / GeV"],
              "jj_e":["E [GeV]","events / 20GeV "],   
              "delta_R_zz":["#DeltaR_{zz}", "events/ "],
              "delta_R_jj":["#DeltaR_{jj}", "events/ "],
              "delta_R_zz_jj":["#DeltaR_{zzjj}", "events/ "],
              "delta_R_ll_1-delta_R_ll_2":["#DeltaR_ll", "events/ "],
              "mcweight":["weight", "events/ "],
              "delta_eta_jj":["#Delta#eta_{jj}","events / 0.2 "],
              "centrality":["centrality","events / 0.5 "]
              }
    LEGENDS = { 
                "deltaEta_jj":"#Delta#eta",
                "centrality":"centrality"
              }

    for cut in CUT:
        for entry in VAR_4:
            for par in entry[0]:
                for var in entry[1]:

                    # load histograms
                    histos = []
                    names = []
                    var_name = par + "_" + var
                    for i in range(1,5):
                        hist_name =cut + "_" + par + str(i) + "_" + var
                        names.append(hist_name)
                        histo_tmp = fin.Get(hist_name)
                        histos.append(histo_tmp)

                    # Draw histograms
                    xtitle = TITLES[var_name][0]
                    ytitle = TITLES[var_name][1]
                    legends = ["leading_e", "subleading_e", "leading_m", "subleading_m"]
                    options = ["HIST", "HIST", "HIST", "HIST"]            
                    opt_legends = ["L", "L", "L", "L"]                    
                    marker_types = [26, 26, 32, 32]                       
                    marker_sizes = [1, 1, 1, 1]                           
                    line_sizes = [2, 2, 2, 2]                             
                    marker_colors=line_colors=fill_colors=[4, 6, 2, 12] 
                    fill_types = [0, 0, 0, 0]                             
                    figname = cut + "_" + var_name
            
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
                        hist_name = cut + "_" +par + str(i) + "_" + var
                        names.append(hist_name)
                        histo_tmp = fin.Get(hist_name)
                        histos.append(histo_tmp)

                    # Draw histograms
                    xtitle = TITLES[var_name][0]
                    ytitle = TITLES[var_name][1]
                    legends = ["z_ee", "z_vv"]
                    options = ["HIST", "HIST"]            
                    opt_legends = ["L", "L"]                    
                    marker_types = [26, 32]                       
                    marker_sizes = [1, 1]                           
                    line_sizes = [2, 2]                             
                    marker_colors=line_colors=fill_colors=[4, 6] 
                    fill_types = [0, 0]                             
                    figname = cut + "_" + var_name
            
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
                    histo_name = cut + "_" + par + "_" + var
                    names = [histo_name]
                    histos = []

                    # load histograms
                    histo_tmp = fin.Get(histo_name)
                    histos.append(histo_tmp)

                    # Draw histograms
                    xtitle = TITLES[par+"_"+var][0]
                    ytitle = TITLES[par+"_"+var][1]
                    legends = [par+"_"+var]
                    options = ["HIST"]
                    opt_legends = ["L"]
                    marker_types = [26]
                    marker_sizes = [1]
                    line_sizes = [2]
                    marker_colors=line_colors=fill_colors=[4]
                    fill_types = [0]
                    figname= histo_name
            
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
            histo_name = cut + "_" + var
            names = [var]
            histos = []

            # load histograms
            histo_tmp = fin.Get(histo_name)
            histos.append(histo_tmp)

            # Draw histograms
            xtitle = TITLES[var][0]
            ytitle = TITLES[var][1]
            legends = [var]
            options = ["HIST"]
            opt_legends = ["L"]
            marker_types = [26]
            marker_sizes = [1]
            line_sizes = [2]
            marker_colors=line_colors=fill_colors=[4]
            fill_types = [0]
            figname= histo_name

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

            histo_tmp = fin.Get(cut + "_" + var1)
            histos.append(histo_tmp)
            histo_tmp = fin.Get(cut + "_" + var2)
            histos.append(histo_tmp)

            xtitle = TITLES[var1+"-"+var2][0]
            ytitle = TITLES[var1+"-"+var2][1]
            var1.replace("1", "ee")
            var2.replace("2", "vv")
            legends = [var1, var2]
            options = ["HIST", "HIST"]            
            opt_legends = ["L", "L"]                    
            marker_types = [26, 32]                       
            marker_sizes = [1, 1]                           
            line_sizes = [2, 2]                             
            marker_colors=line_colors=fill_colors=[4, 6] 
            fill_types = [0, 0]                             
            figname = cut + "_" + var1 + "_" + var2

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
