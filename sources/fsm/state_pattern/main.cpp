#include <iostream>
#include <typeinfo>

using namespace std;

class Event { private: virtual void _use_rtti() {} };

template <class Automaton> class CommonState {
public:
	typedef CommonState (Automaton::*Handler)(const Event&);

	CommonState(const Handler& s) : dispatch(s) { }

	CommonState invoke(Automaton* au, const Event& e) const {
		return (au->*dispatch)(e);
	}
		
private:
	Handler dispatch;
};

class Event1 : public Event { };
class Event2 : public Event { };

class Fsm {
public:
	Fsm() : state(&Fsm::A) { }

	void dispatch(const Event& e) {
		cout << "State ";
		state = state.invoke(this, e);
		cout << ". Event " << typeid(e).name() << endl;
	}

private:
	typedef CommonState<Fsm> State;

	State state;

	State A(const Event& e) {
		cout << "A";
		if (typeid(e) == typeid(Event1)) {
			/* do_A_onEvent1() */
			return State(&Fsm::B);
		}
		else if (typeid(e) == typeid(Event2)) {
			/* do_A_onEvent2() */
		}
		return State(&Fsm::A);
	}

	State B(const Event &e) {
		cout << "B";
		if (typeid(e) == typeid(Event2)) {
			if (guard_B_onEvent2()) {
				/* do_B_onEvent2() */
				return State(&Fsm::A);
			}
			else {
				return State(&Fsm::C);
			}
		}
		return State(&Fsm::B);
	}

	State C(const Event &e) {
		cout << "C";
		/* ... */
		return State(&Fsm::C);
	}

	State D(const Event &e) {
		cout << "D";
		/* ... */
		return State(&Fsm::D);
	}

	bool guard_B_onEvent2() {
		return true;
	}
};

int main() {
	Fsm fsm;

	fsm.dispatch(Event1());
	fsm.dispatch(Event2());
	fsm.dispatch(Event2());
	fsm.dispatch(Event1());
	fsm.dispatch(Event1());
	fsm.dispatch(Event1());

	return 0;
}
