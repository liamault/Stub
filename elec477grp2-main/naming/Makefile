CXXFLAGS=-std=c++2a -DDEBUG

#CLIENT_OBJS=testClient.o svcDirClient.o HexDump.o
CLIENT_OBJS=svcDirClient.o HexDump.o
EXECS=bin/testclient bin/testclientregister bin/testclientsearch bin/testclientreset bin/testclientremove
all: $(EXECS)

bin/testclient: testClient.o $(CLIENT_OBJS)
	c++ -o bin/testclient testClient.o $(CLIENT_OBJS)
bin/testclientregister: testClientRegister.o $(CLIENT_OBJS)
	c++ -o bin/testclientregister testClientRegister.o $(CLIENT_OBJS)
bin/testclientsearch: testClientSearch.o $(CLIENT_OBJS)
	c++ -o bin/testclientsearch testClientSearch.o $(CLIENT_OBJS)
bin/testclientreset: testClientReset.o $(CLIENT_OBJS)
	c++ -o bin/testclientreset testClientReset.o $(CLIENT_OBJS)
bin/testclientremove: testClientRemove.o $(CLIENT_OBJS)
	c++ -o bin/testclientremove testClientRemove.o $(CLIENT_OBJS)

svcDirClient.o: svcDirClient.hpp HexDump.hpp
testClient.o: svcDirClient.hpp HexDump.hpp
testClientRegister.o: svcDirClient.hpp HexDump.hpp
testClientSearch.o: svcDirClient.hpp HexDump.hpp
testClientReset.o: svcDirClient.hpp HexDump.hpp
testClientRemove.o: svcDirClient.hpp HexDump.hpp

clean:
	-rm *.o $(EXECS)
