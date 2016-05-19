#ifndef VVFRAMERATE_H
#define VVFRAMERATE_H

#include "vvGLObject.h"

#include <Misc/Timer.h>

#include <vtkNew.h>

#include <vector>

class vtkTextActor;
class vtkTextProperty;

/**
 * @brief The vvFramerate class renders the framerate as a vtkTextActor.
 */
class vvFramerate : public vvGLObject
{
public:
  using Superclass = vvGLObject;

  struct DataItem : public Superclass::DataItem
  {
    DataItem();
    vtkNew<vtkTextActor> actor;
  };

  vvFramerate();
  ~vvFramerate();

  // vvGLObjectAPI:
  void initVvContext(vvContextState &vvContext,
                     GLContextData &contextData) const override;
  void syncApplicationState(const vvApplicationState &state) override;
  void syncContextState(const vvApplicationState &appState,
                        const vvContextState &contextState,
                        GLContextData &contextData) const override;

  /**
   * Toggle visibility of the contour props on/off.
   */
  bool visible() const { return m_visible; }
  void setVisible(bool visible) { m_visible = visible; }

private:
  // Not implemented -- disable copy:
  vvFramerate(const vvFramerate&);
  vvFramerate& operator=(const vvFramerate&);

private:
  Misc::Timer m_timer;
  std::vector<double> m_times;

  vtkNew<vtkTextProperty> m_tprop;
  bool m_visible;
};

#endif // VVFRAMERATE_H
