  # the compiler: gcc for C program, define as g++ for C++
  CC = gcc

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  CFLAGS  = -g -Wall -m32 -pthread

  # the build target executable:
  TARGET = radio_server
  TARGET1 = main
  TARGET2 = TCPConectionManage
  TARGET3 = UDPConectionManage

all: $(TARGET)

$(TARGET): $(TARGET1).c $(TARGET2).c  $(TARGET3).c 
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET1).c $(TARGET2).c $(TARGET3).c 

clean:
	$(RM) $(TARGET)
