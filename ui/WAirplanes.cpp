#include "WAirplanes.h"

#include "ui_WAirplanes.h"

WAirplanes::WAirplanes(QWidget *parent) :
    WBase(parent),
    m_ui(new Ui::WAirplanes)
{
  m_ui->setupUi(this);
}

WAirplanes::~WAirplanes(void)
{
  if (m_ui)
  {
    delete m_ui;
  }
}
