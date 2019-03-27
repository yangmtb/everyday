CC = g++
FLAG = -g -std=c++17
INCLUDE =
LIBDIR =
LIB = -lboost_thread -lboost_system -lmariadb -lz -ldl -lm -lpthread -lssl -lcrypto
TARGET = server
SRCS = main.cc game.cc shape.cc common.cc debug_log.cc network_interface.cc websocket_handler.cc websocket_request.cc websocket_respond.cc db.cc

$(TARGET): $(SRCS:.cc=.o)
	$(CC) $(FLAG) $(LIBDIR) $(LIB) -o $@ $^
	-rm -f *.o *.d

test: test.cc common.cc socket.cc websocket.cc service.cc
	$(CC) $(FLAG) $(LIB) -o $@ $^

clean:
	-rm -f *.o *.d
