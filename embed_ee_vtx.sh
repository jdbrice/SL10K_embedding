#!/bin/tcsh

set nEvts=$1
#set nEvts=5
echo "Running on $nEvts Events"
echo "#################### STARSIM #######################"
starver SL16d
root4star -b -q -l 'starsim.C('"$nEvts"')' >& starsim.log

echo "#################### Embedding BFC Chain #######################"
starver SL10k_embed
echo $STAR
root4star -b > bfcMixer.log <<EEE
  .L bfcVtx.C
  bfcMixer_Tpx($nEvts,"$SCRATCH/st_zerobias.daq","", 1, 5, -0.1, 0.1, -10, 10, 10, 8, 2, 0,
               "P10ikAuAu200", "FlatPt", 1, "pythia6.starsim.fzd");
  .q
EEE
