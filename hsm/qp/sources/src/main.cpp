#include <iostream>
#include "qf_port.h"
#include <stdlib.h>

using namespace std;

enum Signals {
	PROCEED_SIG = Q_USER_SIG,
	CANCEL_SIG,
};

struct ProceedEvt : public QEvent {
	ProceedEvt(int value = 0) : value(value) { sig = PROCEED_SIG; }
	int value;
};

struct CancelEvt : public QEvent {
	CancelEvt() { sig = CANCEL_SIG; }
};

class Hsm : public QHsm {
	public:
		Hsm() : QHsm((QStateHandler)&initial) { init(); }

	private:
		static QState initial(Hsm* me, const QEvent* e) {
			return Q_TRAN(&superState);
		}

		static QState superState(Hsm* me, const QEvent* e) {
			switch (e->sig) {
				case Q_ENTRY_SIG: {
					me->count = 10;
					return Q_HANDLED();
				}
				case Q_INIT_SIG:  {
					return Q_TRAN(&stateA);
				}
				case CANCEL_SIG: {
					return Q_TRAN(&stateC);
				}
			}
			return Q_SUPER(&QHsm::top);
		}

		static QState stateA(Hsm* me, const QEvent* e) {
			switch (e->sig) {
				case PROCEED_SIG: {
					return Q_TRAN(&stateB);
				}
			}
			return Q_SUPER(&superState);
		}

		static QState stateB(Hsm* me, const QEvent* e) {
			switch (e->sig) {
				case PROCEED_SIG: {
					if (me->count > 1) {
						me->count *= 2;
						return Q_TRAN(&stateA);
					}
					++me->count;
					return Q_HANDLED();
				}
				case Q_EXIT_SIG: {
					cout << "count = " << me->count << endl;
					me->count = 0;
					return Q_HANDLED();
				}
			}
			return Q_SUPER(&superState);
		}

		static QState stateC(Hsm* me, const QEvent* e) {
			switch (e->sig) {
				case PROCEED_SIG: {
					if (static_cast<const ProceedEvt*>(e)->value == 1) {
						return Q_TRAN(&superState);
					}
					break;
				}
			}
			return Q_SUPER(&QHsm::top);
		}

		int count;
};

// this function is used by the QP embedded systems-friendly assertions
extern "C" void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    cout << "Assertion failed in " << file << " , line " << line << endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	Hsm sm; // stateA, count = 10

	for (int i = 0; i < 2; ++i) {
		sm.dispatch(&ProceedEvt());		// stateB, count = 10
		sm.dispatch(&ProceedEvt());		// stateA, count = 0
		sm.dispatch(&ProceedEvt());		// stateB, count = 0
		sm.dispatch(&ProceedEvt());		// stateB, count = 1
		sm.dispatch(&ProceedEvt());		// stateB, count = 2
		sm.dispatch(&ProceedEvt());		// stateA, count = 0
		sm.dispatch(&ProceedEvt());		// stateB, count = 0
		sm.dispatch(&ProceedEvt());		// stateB, count = 1
		sm.dispatch(&ProceedEvt());		// stateB, count = 2
		sm.dispatch(&CancelEvt());		// stateC, count = 2
		sm.dispatch(&CancelEvt());		// stateC, count = 2
		sm.dispatch(&ProceedEvt());		// stateC, count = 2
		sm.dispatch(&ProceedEvt(1));	// stateA, count = 10
	}

	return 0;
}
