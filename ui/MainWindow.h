#ifndef _MAIN_WINDOW_H_
#define _MAIN_WINDOW_H_

#include <QMainWindow>
#include <QMap>
#include <QString>

#include "WBase.h"

class DWRibbon;
class QTabWidget;
class QLabel;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  virtual ~MainWindow(void);

private:
  struct Tab
  {
    int index;
    WBase *widget;
  };

  void setupUi(void);
  void setupHandlers(void);

  template <typename T>
  void showTab(QString sKey, QString sTitle)
  {
    if (m_tabs.contains(sKey))
    {
      m_twCentralWidget->setCurrentIndex(m_tabs[sKey].index);
      return;
    }

    T *frm = new T(this);
    int idx = m_twCentralWidget->addTab(frm, sTitle);
    m_twCentralWidget->setCurrentIndex(idx);
    m_tabs[sKey] = {idx, frm};

    connect(frm, &WBase::dirtyChanged,
            [this, sKey, sTitle](bool bDirty)
            {
              if (!m_tabs.contains(sKey))
              {
                return;
              }

              int idx = m_tabs[sKey].index;
              if (bDirty)
              {
                m_twCentralWidget->setTabText(idx, sTitle + "*");
              }
              else
              {
                m_twCentralWidget->setTabText(idx, sTitle);
              }
            });
  }

  DWRibbon *m_dwRibbon;
  QTabWidget *m_twCentralWidget;
  QLabel *m_lblStatus;

  QMap<QString, Tab> m_tabs;
};

#endif // _MAIN_WINDOW_H_
