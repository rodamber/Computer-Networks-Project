run: all
	./user

all: user

user: user.c ../utils.c
	gcc -g -o $@ $^

clean:
	rm -f user
