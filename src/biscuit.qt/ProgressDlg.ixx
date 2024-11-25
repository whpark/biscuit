module;

#include "ui_ProgressDlg.h"
#include <QtWidgets/QDialog>
#include <QtWidgets/QPushButton>
#include <QTimer>
#include "verdigris/wobjectcpp.h"
#include "verdigris/wobjectimpl.h"

export module biscuit.qt.ProgressDlg;
import std;
import biscuit;
import biscuit.opencv;
import biscuit.qt.utils;

using namespace std::literals;

QT_BEGIN_NAMESPACE
namespace Ui {
	class ProgressDlgClass;
} // namespace Ui
QT_END_NAMESPACE

export namespace biscuit::qt {
	class xProgressDlg  : public QDialog {
		W_OBJECT(xProgressDlg)
			//Q_OBJECT
	public:
		using this_t = xProgressDlg;
		using base_t = QDialog;

	public:
		std::wstring m_title;
		std::wstring m_message;
		std::chrono::system_clock::time_point m_tStart;
		int m_iPercent{};
		bool m_bUserAbort{};
		bool m_bDone{};
		bool m_bError{};

		std::unique_ptr<std::jthread> m_rThreadWorker;

	protected:
		QTimer m_timerUI;
		callback_progress_t m_callback;

	public:
		xProgressDlg(QWidget *parent);
		~xProgressDlg();

		void Init();
		bool UpdateProgress(int iPercent, bool bDone, bool bError) {
			m_iPercent = iPercent;
			m_bDone = bDone;
			m_bError = bError;
			return !m_bUserAbort;
		}
		callback_progress_t GetCallback() const { return m_callback; }

	protected:
		void OnTimer_UpdateUI();
		void OnButton(QAbstractButton *button);

	private:
		std::unique_ptr<Ui::ProgressDlgClass> ui;
	};
}

namespace biscuit::qt {

	W_OBJECT_IMPL(xProgressDlg)

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

