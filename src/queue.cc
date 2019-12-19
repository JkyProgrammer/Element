#include <iostream>

#include "element.h"

void actionqueue::push (charge_i c) {
	m.lock();
	queue.push (c);
	m.unlock();
}

charge_i actionqueue::pop () {
	charge_i rval = charge_i (nullptr, -1);
	m.lock();
	if (!queue.empty()) { rval = queue.front(); queue.pop(); }
	m.unlock();
	return rval;
}

actionqueue::actionqueue () {
	for (int i = 0; i < WORKER_THREADS_NUM; i++) {
		thread *t = new thread ([&] (actionqueue *q) {
			while (true) {
				this_thread::sleep_for (chrono::milliseconds(WORKER_UPDATE_DELAY));
				charge_i i = q->pop();
				if (i.chargee != nullptr) {
					i.chargee->update(i.charge);
				}
			}
		}, this);
		threads.push_back (t);
	}
}
