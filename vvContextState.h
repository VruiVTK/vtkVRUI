#ifndef VVCONTEXTSTATE_H
#define VVCONTEXTSTATE_H

#include <GL/GLObject.h>

#include <vtkNew.h>

class ExternalVTKWidget;
class vtkExternalOpenGLRenderer;

/**
 * @brief The vvContextState class holds shared context state.
 *
 * vvContextState holds per-context state that is shared between vvGLObjects.
 */
class vvContextState : public GLObject::DataItem
{
public:
  vvContextState();
  ~vvContextState();

  // Triggers a render of the scene
  void render();

  // These aren't const-correct bc VTK is not const-correct.
  vtkExternalOpenGLRenderer& renderer() const { return *m_renderer.Get(); }
  ExternalVTKWidget& widget() const { return *m_widget.Get(); }

private:
  vtkNew<vtkExternalOpenGLRenderer> m_renderer;
  vtkNew<ExternalVTKWidget> m_widget;
};

#endif // VVCONTEXTSTATE_H
