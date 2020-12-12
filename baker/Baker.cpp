#include <mutex>

#include "../includes/baker.h"
#include "../includes/externs.h"
#include "../includes/PRINT.h"
using namespace std;

//ID is just a number used to identify this particular baker
//(used with PRINT statements)
Baker::Baker(int id):id(id)
{
}

Baker::~Baker()
{
}

//bake, box and append to anOrder.boxes vector
//if order has 13 donuts there should be 2 boxes
//1 with 12 donuts, 1 with 1 donut
void Baker::bake_and_box(ORDER &anOrder) {
	Box boxx;
	int dCount = anOrder.number_donuts;
	int donutsInBox = 0;
	
	//Iterate through our order, adding donuts to box
	for (int i = 0; i < dCount; i++) {
		DONUT donutt;
		boxx.addDonut(donutt);
		donutsInBox++;

		//If the number of donuts in our boxx = 12, box is full so push to vector and clear for restart
		if (dCount == 12) {
			lock_guard<mutex> lg(mutex_order_outQ);
			anOrder.boxes.push_back(boxx);
			boxx.clear();
		}
	}

	//Make sure we're not going to be wasting a box, If box has donuts we push box and clear for next order
	if (boxx.size() > 0) {
		anOrder.boxes.push_back(boxx);
		boxx.clear();
	}
}

//as long as there are orders in order_in_Q then
//for each order:
//	create box(es) filled with number of donuts in the order
//  then place finished order on order_outvector
//  if waiter is finished (b_WaiterIsFinished) then
//  finish up remaining orders in order_in_Q and exit
//
//You will use cv_order_inQ to be notified by waiter
//when either order_in_Q.size() > 0 or b_WaiterIsFinished == true
//hint: wait for something to be in order_in_Q or b_WaiterIsFinished == true
void Baker::beBaker() {
	while(!order_in_Q.empty() && !b_WaiterIsFinished) {
		unique_lock<mutex> ul(mutex_order_inQ);

		//if we have no orders or if the waiter isn't done taking orders, then we wait
		while (order_in_Q.empty() && !b_WaiterIsFinished) {
			cv_order_inQ.wait(ul);
		}

		//If we have orders to fill, select first order in line and fill it
		//Send to our outgoing orders to go to customer
		if (!order_in_Q.empty()) {
			ORDER order = order_in_Q.front();
			bake_and_box(order);
			order_in_Q.pop();
			lock_guard<mutex> lg(mutex_order_outQ);
			order_out_Vector.push_back(order);
		}
	}
}
