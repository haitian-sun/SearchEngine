all: totalpages wordpagecount normalweight combineindex printoutput

totalpages:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin totalPages.java filter.java XmlInputFormat.java
	jar -cvf ./totalPages.jar -C bin/ .

wordpagecount:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin wordPageCount.java filter.java XmlInputFormat.java
	jar -cvf ./wordPageCount.jar -C bin/ .

normalweight:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin normalWeight.java
	jar -cvf ./normalWeight.jar -C bin/ .	

combineindex:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin combineIndex.java
	jar -cvf ./combineIndex.jar -C bin/ .

printoutput:
	javac -cp .:../lib/* -d bin printOutput.java
	jar -cvf ./printOutput.jar -C bin/ .	

xom:
	javac -cp .:../hadoop-core-1.0.1.jar:../lib/* -d bin XOMtest.java
	java -Xms1024m -Xmx1024m -cp .:../lib/*:bin/ edu.umich.cse.eecs485.XOMtest

clean:
	rm -rf *.jar bin/* output
