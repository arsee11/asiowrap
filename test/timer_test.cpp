#include <iostream>
#include <list>

#include "../timer.h"
#include "../networkpool.h"


using namespace std;
using namespace std::placeholders;
using namespace asiow;

std::mutex locker;
void onTimeout(const timer_ptr& t)
{
	locker.lock();
	cout<<"timer:"<<t.get();
	cout<<" thread:"<<NetworkThread::get_curid()<<endl;
	locker.unlock();
}

int main()
{
	NetworkPool::instance().init(2);

	timer_ptr timer1 = Timer::create(1000);
	timer1->start(onTimeout);

	timer_ptr timer2 = Timer::create(1000);
	timer2->start(onTimeout);
	
	timer_ptr timer3 = Timer::create(1000);
	timer3->start(onTimeout);

	cout<<"press enter to stop timer"<<endl;
	getchar();
	timer1->stop();
	timer2->stop();
	timer3->stop();

	getchar();
	cout<<"press enter to exit"<<endl;

	return 0;
}
