#include <iostream>
#include <typeinfo>

using namespace std;

class Event { private: virtual void _use_rtti() {} };

template <class A> class Automaton {
	public:
		void dispatch(const Event& e) {
			state = state.invoke(static_cast<A*>(this), e);
		}

	protected:
		class State {
			public:
				typedef State (A::*Handler)(const Event&);

				State(const Handler& s) : dispatch(s) { }

				State invoke(A* au, const Event& e) const {
					return (au->*dispatch)(e);
				}
		
			private:
				Handler dispatch;
		};
 
		Automaton(const State& s) : state(s) { }

	private:
		State state;
};

class Event1 : public Event { };
class Event2 : public Event { };

class Fsm : public Automaton<Fsm> {
	public:
		Fsm() : Automaton<Fsm>(State(&Fsm::A)) { }

	private:
		State A(const Event& e) {
			printState("A", e);
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
			printState("B", e);
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
			printState("C", e);
			/* ... */
			return State(&Fsm::C);
		}

		State D(const Event &e) {
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
