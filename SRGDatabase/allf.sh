#!/bin/bash

for filename in *.g6.txt; do
	./../SRG/AllFind "$filename"
done
