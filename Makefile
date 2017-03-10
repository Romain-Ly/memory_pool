TARGET = pool

#libs
TARGET_SO = lib$(TARGET).so
TARGET_ST = lib$(TARGET).a

#test binary
TARGET_TEST = test_$(TARGET)

CC ?= gcc
INCLUDE = -I.
SRC = pool.c
OBJ = $(SRC:.c=.o)
SAMPLE_SRC = main.c test_tools/test.c
SAMPLE_OBJ = $(SRC:.c=.o)
FPIC_OBJ = $(SRC:.c=.fpic.o)
CFLAGS ?= -Wall -Wextra
LDFLAGS ?=
DEBUG ?= 0
VALGRIND ?=0

#Mandatory flags
CFLAGS += ${INCLUDE}
LDFLAGS += -L.

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -O0 -ggdb3 -fno-inline
else
	CFLAGS += -DNDEBUG -DNVALGRIND -O3
endif

all: $(TARGET_SO) $(TARGET_ST)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

#static
$(TARGET_ST) : $(OBJ)
	$(AR) rcs $@ $^

#shared
$(TARGET_SO) : $(FPIC_OBJ)
	$(CC) -shared $(LDFLAGS) -o $@ $<

%.fpic.o: %.c
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

#test
test: $(TARGET_ST) $(TARGET_SO)
	$(CC) $(CFLAGS) $(SAMPLE_SRC) -o $(TARGET_TEST)_static $(LDFLAGS) $(TARGET_ST)
	$(CC) $(CFLAGS) $(SAMPLE_SRC) -o $(TARGET_TEST)_shared $(LDFLAGS) $(TARGET_SO)

.PHONY: clean
clean :
	@rm -f $(OBJ) $(SAMPLE_OBJ) $(FPIC_OBJ) $(TARGET_SO) $(TARGET_ST) $(TARGET_TEST)_static $(TARGET_TEST)_shared
