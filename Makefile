include_dir = ./includes
src_dir = ./sources
bin_dir = ./bin
#FLAGS = -std=c11 -O0 -pthread -g

all: lab
main.o:
	g++ -c -I$(include_dir) $(src_dir)/main.cpp -o $(bin_dir)/main.o
huffman.o:
	g++ -c -I$(include_dir) $(src_dir)/huffman.cpp -o $(bin_dir)/huffman.o
lab: main.o common.o
	g++ -o $(bin_dir)/huffman $(bin_dir)/main.o $(bin_dir)/huffman.o
clean:
	rm $(bin_dir)/*.o
	rm huffman
