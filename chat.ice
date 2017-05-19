module Chat {

interface GroupServer;
interface GroupServerManager;
interface User;

exception NameDoesNotExist{};
exception NameAlreadyExists{};
exception ServerAlreadyRegistered{};
exception ServerDoesNotExist{};
exception UserAlreadyRegistered{};
exception UserDoesNotExist{};

sequence<GroupServerManager*> Managers;
sequence<GroupServer*> Groups;
sequence<User*> Users; 

interface ChatServer {
	User* LogIn(string name) throws NameAlreadyExists;
	User* getUserByName(string name);
	Groups GroupList();
	GroupServer* getGroupServerByName(string name) throws NameDoesNotExist;
	void CreateGroup(string name) throws NameAlreadyExists;
	void DeleteGroup(string name) throws NameDoesNotExist;
	void registerServer(GroupServerManager* serverManager) throws ServerAlreadyRegistered;
	void unregisterServer(GroupServerManager* serverManager) throws ServerDoesNotExist;
	void LeaveChat(User* sender);
	
};

interface GroupServer {
	void join(User* who)  throws UserAlreadyRegistered;
	void Leave(User* who) throws UserDoesNotExist;
	void SendMessage(string message, User* sender) throws UserDoesNotExist;
	Users UserList();
	string Name();
	bool getWasAnyUser();
};

interface GroupServerManager {
	GroupServer* CreateGroup(string name) throws NameAlreadyExists;
	Groups ListGroups();
	void DeleteGroup(string name) throws NameDoesNotExist;
	GroupServer* getGroupServerByName(string name) ;
};

interface User {
	string getName();
	void receiveText(string msg, User* sender, string serverSender);
	void receivePrivateText(string msg, User* sender);
	string saveText();
	string savePrivateText();
};

};
