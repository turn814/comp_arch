default:
	g++ -Wall -Wextra *.cpp -o cpusim
clean:
	rm -f cpusim
all: default
	./cpusim ../Trace/22instMem-all.txt
r: default
	./cpusim ../Trace/22instMem-r.txt
sw: default
	./cpusim ../Trace/22instMem-sw.txt
test: default
	./cpusim ../Trace/test.txt