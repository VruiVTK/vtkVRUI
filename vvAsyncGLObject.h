#ifndef VVASYNCGLOBJECT_H
#define VVASYNCGLOBJECT_H

#include "vvGLObject.h"

#include <future>

class vvProgressCookie;

/**
 * @brief The vvAsyncGLObject class implements an asynchronous vvGLObject.
 *
 * vvAsyncGLObject provides a vvGLObject implementation that executes a data
 * pipeline in a background thread. This allows a separate rendering pipeline
 * to continue rendering stale data until the recomputed objects are ready.
 *
 * This class provides a final implementation of syncApplicationState, which
 * manages all interactions with the data pipeline. Subclasses implement their
 * data pipelines via the following virtuals:
 *
 * - void vvAsyncGLObject::configureDataPipeline(const vvApplicationState &)
 * - bool vvAsyncGLObject::dataPipelineNeedsUpdate() const
 * - void vvAsyncGLObject::executeDataPipeline() const
 * - void vvAsyncGLObject::retrieveDataPipelineResult()
 *
 * For proper usage, the data pipeline objects must only be modified from within
 * configureDataPipeline. Pipeline options should be stored in ivars and
 * synchronized with pipeline during this call, rather than having
 * setters/getters access pipeline state directly. This is because the pipeline
 * may be executing when the setter is called, leading to undefined behavior.
 */
class vvAsyncGLObject : public vvGLObject
{
public:
  using Superclass = vvGLObject;

  struct DataItem : public Superclass::DataItem
  { /* Placeholder for future shared state. */ };

  vvAsyncGLObject();
  ~vvAsyncGLObject();

  /**
   * Implements thread handling and data syncing. Note that this implementation
   * is final -- subclasses may not reimplement this. If per-frame updates are
   * needed, use the virtual frame() method, but be sure not to modify the data
   * pipeline.
   */
  void syncApplicationState(const vvApplicationState &appState) final;

private: // Virtual API:

  /**
   * Called once per frame. This can be used to update object state, but do
   * NOT modify the data pipeline from this method, as it will be called
   * regardless of whether an asychronous update is happening.
   */
  virtual void frame(const vvApplicationState &appState) { /* no-op */ }

  /**
   * Update the data pipeline objects here. Ensure that any vtkObjects touched
   * here will not change their modification times unless the parameters really
   * have changed.
   */
  virtual void configureDataPipeline(const vvApplicationState &state) = 0;

  /**
   * Return true if the pipeline needs to be updated. A typical implementation
   * compares the modification times of cached data objects to the filters that
   * produced them.
   */
  virtual bool dataPipelineNeedsUpdate() const = 0;

  /**
   * Execute the data pipeline. This is called in a background thread. A typical
   * implementation simply calls 'Update()' on the data pipeline's sink
   * vtkAlgorithms.
   */
  virtual void executeDataPipeline() const = 0;

  /**
   * Retrieve the output data objects from the data pipeline. A typical
   * implementation will store them as ivars of the subclass so that they will
   * be available for rendering.
   *
   * To ensure that future pipeline updates will not modify the rendered object,
   * the following pattern should be used:
   *
   * @code
   * vtkDataObject *dataPipelineOutput = ...;
   * vtkSmartPointer<vtkDataObject> renderPipelineInput = ...;
   *
   * renderedPipelineInput.TakeReference(dataPipelineOutput->NewInstance());
   * renderedPipelineInput->ShallowCopy(dataPipelineOutput);
   * @endcode
   *
   * This will ensure that background modifications to dataPipelineOutput
   * (during future executions of the data pipeline) will be isolated from the
   * reference counted internals of the rendered object, and avoids the need to
   * deep copy the data.
   *
   */
  virtual void retrieveDataPipelineResult() = 0;

  /**
   * The string returned here will be used to label the background indicator.
   * See vvProgress.
   */
  virtual std::string progressLabel() const { return "Unknown Async Process"; }

private:
  /**
   * Wrapper around the data pipeline update call. Ensures that a new frame is
   * requested when the pipeline updates.
   */
  void internalExecutePipeline() const;

  std::future<void> m_monitor;
  vvProgressCookie *m_cookie;
};

#endif // VVASYNCGLOBJECT_H
