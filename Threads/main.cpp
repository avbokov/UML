#include<iostream>
#include<thread>
#include<chrono>
using namespace std::literals::chrono_literals;

bool finish = false;

void plus()
{
	while (!finish)
	{
		std::cout << "+ ";
		std::this_thread::sleep_for(1s);
	}
}
void minus()
{
	while (!finish)
	{
		std::cout << "- ";
		std::this_thread::sleep_for(1s);
	}
}

void main()
{
	std::thread plus_thread(plus);
	std::thread minus_thread(minus);

	std::cin.get();
	finish = true;

	minus_thread.join();	// Синхронизирует поток с основным потоком
	plus_thread.join();
	
}