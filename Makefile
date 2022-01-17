CC = g++
TARGET = echo_server echo_client
LDLIBS = -lpthread

all: $(TARGET)
