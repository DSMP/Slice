#include <Ice/Ice.h>
#include <chatI.h>
#include <pthread.h>
using namespace std;
using namespace Chat;

void listGroups(::Chat::Groups);
void * readMessageFunction(void*);
bool killThread = false;
UserPrx user;
Ice::CommunicatorPtr ic;
pthread_t readMessages;
ChatServerPrx chatServer;
GroupServerManagerPrx groupServerManager;

int main(int argc, char* argv[])
{
	int status = 0;
	try {
		ic = Ice::initialize(argc, argv);
		Ice::ObjectPrx serverBase = ic->stringToProxy("ChatServer:default -p 10000");
		Ice::ObjectPrx groupBase = ic->stringToProxy("GroupServerManager:default -p 10000");
		chatServer = ChatServerPrx::checkedCast(serverBase);
		if (!chatServer)
			throw "Invalid proxy chat";
		groupServerManager = GroupServerManagerPrx::checkedCast(groupBase);
		if(!groupServerManager)
			throw "Invalid proxy group";
        cout << "Podaj swoj nickname: ";
		string name;
		cin >> name;
//		MyCallbackPtr cb = new MyCallback;
//			Ice::CallbackPtr receiveTextCB = Ice::newCallback(cb, &MyCallback::receiveText);
		user = chatServer->LogIn(name);
//		groupServer->join(user);
//		groupServer->SendMessage("lol", user);
		pthread_create(&readMessages,NULL,readMessageFunction, NULL);
		string nameOfCzat;
		GroupServerPrx searchedCzat;
		string textToSend;
		while(true)
		{
			if(nameOfCzat != "")
			{
				cout << "Należysz do czatu: " << nameOfCzat << endl;
			}
			char c;
			cout << "MainMenu:\n1 - wyszukaj/dolacz do grupy\n2 - wyszukaj osobe\n3 - " <<
					"stworz czat\n4 - opusc czat\n5 - napisz do czatu\nq - wyjscie"; //4 - odbierz prywatna wiadomosc\n5 - odbierz wiadomosc z grupy\n
			cin >> c;
			switch(c)
			{
				case '1':
				{
					cout << "wylistować wszystkie? (T/N) "; cin >> c; cout << endl;
					if(c == 'T' || c == 't')
					{
						listGroups(groupServerManager->ListGroups());
						cout << "Chcesz dolaczyc do ktoregos? (T/N) "; cin >> c; cout << endl;
					}
					else {c = 'T';}
					if(c == 'T' || c == 't')
					{
						cout << "Podaj nazwe czatu: ";
						cin >> nameOfCzat; cout << endl;
						searchedCzat = groupServerManager->getGroupServerByName(nameOfCzat);
						searchedCzat->join(user);
					}
					break;
				}
				case '2':
				{
					string nameOfUser;
					cout << "Podaj nick osoby: ";
					cin >> nameOfUser; cout << endl;
					UserPrx foundUser = chatServer->getUserByName(nameOfUser);
					cout << foundUser->getName() << endl;
					cout << "Czy chcesz wyslac do niego wiadomosc? T";
					cin >> c; cout << endl;
					cout.flush();
					if (c == 'T' || c == 't')
					{
						cout << "Podaj tekst ktory chcesz wyslac:\n";
						cout.flush();
						getline(cin,textToSend);
						getline(cin,textToSend);
						foundUser->receivePrivateText(textToSend, user);
					}

					break;
				}
				case '3':
				{
					string MyGroupName;
					cout << "Podaj nazwe grupy: "; cin >> MyGroupName; cout << endl;
					groupServerManager->CreateGroup(MyGroupName);
					break;
				}
//				case '4':
//				{
//					cout << user->savePrivateText() << endl;
//					break;
//				}
//				case '5':
//				{
//					cout << user->saveText() << endl;
//					break;
//				}
				case '4':
				{
					if(searchedCzat != NULL)
					{
						searchedCzat->Leave(user);
						cout << "opusciles wlasnie czat" << endl;
						nameOfCzat = "";
						searchedCzat = NULL;
						break;
					}
					else
						cout << "Nie mozesz opuscic czatu, bo juz nie nalezysz do grupy" << endl;
				}
				case '5':
				{
					if(searchedCzat != NULL)
					{
						cout.flush();
						getline(cin,textToSend);
						getline(cin,textToSend);
						searchedCzat->SendMessage(textToSend,user);
						break;
					}
				}
				case 'q':
				{
					killThread = true;
					break;
				}
				default:
					cout << "wybierz z zakresu 1-5" << endl;
			}
		}
	} catch (const Ice::Exception& ex) {
		cerr << ex << endl;
		status = 1;
	} catch (const char* msg) {
		cerr << msg << endl;
		status = 1;
	}
	if (ic)
		ic->destroy();

	pthread_join(readMessages, NULL);
	return status;
}

void listGroups(::Chat::Groups groups)
{
	for(int i=0 ; i< groups.size();i++)
	{
		cout <<  i+1 << ": " << groups[i]->Name() << endl;
	}
}
void * readMessageFunction(void* args)
{
	while(true)
	{
		if (killThread == true)
			break;
		if(user->saveText() != "")
			cout << user->saveText() << endl;
		if(user->savePrivateText() != "")
		{
			cout << "lolek " << user->savePrivateText()  << endl;
		}
	}
}




