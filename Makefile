CC=gcc
CFLAGS = -I$(INCLUDES_DIR)

INCLUDES_DIR = include
BUILD_DIR = build
SRC_DIR = src

all: usb clean
	@echo Build completed!

usb: usb.o drive.o scanner.o
	@echo Building binary file
	@$(CC) $(CFLAGS) usb.o drive.o scanner.o -o usb

usb.o: src\usb.c
	@echo Compiling usb.c file
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\usb.c

drive.o: src\drive.c
	@echo Compiling drive.c file
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\drive.c

scanner.o: src\scanner.c
	@echo Compiling scanner.c file
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\scanner.c

clean:
	@echo Cleaning up everything
	@del *.o