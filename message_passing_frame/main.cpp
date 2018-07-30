#include <iostream>
#include <thread>
#include "bank_machine.cpp"
#include "interface_machine.cpp"
#include "atm.cpp"
using namespace std;

int main(int argc, char *argv[]){
	BankMachine bank;
	InterfaceMachine interface_machine;
	Atm atm(bank.get_sender(),interface_machine.get_sender());
	thread bank_thread(&BankMachine::run,&bank);
	thread interface_machine_thread(&InterfaceMachine::run,&interface_machine);
	thread atm_thread(&Atm::run,&atm);
	
	messaging::Sender atmqueue(atm.get_sender());
	bool quit_pressed = false;
	while(!quit_pressed){
		char c = getchar();
		switch(c){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				atmqueue.send(digit_pressed(c));
				break;
			case 'b':
				atmqueue.send(balance_pressed());
				break;
			case 'w':
				atmqueue.send(withdraw_pressed(50));
				break;
			case 'c':
				atmqueue.send(cancel_pressed());
				break;
			case 'q':
				quit_pressed = true;
				break;
			case 'i':
				atmqueue.send(card_inserted("acc1234"));
				break;
		}
		bank.done();
		atm.done();
		interface_machine.done();
		atm_thread.join();
		bank_thread.join();
		interface_machine_thread.join();
	}
}
