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

	const MessageType GetMessageType() const
	{
		return m_messageType;
	}

protected:
	const MessageType m_messageType;
};

class IComponent;
class IEngine;

enum class SystemType : char
{
	ST_WINDOW,
	ST_GRAPHICS,
	ST_INPUT,

	//Implement these later on
	//ST_PHYSICS,
	//ST_SCRIPTING, 

	COUNT
};

class ISystem
{
public:
	ISystem(const SystemType type, IEngine* const eng) 
		: m_systemType(type)
		, m_engineOwner(eng)
		, m_isInitialized(false)
		{}
	virtual ~ISystem() {}

	virtual bool Initialize() = 0;
	virtual void Update(const float dt) = 0;
	virtual void Shutdown() = 0;
	
	const SystemType GetType()const { return m_systemType; }
	bool IsInitialized()const { return m_isInitialized; }
	virtual void ReceiveMessage(const IMessage& msg) = 0;
	virtual void AddComponent(IComponent* component) {}

protected:
	const SystemType m_systemType;
	bool m_isInitialized;
	IEngine* const m_engineOwner;
};