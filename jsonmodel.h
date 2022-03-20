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
