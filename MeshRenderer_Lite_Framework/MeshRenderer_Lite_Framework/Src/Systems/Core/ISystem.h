#pragma once

#include <string>
using std::string;

//message base class - each system may interpret the message however they want
//You may create new Message classes that inherit from this
enum MessageType : unsigned int
{
	/*
	Include all of the message types that you need here...
	*/
	ReinitializeScene,
	UnloadScene,
	WindowResized,
	ApplyColorMask,
	MessageType_count
};

class IMessage
{
public:
	IMessage(const MessageType mt) :m_messageType(mt) {}
	virtual ~IMessage() {}

	const MessageType GetMessageType()const
	{
		return m_messageType;
	}

protected:
	const MessageType m_messageType;
};

class Engine;

class ISystem
{
public:
	ISystem(Engine* eng) :m_isInitialized(false), m_engineOwner(eng) {}
	virtual ~ISystem() {}
	virtual bool Initialize() = 0;
	virtual void Update(const float dt) = 0;
	virtual void Shutdown() = 0;
	virtual const string& GetName()const { return m_systemName; }
	virtual bool IsInitialized()const { return m_isInitialized; }
	virtual void ReceiveMessage(const IMessage& msg) = 0;

protected:
	string m_systemName;
	bool m_isInitialized;
	Engine* m_engineOwner;
};