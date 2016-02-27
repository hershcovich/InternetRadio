  # the compiler: gcc for C program, define as g++ for C++
  CC = gcc

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  CFLAGS  = -g -Wall -m32 -pthread

  # the build target executable:
  TARGET = radio_listener
  TARGET1 = Listener/main
  TARGET2 = Listener/ConectionManage
  TARGET3 = radio_control
  TARGET4 = Control/main
  TARGET5 = Control/ConectionManage
  TARGET6 = radio_server
  TARGET7 = Server/main
  TARGET8 = Server/TCPConectionManage
  TARGET9 = Server/UDPConectionManage
	
  all: $(TARGET) $(TARGET3) $(TARGET6)

  $(TARGET): $(TARGET1).c $(TARGET2).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET1).c $(TARGET2).c


  $(TARGET3): $(TARGET4).c $(TARGET4).c
	$(CC) $(CFLAGS) -o $(TARGET3) $(TARGET4).c $(TARGET5).c

  $(TARGET6): $(TARGET7).c $(TARGET8).c $(TARGET9).c
	$(CC) $(CFLAGS) -o $(TARGET6) $(TARGET7).c $(TARGET8).c $(TARGET9).c

  clean:
	$(RM) $(TARGET) $(TARGET3) $(TARGET6)
