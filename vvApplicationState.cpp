#include "vvApplicationState.h"

#include "vvFramerate.h"
#include "vvProgress.h"

//------------------------------------------------------------------------------
vvApplicationState::vvApplicationState()
  : m_framerate(new vvFramerate),
    m_progress(new vvProgress)
{
  m_objects.push_back(m_framerate);
  m_objects.push_back(m_progress);
}

//------------------------------------------------------------------------------
vvApplicationState::~vvApplicationState()
{
  delete m_framerate;
  delete m_progress;
}

//------------------------------------------------------------------------------
void vvApplicationState::init()
{
  for (auto object : m_objects)
    {
    object->init(*this);
    }
}

//------------------------------------------------------------------------------
void vvApplicationState::initContext(vvContextState &state,
                                     GLContextData &data) const
{
  for (auto object : m_objects)
    {
    object->initVvContext(state, data);
    }
}

//------------------------------------------------------------------------------
void vvApplicationState::syncApplicationState()
{
  for (auto object : m_objects)
    {
    object->syncApplicationState(*this);
    }
}

//------------------------------------------------------------------------------
void vvApplicationState::syncContextState(const vvContextState &contextState,
                                          GLContextData &contextData) const
{
  for (auto object : m_objects)
    {
    object->syncContextState(*this, contextState, contextData);
    }
}
