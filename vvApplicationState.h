#ifndef VVAPPLICATIONSTATE_H
#define VVAPPLICATIONSTATE_H

class GLContextData;

class vvContextState;
class vvFramerate;
class vvGLObject;
class vvProgress;

#include <string>
#include <vector>

/**
 * @brief The vvApplicationState class holds application state.
 *
 * vvApplicationState holds application state needed by vvGLObjects.
 * An instance of this class is used to update vvGLObjects via
 * vvGLObject::syncApplicationState in vvApplication::frame().
 */
class vvApplicationState
{
public:
  using Objects = std::vector<vvGLObject*>;

  vvApplicationState();
  ~vvApplicationState();

  /**
   * Initialize application state. Calls vvGLObject::init on all objects in
   * objects().
   */
  virtual void init();

  /**
   * Initialize per-context state. Calls vvGLObject::initVvContext on all
   * objects in objects().
   */
  virtual void initContext(vvContextState &state, GLContextData &data) const;

  /**
   * Per-frame sync of application state. Calls vvGLObject::syncApplicationState
   * on all objects in objects().
   */
  virtual void syncApplicationState();

  /**
   * Sync per-context state. Calls vvGLObject::syncContextState on all
   * objects in objects().
   */
  virtual void syncContextState(const vvContextState &contextState,
                                GLContextData &contextData) const;

  /** List of all vvGLObjects. */
  Objects& objects() { return m_objects; }
  const Objects& objects() const { return m_objects; }

  /** Framerate overlay. */
  vvFramerate& framerate() { return *m_framerate; }
  const vvFramerate& framerate() const { return *m_framerate; }

  /** Async progress monitor.
   * Unfortunately not const-correct, since this needs to be modified in
   * vvAsyncGLObject::syncApplicationState(const vvAppState&). */
  vvProgress& progress() const { return *m_progress; }

protected:
  Objects m_objects;

private:
  // Not implemented:
  vvApplicationState(const vvApplicationState&);
  vvApplicationState& operator=(const vvApplicationState&);

  vvFramerate *m_framerate;
  vvProgress *m_progress;
};

#endif // VVAPPLICATIONSTATE_H
