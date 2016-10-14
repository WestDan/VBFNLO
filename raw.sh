#!/bin/bash

ERR_WRONG_INPUT_NUMBER=51
ERR_WRONG_INPUT=52
ERR_CONVERT_ROOT=101
ERR_RAW=102
ERR_TREE2HIST=103

# parse arguments
getopt $@
# check input file
if [ $# -ne 1 ] 
then
    echo "one input lhe file needed, please retry." >&2
    exit $ERR_WRONG_INPUT_NUMBER
elif [ ! "${1: -4}" == ".lhe" ]
then
    echo "input argument must be a lhe file end with .lhe, please retry. " >&2
    exit $ERR_WRONG_INPUT
else
    LHE="$1"
    PREFIX=${LHE%.lhe}
fi

echo "====================================="
echo "convert lhe file to root."
./convert.py $LHE vbfnlo $PREFIX".root" 
if [ $? -eq 0 ]
then
    echo "end of convertion"
else
    echo "convert fails, please check your input lhe file." >&2
    rm $PREFIX".root"
    exit $ERR_CONVERT_ROOT
fi

echo
echo "===================================="
echo "extract raw info."
root -l << END
.L VBFNLO_Raw.cxx
raw("${PREFIX}.root", "${PREFIX}_raw.root")
.q
END
if [ -e $PREFIX"_raw.root" ]
then
    echo "end of extraction."
else
    echo "unable to extract raw data from input root file" >&2
    exit $ERR_RAW
fi

echo 
echo "==================================="
echo "convert to histograms from root tree."
root -l << END
.L tree2hist.cxx
Tree2Hist("${PREFIX}_raw.root", "${PREFIX}_raw_hist.root")
.q
END
if [ -e $PREFIX"_raw._hist.root" ]
then
    echo "end of transformation."
else
    echo "unable to convert tree to hist" >&2
    exit $ERR_TREE2HIST
fi

# setup oneplot environment
source oneplot_setup.sh
sed -e "/files=/s/\".*\"/\"${PREFIX}_raw_hist.root\"/" VBFNLO_Raw.py >VBFNLO_Raw_1.py
chmod +x VBFNLO_Raw_1.py
echo 
echo "==================================="
echo "draw histograms"
./VBFNLO_Raw_1.py -b
echo "end of drawing"

rm ${PREFIX}_raw*.root
rm VBFNLO_Raw_1.py
