CC = gcc

.PHONY: docs clean install uninstall test
.DEFAULT_GOAL = error

DELETE_TEST_FILES = true
test_end = rm ./test/test_strbuf.out

header_files: ./src/strbuf.h
test_files: ./test/test_strbuf.c

error:
	@echo "Please choose one of the following targets: clean, docs, install, uninstall, test"
	@exit 2

clean:
	rm -f ./test/*.out
	rm -rf ./doxygen-dark-theme ./html

docs:
	sh ./make_docs.sh

install: uninstall
	cp ./src/*.h /usr/include/clz/

uninstall:
	sudo rm -rf /usr/include/clz/*.h

test: ./test/test_strbuf.out

./test/test_strbuf.out: header_files test_files
	$(CC) -Wall -Wextra -Werror ./test/test_strbuf.c -o ./test/test_strbuf.out -ggdb
	./test/test_strbuf.out
	$(test_end)