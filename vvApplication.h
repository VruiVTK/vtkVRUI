#ifndef VVAPPLICATION_H
#define VVAPPLICATION_H

#include <GL/GLObject.h>
#include <Vrui/Application.h>

class vvApplicationState;
class vvContextState;

/**
 * @brief The vvApplication class is the core application class for vtkVRUI
 * apps.
 *
 * This class inherits both Vrui::Application and GLObject, and should be
 * subclassed to implement a vtkVRUI application. To use an implementation
 * of this class, construct it, set the initial application state, then call
 * initialize(), followed by run() to start the VRUI event loop.
 */
class vvApplication : public Vrui::Application, public GLObject
{
public:
  /**
   * Construct a vvApplication. The optional @a state object can be used to
   * supply an application specific state object. If not provided, a default
   * vvApplicationState object will be created. This class will take ownership
   * of the state object (if provided).
   */
  vvApplication(int &argc, char **&argv, vvApplicationState *state = nullptr);
  ~vvApplication();

  /**
   * Override this method to create a VRUI UI, setup callbacks, initialize
   * data, etc. prior to starting the rendering loop.
   */
  virtual void initialize();

  /**
   * Called once per-context to initialize an instance of vvContextState.
   * Call this base implementation before looking up the vvContextState object
   * in contextData to ensure that it exists.
   *
   * The base implementation creates the vvContextState object for the current
   * rendering context and calls initVvContext on the vvGLObjects in
   * m_state->objects().
   *
   * Called once per-context.
   */
  void initContext(GLContextData &contextData) const override;

  /**
   * Update context state and renders. Calls vvGLObject::syncContextState on all
   * objects in m_state->objects(), followed by vvContextState->render().
   *
   * Called once per-frame-per-context.
   */
  void display(GLContextData &contextData) const override;

  /**
   * Synchronization point called once per-frame, prior to rendering.
   *
   * Calls vvGLObject::syncApplicationState on all objects in
   * m_state->objects().
   */
  void frame() override;

  /**
   * If true, a FPS indicator is shown in the upper left corner of each context.
   * @{
   */
  bool showFPS() const;
  void setShowFPS(bool show);
  /** @} */

protected:

  /**
   * Create and return an instance (or subclass instance) of vvContextState.
   */
  virtual vvContextState* createContextState() const;

protected:
  vvApplicationState *m_state;

};

#endif // VVAPPLICATION_H
