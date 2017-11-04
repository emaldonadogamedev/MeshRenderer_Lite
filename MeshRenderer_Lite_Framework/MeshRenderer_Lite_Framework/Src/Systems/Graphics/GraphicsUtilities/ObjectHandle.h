#pragma once

#include<Systems/Graphics/GraphicsUtilities/GraphicsTypes.h>

class ObjectHandle
{
public:
  ObjectHandle();
  ObjectHandle(ObjectType type, int id);
  ~ObjectHandle();

  ObjectType GetType()const;
  int GetHandleID()const;

  //Conversion operator
  operator bool(void) const;

  //Operator overloads
  const int operator*(void) const;
  bool operator==(const ObjectHandle& rhs) const;
  bool operator!=(const ObjectHandle& rhs) const;

  //Returning a null handle
  static const ObjectHandle& Null(void);

  //Setters
  void SetType(ObjectType type);
  void SetHandleID(int handle);

private:
  ObjectType m_objType;
  int m_handleId;

  static const ObjectHandle s_nullHandle;
};