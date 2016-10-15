#!/bin/bash

ERR_NO_ARG=1
ERR_INPUT_NUMBER=51
ERR_INPUT=52
ERR_INPUT_1=53
ERR_UNRECOGNIZED_OPTION=54
ERR_CHANNEL=55
ERR_PROCESS=56
ERR_CONVERT_ROOT=101
ERR_RAW=102
ERR_FID=103
ERR_CUT=104
ERR_TREE2HIST=105

EXECUTABLE=`basename $0`
usage()
{
    cat << END
$EXECUTABLE [option...] input[.lhe|.root]

Options:
    --help : show this help message
    --channel : specify channel, whose value can be:
	eeee, eemm, mmmm
    --process : indicate what you want to do with input file
	raw: extract raw info and draw it 
	fiducial: extract fiducial info and draw it
	cut: cut on input file and draw it 
    --cut-on-fiducial : apply cut on fiducial root file, its optional
	argument must be a fiducial root file.

Input:
    input must be a lhe or root file, more than one input file of same 
    channel at one time is supported. lhe file will be converted to 
    corresponding root file firstly. 

Workflow:
    [lhe]--> root --> fid.root --> cut_hist.root --> draw 
               |          |
	       | 	  |--> fid_hist.root --> draw
               |
            raw.root --> raw_hist.root --> draw

Example : 
1: to run raw process over 210_eemm
$EXECUTABLE --channel=eemm --process=raw 210_eemm.lhe
2: run cut-on-fiducial over 210_eemm
$EXECUTABLE --channel=eemm --run-on-fiducial=210_eemm_fid.root 
END
}

if [ $# -eq 0 ]
then
    usage
    exit $ERR_NO_ARG
fi

# parse arguments
OPTIONS=`getopt -o '' -l help,channel:,process:,cut-on-fiducial: \
    -n $EXECUTABLE --  "$@"`
if [ $? -ne 0 ]
then
    echo "Terminating..." >&2
    exit $ERR_UNRECOGNIZED_OPTION
fi

eval set -- "$OPTIONS"

CUT_ON_FIDUCIAL=()  # array for holding cut on fiducial files
while true
do
    case "$1" in
	--help)
	    usage
	    exit 0
	    ;;
	--channel)
	    CHANNEL="$2"
	    echo "you set channle as $CHANNEL"
	    shift 2
	    ;;
	--process)
	    PROCESS=$2
	    echo "your choice of process is $PROCESS"
	    shift 2
	    ;;
	--cut-on-fiducial)
	    CUT_ON_FIDUCIAL+=("$2")
	    shift 2
	    ;;
	--)
	    shift
	    break
	    ;;
	*)
	    echo "Internal error! Unrecognized option $1" >&2
	    exit $ERR_UNRECOGNIZED_OPTION
	    ;;
    esac
done

case "$CHANNEL" in
    "")
	CHANNEL="eeee" && echo "no channel is specified, default to eeee(mmmm) channel" 
	CHANNEL_PREFIX="VBFNLO_"
	;;
    eemm)
	CHANNEL_PREFIX="VBFNLO_eemm_"
	;;
    eeee|mmmm)
	CHANNEL_PREFIX="VBFNLO_"
	;;
    *)
	echo "Invalid channel names; channel must be one of: eeee, eemm, mmmm" >&2
	exit $ERR_CHANNEL
	;;
esac

case "$PROCESS" in
    "")
	echo "No process has been specified, default to raw"
	RAW=${CHANNEL_PREFIX}"Raw.cxx"
	TREE2HIST="tree2hist.cxx"
	PLOT=${CHANNEL_PREFIX}"Raw.py"
	;;
    raw|RAW)
	RAW=${CHANNEL_PREFIX}"Raw.cxx"
	TREE2HIST="tree2hist.cxx"
	PLOT=${CHANNEL_PREFIX}"Raw.py"
	;;
    fid*|FID*)
	FIDUCIAL=${CHANNEL_PREFIX}"Fiducial.cxx"
	TREE2HIST="tree2hist.cxx"
	PLOT=${CHANNEL_PREFIX}"Fid.py"
	;;
    cut|CUT)
	FIDUCIAL=${CHANNEL_PREFIX}"Fiducial.cxx"
	CUT=${CHANNEL_PREFIX}"Cut.cxx"
	PLOT=${CHANNEL_PREFIX}"Cut.py"
	;;
    cut-on-fid*)
	CUT=${CHANNEL_PREFIX}"Cut.cxx"
	PLOT=${CHANNEL_PREFIX}"Cut.py"
	;;
    *)
	echo "Invalid argument for process; it must be one of the following:
	raw	fid[ucial]  cut	    cut-on-fid[ucial] " >&2
	exit $ERR_PROCESS
	;;
esac

# cut on fiducial
for index in ${!CUT_ON_FIDUCIAL[*]}
do
    echo "deal with file ${CUT_ON_FIDUCIAL[index]}"
    CUT_1=${CHANNEL_PREFIX}"Cut.cxx"
    PLOT_1=${CHANNEL_PREFIX}"Cut.py"

    # check file
    if [[ "${CUT_ON_FIDUCIAL[index]}" != *.root ]]
    then
	echo "Input for cut on fiducial must be a root file end with .root" >&2
	exit $ERR_INPUT_1
    fi
    PREFIX_1=${CUT_ON_FIDUCIAL[index]%.root}

    # cut on root file
    echo 
    echo "==================================="
    echo "cut on ${PREFIX_1}.root"
    root -l << END
