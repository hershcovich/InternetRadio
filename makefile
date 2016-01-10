  # the compiler: gcc for C program, define as g++ for C++
  CC = gcc

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  CFLAGS  = -g -Wall -m32

  # the build target executable:
  TARGET = radio_listener
  TARGET1 = Listener/main
  TARGET2 = Listener/ConectionManage
  TARGET3 = radio_control
  TARGET4 = Control/main
  TARGET5 = Control/ConectionManage

  all: $(TARGET) $(TARGET3)

  $(TARGET): $(TARGET1).c $(TARGET2).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET1).c $(TARGET2).c


  $(TARGET3): $(TARGET4).c $(TARGET4).c
	$(CC) $(CFLAGS) -o $(TARGET3) $(TARGET4).c $(TARGET5).c

  clean:
	$(RM) $(TARGET)
