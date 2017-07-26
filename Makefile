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
CFLAGS= -lc -shared -fPIC -Wall
agent: setup
	$(CC) $(CFLAGS) -o target/ldsagent.so -I $(JAVA_HOME)/include/ -I $(JAVA_HOME)/include/$(OS)/ c/*.c

stubs: setup jar
	$(JAVA_HOME)/bin/javac -h target/ -d target/ java/is/jcdav/darkseer/*

demo: setup jar agent
	$(JAVA_HOME)/bin/javac -cp target/ -d target test/*
	$(JAVA_HOME)/bin/java -cp target/ -agentpath:target/ldsagent.so Demo

#Uses whatever your path scala is. Override with make SCALA=/path/to/scala scalarepl
SCALA=$(shell which scala)
scalarepl: jar agent
	$(SCALA) -cp target/ -J-agentpath:target/ldsagent.so

jshell: jar agent
	$(JAVA_HOME)/bin/jshell -class-path target/ -R-agentpath:target/ldsagent.so
