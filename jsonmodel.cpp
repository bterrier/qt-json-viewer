#include "jsonmodel.h"
#include <vector>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>

//using ModelIndex = QModelIndex;
using ModelIndex = QPersistentModelIndex;

static QString type2string(QJsonValue::Type type)
{
	switch (type)
	{
	case QJsonValue::Null:
		return "null";
	case QJsonValue::Bool:
		return "bool";
	case QJsonValue::Double:
		return "double";
	case QJsonValue::String:
		return "string";
	case QJsonValue::Array:
		return "array";
	case QJsonValue::Object:
		return "object";
	case QJsonValue::Undefined:
	default:
		return "undefined";
	}
}

class JsonNode
{
public:
	explicit JsonNode(const QString name, const QJsonValue &value, ModelIndex index ,ModelIndex parent) :
	    m_value(value),
	    m_name(name),
	    m_type(type2string(value.type())),
	    m_parent(parent),
	    m_index({index})
	{
		if (value.isObject())
		{
			auto object = value.toObject();
			m_childCount = object.count();
		}
		else if (value.isArray())
		{
			auto array = value.toArray();
			m_childCount = array.count();
		} else {
			m_cachedValue = m_value.toVariant();
		}

		m_children.reserve(m_childCount);
	}

	QString name() const
	{
		return m_name;
	}

	QString type() const
	{
		return m_type;
	}

	QVariant value() const
	{
		return m_cachedValue;
	}

	ModelIndex parent() const;
	JsonNode *child(int index) const
	{
		if (index >= m_children.count())
			return nullptr;

		return &m_children[index];
	}
	constexpr int childCount() const noexcept
	{
		return m_childCount;
	}

	int parsedCount() const
	{
		return m_children.count();
	}

	constexpr bool hasChildren() const noexcept
	{
		return childCount() != 0;
	}

	void setIndex(int column, const ModelIndex &newIndex);
	ModelIndex index(int column) const {
		if (column >= m_index.size())
		{
			m_index.resize(column+1);
		}
		return m_index.at(column);
	}

	void setParent(const ModelIndex &newParent);
	bool canParse() const {
		return m_children.count() != m_childCount;
	}

	void parse() const
	{
		qDebug() << Q_FUNC_INFO << this;
		if (m_children.count() == m_childCount)
			return;

		if (m_value.isObject()) {
			auto object = m_value.toObject();
			int count = 0;
			for (auto it = object.constBegin() + m_children.count(); it != object.constEnd() ; ++it)
			{
				++count;
				m_children.append(JsonNode(it.key(), it.value(), {}, m_index.first()));
				if (count >= 10)
					break;
			}
		} else if (m_value.isArray()) {
			auto array = m_value.toArray();
			int count = 0;
			for (int idx = m_children.count() ; idx < array.count() ; ++idx)
			{
				++count;
				m_children.append(JsonNode(QString::number(idx), array.at(idx), {}, m_index.first()));
				if (count >= 10)
					break;
			}
		}
	}

private:
	QJsonValue m_value;
	QString m_name;
	QString m_type;
	QVariant m_cachedValue;
	ModelIndex m_parent;
	mutable QList<ModelIndex> m_index;
	mutable QList<JsonNode> m_children;
	int m_childCount = 0;
};

JsonModel::JsonModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant JsonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (section)
		{
		case Name:
			return "Name";
		case Type:
			return "Type";
		case Value:
			return "Value";
		}

	}
	return QAbstractItemModel::headerData(section, orientation, role);
}

QModelIndex JsonModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!parent.isValid()) {
		if (m_root == nullptr)
			return QModelIndex();
		else {

			QModelIndex index = m_root->index(column);
			if (!index.isValid())
			{
				index = createIndex(row, column, m_root);
				m_root->setIndex(column, index);
			}
			return index;

		}
	} else {

		auto parentNode = reinterpret_cast<JsonNode *>(parent.internalPointer());
		auto node = parentNode->child(row);


		QModelIndex index = node->index(column);

		if (!index.isValid()) {
			index = createIndex(row, column, node);
			node->setIndex(column, index);
		}
		return index;

	}
}

QModelIndex JsonModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return {};

	auto node = reinterpret_cast<JsonNode *>(index.internalPointer());

	if (node == nullptr)
		return {};

	return node->parent();
}

int JsonModel::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid()) {

		if (m_root != nullptr)
			return 1;
		else
			return 0;
	}
	auto parentNode = reinterpret_cast<JsonNode *>(parent.internalPointer());
	return parentNode->parsedCount();
}

int JsonModel::columnCount(const QModelIndex &parent) const
{
	return ColumnCount;
}

bool JsonModel::hasChildren(const QModelIndex &parent) const
{
	auto parentNode = reinterpret_cast<JsonNode *>(parent.internalPointer());
	if (parentNode == nullptr)
		return m_root != nullptr;
	return parentNode->hasChildren();
}

bool JsonModel::canFetchMore(const QModelIndex &parent) const
{
	qDebug() << Q_FUNC_INFO << parent;
	bool result = false;
	auto node = reinterpret_cast<JsonNode *>(parent.internalPointer());
	if (node) {
		result =  node->canParse();
	}
	qDebug() << result;
	return result;
}

void JsonModel::fetchMore(const QModelIndex &parent)
{
	auto node = reinterpret_cast<JsonNode *>(parent.internalPointer());
	if (node) {
		int old = node->parsedCount();
		node->parse();

		beginInsertRows(parent,old, node->parsedCount()-1);
		endInsertRows();
	}
}

QVariant JsonModel::data(const QModelIndex &index, int role) const
{
	qDebug() << Q_FUNC_INFO << index << role;
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole)
	{
		auto node = reinterpret_cast<JsonNode *>(index.internalPointer());
		if (node)
			switch (index.column())
			{
			case Name:
				return node->name();
			case Type:
				return node->type();
			case Value:
				return node->value();
			}
	}
	return QVariant();
}

void JsonModel::setJson(const QJsonValue &value)
{
	beginResetModel();
	delete m_root;
	m_root = new JsonNode({}, value, {}, {});
	endResetModel();
}

ModelIndex JsonNode::parent() const
{
	return m_parent;
}

void JsonNode::setIndex(int column, const ModelIndex &newIndex)
{
	if (column >= m_index.size())
	{
		m_index.resize(column+1);
	}

	if (newIndex == m_index.at(column))
		return;

	m_index[column] = newIndex;
	if (column == 0) {
		for (auto node : m_children)
		{
			node.setParent(m_index.first());
		}
	}
}

void JsonNode::setParent(const ModelIndex &newParent)
{
	m_parent = newParent;
}
