	CC = gcc
	XX = g++
	CINCLUDE = -I/home/memory/apr-1.5.2/include/ -L/usr/local/apache2/lib/
	CXXLIB   = -lpthread
	CXXFLAGS = -std=gnu++0x -g -O0
	CXXSOURCE = Buff.cpp Buff_Manager.cpp Config.cpp Socket.cpp Log.cpp


Robot:$(CXXSOURCE)
	$(XX) -o Robot $(CXXFLAGS) $(CINCLUDE) $(CXXSOURCE) -L. $(CXXLIB)

clean:
	rm -rf *.o Robot
