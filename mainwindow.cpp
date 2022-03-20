#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFileDialog>

#include "jsonmodel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      m_model(new JsonModel(this))
{
	ui->setupUi(this);

	ui->treeView->setModel(m_model);

	QJsonDocument doc = QJsonDocument::fromJson(R"(
{
"a": "foo",
"b": "bar",
"c": [
	null,
 2.0,
false,
"Hello, World"
],
"d": "bar",
"e": 3.14
}
)");
	if (doc.isArray())
		m_model->setJson(doc.array());
	else if (doc.isObject())
		m_model->setJson(doc.object());
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
	auto path = QFileDialog::getOpenFileName(this, "Open...");
	if (path.isEmpty())
		return;

	QFile file(path);

	qDebug() << "Opening file...";
	if (!file.open(QFile::ReadOnly)) {
		return;
	}
	qDebug() << "File opened";
	qDebug() << "Reading...";

	auto data = file.readAll();
	file.close();

	qDebug() << "Parsing...";
	auto doc = QJsonDocument::fromJson(data);
	qDebug() << "Done";
	if (doc.isArray())
		m_model->setJson(doc.array());
	else if (doc.isObject())
		m_model->setJson(doc.object());

}

