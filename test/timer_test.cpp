#include "../timer.h"
#include <iostream>
#include <list>

#include "../networkpool.h"


using namespace std;
using namespace std::placeholders;
using namespace asiow;

void onTimeout(const Timer* t)
{
	cout<<"timer tick:"<<t<<endl;
}

int main()
{
	//boost::asio::io_context ioc;
	//Timer timer(ioc, 1000);
	//timer.start(&onTimeout);
	//ioc.run();

	NetworkPool np;
	np.init();

	Timer* timer = np.createTimer<Timer>(1000);
	timer->start(onTimeout);

	getchar();
	timer->stop();
	np.uninit();

	return 0;
}
