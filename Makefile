all:
	 g++ -o lain src/main.cpp src/lain.cpp -ljsoncpp -lcurl -lboost_program_options
