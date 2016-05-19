// Must come before any gl.h include
#include <GL/glew.h>

#include "vvApplication.h"

#include "vvApplicationState.h"
#include "vvContextState.h"
#include "vvFramerate.h"

#include <Vrui/WindowProperties.h>

#include <GL/GLContextData.h>

#include <cassert>

//------------------------------------------------------------------------------
vvApplication::vvApplication(int &argc, char **&argv, vvApplicationState *state)
  : Vrui::Application(argc, argv),
    m_state(state ? state : new vvApplicationState)
{
  // Set Window properties:
  // Since the application requires translucency, GLX_ALPHA_SIZE is set to 1 at
  // context (VRWindow) creation time. To do this, we set the 4th component of
  // ColorBufferSize in WindowProperties to 1. This should be done in the
  // constructor to make sure it is set before the main loop is called.
  Vrui::WindowProperties properties;
  properties.setColorBufferSize(0, 1);
  Vrui::requestWindowProperties(properties);

  // Initialize state objects:
  m_state->init();
}

//------------------------------------------------------------------------------
vvApplication::~vvApplication()
{
  delete m_state;
}

//------------------------------------------------------------------------------
void vvApplication::initialize()
{
}

//------------------------------------------------------------------------------
void vvApplication::initContext(GLContextData &contextData) const
{
  // The VTK OpenGL2 backend requires this:
  GLenum glewInitResult = glewInit();
  if (glewInitResult != GLEW_OK)
    {
    std::cerr << "Error: Could not initialize GLEW (glewInit() returned: "
      << glewInitResult << ")." << std::endl;
    }

  // Create a state object for this context:
  vvContextState *contextState = this->createContextState();
  assert("Valid object returned from createContextState()" &&
         contextState != nullptr);
  contextData.addDataItem(this, contextState);

  // Synchronize vvGLObjects:
  m_state->initContext(*contextState, contextData);
}

//------------------------------------------------------------------------------
void vvApplication::display(GLContextData &contextData) const
{
  vvContextState *context = contextData.retrieveDataItem<vvContextState>(this);

  // Synchronize vvGLObjects:
  m_state->syncContextState(*context, contextData);

  /* Render the scene */
  context->render();
}

//------------------------------------------------------------------------------
void vvApplication::frame()
{
  // Synchronize vvGLObjects:
  m_state->syncApplicationState();
}

//------------------------------------------------------------------------------
bool vvApplication::showFPS() const
{
  return m_state->framerate().visible();
}

//------------------------------------------------------------------------------
void vvApplication::setShowFPS(bool show)
{
  m_state->framerate().setVisible(show);
}

//------------------------------------------------------------------------------
vvContextState *vvApplication::createContextState() const
{
  return new vvContextState;
}
