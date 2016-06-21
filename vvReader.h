#ifndef VVREADER_H
#define VVREADER_H

#include <vtkBoundingBox.h>
#include <vtkSmartPointer.h>

#include <chrono>
#include <future>
#include <string>

class vtkDataObject;
class vvApplicationState;
class vvProgressCookie;

/**
 * @brief The vvReader class is a base interface to a data source. It supports
 * asynchronous updates, and data reduction for LOD rendering.
 */
class vvReader
{
public:
  vvReader();
  virtual ~vvReader();

  /**
   * This immediately and synchronously reads the file's metadata and updates
   * lightweight data, if any.
   *
   * This method does nothing if the file reader is already doing an
   * asynchronous read from a call to update().
   */
  void updateInformation();

  /**
   * Trigger an update cycle. This is the synchronization point between the
   * background reading thread and the main GUI thread. It will start a new
   * asynchronous read if the reading parameters have changed, and it will
   * update heavier data (e.g. dataObject()) the first time it is called after
   * a background read completes.
   */
  void update(const vvApplicationState &appState);

  /**
   * Returns true if dataObject() is being updated in a background thread.
   *
   * If a non-zero @a duration is set, the background process will be given
   * the specified amount of time to complete and this function will block. If
   * @a duration is zero, the call will not block.
   *
   * @note This method should be used very sparingly. Rather than synchronizing
   * data changes using this method, write code that is capable of waiting for
   * the result to be available in a later renderframe.
   */
  template <typename Rep, typename Period>
  bool running(const std::chrono::duration<Rep, Period> &duration =
               std::chrono::seconds(0)) const;

  /**
   * Returns true if reducedDataObject() is being updated in a background
   * thread.
   *
   * If a non-zero @a duration is set, the background process will be given
   * the specified amount of time to complete and this function will block. If
   * @a duration is zero, the call will not block.
   *
   * @note This method should be used very sparingly. Rather than synchronizing
   * data changes using this method, write code that is capable of waiting for
   * the result to be available in a later renderframe.
   */
  template <typename Rep, typename Period>
  bool reducing(const std::chrono::duration<Rep, Period> &duration =
                std::chrono::seconds(0)) const;

  /**
   * A pointer to the full resolution data object.
   */
  virtual vtkDataObject* dataObject() const;

  /**
   * A pointer to a reduced version of the full data object. Used for low
   * level of detail renderings.
   */
  virtual vtkDataObject* reducedDataObject() const;

  /**
   * The bounding box for the dataObject().
   */
  const vtkBoundingBox& bounds() const;

  /** The name of the file to read. @{ */
  void setFileName(const std::string &name) { m_fileName = name; }
  const std::string& fileName() const { return m_fileName; }
  /** @} */

  /** Set true to print timing information to std::cerr. @{ */
  bool benchmark() const { return m_benchmark; }
  void setBenchmark(bool benchmark) { m_benchmark = benchmark; }
  /** @} */

protected:
  bool m_benchmark;
  std::string m_fileName;
  vtkSmartPointer<vtkDataObject> m_dataObject;
  vtkSmartPointer<vtkDataObject> m_reducedData;
  vtkBoundingBox m_bounds;

  std::future<void> m_future;
  vvProgressCookie *m_cookie;

  std::future<void> m_reducerFuture;
  vvProgressCookie *m_reducerCookie;

private: // Virtual API:
  /**
   * Update the internal file reader state from this class's member variables.
   * Do not modify the reader outside of this method, as it may be executing in
   * a background thread.
   */
  virtual void syncReaderState() = 0;

  /**
   * @return True if the reader is out-of-date. This is typically implemented
   * by comparing the reader's MTime to the dataObject()'s MTime.
   */
  virtual bool dataNeedsUpdate() = 0;

  /**
   * Execute the reader to read file metadata. This is called from a background
   * thread. Typical implementations can simply call UpdateInformation() on the
   * VTK reader.
   */
  virtual void executeReaderInformation() = 0;

  /**
   * Copy metadata from the VTK reader to caching member variables in the
   * vvReader subclass.
   */
  virtual void updateInformationCache() = 0;

  /**
   * Execute the reader to read the data object from the file. This is called
   * from a background thread. Typical implementations may simply call
   * Update() on the VTK reader.
   */
  virtual void executeReaderData() = 0;

  /**
   * Copy any heavy data from the VTK reader to caching variables on the
   * vvReader subclass. This should, at minimum, update m_bounds and
   * m_dataObject.
   */
  virtual void updateDataCache() = 0;

  /**
   * Sync the VTK reducer filter's state. Do not modify the reducer outside of
   * this method, as it may be executing in a background thread.
   */
  virtual void syncReducerState() = 0;

  /**
   * Return true if the VTK reducer filter needs to be updated. Typical
   * implementations should simply compare m_dataObject's mtime with the
   * reducer filter's mtime.
   */
  virtual bool reducerNeedsUpdate() = 0;

  /**
   * Execute the VTK reducer filter. This is executed in a background thread.
   * Typical implementations simply call Update() on the filter.
   */
  virtual void executeReducer() = 0;

  /**
   * Copy the output of the VTK reducer filter to m_reducedData.
   */
  virtual void updateReducedData() = 0;

private: // Internal methods:
  /**
   * Trampoline that wraps a virtual call in a vvProgress notification. @{
   */
  virtual void internalExecuteReaderData();
  virtual void internalExecuteReducer();
  virtual bool invalidateReducedData();
  /** @} */
};

//------------------------------------------------------------------------------
template <typename Rep, typename Period>
bool vvReader::
running(const std::chrono::duration<Rep, Period> &duration) const
{
  if (m_future.valid())
    {
    return m_future.wait_for(duration) != std::future_status::ready;
    }
  return false;
}

//------------------------------------------------------------------------------
template <typename Rep, typename Period>
bool vvReader::
reducing(const std::chrono::duration<Rep, Period> &duration) const
{
  if (m_reducerFuture.valid())
    {
    return m_reducerFuture.wait_for(duration) != std::future_status::ready;
    }
  return false;
}

#endif // VVREADER_H
