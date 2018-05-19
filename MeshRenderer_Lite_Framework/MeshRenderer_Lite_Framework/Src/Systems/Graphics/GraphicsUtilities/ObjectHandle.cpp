#include <Utilities/precompiled.h>
#include <Systems/Graphics/GraphicsUtilities/ObjectHandle.h>

//leave the null handle with the default data from the default constructor
const ObjectHandle ObjectHandle::s_nullHandle;

ObjectHandle::ObjectHandle() : m_objType(ObjectType::NONE), m_handleId(-1)
{

}

ObjectHandle::ObjectHandle(ObjectType type, int id)
	:m_objType(type)
	,m_handleId(id)
{
}

ObjectHandle::~ObjectHandle()
{

}

ObjectType ObjectHandle::GetType(void) const
{
  return m_objType;
}


int ObjectHandle::GetHandleID() const
{
	return m_handleId;
}

ObjectHandle::operator bool(void) const
{
  return (m_objType != ObjectType::NONE && m_handleId != -1);
}

const int ObjectHandle::operator*(void) const
{
  return m_handleId;
}

ObjectHandle& ObjectHandle::operator=(const ObjectHandle& rhs)
{
	m_handleId = rhs.m_handleId;
	m_objType = rhs.m_objType;

	return *this;
}

bool ObjectHandle::operator==(const ObjectHandle& rhs) const
{
  return (m_objType == rhs.m_objType && m_handleId == rhs.m_handleId);
}

bool ObjectHandle::operator!=(const ObjectHandle& rhs) const
{
  return (m_objType != rhs.m_objType || m_handleId != rhs.m_handleId);
}

const ObjectHandle& ObjectHandle::Null(void)
{
  return s_nullHandle;
}

void ObjectHandle::SetType(ObjectType type)
{
  m_objType = type;
}

void ObjectHandle::SetHandleID(int handle)
{
  m_handleId = handle;
}

void ObjectHandle::MakeNull()
{
	m_objType = ObjectType::NONE;
	m_handleId = -1;
}
