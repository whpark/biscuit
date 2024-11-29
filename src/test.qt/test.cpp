#include <QMessageBox>

#include "test.h"

test::test(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);
	
	connect(ui.browser, &biscuit::qt::QFileFolderBrowser::SigPathBrowsed, [this](auto const& path) {
		QMessageBox::information(this, "Path", path);
	});
	//m_view = new biscuit::qt::xMatView(this);
	//ui.grpImage->layout()->addWidget(m_view);
}

test::~test() {
}

