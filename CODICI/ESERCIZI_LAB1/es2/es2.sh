#!/bin/bash
count=0;
lista=();
narg=$#;
while [[ $count < $narg ]]; do
	lista+=($1)
	let count=count+1
	shift
done
while [[ $count > 0 ]]; do
	let count=count-1
	echo ${lista[count]}
done