.L $CUT_1
cut("${CUT_ON_FIDUCIAL[0]}", "${PREFIX_1}_cut_hist.root")
.q
END
    if [ -e "${PREFIX_1}_cut_hist.root" ]
    then
	echo "cut successfully"
    else
	echo "unable to cut on ${PREFIX_1}.root" >&2
	exit $ERR_CUT
    fi

    # draw histograms
    source oneplot_setup.sh
    sed -e "/files=/s/\".*\"/\"${PREFIX_1}_cut_hist.root\"/" $PLOT_1 >VBFNLO_plot.py
    chmod +x VBFNLO_plot.py
    echo 
    echo "==================================="
    echo "draw histograms from ${PREFIX_1}_cut_hist.root"
    ./VBFNLO_plot.py -b
    echo "end of drawing"

    rm ${PREFIX_1}_*.root
    rm VBFNLO_plot.py
done
unset CUT_ON_FIDUCIAL


[ $# -eq 0 ] && echo "at least one input file needed. " >&2 && exit $ERR_NO_FILE

while [ $# -gt 0 ]
do
    echo "deal with file ${1}"
    # check input file
    if [ "${1: -4}" == ".lhe" ]
    then
	echo "Input is a LHE file, convert it to root file firstly, and the root file will be kept"
	PREFIX=${1%.lhe}

	echo "====================================="
	echo "convert ${1}.lhe to root."
	./convert.py ${1} vbfnlo $PREFIX".root" 
	if [ $? -eq 0 ]
	then
	    echo "end of convertion"
	else
	    echo "convert fails, please check your input lhe file." >&2
	    rm $PREFIX".root"
	    exit $ERR_CONVERT_ROOT
	fi
    elif [ "${1: -5}" == ".root" ]
    then
	echo "Input is a root file"
	PREFIX=${1%.root}
    else 
	echo "Wrong input, it must be a lhe file or a root file" >&2
	exit $ERR_INPUT
    fi

    # raw
    if [ -n "$RAW" ]
    then
	echo
	echo "===================================="
	echo "extract raw info from ${PREFIX}.root."
	root -l << END
.L $RAW
raw("${PREFIX}.root", "${PREFIX}_raw.root")
.q
END
	if [ -e "${PREFIX}_raw.root" ]
	then
	    echo "end of extraction."
	    TREEINPUT="${PREFIX}_raw.root"
	    TREEOUTPUT="${PREFIX}_raw_hist.root"
	    PLOT_HIST="$TREEOUTPUT"
	else
	    echo "unable to extract raw data from input root file" >&2
	    exit $ERR_RAW
	fi
    fi

    # fiducial
    if [ -n "$FIDUCIAL" ]
    then
	echo 
	echo "==================================="
	echo "extract fiducial info from ${PREFIX}.root."
	root -l << END
.L $FIDUCIAL
fiducial("${PREFIX}.root", "${PREFIX}_fid.root")
.q
END
	if [ -e "${PREFIX}_fid.root" ]
	then
	    echo "end of extraction"
	    TREEINPUT="${PREFIX}_fid.root"
	    TREEOUTPUT="${PREFIX}_fid_hist.root"
	    PLOT_HIST="$TREEOUTPUT"
	else
	    echo "unable to extract fiducial from input root file" >&2
	    exit $ERR_FID
	fi
    fi

    # cut
    if [ -n "$CUT" ]
    then
	echo 
	echo "==================================="
	echo "cut on fiducial root file: ${PREFIX}_fid.root"
        root -l << END
.L $CUT
cut("${PREFIX}_fid.root", "${PREFIX}_cut_hist.root")
.q
END
	if [ -e "${PREFIX}_cut_hist.root" ]
	then
	    echo "cut successfully"
	    PLOT_HIST="${PREFIX}_cut_hist.root"
	else
	    echo "unable to cut on ${PREFIX}_fid.root" >&2
	    exit $ERR_CUT
	fi
    fi

    # tree2hist
    if [ -n "$TREE2HIST" ]
    then
	echo 
	echo "=================================="
	echo "transform tree to hist with file $TREE2HIST"
	root -l << END
.L tree2hist.cxx
Tree2Hist("$TREEINPUT", "$TREEOUTPUT")
.q
END
	if [ -e "$TREEOUTPUT" ]
	then
	    echo "end of transformation."
	else
	    echo "unable to convert tree to hist" >&2
	    exit $ERR_TREE2HIST
	fi
    fi

    # draw plot
    source oneplot_setup.sh
    sed -e "/files=/s/\".*\"/\"$PLOT_HIST\"/" $PLOT >VBFNLO_plot.py
    chmod +x VBFNLO_plot.py
    echo 
    echo "==================================="
    echo "draw histograms from file $PLOT_HIST"
    ./VBFNLO_plot.py -b
    echo "end of drawing"

    rm ${PREFIX}_*.root
    rm VBFNLO_plot.py

    shift
done
exit 0
