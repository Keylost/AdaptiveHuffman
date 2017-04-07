include_dir = ./includes
src_dir = ./sources
bin_dir = ./bin
FLAGS = -g

all: lab
main.o:
	g++ $(FLAGS) -c -I$(include_dir) $(src_dir)/main.cpp -o $(bin_dir)/main.o
timer.o:
	g++ $(FLAGS) -c -I$(include_dir) $(src_dir)/timer.cpp -o $(bin_dir)/timer.o
huffman.o:
	g++ $(FLAGS) -c -I$(include_dir) $(src_dir)/huffman.cpp -o $(bin_dir)/huffman.o
lab: main.o timer.o huffman.o
	g++ $(FLAGS) -o $(bin_dir)/huffman $(bin_dir)/main.o $(bin_dir)/timer.o $(bin_dir)/huffman.o
clean:
	rm $(bin_dir)/*.o
	rm $(bin_dir)/huffman
