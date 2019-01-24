root4star -b <<EEE
  .L bfcMixer_Tpx.C
  bfcMixer_Tpx(200,"/star/data03/pwg/jdb/upc/zerobias/st_zerobias_adc_11057034_raw_1590001.daq","st_zerobias_adc_11057034_raw_1590001.tags.root", 1, 5, -0.1, 0.1, -10, 10, 10, 8, 2, 0,
               "P10ikAuAu200", "FlatPt", 1, "output.fzd");
.q 
EEE
