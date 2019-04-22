#include <iostream>
#include <mutex>
#include <thread>

constexpr auto COUNT = 500000;

using std::cout;
using std::endl;

std::mutex m1;
std::mutex m2;

void ping()
{
	for(int i = 0; i < COUNT; i++)
	{
		m1.lock();
		cout << "ping\n";
		m2.unlock();
	}
}

void pong()
{
	for(int i = 0; i < COUNT; i++)
	{
		m2.lock();
		cout << "pong\n";
		m1.unlock();
	}
}

int main()
{
	std::thread t_ping(ping); m2.lock();
	std::thread t_pong(pong);
	t_ping.join();
	t_pong.join();
	return 0;
}
