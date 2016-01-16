  # the compiler: gcc for C program, define as g++ for C++
  CC = gcc

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  CFLAGS  = -g -Wall -m32

  # the build target executable:
  TARGET = radio_control
TARGET1 = main
TARGET2 = ConectionManage

all: $(TARGET)

$(TARGET): $(TARGET1).c $(TARGET2).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET1).c $(TARGET2).c

clean:
	$(RM) $(TARGET)
