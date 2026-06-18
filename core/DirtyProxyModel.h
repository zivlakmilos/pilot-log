#ifndef _DIRTY_PROXY_MODEL_
#define _DIRTY_PROXY_MODEL_

#include <QIdentityProxyModel>

class DirtyProxyModel : public QIdentityProxyModel
{
  Q_OBJECT

public:
  DirtyProxyModel(QObject *parent);
  virtual ~DirtyProxyModel(void);

  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

  void setDirtyRow(int row = -1);

  QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

private:
  int m_nDrtyRow;
};

#endif //_DIRTY_PROXY_MODEL_
