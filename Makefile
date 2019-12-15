CC=gcc
BUILD_DIR=build/
NAME=memalloc

all: libmemalloc app

libmemalloc: build_dir memalloc.c
	$(CC) -Wall -c -std=c99 memalloc.c -o memalloc.o
	ar -cvq libmemalloc.a memalloc.o
	ranlib libmemalloc.a

app: build_dir app.c
	$(CC) -o $(BUILD_DIR)app app.c $(NAME).c -std=c99 -lpthread -L. -lmemalloc

main: build_dir
	$(CC) -Wall -o $(BUILD_DIR)main main.c $(NAME).c -std=c99 -lpthread -L. -lmemalloc

build_dir:
	if [ ! -d $(BUILD_DIR) ]; then \
		mkdir $(BUILD_DIR); \
	fi

clean:
	rm -rf $(BUILD_DIR)
	rm -f memalloc.o libmemalloc.a

rebuild: clean main