#include "../includes_usr/constants.h"
#include "../includes_usr/dispatcher.h"
#include "../includes_usr/file_io.h"
#include "../includes_usr/joblist.h"
#include "../includes_usr/logger_single_thread.h"
#include <queue>

PCB runningPCB;
std::queue<PCB> ready_Q;
std::queue<PCB> blocked_Q;
//Function clears queue by swapping it with an empty queue
void clearQs( std::queue<PCB> &q ) {
   std::queue<PCB> empty;
   std::swap( q, empty );
}
void dispatcher::init() {
	clearQs(ready_Q);
	clearQs(blocked_Q);
	runningPCB.cpu_time = UNINITIALIZED;
	runningPCB.io_time = UNINITIALIZED;
	runningPCB.process_number = UNINITIALIZED;
	runningPCB.start_time = UNINITIALIZED;
}

PCB dispatcher::getCurrentJob() {
	return runningPCB;
}

void dispatcher::addJob(PCB &myPCB) {
	ready_Q.push(getNext());
}

int dispatcher::processInterrupt(int interrupt) {

	return NO_JOBS;
}
void markInvalid(PCB pcb){
	pcb.cpu_time = UNINITIALIZED;
	pcb.io_time = UNINITIALIZED;
	pcb.process_number = UNINITIALIZED;
	pcb.start_time = UNINITIALIZED;
}
int dispatcher::doTick() {
	int ender;
	if(runningPCB.cpu_time == 1 ){
		runningPCB.cpu_time--;
	} else {
		bool readyQempty = ready_Q.empty();
		bool blockedQempty = blocked_Q.empty();
		if(!readyQempty){
			runningPCB = ready_Q.front();
			ready_Q.pop();
			return PCB_MOVED_FROM_READY_TO_RUNNING;
		}
		if (readyQempty && blockedQempty){
			return NO_JOBS;
		}
		if (!blockedQempty && readyQempty){
			return BLOCKED_JOBS;
		}
	}
	if(getCurrentJob().cpu_time == 0){
		if(getCurrentJob().io_time == 1){
			blocked_Q.push(getCurrentJob());
			ender = PCB_ADDED_TO_BLOCKED_QUEUE;
		} else {
			ender = PCB_FINISHED;
		}
		markInvalid(runningPCB);
		return ender;
	} else {
		return PCB_CPUTIME_DECREMENTED;
	}
	return NO_JOBS;
}
