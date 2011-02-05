#include <iostream>
#include <typeinfo>

using namespace std;

class Event { private: virtual void _use_rtti() {} };
class Event1 : public Event { };
class Event2 : public Event { };

class Fsm {
public:
	void dispatch(const Event& e) {
		cout << "State ";
		switch (state) {
			case A:
				dispatchA(e);
				break;
			case B:
				dispatchB(e);
				break;
			case C:
				dispatchC(e);
				break;
			case D:
				dispatchD(e);
				break;
		}
		cout << ". Event " << typeid(e).name() << endl;
	}

private:
	enum { A, B, C, D } state;

	void dispatchA(const Event& e) {
		cout << "A";
		if (typeid(e) == typeid(Event1)) {
			/* do_A_onEvent1() */
			state = B;
		}
		else if (typeid(e) == typeid(Event2)) {
			/* do_A_onEvent2() */
		}
	}

	void dispatchB(const Event &e) {
		cout << "B";
		if (typeid(e) == typeid(Event2)) {
			if (guard_B_onEvent2()) {
				/* do_B_onEvent2() */
				state = A;
			}
			else {
				state = C;
			}
		}
	}

	void dispatchC(const Event &e) {
		cout << "C";
		/* ... */
	}

	void dispatchD(const Event &e) {
		cout << "D";
		/* ... */
	}

	bool guard_B_onEvent2() { }
};

int main() {
	Fsm fsm;

	Event* e = new Event2();

	cout << typeid(*e).name() << endl;

	for (int i = 0; i < 3; ++i) {
		fsm.dispatch(Event1());
		fsm.dispatch(Event2());
	}

	return 0;
}
