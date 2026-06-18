#include "DirtyProxyModel.h"

#include <QIcon>

DirtyProxyModel::DirtyProxyModel(QObject *parent) :
    QIdentityProxyModel(parent),
    m_nDrtyRow(-1)
{
}

DirtyProxyModel::~DirtyProxyModel(void)
{
}

int DirtyProxyModel::columnCount(const QModelIndex &parent) const
{
  return QIdentityProxyModel::columnCount(parent) + 1;
}

QVariant DirtyProxyModel::data(const QModelIndex &proxyIndex, int role) const
{
  if (!proxyIndex.isValid())
  {
    return QVariant();
  }

  if (proxyIndex.column() == 0)
  {
    if (proxyIndex.row() == m_nDrtyRow && role == Qt::DecorationRole)
    {
      return QIcon::fromTheme("document-edit");
    }

    return QVariant();
  }

  QModelIndex index = sourceModel()->index(proxyIndex.row(), proxyIndex.column() - 1, proxyIndex.parent());
  return sourceModel()->data(index, role);
}

QVariant DirtyProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal)
  {
    if (section == 0)
    {
      return QVariant();
    }

    return sourceModel()->headerData(section - 1, orientation, role);
  }

  return sourceModel()->headerData(section, orientation, role);
}

QModelIndex DirtyProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
  if (!proxyIndex.isValid())
  {
    return QModelIndex();
  }
  if (proxyIndex.column() == 0)
  {
    return QModelIndex();
  }

  return sourceModel()->index(proxyIndex.row(), proxyIndex.column() - 1);
}

QModelIndex DirtyProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
  if (!sourceIndex.isValid())
  {
    return QModelIndex();
  }

  return createIndex(sourceIndex.row(), sourceIndex.column() + 1);
}

QModelIndex DirtyProxyModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
  {
    return QModelIndex();
  }

  return createIndex(row, column);
}

void DirtyProxyModel::setDirtyRow(int row)
{
  int old = m_nDrtyRow;
  m_nDrtyRow = row;
  if (old >= 0)
  {
    emit dataChanged(index(old, 0), index(old, 0), {Qt::DecorationRole});
  }
  if (row >= 0)
  {
    emit dataChanged(index(row, 0), index(row, 0), {Qt::DecorationRole});
  }
};
