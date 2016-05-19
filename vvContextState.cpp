#include "vvContextState.h"

#include <ExternalVTKWidget.h>
#include <vtkExternalOpenGLRenderer.h>

vvContextState::vvContextState()
{
  m_widget->GetRenderWindow()->AddRenderer(m_renderer.GetPointer());

  m_renderer->SetUseDepthPeeling(1);
  m_renderer->SetMaximumNumberOfPeels(4);
  m_renderer->SetOcclusionRatio(0.001);
}

vvContextState::~vvContextState()
{
}

void vvContextState::render()
{
  m_widget->GetRenderWindow()->Render();
}
