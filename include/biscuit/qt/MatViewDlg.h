#pragma once

#include <QDialog>

#include "biscuit/qt/MatView.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MatViewDlgClass; };
QT_END_NAMESPACE

namespace biscuit::qt {
	class BSC__QT_CLASS xMatViewDlg : public QDialog {
		Q_OBJECT
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

}	// namespace biscuit::qt
