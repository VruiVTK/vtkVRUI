#ifndef VVPROGRESS_H
#define VVPROGRESS_H

#include "vvGLObject.h"

#include <vtkNew.h>

#include <vector>

class vvProgressCookie;
class vtkTextActor;
class vtkTextProperty;

/**
 * @brief The vvProgress class notifies the user about background updates.
 *
 * When an asynchronous update starts, the user is shown stale data, or no data.
 * This vvGLObject displays an on-screen notification to the user, informing
 * them that some data is not yet available for rendering.
 *
 * Background processes indicate that they have begun a background by calling
 * addEntry, providing a string containing the text to be displayed in the
 * notification. addEntry returns a pointer to an vvProgressCookie, which
 * is used to update the notification text and remove the notification. The
 * notification is removed by passing the cookie back to removeEntry.
 *
 * Note that vvProgressCookie is not thread-safe, and any updates to it should
 * be performed from the GUI thread.
 */
class vvProgress : public vvGLObject
{
public:
  using Superclass = vvGLObject;

  struct DataItem : public Superclass::DataItem
  {
    DataItem();
    ~DataItem();

    vtkNew<vtkTextActor> actor;
  };

  vvProgress();
  ~vvProgress();

  /** Can be used to hide all progress notifications. */
  bool visible() const;
  void setVisible(bool visible);

  /**
   * Create a new update notification entry with text @a text and obtain a
   * cookie.
   */
  vvProgressCookie* addEntry(std::string text);

  /**
   * Clear the notification text associated with the cookie.
   */
  void removeEntry(vvProgressCookie *cookie);

  virtual void initVvContext(vvContextState &vvContext,
                             GLContextData &contextData) const;
  virtual void syncApplicationState(const vvApplicationState &state);
  virtual void syncContextState(const vvApplicationState &appState,
                                const vvContextState &contextState,
                                GLContextData &contextData) const;

private:
  bool m_visible;
  std::string m_text;
  vtkNew<vtkTextProperty> m_tprop;
  std::vector<vvProgressCookie*> m_entries;
};


#endif // VVPROGRESS_H
