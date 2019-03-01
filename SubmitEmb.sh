#!/bin/bash

prod="ee_btof_starlight_02"

#list of daq files
daqlist="daqlist_nevt.txt"

#first job and number of jobs, at most the number of daq files
start=0
njobs=500

#base directory for production macros and jobs working directory
basedir="/star/u/jdb/work/upc_dielectron/embed/"
jobdir="/star/data03/pwg/jdb/upc/output/$prod"

#macros and StarlightSelector
bfc="bfcMixer_Tpx.C"
xml="submit.xml"
script="embed_ee.sh"
ss="starsim_gammagamma.C"

#-- end of configuration --


#append production name to base and job directories
basedir=$basedir"/"
jobdir=$jobdir"/"

#loop over jobs
ijob=0
ntot=0
while read -r daqline
do
  if [ $ijob -ge $njobs ]
  then
    break
  fi

  echo "Submitting job "$ijob

  #daq input and number of events
  daqname=$(echo $daqline | awk '{print $1}')
  nevt=$(echo $daqline | awk '{print $2}')
  # nevt=10
  echo $daqname
  echo "Number of events: "$nevt

  #create directory for current job
  iform=`printf %04d $ijob`
  cjob=$jobdir"/"$iform
  mkdir -p $cjob
  echo "iform: $iform"
  echo "cjob: $cjob"
  

  #scheduler output
  sched=$cjob"/sched"
  mkdir -p $sched

  #job outputs
  outfiles=$cjob"/out"
  mkdir -p $outfiles

  #copy bfc macro and put number of events
  #cat $bfc | sed "s/__NEVT__/$nevt/g" > $cjob"/"$bfc
  cp $bfc $cjob"/"$bfc
  cp $script $cjob"/"$script
  #copy starsim configuration
  cp $ss $cjob"/"$ss

  #copy scheduler xml file and make substitutions
  cat $xml | sed "s/__NEVT__/$nevt/g"\
           | sed "s%__DAQ__%$daqname%g"\
           | sed "s%__OUT__%$outfiles%g"\
           | sed "s%__CJOB__%$cjob%g"\
           | sed "s%__GEOM__%$geom%g"\
           | sed "s%__SCHED__%$sched%g"\
           | sed "s%__SCRIPT__%$script%g" > $cjob"/"$xml

  #submit the job
  cd $cjob
  echo "star-submit $xml > submit.log 2>&1"
  star-submit $xml > submit.log 2>&1
  cd $basedir

  #increment counters for number of events and jobs
  ((ntot += nevt))
  ((ijob++))

  sleep 0.1
done < "$daqlist"

echo "Total number of events: "$ntot


