all: test_hmap search_hmap

#COMMON_INCLUDE=./../common

#INCLUDES=-I${COMMON_INCLUDE}


clean:
	rm test_hmap search_hmap

test_hmap: 
	gcc -g -Wall test_hmap.c hmap.c -I. -o test_hmap
search_hmap:
	gcc -g -Wall search_hmap.c hmap.c -I. -o search_hmap
