
#include <chatI.h>

::Chat::UserPrx
Chat::ChatServerI::LogIn(const ::std::string& name,
                         const Ice::Current& current)
{
	UserPrx testUser = this->getUserByName(name,current);
	if(testUser == 0)
	{
		std::cout << "User: " << name << " login/created!" << std::endl;
		UserPtr servant = new UserI(name);
		Chat::UserPrx user = UserPrx::uncheckedCast(current.adapter->addWithUUID(servant));
		userList.push_back(user);
		std::cout << "Users on Server:" << std::endl;
		for(int i=0;i<userList.size();i++){
			std::cout << userList[i]->getName() << std::endl;
		}
		return user;
	}else
	{
		throw NameAlreadyExists();
	}
}

::Chat::UserPrx
Chat::ChatServerI::getUserByName(const ::std::string& name,
                                 const Ice::Current& current)
{
	try
	{
		for(int i=0;i<userList.size();i++){
			if(userList[i]->getName() == name)
			{
				return userList[i]; // mysle ze ok
			}
		}
	}catch (IceUtil::NullHandleException e) {
		std::cout << "pomylka usera, nie ma takiego usera" << std::endl;
	}
    return 0;
}

::Chat::Groups
Chat::ChatServerI::GroupList(const Ice::Current& current)
{
    return chats;
}

::Chat::GroupServerPrx
Chat::ChatServerI::getGroupServerByName(const ::std::string& name,
                                        const Ice::Current& current)
{
	for(int i=0;i<chats.size();i++){
		if(chats[i]->Name() == name)
		{
			return chats[i]; // mysle ze ok
		}
	}
}

void
Chat::ChatServerI::CreateGroup(const ::std::string& name,
                               const Ice::Current& current)
{
	GroupServerPtr servant = new GroupServerI(name);
	Chat::GroupServerPrx newGroup = GroupServerPrx::uncheckedCast(current.adapter->addWithUUID(servant));
	chats.push_back(newGroup);
}

void
Chat::ChatServerI::DeleteGroup(const ::std::string& name,
                               const Ice::Current& current)
{
	for(int i=0;i<chats.size();i++){
		if(chats[i]->Name() == name)
		{
			chats.erase(chats.begin()+i); // mysle ze ok, sprawdzic czy dobry lement usuwa +-1
		}
	}
}

void
Chat::ChatServerI::registerServer(const ::Chat::GroupServerManagerPrx& serverManager,
                                  const Ice::Current& current)
{
	managerServers.push_back(serverManager);
}

void
Chat::ChatServerI::unregisterServer(const ::Chat::GroupServerManagerPrx& serverManager,
                                    const Ice::Current& current)
{
	for(int i=0;i<managerServers.size();i++){
		if(managerServers[i] == serverManager)
		{
			managerServers.erase(managerServers.begin()+i); // mysle ze ok, sprawdzic czy dobry lement usuwa +-1
		}
	}
}

void
Chat::ChatServerI::LeaveChat(const ::Chat::UserPrx& sender, const Ice::Current&)
{
	for(int i=0;i<userList.size();i++){
		if(userList[i]->getName() == sender->getName())
		{
			userList.erase(userList.begin()+i); // mysle ze ok, sprawdzic czy dobry lement usuwa +-1
		}
	}
}

Chat::GroupServerI::GroupServerI(const std::string& name) : id(0)
{
	 chatName = name;
	 id_Name_Text = "";
}

void
Chat::GroupServerI::join(const ::Chat::UserPrx& who,
                         const Ice::Current& current)
{
	usersInGroup.push_back(who);
	std::cout << "User: " << who->getName() << "joined server" << std::endl;
	std::cout << "Users in group:" << std::endl;
	for(int i=0;i<usersInGroup.size();i++){
		std::cout << usersInGroup[i]->getName() << std::endl;
	}
}

void
Chat::GroupServerI::Leave(const ::Chat::UserPrx& who,
                          const Ice::Current& current)
{
	for(int i=0;i<usersInGroup.size();i++){
		if(usersInGroup[i] == who)
		{
			usersInGroup.erase(usersInGroup.begin()+i); // mysle ze ok, sprawdzic czy dobry lement usuwa +-1
			std::cout << who->getName() << std::endl;
		}
	}
}

