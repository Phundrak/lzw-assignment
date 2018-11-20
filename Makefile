all:
	@mkdir -p build
	@cd build && pwd && cmake -DCMAKE_BUILD_TYPE=Release .. && make
release:
	@mkdir -p build
	@cd build && pwd && cmake -DCMAKE_BUILD_TYPE=Release .. && make

debug:
	@mkdir -p build
	@cd build && pwd && cmake -DCMAKE_BUILD_TYPE=Debug .. && make

clean:
	rm -rf bin/* build/* debug/*
