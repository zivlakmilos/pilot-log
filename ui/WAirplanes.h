#ifndef _WAIRPLANES_H_
#define _WAIRPLANES_H_

#include "WBase.h"

class QSqlTableModel;

class DirtyProxyModel;

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
  void setupHandlers(void);
  void setupModels(void);
  void save(void);
  void add(void);
  void remove(void);
  void setDirty(bool bDirty);

  Ui::WAirplanes *m_ui;
  QSqlTableModel *m_model;
  DirtyProxyModel *m_proxy;

  QString m_sSelectedId;
  bool m_bEdited;
};

#endif // _WAIRPLANES_H_
