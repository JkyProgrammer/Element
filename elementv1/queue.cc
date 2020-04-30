#include <iostream>

#include "element.h"

void actionqueue::push (charge_i c) {
	m.lock();
	queueobj.push (c);
	m.unlock();
}

charge_i actionqueue::pop () {
	charge_i rval = charge_i (nullptr, -1);
	m.lock();
	if (!queueobj.empty()) { rval = queueobj.front(); queueobj.pop(); }
	m.unlock();
	return rval;
}

actionqueue::actionqueue () {
	for (int j = 0; j < WORKER_THREADS_NUM; j++) {
		thread *t = new thread ([&] (actionqueue *q, int j) {
			while (true) {
				this_thread::sleep_for (chrono::milliseconds(WORKER_UPDATE_DELAY));
				charge_i i = q->pop();
				if (i.chargee != nullptr) {
					i.chargee->update(i.charge);
				}
			}
		}, this, j);
		threads.push_back (t);
	}
}
