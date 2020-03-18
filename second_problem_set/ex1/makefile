CC = gcc -Wall -std=c11


main: main.c lib.c 
	$(CC) -o program main.c lib.c -I.

clean:
	rm test.txt test_sys.txt test_lib.txt program

tests:
	make main

	#tests - > 1 byte lines

	./program generate test.txt 50000 1
	./program copy test.txt test_sys.txt 50000 1 sys
	./program copy test.txt test_lib.txt 50000 1 lib
	./program sort test_sys.txt 50000 1 sys
	./program sort test_lib.txt 50000 1 lib

	./program generate test.txt 60000 1
	./program copy test.txt test_sys.txt 60000 1 sys
	./program copy test.txt test_lib.txt 60000 1 lib
	./program sort test_sys.txt 60000 1 sys
	./program sort test_lib.txt 60000 1 lib

	#tests - > 4 byte lines

	./program generate test.txt 50000 4
	./program copy test.txt test_sys.txt 50000 4 sys
	./program copy test.txt test_lib.txt 50000 4 lib
	./program sort test_sys.txt 50000 4 sys
	./program sort test_lib.txt 50000 4 lib

	./program generate test.txt 70000 4
	./program copy test.txt test_sys.txt 70000 4 sys
	./program copy test.txt test_lib.txt 70000 4 lib
	./program sort test_sys.txt 70000 4 sys
	./program sort test_lib.txt 70000 4 lib

	#tests - > 512 byte lines

	./program generate test.txt 50000 512
	./program copy test.txt test_sys.txt 50000 512 sys
	./program copy test.txt test_lib.txt 50000 512 lib
	./program sort test_sys.txt 50000 512 sys
	./program sort test_lib.txt 50000 512 lib

	./program generate test.txt 70000 512
	./program copy test.txt test_sys.txt 70000 512 sys
	./program copy test.txt test_lib.txt 70000 512 lib
	./program sort test_sys.txt 70000 512 sys
	./program sort test_lib.txt 70000 512 lib

	#tests - > 1024 byte lines

	./program generate test.txt 50000 1024
	./program copy test.txt test_sys.txt 50000 1024 sys
	./program copy test.txt test_lib.txt 50000 1024 lib
	./program sort test_sys.txt 50000 1024 sys
	./program sort test_lib.txt 50000 1024 lib

	./program generate test.txt 70000 1024
	./program copy test.txt test_sys.txt 70000 1024 sys
	./program copy test.txt test_lib.txt 70000 1024 lib
	./program sort test_sys.txt 70000 1024 sys
	./program sort test_lib.txt 70000 1024 lib

	#tests - > 4096 byte lines

	./program generate test.txt 50000 4096
	./program copy test.txt test_sys.txt 50000 4096 sys
	./program copy test.txt test_lib.txt 50000 4096 lib
	./program sort test_sys.txt 50000 4096 sys
	./program sort test_lib.txt 50000 4096 lib

	./program generate test.txt 70000 4096
	./program copy test.txt test_sys.txt 70000 4096 sys
	./program copy test.txt test_lib.txt 70000 4096 lib
	./program sort test_sys.txt 70000 4096 sys
	./program sort test_lib.txt 70000 4096 lib

	#tests - > 8192 byte lines

	./program generate test.txt 50000 8196
	./program copy test.txt test_sys.txt 50000 8196 sys
	./program copy test.txt test_lib.txt 50000 8196mak lib
	./program sort test_sys.txt 50000 8196 sys
	./program sort test_lib.txt 50000 8196 lib


	./program generate test.txt 70000 8196
	./program copy test.txt test_sys.txt 70000 8196 sys
	./program copy test.txt test_lib.txt 70000 8196mak lib
	./program sort test_sys.txt 70000 8196 sys
	./program sort test_lib.txt 70000 8196 lib

	make clean