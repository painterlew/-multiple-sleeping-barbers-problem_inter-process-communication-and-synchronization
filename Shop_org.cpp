// ---------------------- Shop_org.cpp ------------------------- 
// 
// Programmer's Chang Liu
// CSS 503
// 5/10/2017 
// 5/13/2107
// ---------------------------------------------------------------- 
// This shop program will extend the original-so-called 
// sleeping-barber problem to a multiple sleeping barbers problem
// where many customers visit a barbershop and receive a haircut 
// service from any one available among barbers in the shop.
//
// A barbershop consists of a waiting room with n chairs and 
// a barber room with m barber chairs. If there are no customers 
// to be served, all the barbers go to sleep. 
// If a customer enters the barbershop and all chairs(including 
// both, waiting and barber chairs) are occupied, then the customer
// leaves the shop.
// If all the barbers are busy but chairs are available, then the
// customer sits in one of the free waiting chairs.If the barbers 
// are asleep, the customer wakes up one of the barbers.
// ---------------------------------------------------------------- 
// the sleeping barber problem is a classic inter-process 
// communication and synchronization problem between multiple
// operating system processes. 
// 
// This program uses mutex in pthread to implement the sleeping 
// barber problema sychronously. Make customer and barber take turns
// to come into their critial section to fllow an orderly manner.
// ---------------------------------------------------------------- 

#include <iostream> // cout
#include <sstream>  // stringstream
#include <string>   // string
#include "Shop_org.h"


// ------------------------init()  ----------------------------- 
//initialize all the condition variables 
// ----------------------------------------------------------------
void Shop_org::init() {
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond_customers_waiting, NULL);
	cond_customer_served = new pthread_cond_t[nBarbers];
	cond_barber_paid = new pthread_cond_t[nBarbers];
	cond_barber_sleeping = new pthread_cond_t[nBarbers];
	for (int i = 0; i < nBarbers; i++)
	{
		pthread_cond_init(&cond_customer_served[i], NULL);
		pthread_cond_init(&cond_barber_paid[i], NULL);
		pthread_cond_init(&cond_barber_sleeping[i], NULL);
	}
}

// ------------------------int2string  ----------------------------- 
//  convert int to a string
// ----------------------------------------------------------------
string Shop_org::int2string(int i) {
	stringstream out;
	out << i;
	return out.str();
}

// ------------------------print --------------------------------- 
//  print either customer or barber and the message
// ----------------------------------------------------------------
void Shop_org::print(int person, string message) {
	cout << ((person > 0) ? "customer[" : "barber  [")
		<< abs(person) << "]: " << message << endl;
}



// ------------------------visitShop----------------------------- 
// simulate the process of a customer visit the barber shop
// return barberId only when a customer got a service
// otherwise, return -1
// ----------------------------------------------------------------
int Shop_org::visitShop(int id) {
	//Enter critical section. 
	pthread_mutex_lock(&mutex);

	int available_barberId = -1;
	//find the first available barber
	for (int b_id = 0; b_id < nBarbers; b_id++) {
		if (service_chair[b_id] == 0)  	available_barberId = b_id;
	}

	if (waiting_chairs.size() == max) {
		// when all waiting chairs are occupied & no barber is available 
		// customer leave the shop
		if (available_barberId == -1) {
			print(id,
				"leaves the shop because of no available waiting chairs.");
			++nDropsOff;
			// leaves the critical section
			pthread_mutex_unlock(&mutex);
			return -1;
		}
		// when all waiting chairs are occupied , but a barber is available 
		// customer go to sit on the barber's chair
	}
	// waiting chairs availale, but all service_chairs are occupied 
	// or someone is waiting, go the the waiting chair
	if (available_barberId == -1 || !waiting_chairs.empty()) {

		waiting_chairs.push(id);
		print(id, "takes a waiting chair. # waiting seats available = "
			+ int2string(max - waiting_chairs.size()));
		//Waits for a barber to wake me up 
		pthread_cond_wait(&cond_customers_waiting, &mutex);

		//Wake up! Pop this customer out from the queue 
		waiting_chairs.pop();

	}

	//Re-find the first available barber
	for (int b_id = 0; b_id < nBarbers; b_id++) {
		if (service_chair[b_id] == 0)  	available_barberId = b_id;
	}

	print(id, "moves to a service chair[" + int2string(available_barberId) +
		"]. # waiting seats available = "
		+ int2string(max - waiting_chairs.size()));
	//Have my barberId's chair set to my id 
	service_chair[available_barberId] = id;

	//set my my barberId's service to true         
	in_service[available_barberId] = true;

	// wake up the barberId just in case if he is sleeping
	pthread_cond_signal(&cond_barber_sleeping[available_barberId]);
	//Leave the critical section, gives back lock
	pthread_mutex_unlock(&mutex);

	return available_barberId;
}



