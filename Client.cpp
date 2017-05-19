#include <Ice/Ice.h>
#include <chatI.h>
#include <pthread.h>
#include <signal.h>
using namespace std;
using namespace Chat;

void listGroups(::Chat::Groups);
void listUsersInGroup(::Chat::Users);
void * readMessageFunction(void*);
void sigint(int);
bool killThread = false;
UserPrx user;
Ice::CommunicatorPtr ic;
pthread_t readMessages;
ChatServerPrx chatServer;
GroupServerManagerPrx groupServerManager;

int main(int argc, char* argv[])
{
	int status = 0;
	signal(SIGINT,sigint);
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
		bool NameOk = false;
		do
		{
			cout << "Podaj swoj nickname: ";
			string name;
			cin >> name;
	//		MyCallbackPtr cb = new MyCallback;
	//			Ice::CallbackPtr receiveTextCB = Ice::newCallback(cb, &MyCallback::receiveText);
			try
			{
				user = chatServer->LogIn(name);
				NameOk = true;
			}
			catch (NameAlreadyExists e) {
				cout << "imie juz takie istnieje" << endl;
				NameOk = false;
			}
		} while(NameOk == false);
//		groupServer->join(user);
//		groupServer->SendMessage("lol", user);
		pthread_create(&readMessages,NULL,readMessageFunction, NULL);
		string nameOfCzat;
		GroupServerPrx searchedCzat;
		string textToSend;
		while(killThread == false)
		{
			cout << "Twoj nick to: " << user->getName() << endl;
			if(nameOfCzat != "")
			{
				cout << "Należysz do czatu: " << nameOfCzat << endl;
			}
			char c;
			cout << "MainMenu:\n1 - wyszukaj/dolacz do grupy\n2 - wyszukaj osobe\n3 - " <<
					"stworz czat\n4 - opusc czat\n5 - napisz do czatu\nq - wyjscie\n"; //4 - odbierz prywatna wiadomosc\n5 - odbierz wiadomosc z grupy\n
			if(nameOfCzat != "")
			{
				cout << "6 - Wyswietl liste osob na tym czacie" << endl;
			}
			cout << "Wybierz: ";
			cin >> c; cout << endl;
			switch(c)
			{
				case '1':
				{
					cout << "Wylistować wszystkie? (T/N) "; cin >> c; cout << endl;
					if(c == 'T' || c == 't')
					{
						listGroups(groupServerManager->ListGroups());
						cout << "Chcesz dolaczyc do ktoregos? (T/N) "; cin >> c; cout << endl;
					}
					else {c = 'T';}
					if(c == 'T' || c == 't')
					{
						try
						{
							if(searchedCzat != 0)
								searchedCzat->Leave(user);
							cout << "Podaj nazwe czatu(bez spacji): ";
							cin >> nameOfCzat; cout << endl;
							searchedCzat = groupServerManager->getGroupServerByName(nameOfCzat);
							searchedCzat->join(user);
						}catch(IceUtil::NullHandleException)
						{
							cout << "Czat nie istnieje" << endl;
						}

					}
					break;
				}
				case '2':
				{
					string nameOfUser;
					cout << "Podaj nick osoby: ";
					cin >> nameOfUser; cout << endl;
					UserPrx foundUser = chatServer->getUserByName(nameOfUser);
					if(foundUser == 0)
					{
						cout << "Nie ma takiego usera" << endl;
						break;
					}
					cout << foundUser->getName() << endl;
					cout << "Czy chcesz wyslac do niego wiadomosc? (T/N) ";
					cin >> c; cout << endl;
					cout.flush();
					if (c == 'T' || c == 't')
					{
						cout << "Podaj tekst ktory chcesz wyslac:\n";
						cout.flush();
						getline(cin,textToSend);
						getline(cin,textToSend);
						try{
							foundUser->receivePrivateText(textToSend, user);
							if(foundUser == 0)
							{
								cout << "Nie ma takiej nazwy" << endl;
								break;
							}
						}catch(IceUtil::NullHandleException)
						{
							cout << "Brak takiego uzyszkodnika" << endl;
						}
					}

					break;
				}
				case '3':
				{
					string MyGroupName;
					cout.flush();
					cout << "Podaj nazwe grupy: "; cin >> MyGroupName; cout << endl;
					try
					{
						groupServerManager->CreateGroup(MyGroupName);
					}catch (NameAlreadyExists e) {
						cout << "Juz istnieje taka nazwa" << endl;
					}
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
					if(searchedCzat != 0)
					{
						searchedCzat->Leave(user);
						cout << "opusciles wlasnie czat" << endl;
						nameOfCzat = "";
						searchedCzat = 0;
						break;
					}
					else
						cout << "Nie mozesz opuscic czatu, bo juz nie nalezysz do grupy" << endl;
				}
				case '5':
				{
					if(searchedCzat != 0)
					{
						cout << "Podaj tekst ktory chcesz wyslac:\n";
						cout.flush();
						getline(cin,textToSend);
						getline(cin,textToSend);
						searchedCzat->SendMessage(textToSend, user);
					}
					break;
				}
				case '6':
				{
					listUsersInGroup(searchedCzat->UserList());
					break;
				}
				case 'q':
				{
					killThread = true;
					if(searchedCzat != 0)
					{
						searchedCzat->Leave(user);
					}
					chatServer->LeaveChat(user);
					break;
				}
				default:
					cout << "Wybierz cyfrę z listy" << endl;
			}
		}
	} catch (const Ice::Exception& ex) {
		cerr << ex << endl;
		status = 1;
	} catch (const char* msg) {
		cerr << msg << endl;
		status = 1;
	}
	pthread_join(readMessages, NULL);
	if (ic)
		ic->destroy();

	return status;
}

void listGroups(::Chat::Groups groups)
{
	for(int i=0 ; i< groups.size();i++)
	{
		cout <<  i+1 << ": " << groups[i]->Name() << endl;
	}
}
void listUsersInGroup(::Chat::Users users)
{
	for(int i=0 ; i< users.size();i++)
	{
		cout <<  i+1 << ": " << users[i]->getName() << endl;
	}
}
void * readMessageFunction(void* args)
{
	string Text;
	while(true)
	{
		if (killThread == true)
			break;
		if((Text = user->saveText()) != "")
			cout << endl << Text << endl;
		if((Text = user->savePrivateText()) != "")
		{
			cout << endl << Text << endl;
		}
	}
}
void sigint(int a)
{
	chatServer->LeaveChat(user);
	killThread = true;
	pthread_join(readMessages, NULL);
	if (ic)
			ic->destroy();
	cout << endl;
	exit(0);
}




