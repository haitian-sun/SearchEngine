#!/bin/bash

export JAVA_HOME=/usr

if [ -d "invoutput" ]; then
    rm -r invoutput
fi
mkdir invoutput
cd hadoop-example/mysrc
../bin/hadoop jar totalPages.jar edu.umich.cse.eecs485.totalPages ../dataset/mining.articles.xml total
cat total/* > total.txt
../bin/hadoop jar wordPageCount.jar edu.umich.cse.eecs485.wordPageCount ../dataset/mining.articles.xml wordpage
../bin/hadoop jar normalWeight.jar edu.umich.cse.eecs485.normalWeight ./wordpage/part-r-00000 normal
../bin/hadoop jar combineIndex.jar edu.umich.cse.eecs485.combineIndex ./normal/part-r-00000 combine
../bin/hadoop jar printOutput.jar edu.umich.cse.eecs485.printOutput ./combine/part-r-00000 > ../../invoutput/invIndex.txt
rm total.txt
rm -r total
rm -r wordpage
rm -r normal
rm -r combine