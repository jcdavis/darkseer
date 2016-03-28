all: jar agent

setup:
ifndef JAVA_HOME
	echo "You need to set JAVA_HOME"
	false
endif
	mkdir -p target

clean: 
	rm -rf target

jar: setup
	$(JAVA_HOME)/bin/javac -d target/ java/is/jcdav/darkseer/*
	$(JAVA_HOME)/bin/jar cvf target/darkseer.jar target/is/jcdav/darkseer/*.class

OS=$(shell uname -s | tr '[:upper:]' '[:lower:]')
agent: setup
	gcc -o target/ldsagent.so -lc -shared -fPIC -I $(JAVA_HOME)/include/ -I $(JAVA_HOME)/include/$(OS)/ c/*.c

stubs: setup jar
	$(JAVA_HOME)/bin/javah -o target/stubs.c -stubs -cp target/ is.jcdav.darkseer.DarkSeer

demo: setup jar agent
	$(JAVA_HOME)/bin/javac -cp target/ -d target test/*
	$(JAVA_HOME)/bin/java -cp target/ -agentpath:target/ldsagent.so Demo		
