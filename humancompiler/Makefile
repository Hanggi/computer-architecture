#------------------------------------------------------------
#
#  4190.308-002: Computer Architecture (Fall 2016)
#
#  Skeleton code for Lab #2: Great human compiler
#
#  Oct 21, 2016.
#  Instructor: Jae W. Lee (jaewlee@snu.ac.kr)
#  Seoul National University
#
#------------------------------------------------------------

CC	= gcc
AS	= as
CFLAGS	= -g -O -w
ASFLAGS	=
LDFLAGS	= 
RM	= rm

CSRCS	= main.c
ASRCS	= get_min_max.s
TARGET	= get_min_max
OBJECTS	= $(CSRCS:.c=.o) $(ASRCS:.s=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.s.o:
	$(AS) $(ASFLAGS) $< -o $@

clean:
	$(RM) -f $(OBJECTS) $(TARGET) 
