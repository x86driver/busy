TARGET = busy

all:$(TARGET)

busy:busy.c
	droid-gcc -o $@ $< -Wall -g

clean:
	rm -rf $(TARGET)

up:busy
	adb push busy /storage
