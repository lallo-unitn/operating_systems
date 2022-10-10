#!/usr/bin/env
alias linecount='read -p "Inserisci il path: " && ([[ -f $REPLY ]] && (wc -l $REPLY) || (echo "Path non valido" ))';
