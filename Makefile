build:
	gcc SimpleSearchEngine.c -o search_index

run:
	./search_index

clean:
	rm -f search_index