// ------------------------leaveShop----------------------------- 
// simulate the process of a customer get the service and 
// leave the barber shop
// wait if the barber still in service. When barber signaled done,
// pay the barber and leave the shop
// ----------------------------------------------------------------
void Shop_org::leaveShop(int id, int barberId) {
	//Enter the critical section
	pthread_mutex_lock(&mutex);

	//customerId wait for barber[barberId] to be done with hair-cut.
	print(id, "wait for barber[" + int2string(barberId) +
		"] to be done with hair-cut.");

	//While barberId is cutting my hair,wait
	while (in_service[barberId] == true)
		pthread_cond_wait(&cond_customer_served[barberId], &mutex);

	//Pay barber and signal that barberId 
	money_paid[barberId] = true;
	pthread_cond_signal(&cond_barber_paid[barberId]);

	print(id, "says good-bye to the barber[" + int2string(barberId) + "].");

	//Leave the critical section.
	pthread_mutex_unlock(&mutex);
}






// ------------------------helloCustomer-------------------------- 
// simulate the process of a barber check if there are customers 
// waiting for hair-cut.
// if there is customer waiting, start service
// if not, barber goes to sleep and waiting for customer signal  
// ----------------------------------------------------------------
void Shop_org::helloCustomer(int barberId) {
	// Enter the critical section
	pthread_mutex_lock(&mutex);

	// if all waiting_chairs are empty & no customer on my service chairs
	// this barberId go to sleep, wait for a customer to wake him up
	if (waiting_chairs.empty() && service_chair[barberId] == 0) {
		print(-barberId, "sleeps because of no customers.");
		pthread_cond_wait(&cond_barber_sleeping[barberId], &mutex);
	}

	//doube check if he service chair still empty, wait
	if (service_chair[barberId] == 0) {
		pthread_cond_wait(&cond_barber_sleeping[barberId], &mutex);
	}
	print(-barberId,
		"starts a hair-cut service for customer["
		+ int2string(service_chair[barberId]) + "].");

	//Leave the critical section
	pthread_mutex_unlock(&mutex);
}





// ------------------------helloCustomer-------------------------- 
// simulate the process of a barber done with the hair-cut.
// barber signals the customer "done" and waits for payment.
// once get pay, the barber calls the next customer  
// ----------------------------------------------------------------
void Shop_org::byeCustomer(int barberId) {
	//Enter the critical section. 
	pthread_mutex_lock(&mutex);

	in_service[barberId] = false;

	print(-barberId, "says he's done with a hair-cut service for customer["
		+ int2string(service_chair[barberId]) + "].");

	money_paid[barberId] = false;

	//signal the customer "done"
	pthread_cond_signal(&cond_customer_served[barberId]);


	//Wait for my customer to pay before I take a new one
	while (money_paid[barberId] == false)
		pthread_cond_wait(&cond_barber_paid[barberId], &mutex);

	service_chair[barberId] = 0;

	print(-barberId, "calls in another customer");

	// call in another one
	pthread_cond_signal(&cond_customers_waiting);

	//Leave the critical section
	pthread_mutex_unlock(&mutex);
}
