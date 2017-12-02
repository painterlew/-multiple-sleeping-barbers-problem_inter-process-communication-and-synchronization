// --------------------compile this program ---------------------  
// compile with: 
//        g++ driver.cpp Shop.cpp -o sleepingBarbers -lpthread 
// run it with five parameter:
//        ./sleepingBarbers 1 1 10 1000
//
// argv[0] -- file name(./sleepingBarbers)
// argv[1] -- nBarbers
// argv[2] -- nChairs
// argv[3] -- nCustomers
// argv[4] -- serviceTime
// ---------------------- driver.cpp -------------------------  
// Programmer's Chang Liu
// CSS 503
// 5/10/2017 
// 5/13/2107
// ---------------------------------------------------------------- 
// The driver.cpp is a driver program that tests sleeping- barbers
// problem.
// ---------------------------------------------------------------- 
// This driver program will Instantiates a shop, Spawns nBarbers 
// barber threads, and nCustomers customer threads. Each cunstomer
// thread is spawned with a random interval in milliseconds.
// 
// The customer visits the barbershop after spawned, and receive a 
// haircut service from any one available among barbers in the shop.?
// Waits until all the customer threads are service and terminated.
// Terminates all the barber threads.
// ---------------------------------------------------------------- 

#include <iostream>    // cout
#include <sstream>
#include <stdlib.h>
#include <sys/time.h> // gettimeofday
#include "Shop_org.h"
#include <unistd.h>    //usleep

using namespace std;

// ------------------------ThreadParam class------------------------
// ThreadParam encapsulate three parameters to be passed 
// to each thread 
// the default agruement as below
// *shop = NULL, id = -1, and serviceTime = 0
// ---------------------------------------------------------------- 
class ThreadParam {
public:

	ThreadParam(Shop_org *shop, int id, int serviceTime) :
		shop(shop), id(id), serviceTime(serviceTime) { };

	// a pointer to the Shop object 
	Shop_org *shop;
	// a thread identifier 
	int id;
	// service time to a barber, 0 to a customer 
	int serviceTime;
};

void *barber(void *);
void *customer(void *);

// ------------------------main----------------------------- 
// this is main method that tests sleeping- barbers problem.
//
// Description: 
// Instantiates a shop, Spawns nBarbers barber threads, and
// nCustomers customer threads. Each cunstomer thread is spawned
// with a random interval in milliseconds.
//
// The customer visits the barbershop after spawned, and receive a 
// haircut service from any one available among barbers in the shop.?
// Waits until all the customer threads are service and terminated.
// Terminates all the barber threads.
// ---------------------------------------------------------------- 
int main(int argc, char *argv[]) {

	// check number of parameter


	int nBarbers = atoi(argv[1]);
	int nChairs = atoi(argv[2]);
	int nCustomers = atoi(argv[3]);
	int serviceTime = atoi(argv[4]);

	pthread_t barber_threads[nBarbers];     // array of barber_thread
	pthread_t customer_threads[nCustomers];// array of customer thread

	Shop_org *shop = new Shop_org(nBarbers, nChairs);   // a shop instance
	//Spawns nBarbers of barber threads
	for (int i = 0; i < nBarbers; i++) {
		int barberId = i;
		ThreadParam *barber_param = new ThreadParam(shop, barberId, serviceTime);
		pthread_create(&barber_threads[i], NULL, barber, barber_param);
	}

	// With a Random interval in milliseconds, spawns one after another 
	// customer. In total spawns nCustomers of customer threads
	for (int i = 0; i < nCustomers; i++) {
		usleep(rand() % 1000);
		int id = i + 1;
		ThreadParam *customer_param = new ThreadParam(shop, id, 0);
		pthread_create(&customer_threads[i], NULL, customer, customer_param);
	}


	for (int i = 0; i < nCustomers; i++) {
		pthread_join(customer_threads[i], NULL);
	}

	for (int i = 0; i < nBarbers; i++) {
		pthread_cancel(barber_threads[i]);
	}

	cout << "# customers who didn't receive a service = "
		<< shop->nDropsOff << endl;

	return 0;
}


// ------------------------barber----------------------------- 
// the barber thread function
// use a infinite loop to make each barber keep working until 
// all the customers get hair cut or leave the shop
// ---------------------------------------------------------------- 
void *barber(void *arg) {

	// extract parameters
	ThreadParam &barber_param = *(ThreadParam*)arg;
	Shop_org &shop = *(barber_param.shop);
	int barberId = barber_param.id;
	int serviceTime = barber_param.serviceTime;
	delete &barber_param;

	while (true) {
		shop.helloCustomer(barberId);
		usleep(serviceTime);
		shop.byeCustomer(barberId);
	}
}


// ------------------------customer----------------------------- 
// the customer thread function
// make each customer thread visit the shop to try to get hair cut
// ---------------------------------------------------------------- 

void *customer(void *arg) {

	// extract parameters
	ThreadParam &cus_param = *(ThreadParam*)arg;
	Shop_org &shop = *(cus_param.shop);
	int id = cus_param.id;
	delete &cus_param;


	int barberId = -1;
	if ((barberId = shop.visitShop(id)) != -1)
	{
		// assigned to a barber, being served now 
		// leave the shop
		shop.leaveShop(id, barberId);
	}

}
