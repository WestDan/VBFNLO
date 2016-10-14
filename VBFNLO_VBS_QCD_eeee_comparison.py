#!/usr/bin/python

##################################
# make plots for process 210_eeee (VBS) and 3210_eeee (QCD)
# generalized to llll final states
##################################

# import Python system modules
import os
import sys
import glob
import logging
import array
from math import sqrt, fabs, sin, pow

# ROOT paths configuration
# make sure to run 'source setup.sh' beforehand
ROOTLIB, PATH_OnePlot_MODULE1, PATH_OnePlot_MODULE2 = "", "", ""

if os.getenv("ROOTLIB"):
    ROOTLIB = os.getenv("ROOTLIB")
if os.getenv("PATH_OnePlot_MODULE1"):
    PATH_OnePlot_MODULE1 = os.getenv("PATH_OnePlot_MODULE1")
if os.getenv("PATH_OnePlot_MODULE2"):
    PATH_OnePlot_MODULE2 = os.getenv("PATH_OnePlot_MODULE2")

sys.path.append(ROOTLIB)
sys.path.append(PATH_OnePlot_MODULE1)
sys.path.append(PATH_OnePlot_MODULE2)

# import ROOT modules
from ROOT import TFile, TTree, TChain, TBranch, TH1F, TH2F
from ROOT import TCanvas, TPad, TLine
from ROOT import TLorentzVector
from ROOT import gROOT, gDirectory, gStyle
from ROOT import TPaveText, TLegend
import ROOT

# import user's modules
import numpy as np
from oneplot import oneplot
from module_syst import systPDFCT10, systEnvelope, syst1v1, systCombine
from module_style import atlas_style

