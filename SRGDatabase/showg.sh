#!/bin/bash

for filename in *.g6; do
	./util/showg -A "$filename" "$filename".txt
done
