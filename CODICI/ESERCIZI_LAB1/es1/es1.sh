#!/bin/bash
DATA=cartellaProva;
( [[ -e $DATA ]] && echo "T" && ([[ -f $DATA ]] && echo "file " && expr $(cat $DATA)) || ([[ -d $DATA ]] && echo "cartella") ) || echo "F"