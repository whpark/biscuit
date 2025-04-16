#include <QMessageBox>

#include "test.h"


import std;
import "biscuit/dependencies_opencv.hxx";
import biscuit;
import biscuit.opencv;
import biscuit.qt;

test::test(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);
	
	connect(ui.browser, &biscuit::qt::QFileFolderBrowser::SigPathBrowsed, [this](auto const& path) {
		//QMessageBox::information(this, "Path", path);
		QString str;
		cv::Mat img = cv::imread(path.toStdString(), 0);
		if (!img.empty())
			ui.view->SetImage(img);
	});
	//m_view = new biscuit::qt::xMatView(this);
	//ui.grpImage->layout()->addWidget(m_view);
}

test::~test() {
}

