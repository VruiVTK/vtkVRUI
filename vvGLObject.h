#ifndef VVGLOBJECT_H
#define VVGLOBJECT_H

#include <GL/GLObject.h>

class vvApplicationState;
class vvContextState;

/**
 * @brief The vvGLObject class extends the VRUI GLObject abstraction.
 *
 * vvGLObject extends VRUI's GLObject system for separating and maintaining
 * application / context state. Each subclass should manage one or more related
 * props in the rendered scene.
 *
 * Do not implement GLObject::initContext -- it will not be called, as auto
 * initialization is disabled for these classes. Instead, put all initialization
 * code into initVvContext.
 *
 * The goal of this class is to simplify the integration of VTK rendering
 * concepts into the VRUI GLObject abstraction.
 *
 * The synchronization points are:
 *
 * initVvContext: Called from vvApplication::initContext(). Use this to do
 *                one-time setup of this object's context-specific state with
 *                respect to the application's vvContextState (e.g. add actors
 *                to the renderer). This replaces the GLObject::initContext
 *                method.
 *
 * syncApplicationState: Called from vvApplication::frame(). Update the
 *                       application-side data pipeline. Any data computations
 *                       that are shared between contexts should be performed
 *                       here, eventually producing renderable polydata.
 * syncContextState: Called from the vvApplication::display() prior to rendering.
 *                   This is where data should be transferred from the
 *                   application-side data pipeline into the context-side
 *                   rendering pipeline. At minimum, this will typically mean
 *                   taking the output polydata from the data pipeline and
 *                   connecting it to a mapper.
 *
 * At all stages, only the minimum work should be done; the data pipeline
 * should only re-execute when inputs changes, and the rendering pipeline
 * should only be updated when the application data changes.
 */
class vvGLObject : public GLObject
{
public:
  using Superclass = GLObject;

  /**
   * All subclasses should inherit their DataItems from this.
   */
  struct DataItem : public Superclass::DataItem
  { /* Placeholder for future shared state. */  };

  vvGLObject();
  ~vvGLObject();

  /**
   * Optionally perform post-construction initialization prior to context
   * initialization.
   */
  virtual void init(const vvApplicationState &appState) { /* no-op */ }

  /**
   * Do not use, this will not be executed. Put all context intialization in
   * initVvContext.
   */
  void initContext(GLContextData &contextData) const final;

  /**
   * Initialize object with respect to the vtkVRUI app. This is the spot
   * to add actors to the context's renderer, etc. All context-specific
   * initialization for GLObject needs to happen here as well, such as adding an
   * instance of DataItem to GLContextData.
   */
  virtual void initVvContext(vvContextState &vvContext,
                             GLContextData &contextData) const;

  /**
   * Synchronization point between main application and object. Called per-frame
   * from vvApplication::frame().
   */
  virtual void syncApplicationState(const vvApplicationState &state);

  /**
   * Prepare the context state for rendering. Called per-frame, per-context from
   * vvApplication::display(), after syncing application state but before
   * rendering.
   */
  virtual void syncContextState(const vvApplicationState &appState,
                                const vvContextState &contextState,
                                GLContextData &contextData) const;
};

#endif // VVGLOBJECT_H
