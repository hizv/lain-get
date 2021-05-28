##
# lainget
#
# @file
# @version 0.1
all:
	g++ -o lain src/main.cpp src/lain.cpp -ljsoncpp -lcurl -lboost_program_options -I/usr/include/jsoncpp/

# end
