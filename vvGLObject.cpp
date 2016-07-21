#include "vvGLObject.h"

#include <iostream>
#include <cstdlib>

//------------------------------------------------------------------------------
vvGLObject::vvGLObject()
  : Superclass(/*autoInit=*/ false)
{
}

//------------------------------------------------------------------------------
vvGLObject::~vvGLObject()
{
}

//------------------------------------------------------------------------------
void vvGLObject::init(const vvApplicationState &)
{
}

//------------------------------------------------------------------------------
void vvGLObject::initContext(GLContextData &) const
{
  std::cerr << "vvGLObject::initContext should never be called. Use "
               "vvGLObject::initVvContext instead." << std::endl;
  abort();
}

//------------------------------------------------------------------------------
void vvGLObject::initVvContext(vvContextState &, GLContextData &) const
{
}

//------------------------------------------------------------------------------
void vvGLObject::syncApplicationState(const vvApplicationState &)
{
}

//------------------------------------------------------------------------------
void vvGLObject::syncContextState(const vvApplicationState &,
                                  const vvContextState &,
                                  GLContextData &) const
{
}
