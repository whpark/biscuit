module;

#include <QDialog>
#include <QColorDialog>
#include "ui_MatViewSettingsDlg.h"

#include "biscuit/qt/MatView.h"
#include "verdigris/wobjectcpp.h"
#include "verdigris/wobjectimpl.h"

export module biscuit.qt.MatViewSettingsDlg;

import std;
import biscuit;
import biscuit.qt.utils;
import biscuit.qt.ui_data_exchange;

using namespace std::literals;

export namespace biscuit::qt {

	class xMatViewSettingsDlg : public QDialog {
		W_OBJECT(xMatViewSettingsDlg)

	public:
		using this_t = xMatViewSettingsDlg;
		using base_t = QDialog;

		xMatView::S_OPTION m_option;

	public:
		xMatViewSettingsDlg(xMatView::S_OPTION option, QWidget* parent = nullptr);
		~xMatViewSettingsDlg() {}

		bool UpdateData(bool bSaveAndValidate = true);

	protected:
		void OnOK();
		void OnCancel();
		void OnBackgroundColor();

	private:
		Ui::MatViewSettingsDlgClass ui;
	};

	W_OBJECT_IMPL(xMatViewSettingsDlg);

	xMatViewSettingsDlg::xMatViewSettingsDlg(xMatView::S_OPTION option, QWidget* parent) : QDialog(parent), m_option(option) {
		ui.setupUi(this);

		connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &this_t::OnOK);
		connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &this_t::OnCancel);
		connect(ui.btnBackgroundColor, &QPushButton::clicked, this, &this_t::OnBackgroundColor);

		UpdateData(false);
	}

	bool xMatViewSettingsDlg::UpdateData(bool bSaveAndValidate) {
		auto const& b = bSaveAndValidate;
		UpdateWidgetValue(b, ui.chkZoomLock, m_option.bZoomLock);
		UpdateWidgetValue(b, ui.chkPanningLock, m_option.bPanningLock);
		UpdateWidgetValue(b, ui.chkExtendedPanning, m_option.bExtendedPanning);
		UpdateWidgetValue(b, ui.chkKeyboardNavigation, m_option.bKeyboardNavigation);
		UpdateWidgetValue(b, ui.chkDrawPixelValue, m_option.bDrawPixelValue);
		UpdateWidgetValue(b, ui.chkBuildPyramidImage, m_option.bPyrImageDown);
		UpdateWidgetValue(b, ui.spinPanningSpeed, m_option.dPanningSpeed);
		UpdateWidgetValue(b, ui.spinScrollMargin, m_option.nScrollMargin);
		UpdateWidgetValue(b, ui.cmbZoomInMethod, m_option.eZoomIn);
		UpdateWidgetValue(b, ui.cmbZoomOutMethod, m_option.eZoomOut);
		UpdateWidgetValue(b, ui.spinScrollDuration, m_option.tsScroll);
		//if (b) {
		//	m_option.tsScroll = std::chrono::milliseconds{(int)ui.spinScrollDuration->value()};
		//}
		//else {
		//	ui.spinScrollDuration->setValue(m_option.tsScroll.count());
		//}
		if (b) {
			auto cr = QColor(ui.edtColorBackground->text());
			m_option.crBackground = cv::Vec3b(cr.red(), cr.green(), cr.blue());
		}
		else {
			ui.edtColorBackground->setText(QColor(m_option.crBackground[0], m_option.crBackground[1], m_option.crBackground[2]).name());
		}

		return false;
	}

	void xMatViewSettingsDlg::OnOK() {
		UpdateData(true);
		base_t::accept();
	}

	void xMatViewSettingsDlg::OnCancel() {
		base_t::reject();
	}

	void xMatViewSettingsDlg::OnBackgroundColor() {
		QColor color = QColorDialog::getColor(QColor(ui.edtColorBackground->text()), this, tr("Select Color"));
		if (color.isValid()) {
			m_option.crBackground[0] = color.red();
			m_option.crBackground[1] = color.green();
			m_option.crBackground[2] = color.blue();
			ui.edtColorBackground->setText(color.name());
		}
	}

}	// namespace biscuit::qt
