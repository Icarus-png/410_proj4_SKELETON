#include <string>
#include "stdlib.h"

#include "../includes/waiter.h"
#include "../includes/file_IO.h"
#include "../includes/externs.h"
#include "../includes/constants.h"

using namespace std;

//ID is just a number used to identify this particular baker
//(used with PRINT statements)
//filename is what waiter reads in orders from
Waiter::Waiter(int id,std::string filename):id(id),myIO(filename){
}

Waiter::~Waiter()
{
}

//gets next Order from file_IO
//if return == SUCCESS then anOrder
//contains new order
//otherwise return contains fileIO error
//gets next Order(s) from file_IO
int Waiter::getNext(ORDER &anOrder){
	return myIO.getNext(anOrder);
}

//contains a loop that will get orders from filename one at a time
//then puts them in order_in_Q then signals baker(s) using cv_order_inQ
//so they can be consumed by baker(s)
//when finished exits loop and signals baker(s) using cv_order_inQ that
//it is done using b_WaiterIsFinished
void Waiter::beWaiter() {
	ORDER order;
	int status = getNext(order);

	//While there are still orders in file, push them to inQ
	//when done notify and then break out of loop
	while (status == SUCCESS) {
		unique_lock<mutex> ul(mutex_order_inQ);
		order_in_Q.push(order);
		status = getNext(order);
		cv_order_inQ.notify_all();
	}

	//Once there are no more orders to take, our waiter is done, let threads know
	unique_lock<mutex> ul(mutex_order_inQ);
	b_WaiterIsFinished = true;
	cv_order_inQ.notify_all();
}






