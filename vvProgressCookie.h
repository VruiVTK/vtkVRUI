#ifndef VVPROGRESSCOOKIE_H
#define VVPROGRESSCOOKIE_H

#include <array>
#include <string>

class vvProgress;

/**
 * @brief The vvProgressCookie class is used to manage vvProgress notifications.
 */
class vvProgressCookie
{
public:
  std::string text() const { return m_text; }
  void setText(std::string text) { m_text = text; }

protected:
  friend class vvProgress;

  explicit vvProgressCookie(std::string text);
  ~vvProgressCookie();

private:
  std::string m_text;
};

#endif // VVPROGRESSCOOKIE_H
