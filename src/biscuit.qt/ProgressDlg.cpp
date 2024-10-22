#include "ui_ProgressDlg.h"
#include "biscuit/qt/ProgressDlg.h"

import std;
import biscuit;
import biscuit.opencv;
import biscuit.qt.utils;

using namespace std::literals;

namespace biscuit::qt {

	xProgressDlg::xProgressDlg(QWidget* parent) : QDialog(parent) {
		ui = std::make_unique<Ui::ProgressDlgClass>();
		ui->setupUi(this);

		Init();
	}

	xProgressDlg::~xProgressDlg() {
	}

	void xProgressDlg::Init() {
		if (!m_title.empty())
			this->setWindowTitle(ToQString(m_title));
		ui->labelMessage->setText(ToQString(m_message));
		ui->progressBar->setRange(0, 100);

		m_callback = [&](int iPercent, bool bDone, bool bError)->bool{ return UpdateProgress(iPercent, bDone, bError); };

		connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &this_t::OnButton);
		m_tStart  = std::chrono::system_clock::now();
		connect(&m_timerUI, &QTimer::timeout, this, &this_t::OnTimer_UpdateUI);
		m_timerUI.start(100ms);
	}

	void xProgressDlg::OnTimer_UpdateUI() {
		if (ui and ui->progressBar) {
			auto& prog = *ui->progressBar;
			if (prog.value() != m_iPercent)
				prog.setValue(m_iPercent);
			auto ts = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::system_clock::now() - m_tStart);
			auto str = ToQString(std::format(L"{:.3f} 초, {} %", ts.count(), m_iPercent));
			if (ui->edtStatus->text() != str)
				ui->edtStatus->setText(str);

			auto msg = ToQString(m_message);
			if (ui->labelMessage->text() != msg)
				ui->labelMessage->setText(msg);
		}

		if (m_bDone) {
			if (m_bUserAbort)
				reject();
			else if (m_bError)
				reject();
			else
				accept();
		}
	}

	void xProgressDlg::OnButton(QAbstractButton* button) {
		//if (button == (QAbstractButton*)ui->buttonBox->button(QDialogButtonBox::Abort)) {
			m_bUserAbort = true;
			reject();
		//}
	}

}	// namespace biscuit::qt

