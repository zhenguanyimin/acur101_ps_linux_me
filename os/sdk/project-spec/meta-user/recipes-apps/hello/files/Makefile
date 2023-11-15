APP = hello

# Add any other object files to this list below
APP_OBJS = hello.o

all: build

build: $(APP)

$(APP): $(APP_OBJS)
	echo cc = $(CC) >>my.txt
	echo ldflags = $(LDFLAGS) >>my.txt
	echo ldlibs = $(LDLIBS) >>my.txt

	$(CC) -o $@ $(APP_OBJS) $(LDFLAGS) $(LDLIBS)
clean:
	rm -f $(APP) *.o

