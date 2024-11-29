module;

#include <QDialog>
#include <QVBoxLayout>

#include "verdigris/wobjectcpp.h"
#include "verdigris/wobjectimpl.h"

#include "ui_MatViewDlg.h"

export module biscuit.qt.MatViewDlg;

import std;
import biscuit;
import biscuit.qt.MatView;

export namespace biscuit::qt {

	class xMatViewDlg : public QDialog {
		W_OBJECT(xMatViewDlg)

	public:
		using this_t = xMatViewDlg;
		using base_t = QDialog;

	protected:
		xMatView* m_view{};
	public:
		xMatViewDlg(QWidget* parent = nullptr);
		~xMatViewDlg();

		xMatView& GetView() { return *m_view; }

	private:
		std::unique_ptr<Ui::MatViewDlgClass> ui;
	};

	W_OBJECT_IMPL(xMatViewDlg);

	xMatViewDlg::xMatViewDlg(QWidget* parent) : QDialog(parent), ui(std::make_unique<Ui::MatViewDlgClass>()) {
		ui->setupUi(this);
		m_view = ui->view;
	}

	xMatViewDlg::~xMatViewDlg() {
	}

}	// namespace biscuit::qt

