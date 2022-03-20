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
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QStandardPaths>

#include "jsonmodel.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      m_model(new JsonModel(this))
{
	ui->setupUi(this);

	ui->treeView->setModel(m_model);

	connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onActionOpenTriggered);
	connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
	connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::open(const QString &filepath)
{
	QFile file(filepath);

	qDebug() << "Opening file...";
	if (!file.open(QFile::ReadOnly)) {
		QMessageBox::critical(this,
		                      tr("Failed to open file!"),
		                      file.errorString());
		return;
	}
	qDebug() << "File opened";
	qDebug() << "Reading...";

	auto data = file.readAll();
	file.close();

	qDebug() << "Parsing...";
	QJsonParseError jsonError;
	auto doc = QJsonDocument::fromJson(data, &jsonError);

	if (jsonError.error != QJsonParseError::NoError) {

		QMessageBox::critical(this,
		                      tr("Failed to parse file!"),
		                      jsonError.errorString());
		return;
	}

	qDebug() << "Done";
	if (doc.isArray())
		m_model->setJson(doc.array());
	else if (doc.isObject())
		m_model->setJson(doc.object());

	ui->treeView->setEnabled(true);
	setWindowFilePath(filepath);
}


void MainWindow::onActionOpenTriggered()
{
	auto path = QFileDialog::getOpenFileName(this,
	                                         tr("Open..."),
	                                         QStandardPaths::writableLocation(QStandardPaths::HomeLocation),
	                                         "JSON (*.json);;All files (*)");
	if (path.isEmpty())
		return;

	open(path);
}

