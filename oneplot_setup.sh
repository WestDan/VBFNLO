#Set up ROOT path for python plotting scripts
# Should be a valid ROOT version with PyROOT setup
export ROOTLIB='/home/zhangweibin/bin/root/lib' # change the ROOTLIB value to your system one

# Path for Python Modules
# This is set automatically
export PATH_OnePlot_MODULE1="/home/zhangweibin/Thesis/Code/VBS_ZZ/Tools/MC/PLOT/OnePlot"    # change the path to where oneplot lies in your system
sed -in '/self._draw_text/s/".*"/"VBFNLO"/' $PATH_OnePlot_MODULE1/oneplot.py
export PATH_OnePlot_MODULE2=$PATH_OnePlot_MODULE1"/modules/"

# Path for atlas plotting style
export PATH_AtlasStyle=${PATH_OnePlot_MODULE1}"/atlas-style/"
