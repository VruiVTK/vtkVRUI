#include "vvProgress.h"

#include <GL/GLContextData.h>

#include <vtkCoordinate.h>
#include <vtkExternalOpenGLRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

#include "vvApplicationState.h"
#include "vvContextState.h"
#include "vvProgressCookie.h"

#include <algorithm>
#include <cassert>
#include <set>
#include <sstream>

//------------------------------------------------------------------------------
vvProgress::DataItem::DataItem()
{
  actor->SetTextScaleModeToViewport();
  vtkCoordinate *coord = actor->GetPositionCoordinate();
  coord->SetCoordinateSystemToNormalizedDisplay();
  coord->SetValue(0.99, 0.99);
}

//------------------------------------------------------------------------------
vvProgress::DataItem::~DataItem()
{
}

//------------------------------------------------------------------------------
vvProgress::vvProgress()
{
  m_tprop->SetJustificationToRight();
  m_tprop->SetVerticalJustificationToTop();
  m_tprop->SetFontSize(8);
  m_tprop->BoldOn();
  m_tprop->SetColor(1., .2, .2);
  m_tprop->SetBackgroundColor(0.25, 0.25, 0.25);
  m_tprop->SetBackgroundOpacity(0.5);
}

//------------------------------------------------------------------------------
vvProgress::~vvProgress()
{
  if (!m_entries.empty())
    {
    std::cerr << "Cleaning up unfinished progress cookies:\n";
    for (auto cookie : m_entries)
      {
      std::cerr << " -- " << cookie->text() << "\n";
      delete cookie;
      }
    m_entries.clear();
    }
}

//------------------------------------------------------------------------------
vvProgressCookie *vvProgress::addEntry(std::string text)
{
  vvProgressCookie *cookie = new vvProgressCookie(text);
  m_entries.push_back(cookie);
  return cookie;
}

//------------------------------------------------------------------------------
void vvProgress::removeEntry(vvProgressCookie *cookie)
{
  auto newEnd = std::remove(m_entries.begin(), m_entries.end(), cookie);
  assert("Double free detected." && newEnd < m_entries.end());
  m_entries.resize(std::distance(m_entries.begin(), newEnd));
  delete cookie;
}

//------------------------------------------------------------------------------
void vvProgress::initVvContext(vvContextState &vvContext,
                               GLContextData &contextData) const
{
  this->Superclass::initVvContext(vvContext, contextData);

  assert("Duplicate context initialization detected!" &&
         !contextData.retrieveDataItem<DataItem>(this));

  DataItem *dataItem = new DataItem;
  contextData.addDataItem(this, dataItem);

  dataItem->actor->SetTextProperty(m_tprop.Get());
  vvContext.renderer().AddActor2D(dataItem->actor.Get());
}

//------------------------------------------------------------------------------
void vvProgress::syncApplicationState(const vvApplicationState &state)
{
  this->Superclass::syncApplicationState(state);

  if (m_entries.empty())
    {
    m_text.clear();
    return;
    }

  std::ostringstream str;
  str << "Update(s) in progress:\n";
  for (const auto cookie : m_entries)
    {
    str << "  - " << cookie->text() << "\n";
    }
  m_text = str.str();
  // Pop off the last newline:
  m_text.pop_back();
}

//------------------------------------------------------------------------------
void vvProgress::syncContextState(const vvApplicationState &appState,
                                  const vvContextState &contextState,
                                  GLContextData &contextData) const
{
  this->Superclass::syncContextState(appState, contextState, contextData);

  DataItem *dataItem = contextData.retrieveDataItem<DataItem>(this);
  assert(dataItem);

  if (m_text.empty())
    {
    dataItem->actor->SetVisibility(0);
    return;
    }

  dataItem->actor->SetInput(m_text.c_str());
  dataItem->actor->SetVisibility(1);
}