void
Chat::GroupServerI::SendMessage(const ::std::string& message,
                                const ::Chat::UserPrx& sender,
                                const Ice::Current& current)
{
	std::cout << sender->getName() << std::endl;
//	textInChat.push_back(sender->getName() + " napisal: " + message);
	id_Name_Text += (id++) + "|"; id_Name_Text += sender->getName() + ": "; id_Name_Text += message + "\n";
	for(int i=0;i<usersInGroup.size();i++){
		usersInGroup[i]->receiveText(message,sender,this->chatName);
	}
}

::Chat::Users
Chat::GroupServerI::UserList(const Ice::Current& current)
{
    return usersInGroup;
}

::std::string
Chat::GroupServerI::Name(const Ice::Current& current)
{
    return chatName;
}

Chat::GroupServerManagerI::GroupServerManagerI(const std::string& name) : managerName(name)
{

}

::Chat::GroupServerPrx
Chat::GroupServerManagerI::CreateGroup(const ::std::string& name, // tu jest cos pomieszane, tworzymy grupe ale zwracamy cala liste grup. Chyba wiem!: tworzy jeden cat i go zwraca
                                       const Ice::Current& current)
{
	if(this->getGroupServerByName(name, current) == 0)
	{
		GroupServerPtr servant = new GroupServerI(name);
		Chat::GroupServerPrx newGroup = GroupServerPrx::uncheckedCast(current.adapter->addWithUUID(servant));
		groupsInManager.push_back(newGroup);
		return newGroup;
	}else
	{
		throw NameAlreadyExists();
	}
}

::Chat::Groups
Chat::GroupServerManagerI::ListGroups(const Ice::Current& current)
{
	for(int i=0;i<groupsInManager.size();i++){
		groupsInManager[i]->Name();
	}
    return groupsInManager;
}

void
Chat::GroupServerManagerI::DeleteGroup(const ::std::string& name,
                                       const Ice::Current& current)
{
	for(int i=0;i<groupsInManager.size();i++){
		if(groupsInManager[i]->Name() == name)
		{
			groupsInManager.erase(groupsInManager.begin()+i); // mysle ze ok, sprawdzic czy dobry lement usuwa +-1
		}
	}
}

::Chat::GroupServerPrx
Chat::GroupServerManagerI::getGroupServerByName(const ::std::string& name,
                                                const Ice::Current& current)
{
	try
	{
		for(int i=0;i<groupsInManager.size();i++){
			if(groupsInManager[i]->Name() == name)
			{
				return groupsInManager[i];
			}
		}
	} catch(IceUtil::NullHandleException)
	{
		std::cout << "user sie pomylil" << std::endl;
	}
    return 0;
}

Chat::UserI::UserI(const ::std::string& name)
{
	myName = name;
	message = "";
	messagePrivate = "";
}

::std::string
Chat::UserI::getName(const Ice::Current& current)
{
    return myName;
}

void
Chat::UserI::receiveText(const ::std::string& msg,
                         const ::Chat::UserPrx& sender,
                         const ::std::string& senderServer,
                         const Ice::Current& current)
{
	std::cout<<"Z czatu: " + senderServer + ":" + sender->getName() + " napisal: " + msg << std::endl;
	message += (senderServer + "@" + sender->getName() + ": " + msg + "\n");
	//sender->receiveText(msg, sender, senderServer);
}

void
Chat::UserI::receivePrivateText(const ::std::string& msg,
                                const ::Chat::UserPrx& sender,
                                const Ice::Current& current)
{
	std::cout<<sender->getName() + " napisal: " + msg << std::endl;
	messagePrivate += (sender->getName() + ": " + msg);
}

::std::string
Chat::UserI::saveText(const Ice::Current& current)
{
	std::string msg = message;
	message = "";
	return msg;
}

::std::string
Chat::UserI::savePrivateText(const Ice::Current& current)
{
	std::string msg = messagePrivate;
	messagePrivate = "";
	return msg;
}
//void
//Chat::MyCallback::receiveText(const Ice::AsyncResultPtr& r)
//{
//	UserPrx e = UserPrx::uncheckedCast(r->getProxy());
//	try
//	{
//		std::string text = e->end_receiveText(r);
//		std::cout << "receiveText: " << text << std::endl;
//	}
//	catch(const Ice::Exception& ex)
//	{
//		handleException(ex);
//	}
//}
//void
//Chat::MyCallback::receivePrivateText(const Ice::AsyncResultPtr& r)
//{
//
//}


