CC = g++
CXXFLAGS = -g -std=c++17
INCLUDE =
LIBDIR =
LIB = -lboost_thread -lboost_system -lmariadb -lz -ldl -lm -lpthread -lssl -lcrypto
TARGET = server
SRCS = test.cc game.cc shape.cc common.cc db.cc service.cc websocket.cc socket.cc

$(TARGET): $(SRCS:.cc=.o)
	$(CC) $(CXXFLAGS) $(LIBDIR) $(LIB) -o $@ $^
	-rm -f *.o *.d

test: test.cc common.cc socket.cc websocket.cc service.cc
	$(CC) $(FLAG) $(LIB) -o $@ $^

clt: client.cc socket.cc
	$(CC) $(CXXFLAGS) $(LIBDIR) $(LIB) -o $@ $^
	-rm -f *.o *.d

clean:
	-rm -f *.o *.d
