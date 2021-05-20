#!/bin/bash

for filename in *.g6.txt; do
	./../SRG/RegFind "$filename"
done
