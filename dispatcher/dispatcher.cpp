#include "../includes_usr/constants.h"
#include "../includes_usr/dispatcher.h"
#include "../includes_usr/file_io.h"
#include "../includes_usr/joblist.h"
#include "../includes_usr/logger_single_thread.h"
#include <queue>

PCB runningPCB;
std::queue<PCB> ready_Q;
std::queue<PCB> blocked_Q;
void markInvalid(PCB &pcb) {
	pcb.cpu_time = UNINITIALIZED;
	pcb.io_time = UNINITIALIZED;
	pcb.process_number = UNINITIALIZED;
	pcb.start_time = UNINITIALIZED;
}
//Function clears queue by swapping it with an empty queue
void clearQs(std::queue<PCB> &q) {
	std::queue<PCB> empty;
	std::swap(q, empty);
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
	PCB temp;
	temp.cpu_time = myPCB.cpu_time;
	temp.io_time = myPCB.io_time;
	temp.process_number = myPCB.process_number;
	temp.start_time = myPCB.start_time;
	ready_Q.push(temp);
}

int dispatcher::processInterrupt(int interrupt) {

	if (interrupt == 1) {
		//do code
		if(!ready_Q.empty()){
			//switch jobs
			PCB tempJob = ready_Q.front();
			//added
			ready_Q.pop();
			//get current pcb if valid and move to ready
			//FIXME
			if(runningPCB.start_time != UNINITIALIZED){ //may need to change the line to chekc if valid
				ready_Q.push(runningPCB);
			}
			runningPCB = tempJob;
		} else {
			if(blocked_Q.empty()){
				return NO_JOBS;
			}
			else {
				return BLOCKED_JOBS;
			}
			// return whether there are jobs in ready or blocked
		}
	} else {
		if (interrupt == 0) {
			if (!blocked_Q.empty()) {
				for (int i = 0; i < blocked_Q.size(); i++) {
					ready_Q.push(blocked_Q.front());
					blocked_Q.pop();
				}
				return PCB_MOVED_FROM_BLOCKED_TO_READY;
			} else {
				return PCB_BLOCKED_QUEUE_EMPTY;
			}
		} else {
			return PCB_UNIMPLEMENTED;
		}
	}

	return PCB_SWITCHED_PROCESSES;

}
int dispatcher::doTick() {
	int ender;
	if (runningPCB.cpu_time != 0 && runningPCB.cpu_time != UNINITIALIZED) {
		runningPCB.cpu_time--;
	} else {
		bool readyQempty = ready_Q.empty();
		bool blockedQempty = blocked_Q.empty();
		if (!readyQempty) {
			runningPCB = ready_Q.front();
			ready_Q.pop();
			return PCB_MOVED_FROM_READY_TO_RUNNING;
		} else {
			if(blockedQempty){
				return NO_JOBS;
			} else{
				return BLOCKED_JOBS;
			}
		}
	}
	if (getCurrentJob().cpu_time == 0) {
		if (getCurrentJob().io_time == 1) {
			runningPCB.io_time = 0;
			blocked_Q.push(getCurrentJob());
			ender = PCB_ADDED_TO_BLOCKED_QUEUE;
		} else {\
			ender = PCB_FINISHED;
		}
		markInvalid(runningPCB);
		return ender;
	} else {
		return PCB_CPUTIME_DECREMENTED;
	}
	return NO_JOBS;
}

