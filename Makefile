CC=gcc
CFLAGS = -I$(INCLUDES_DIR)

INCLUDES_DIR = include
BUILD_DIR = build
SRC_DIR = src

all: start usb clean
	@echo Build completed!

start:
	@echo Building binary file

usb: start usb.o drive.o scanner.o
	@$(CC) $(CFLAGS) usb.o drive.o scanner.o -o $(BUILD_DIR)\usb

usb.o: src\usb.c
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\usb.c

drive.o: src\drive.c
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\drive.c

scanner.o: src\scanner.c
	@$(CC) $(CFLAGS) -c $(SRC_DIR)\scanner.c

clean:
	@echo Cleaning up everything
	@del *.o