#ifndef _WINSTRUCTORS_H_
#define _WINSTRUCTORS_H_

#include "WBase.h"

class QSqlTableModel;

class DirtyProxyModel;

namespace Ui
{
class WInstructors;
}

class WInstructors : public WBase
{
public:
  WInstructors(QWidget *parent = nullptr);
  ~WInstructors(void);

private:
  void setupHandlers(void);
  void setupModels(void);
  void save(void);
  void add(void);
  void remove(void);
  void setDirty(bool bDirty);

  Ui::WInstructors *m_ui;
  QSqlTableModel *m_model;
  DirtyProxyModel *m_proxy;

  QString m_sSelectedId;
  bool m_bEdited;
};

#endif // _WINSTRUCTORS_H_
