test_main: test_main.cpp co_schedule.cpp
	g++ -g -o test_main test_main.cpp co_schedule.cpp

clean:
	rm *.o test_main