#include "vvReader.h"

#include <vtkDataObject.h>
#include <vtkTimerLog.h>

#include <Vrui/Vrui.h>

#include "vvApplicationState.h"
#include "vvProgress.h"

#include <cassert>
#include <iostream>
#include <sstream>

//------------------------------------------------------------------------------
vvReader::vvReader()
  : m_benchmark(false),
    m_cookie(nullptr),
    m_reducerCookie(nullptr)
{

}

//------------------------------------------------------------------------------
vvReader::~vvReader()
{
  // Wait for background processes to finish:
  if (m_future.valid())
    {
    std::cout << "Waiting for file read to complete..." << std::endl;
    m_future.wait();
    }

  if (m_reducerFuture.valid())
    {
    std::cout << "Waiting for async data reduction to complete..." << std::endl;
    m_reducerFuture.wait();
    }
}

//------------------------------------------------------------------------------
void vvReader::updateInformation()
{
  // Only update when the background thread is not running:
  if (m_future.valid())
    {
    return;
    }

  this->syncReaderState();
  this->executeReaderInformation();
  this->updateInformationCache();
}

//------------------------------------------------------------------------------
void vvReader::update(const vvApplicationState &appState)
{
  // Are we currently reading the file?
  if (m_future.valid())
    {
    // Get the current state of the update thread:
    std::future_status state = m_future.wait_for(std::chrono::milliseconds(0));

    // We always use the async launch policy:
    assert("Always async." && state != std::future_status::deferred);

    if (state == std::future_status::ready)
      {
      // Clear the future.
      m_future.get();

      // Sync the cached data.
      this->updateInformationCache();
      this->updateDataCache();

      // Invalidate the reduced dataset as it is now out of date. This prevents
      // LOD actors from displaying incorrect lowres data.
      this->invalidateReducedData();

      // Clean up the progress monitor:
      assert("Cookie exists." && m_cookie != nullptr);
      appState.progress().removeEntry(m_cookie);
      m_cookie = nullptr;
      }
    else
      {
      // Still running, do nothing.
      return;
      }
    }

  // At this point, we know the background thread is not running and the
  // cache object is up-to-date with the last execution. Check to see if the
  // reader needs to re-run:
  this->syncReaderState();
  if (this->dataNeedsUpdate())
    {
    assert("Cookie cleaned up." && m_cookie == nullptr);
    m_cookie = appState.progress().addEntry("Reading Data File");

    m_future = std::async(std::launch::async,
                          &vvReader::internalExecuteReaderData, this);

    // Don't bother updating reduced data until the main data is up-to-date:
    return;
    }

  // Update the reduced data as well. Logic is the same as above.
  if (m_reducerFuture.valid())
    {
    std::chrono::seconds now(0);
    std::future_status state = m_reducerFuture.wait_for(now);
    assert("Always async." && state != std::future_status::deferred);
    if (state == std::future_status::ready)
      {
      m_reducerFuture.get(); // Clear the thread state.
      this->updateReducedData();
      assert("Cookie exists." && m_reducerCookie != nullptr);
      appState.progress().removeEntry(m_reducerCookie);
      m_reducerCookie = nullptr;
      }
    else
      {
      return;
      }
    }

  this->syncReducerState();
  if (this->reducerNeedsUpdate())
    {
    this->invalidateReducedData();
    assert("Cookie cleaned up." && m_reducerCookie == nullptr);
    m_reducerCookie = appState.progress().addEntry("Generating Reduced Data");
    m_reducerFuture = std::async(std::launch::async,
                                 &vvReader::internalExecuteReducer, this);
    }
}

//------------------------------------------------------------------------------
vtkDataObject *vvReader::dataObject() const
{
  return m_dataObject.Get();
}

//------------------------------------------------------------------------------
vtkDataObject *vvReader::reducedDataObject() const
{
  return m_reducedData.Get();
}

//------------------------------------------------------------------------------
const vtkBoundingBox &vvReader::bounds() const
{
  return m_bounds;
}

//------------------------------------------------------------------------------
void vvReader::internalExecuteReaderData()
{
  vtkTimerLog *log = nullptr;
  if (m_benchmark)
    {
    log = vtkTimerLog::New();
    std::cerr << "Updating full dataset.\n";
    log->StartTimer();
    }

  this->executeReaderData();

  if (log != nullptr)
    {
    log->StopTimer();
    std::ostringstream out;
    out << "Full dataset ready (" << log->GetElapsedTime() << "s)\n";
    std::cerr << out.str();
    log->Delete();
    }

  Vrui::requestUpdate();
}

//------------------------------------------------------------------------------
void vvReader::internalExecuteReducer()
{
  vtkTimerLog *log = nullptr;
  if (m_benchmark)
    {
    log = vtkTimerLog::New();
    std::cerr << "Generating reduced data.\n";
    log->StartTimer();
    }

  this->executeReducer();

  if (log != nullptr)
    {
    log->StopTimer();
    std::ostringstream out;
    out << "Reduced data ready (" << log->GetElapsedTime() << "s)\n";
    std::cerr << out.str();
    log->Delete();
    }

  Vrui::requestUpdate();
}

//------------------------------------------------------------------------------
bool vvReader::invalidateReducedData()
{
  if (m_reducedData && m_benchmark)
    {
    std::cerr << "Reduced data invalidated.\n";
    }
  m_reducedData = nullptr;
}
