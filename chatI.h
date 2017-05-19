#ifndef __chatI_h__
#define __chatI_h__

#include <chat.h>
#include <Ice/ObjectAdapter.h>

namespace Chat
{

class ChatServerI : virtual public ChatServer
{
private:
	Users userList;
	Groups chats;
	Managers servers;
public:

    virtual ::Chat::UserPrx LogIn(const ::std::string&,
                                  const Ice::Current&);

    virtual ::Chat::UserPrx getUserByName(const ::std::string&,
                                          const Ice::Current&);

    virtual ::Chat::Groups GroupList(const Ice::Current&);

    virtual ::Chat::GroupServerPrx getGroupServerByName(const ::std::string&,
                                                        const Ice::Current&);

    virtual void CreateGroup(const ::std::string&,
                             const Ice::Current&);

    virtual void DeleteGroup(const ::std::string&,
                             const Ice::Current&);

    virtual void registerServer(const ::Chat::GroupServerManagerPrx&,
                                const Ice::Current&);

    virtual void unregisterServer(const ::Chat::GroupServerManagerPrx&,
                                  const Ice::Current&);

    virtual void LeaveChat(const ::Chat::UserPrx&,
    						const Ice::Current&);
};

class GroupServerI : virtual public GroupServer
{
private: 
	Users usersInGroup;
	std::vector<std::string> textInChat;
	int id;
	std::string id_Name_Text;
	bool wasAnyUser;
	std::string chatName;
public:

	GroupServerI(const std::string& name);

    virtual void join(const ::Chat::UserPrx&,
                      const Ice::Current&);

    virtual void Leave(const ::Chat::UserPrx&,
                       const Ice::Current&);

    virtual void SendMessage(const ::std::string&,
                             const ::Chat::UserPrx&,
                             const Ice::Current&);

    virtual ::Chat::Users UserList(const Ice::Current&);

    virtual ::std::string Name(const Ice::Current&);

    virtual bool getWasAnyUser(const Ice::Current&);
};

class GroupServerManagerI : virtual public GroupServerManager
{
private:
	std::string managerName;
	Groups groupsInManager;
public:

	GroupServerManagerI(const std::string&);

    virtual ::Chat::GroupServerPrx CreateGroup(const ::std::string&,
                                               const Ice::Current&);

    virtual ::Chat::Groups ListGroups(const Ice::Current&);

    virtual void DeleteGroup(const ::std::string&,
                             const Ice::Current&);

    virtual ::Chat::GroupServerPrx getGroupServerByName(const ::std::string&,
                                                        const Ice::Current&);
};

class UserI : virtual public User
{
	::std::string myName;
	::std::string message;
	::std::string messagePrivate;
public:
	UserI(const ::std::string& name);

    virtual ::std::string getName(const Ice::Current&);

    virtual void receiveText(const ::std::string&,
                             const ::Chat::UserPrx&,
                             const ::std::string&,
                             const Ice::Current&);

    virtual void receivePrivateText(const ::std::string&,
                                    const ::Chat::UserPrx&,
                                    const Ice::Current&);
    virtual ::std::string saveText(const Ice::Current&);

    virtual ::std::string savePrivateText(const Ice::Current&);
};

//class MyCallback : public IceUtil::Shared
//{
//public:
//	void receiveText(const Ice::AsyncResultPtr&);
//	void receivePrivateText(const Ice::AsyncResultPtr&);
//	private:
//	void handleException(const Ice::Exception&);
//
//};
//typedef IceUtil::Handle<MyCallback> MyCallbackPtr;
}
#endif
