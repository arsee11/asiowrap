CC=g++
CXXFLAGS+= -g --std=c++11
TARGET=tcptest udptest tcpclient netpool timer mcaster
INC_DIR=-I/root/work/lib

LINKS= -lboost_system -L../ -lasiowrap -lpthread


all:$(TARGET)

tcptest: tcp_test.cpp 
	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

udptest: udp_test.cpp
	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

tcpclient: tcpclient_test.cpp
	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

netpool: networkpool_test.cpp
	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

timer: timer_test.cpp
	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

mcaster: mcast_test.cpp
	$(CC) $(INC_DIR) -o $@ $(CXXFLAGS) $<  $(LINKS)

.PHONY:clean
clean:
	rm -rf $(TARGET) *.o
