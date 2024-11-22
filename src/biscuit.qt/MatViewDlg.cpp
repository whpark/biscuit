//#include "pch.h"

#include "ui_MatViewDlg.h"
#include "biscuit/qt/MatViewDlg.h"

namespace biscuit::qt {
	xMatViewDlg::xMatViewDlg(QWidget* parent) : QDialog(parent), ui(std::make_unique<Ui::MatViewDlgClass>()) {
		ui->setupUi(this);
		m_view = ui->view;
	}

	xMatViewDlg::~xMatViewDlg() {
	}

}	// namespace biscuit::qt