if __name__ == "__main__":

    # read the root files
    file1 = "210_eeee_cut.root"  # VBS
    file2 = "3210_eeee_cut.root"  # QCD
    fin1 = TFile(file1)
    fin2 = TFile(file2)

    CUT = ["none", "lepEta", "lepPt", "z1", "z2", "numJet"]

    # common variables in VBS and QCD
    VAR_1 = ["pt", "eta", "phi"]  # for leptons
    VAR_2 = ["pt", "eta", "phi", "m"]  # for bosons
    VAR_3 = ["zz_m", "jj_m"]

    AXES_TITLES = {
        "pt": ["Pt [Gev]", "events / 10GeV normalized"],
        "eta": ["#eta", "events / 0.35 normalized"],
        "phi": ["#phi", "events / 0.35 5normalized"],
        "pt_Z": ["Pt_{Z} [GeV]", "events / 20GeV normalized"],
        "eta_Z": ["#eta_{Z}", "events / 0.35 normalized"],
        "phi_Z": ["#phi_{Z}", "events / 0.35 normalized"],
        "m_Z": ["M_{Z} [GeV]", "events / 4GeV normalized"],
        "zz_m": ["M_{ZZ} [GeV]", "events / 10GeV normalized"],
        "jj_m": ["M_{jj} [GeV]", "events / 40GeV normalized"],
    }

    # specifically for VAR_3
    LEGENDS = {
        "zz_m_VBS": "VBS ZZjj",
        "jj_m_VBS": "VBS ZZjj",
        "zz_m_QCD": "QCD ZZjj",
        "jj_m_QCD": "QCD ZZjj"
    }

    # binning setting of histograms referred from VBFNLO_CUT.cxx
    BINNING = {
        "pt": np.arange(0, 350, 10).tolist(),
        "eta": np.arange(-3.5, 3.5, 7. / 20).tolist(),
        "phi": np.arange(-3.5, 3.5, 7. / 20).tolist(),
        "pt_Z": np.arange(0, 500, 10).tolist(),
        "eta_Z": np.arange(-5., 5., 10. / 20).tolist(),
        "phi_Z": np.arange(-3.5, 3.5, 7. / 20).tolist(),
        "m_Z": np.arange(0, 200, 4).tolist(),
        "zz_m": np.arange(0, 500, 10).tolist(),
        "jj_m": np.arange(0, 2000, 40).tolist(),
    }

    for cut in CUT:

        for var in VAR_1:
            for i in range(1, 5):
                # load histograms
                histos = []
                names = []
                var_name = "l" + str(i) + "_" + var
                names.append(var_name)
                names.append(var_name + "_QCD")
                hist_name = cut + "_" + var_name
                histo_tmp1 = fin1.Get(hist_name)
                # normalize the histogram
                histo_tmp1.Scale(1. / histo_tmp1.Integral(0, -1))
                # set the title of the output png file
                histo_tmp1.SetTitle(
                    "cut on " + cut + " with respect to " + var + " in eeee channel")
                histos.append(histo_tmp1)
                histo_tmp2 = fin2.Get(hist_name)
                histo_tmp2.Scale(1. / histo_tmp2.Integral(0, -1))
                histos.append(histo_tmp2)

                # draw histograms
                xtitle = AXES_TITLES[var][0]
                ytitle = AXES_TITLES[var][1]
                legends = ["l" + str(i) + " VBS ZZjj",
                           "l" + str(i) + " QCD ZZjj"]
                options = ["HIST", "HIST"]
                opt_legends = ["L", "L"]
                marker_types = [26, 32]
                marker_sizes = [1, 3]
                line_sizes = [2, 2]
                marker_colors = line_colors = fill_colors = [30, 38]
                fill_types = [0, 0]
                # specify the output png file name
                figname = cut + "_l" + str(i) + "_" + var + "_normalized"

                # call oneplot.py for customized plotting
                theone = oneplot()
                theone._figtype = ["png"]
                theone.initialize(list_histo=histos, names=names, legends=legends, opt_legends=opt_legends,
                                  xtitle=xtitle, ytitle=ytitle,
                                  figname=figname,
                                  binning=BINNING[var],
                                  options=options, marker_types=marker_types, marker_colors=marker_colors, marker_sizes=marker_sizes,
                                  line_colors=line_colors, line_sizes=line_sizes,
                                  fill_colors=fill_colors, fill_types=fill_types)
                theone.plot1DHistogram()
                theone.finish()

        for var in VAR_2:
            for i in range(1, 3):
                # load histograms
                histos = []
                names = []
                var_name = "z" + str(i) + "_" + var
                names.append(var_name)
                names.append(var_name + "_QCD")
                hist_name = cut + "_" + var_name
                histo_tmp1 = fin1.Get(hist_name)
                # normalize the histogram
                histo_tmp1.Scale(1. / histo_tmp1.Integral(0, -1))
                histo_tmp1.SetTitle(
                    "cut on " + cut + " with respect to " + var + " in eeee channel")
                histos.append(histo_tmp1)
                histo_tmp2 = fin2.Get(hist_name)
                histo_tmp2.Scale(1. / histo_tmp2.Integral(0, -1))
                histos.append(histo_tmp2)

                # draw histograms
                xtitle = AXES_TITLES[var + "_Z"][0]
                ytitle = AXES_TITLES[var + "_Z"][1]
                legends = ["Z" + str(i) + " VBS ZZjj",
                           "Z" + str(i) + " QCD ZZjj"]
                options = ["HIST", "HIST"]
                opt_legends = ["L", "L"]
                marker_types = [26, 32]
                marker_sizes = [1, 3]
                line_sizes = [2, 2]
                marker_colors = line_colors = fill_colors = [30, 38]
                fill_types = [0, 0]
                # set the title of the output png file
                figname = cut + "_Z" + str(i) + "_" + var + "_normalized"

                # call oneplot.py for customized plotting
                theone = oneplot()
                theone._figtype = ["png"]
                theone.initialize(list_histo=histos, names=names, legends=legends, opt_legends=opt_legends,
                                  xtitle=xtitle, ytitle=ytitle,
                                  figname=figname,
                                  binning=BINNING[var + "_Z"],
                                  options=options, marker_types=marker_types, marker_colors=marker_colors, marker_sizes=marker_sizes,
                                  line_colors=line_colors, line_sizes=line_sizes,
                                  fill_colors=fill_colors, fill_types=fill_types)
                theone.plot1DHistogram()
                theone.finish()

        for var in VAR_3:
            # load histograms
            names = []
            histos = []
            names.append(var)
            names.append(var + "_QCD")
            hist_name = cut + "_" + var
            histo_tmp1 = fin1.Get(hist_name)
            # normalize the histogram
            histo_tmp1.Scale(1. / histo_tmp1.Integral(0, -1))
            # set the title of histogram
            histo_tmp1.SetTitle(
                "cut on " + cut + " with respect to " + var + " in eeee channel")
            histos.append(histo_tmp1)
            histo_tmp2 = fin2.Get(hist_name)
            histo_tmp2.Scale(1. / histo_tmp2.Integral(0, -1))
            histos.append(histo_tmp2)

            # draw histograms
            xtitle = AXES_TITLES[var][0]
            ytitle = AXES_TITLES[var][1]
            legends = [LEGENDS[var + "_VBS"], LEGENDS[var + "_QCD"]]
            options = ["HIST", "HIST"]
            opt_legends = ["L", "L"]
            marker_types = [26, 32]
            marker_sizes = [1, 3]
            line_sizes = [2, 2]
            marker_colors = line_colors = fill_colors = [30, 38]
            fill_types = [0, 0]
            # set the title of the output png file
            figname = cut + "_" + var + "_normalized"

            # call oneplot.py for customized plotting
            theone = oneplot()
            theone._figtype = ["png"]
            theone.initialize(list_histo=histos, names=names, legends=legends, opt_legends=opt_legends,
                              xtitle=xtitle, ytitle=ytitle,
                              figname=figname,
                              binning=BINNING[var],
                              options=options, marker_types=marker_types, marker_colors=marker_colors, marker_sizes=marker_sizes,
                              line_colors=line_colors, line_sizes=line_sizes,
                              fill_colors=fill_colors, fill_types=fill_types)
            newline = TLine(0.2, 0.2, 0.4, 0.4)
            newline.Draw()
            theone.plot1DHistogram()
            theone.finish()

fin1.Close()
fin2.Close()
