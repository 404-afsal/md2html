# 1. Variables
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
TARGET = md2html

# 2. List all your .c files here (Separated by spaces)
SRCS = main.c parser.c ds_handle.c

# 3. Automatically generate .o (object) file names from the .c files
OBJS = $(SRCS:.c=.o)

# 4. The default rule (What happens when you just type 'make')
all: $(TARGET)

# 5. Link the object files to create the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# 6. Compile .c files into .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 7. Clean up rule (Type 'make clean' to wipe executables and object files)
clean:
	rm -f $(TARGET) $(OBJS)