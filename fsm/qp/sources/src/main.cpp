#include <iostream>
#include "qf_port.h"
#include <stdlib.h>

using namespace std;

enum Signals {
	EVENT1_SIG = Q_USER_SIG,
	EVENT2_SIG,
};

class Event1 : public QEvent {
	public:
		Event1() { sig = EVENT1_SIG; }
};
class Event2 : public QEvent {
	public:
		Event2() { sig = EVENT2_SIG; }
};

class Fsm : public QFsm {
	public:
		Fsm() : QFsm((QStateHandler)&initial) { init(); }

	private:
		static QState initial(Fsm* me, const QEvent* e) {
			return Q_TRAN(&A);
		}

		static QState A(Fsm* me, const QEvent* e) {
			printState("A", e);
			switch(e->sig) {
				case EVENT1_SIG: {
					/* do_A_onEvent1() */
					return Q_TRAN(&B);
				}
				case EVENT2_SIG: {
					/* do_A_onEvent2() */
					return Q_TRAN(&A);
				}
			}
			return Q_HANDLED();
		}

		static QState B(Fsm* me, const QEvent* e) {
			printState("B", e);
			switch(e->sig) {
				case EVENT2_SIG: {
					if (guard_B_onEvent2()) {
						/* do_B_onEvent2() */
						return Q_TRAN(&A);
					}
					else {
						return Q_TRAN(&C);
					}
				}
			}
			return Q_HANDLED();
		}

		static QState C(Fsm* me, const QEvent* e) {
			printState("C", e);
			/* ... */
			return Q_HANDLED();
		}

		static QState D(Fsm* me, const QEvent* e) {
			printState("D", e);
			/* ... */
			return Q_HANDLED();
		}

		static bool guard_B_onEvent2() {
			return true;
		}

		static void printState(const char* title, const QEvent* e) {
			static const char* e_name[] = {
				"entry event",
				"exit event",
				"init event",
				"Event1",
				"Event2",
			};

			cout << "State " << title << ", " << e_name[e->sig-1] << endl;
		}
};

// this function is used by the QP embedded systems-friendly assertions
extern "C" void Q_onAssert(char const Q_ROM * const Q_ROM_VAR file, int line) {
    cout << "Assertion failed in " << file << " , line " << line << endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
	Fsm fsm;

	fsm.dispatch(&Event1());
	fsm.dispatch(&Event2());
	fsm.dispatch(&Event2());
	fsm.dispatch(&Event1());
	fsm.dispatch(&Event1());
	fsm.dispatch(&Event1());

	return 0;
}
