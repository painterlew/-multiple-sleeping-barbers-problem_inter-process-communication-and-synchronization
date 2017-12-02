# -multiple-sleeping-barbers-problem_inter-process-communication-and-synchronization

This shop program will extend the original-so-called

sleeping-barber problem to a multiple sleeping barbers problem

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
