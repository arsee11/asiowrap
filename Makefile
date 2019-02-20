CC=g++
CXXFLAGS+= -g -std=c++11 
TARGET=../libs/libasiowrap.a

CUR_SOURCE=${wildcard *.cpp}
CUR_OBJS=${patsubst %.cpp, %.o, $(CUR_SOURCE)}

TARGET:$(CUR_OBJS)
	ar -rv $(TARGET) $(CUR_OBJS)

 
$(CUR_OBJS): %.o: %.cpp
	$(CC) $(INC_DIR) $(CXXFLAGS) -c $^ -o $@


.PHONY:clean
clean:
	rm -rf $(TARGET) $(CUR_OBJS)
