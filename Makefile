XX = g++
CXXLIB   = -lpthread gprof-helper.so
CXXFLAGS = -std=gnu++0x -pg -g 
CXXSOURCE = Buff.cpp Buff_Manager.cpp Config.cpp Socket.cpp Log.cpp
all:
	$(XX) -o Robot $(CXXFLAGS) $(CXXSOURCE) $(CXXLIB)
clean:
	rm -rf *.o Robot
