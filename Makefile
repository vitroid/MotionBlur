LDFLAGS=`pkg-config --libs opencv`
all: motionblur motionblur.o
%: %.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
