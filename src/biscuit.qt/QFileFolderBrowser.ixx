module;

#include <QWidget>
#include <QFileDialog>
#include <QKeyEvent>

//#include "ui_QFileFolderBrowser.h"
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

#include "verdigris/wobjectcpp.h"
#include "verdigris/wobjectimpl.h"

export module biscuit.qt.Widgets.QFileFolderBrowser;


namespace biscuit::qt {

	class QComboBoxEnter : public QComboBox {
		W_OBJECT(QComboBoxEnter);
		using this_t = QComboBoxEnter;
		using base_t = QComboBox;

	public:
		using base_t::base_t;

		void SigEnterPressed()
			W_SIGNAL(SigEnterPressed);

		void keyPressEvent(QKeyEvent* event) override {
			if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
				SigEnterPressed();
			}
			base_t::keyPressEvent(event);
		}

	};
	W_OBJECT_IMPL(QComboBoxEnter);

}

QT_BEGIN_NAMESPACE
class Ui_QFileFolderBrowserClass {
public:
	QHBoxLayout* horizontalLayout{};
	biscuit::qt::QComboBoxEnter* cmbPath{};
	QToolButton* btnBrowse{};

	void setupUi(QWidget* QFileFolderBrowserClass) {
		if (QFileFolderBrowserClass->objectName().isEmpty())
			QFileFolderBrowserClass->setObjectName("QFileFolderBrowserClass");
		QFileFolderBrowserClass->resize(402, 25);
		horizontalLayout = new QHBoxLayout(QFileFolderBrowserClass);
		horizontalLayout->setSpacing(3);
		horizontalLayout->setObjectName("horizontalLayout");
		horizontalLayout->setContentsMargins(0, 0, 0, 0);
		cmbPath = new biscuit::qt::QComboBoxEnter(QFileFolderBrowserClass);
		cmbPath->setObjectName("cmbPath");
		cmbPath->setEditable(true);

		horizontalLayout->addWidget(cmbPath);

		btnBrowse = new QToolButton(QFileFolderBrowserClass);
		btnBrowse->setObjectName("btnBrowse");

		horizontalLayout->addWidget(btnBrowse);

		horizontalLayout->setStretch(0, 1);

		retranslateUi(QFileFolderBrowserClass);

		QMetaObject::connectSlotsByName(QFileFolderBrowserClass);
	} // setupUi

	void retranslateUi(QWidget* QFileFolderBrowserClass) {
		QFileFolderBrowserClass->setWindowTitle(QCoreApplication::translate("QFileFolderBrowserClass", "QFileFolderBrowser", nullptr));
		btnBrowse->setText(QCoreApplication::translate("QFileFolderBrowserClass", "...", nullptr));
	} // retranslateUi

};

namespace Ui {
	class QFileFolderBrowserClass : public Ui_QFileFolderBrowserClass {};
} // namespace Ui

QT_END_NAMESPACE

W_REGISTER_ARGTYPE(std::filesystem::path);

export namespace biscuit::qt {

	class QFileFolderBrowser : public QWidget {
		W_OBJECT(QFileFolderBrowser);

	public:
		bool m_bFileBrowser{ true };

	public:
		QFileFolderBrowser(QWidget* parent = nullptr);
		~QFileFolderBrowser();

		void SigModeChanged(bool bFileBrowse)
			W_SIGNAL(SigModeChanged, bFileBrowse);

		void SigPathBrowsed(QString const& path)
			W_SIGNAL(SigPathBrowsed, path);

		void SigPathChanged(QString const& path)
			W_SIGNAL(SigPathChanged, path);

		auto& GetComboBox() { return *ui.cmbPath; }
		std::filesystem::path GetPath() {
			return ui.cmbPath->currentText().toStdWString();
		}
		void SetPath(const std::filesystem::path& path) {
			ui.cmbPath->setCurrentText(QString::fromStdWString(path.wstring()));
		}

	private:
		Ui::QFileFolderBrowserClass ui;

	public:
		W_PROPERTY(bool, bFileBrowse MEMBER m_bFileBrowser NOTIFY SigModeChanged);
	};

	W_OBJECT_IMPL(QFileFolderBrowser);

	QFileFolderBrowser::QFileFolderBrowser(QWidget* parent) : QWidget(parent) {
		ui.setupUi(this);

		connect(ui.btnBrowse, &QToolButton::clicked, [this]() {
			QString path;
			QString pathCurrent = ui.cmbPath->currentText();
			if (pathCurrent.isEmpty())
				pathCurrent = QDir::homePath();
			if (m_bFileBrowser)
				path = QFileDialog::getOpenFileName(this, "Select File", pathCurrent);
			else
				path = QFileDialog::getExistingDirectory(this, "Select Folder", pathCurrent);

			if (!path.isEmpty()) {
				ui.cmbPath->addItem(path);
				ui.cmbPath->setCurrentText(path);

				SigPathBrowsed(path);
			}
		});

		connect(ui.cmbPath, &QComboBoxEnter::currentTextChanged, [this](const QString& text) {
			SigPathChanged(text);
		});
		connect(ui.cmbPath, &QComboBoxEnter::currentIndexChanged, [this](int index) {
			SigPathBrowsed(ui.cmbPath->itemText(index));
		});
		connect(ui.cmbPath, &QComboBoxEnter::editTextChanged, [this](const QString& text) {
			SigPathChanged(text);
		});
		connect(ui.cmbPath, &QComboBoxEnter::SigEnterPressed, [this]() {
			SigPathBrowsed(ui.cmbPath->currentText());
		});
	}

	QFileFolderBrowser::~QFileFolderBrowser() {
	}

};	// namespace biscuit::qt

