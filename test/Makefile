CC=g++
CXXFLAGS+= -g --std=c++11
TARGET=udptest 

LINKS= -lboost_system -L../build -lasiow -lpthread


all:$(TARGET)

#tcptest: tcp_test.cpp 
#	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

udptest: udp_test.cpp
	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

#tcpclient: tcpclient_test.cpp
#	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

#netpool: networkpool_test.cpp
#	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

#timer: timer_test.cpp
#	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

#mcaster: mcast_test.cpp
#	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

.PHONY:clean
clean:
	rm -rf $(TARGET) *.o
