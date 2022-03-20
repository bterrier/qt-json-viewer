/****************************************************************************
**
** This program is free software: you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by the Free
** Software Foundation; either version 3 of the License, or (at your option)
** any later version.
**
** This program is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
** for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program. If not, see https://www.gnu.org/licenses/.
**
****************************************************************************/

#ifndef JSONMODEL_H
#define JSONMODEL_H

#include <QAbstractItemModel>

class JsonNode;
class JsonModel : public QAbstractItemModel
{
	Q_OBJECT
	enum Column {
		Name,
		Type,
		Value,
		ColumnCount
	};

public:
	explicit JsonModel(QObject *parent = nullptr);

	// Header:
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	// Basic functionality:
	QModelIndex index(int row, int column,
	                  const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;

	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

	// Fetch data dynamically:
	bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

	bool canFetchMore(const QModelIndex &parent) const override;
	void fetchMore(const QModelIndex &parent) override;

	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	void setJson(const QJsonValue &value);

private:
	JsonNode *m_root = nullptr;
};

#endif // JSONMODEL_H
