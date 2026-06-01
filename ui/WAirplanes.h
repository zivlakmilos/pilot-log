#ifndef _WAIRPLANES_H_
#define _WAIRPLANES_H_

#include "WBase.h"

namespace Ui
{
class WAirplanes;
}

class WAirplanes : public WBase
{
public:
  WAirplanes(QWidget *parent = nullptr);
  ~WAirplanes(void);

private:
  Ui::WAirplanes *m_ui;
};

#endif // _WAIRPLANES_H_
