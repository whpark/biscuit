#include "test.h"

test::test(QWidget* parent) : QMainWindow(parent) {
	ui.setupUi(this);
	
	m_view = new biscuit::qt::xMatView(this);
	ui.grpImage->layout()->addWidget(m_view);
}

test::~test() {
}
