// ---------------------- Shop_org.h ------------------------- 
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
#ifndef _SHOP_ORG_H_
#define _SHOP_ORG_H_
#include <pthread.h>
#include <sstream>
#include <stdlib.h>
#include <queue>
#include <unistd.h> // usleep
#include <string>   // string
#include <iostream>

using namespace std;

#define DEFAULT_CHAIRS 3
#define DEFAULT_BARBERS 1

class Shop_org {
public:

	// -------------------constructor with parameter ------------------ 
	// initialize all the data and condition variables, according to 
	// the parameter pass by input
	//
	// if the input of nBarbers > 1 read it to nBarbers
	// otherwise set the DEFAULT_BARBERS to nBarbers
	//
	// if the input of nChairs > 0 read it to max 
	// otherwise set the default DEFAULT_CHAIRS to max
	// ----------------------------------------------------------------
	Shop_org(int num_Barbers, int nChairs) :
		nBarbers((num_Barbers > 1) ? num_Barbers : DEFAULT_BARBERS),
		max((nChairs > 0) ? nChairs : DEFAULT_CHAIRS)
	{
		service_chair = new int[nBarbers];
		in_service = new bool[nBarbers];
		money_paid = new bool[nBarbers];

		for (int i = 0; i < nBarbers; i++)
		{
			service_chair[i] = 0;  // set all barber's service_chair 0
			in_service[i] = false; // set all barber's in_service false
			money_paid[i] = false; // set all barber's money_paid false
		}
		init();
		nDropsOff = 0;
	}


	// ------------------default constructor--------------------------- 
	// the nBarbers's default value is DEFAULT_BARBERS, which is 1
	// max default value is DEFAULT_CHAIRS, which is 3
	// base on the default value of nBarbers and max value, initialize 
	// all the data and condition variables
	// ----------------------------------------------------------------
	Shop_org() :
		max(DEFAULT_CHAIRS),
		nBarbers(DEFAULT_BARBERS)
	{
		// set service_chair array to size 1, default value 0
		// set in_service array to size 1, default value false
		// set money_paid array to size 1, default valuefalse
		service_chair = new int[nBarbers] {0};
		in_service = new bool[nBarbers] {false};
		money_paid = new bool[nBarbers] {false};

		init();
		nDropsOff = 0;

	}


	// simulate the process of a customer visit the barber shop
	int visitShop(int id);

	// simulate the process of a customer get the service and 
	// leave the barber shop
	void leaveShop(int customerId, int barberId);

	// simulate the process of a barber check if there are customers 
	// waiting for hair-cut.
	void helloCustomer(int barberId);

	// simulate the process of a barber done with the hair-cut.
	void byeCustomer(int barberId);

	// the number of customers dropped off
	int nDropsOff;

private:

	// the max number of waiting chairs ,default value is 3
	const int max;

	// number of barbers, default value is 1
	const int nBarbers;
	// indicate the current customer thread id
	int * service_chair;
	// indicate each barber's chair is serving or not
	bool * in_service;
	// indicate each barber has been paid or not
	bool * money_paid;
	// includes the ids of all waiting threads
	queue<int> waiting_chairs;

	// the lock for the whole program
	pthread_mutex_t  mutex;
	// condition variables
	pthread_cond_t   cond_customers_waiting;
	pthread_cond_t*  cond_customer_served;
	pthread_cond_t*  cond_barber_paid;
	pthread_cond_t*  cond_barber_sleeping;

	//initialize all conditional variables
	void init();
	//  convert int to a string
	string int2string(int i);
	//  print either customer or barber and the message
	void print(int person, string message);

};

#endif
