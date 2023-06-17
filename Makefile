CPPFLAGS = -std=c99
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(OBJS:.o=.d)

ifeq ($(OS), Windows_NT)
    CLEAN_CMD = del *.o *.exe *.d
else
    CLEAN_CMD = rm -f *.o *.exe *.d
endif

all: main.exe

main.exe: $(OBJS)
	gcc -o $@ $(CPPFLAGS) $^

%.o: %.cpp
	gcc $(CPPFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

clean:; rm -f *.o *.exe *.d
# clean:; $(CLEAN_CMD)
# on Windows, use: del *.o *.exe *.d
# on Linux or MacOS, use: rm -f *.o *.exe *.d

tar:; tar -czvf main.tgz *.cpp *.h

.PRECIOUS: $(OBJS)

.PHONY: clean tar