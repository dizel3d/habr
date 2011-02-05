#include <iostream>
#include <typeinfo>

using namespace std;

/* common event type */
class Event { private: virtual void useRTTI() {} };

/* common automaton type */
template <class SM> class StateMachine {
	public:
		void dispatch(const Event& e) {
			state = (static_cast<SM*>(this)->*state.handler)(e);
		}

	protected:
		struct State {
			typedef State (SM::*Handler)(const Event&);
			State(const Handler& s) : handler(s) { }
			Handler handler;
		};

		StateMachine(const State& s) : state(s) { }

	private:
		State state;
};

class Event1 : public Event { };
class Event2 : public Event { };

class Fsm : public StateMachine<Fsm> {
	public:
		Fsm() : StateMachine<Fsm>(State(&Fsm::A)) { }

	private:
		State A(const Event& e) {
			printState("A", e);
			if (dynamic_cast<const Event1*>(&e)) {
				/* do_A_onEvent1() */
				return State(&Fsm::B);
			}
			else if (dynamic_cast<const Event2*>(&e)) {
				/* do_A_onEvent2() */
			}
			return State(&Fsm::A);
		}

		State B(const Event& e) {
			printState("B", e);
			if (dynamic_cast<const Event2*>(&e)) {
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

		State C(const Event& e) {
			printState("C", e);
			/* ... */
			return State(&Fsm::C);
		}

		State D(const Event& e) {
			printState("D", e);
			/* ... */
			return State(&Fsm::D);
		}

		bool guard_B_onEvent2() {
			return true;
		}

		void printState(const char* title, const Event& e) {
			cout << "State " << title << ". Event " << typeid(e).name() << endl;
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
