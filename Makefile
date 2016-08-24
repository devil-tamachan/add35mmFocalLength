SRC = exif.c sqlite3.c sample_main.c
SRC2= exif.c sqlite3.c initdb.c
SRC3= exif.c sqlite3.c add35mmFocalLength.c
OBJ = $(SRC:.c=.o)
OBJ2= $(SRC2:.c=.o)
OBJ3= $(SRC3:.c=.o)
TARGET = exif initdb add35mmFocalLength
CFLAGS = -Wall
CC = gcc

all: $(TARGET)

exif: $(OBJ)
	$(CC) -o exif $^

initdb: $(OBJ2)
	$(CC) -mstack-probe-size=65535 -o initdb $^

add35mmFocalLength: $(OBJ3)
	$(CC) -mstack-probe-size=65535 -o add35mmFocalLength $^

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)

