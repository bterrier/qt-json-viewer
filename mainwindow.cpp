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

