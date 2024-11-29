module;

#include <QWidget>
#include <QFileDialog>
#include "ui_QFileFolderBrowser.h"

#include "verdigris/wobjectcpp.h"
#include "verdigris/wobjectimpl.h"

export module biscuit.qt.Widgets.QFileFolderBrowser;

W_REGISTER_ARGTYPE(std::filesystem::path);

export namespace biscuit::qt {
	class QFileFolderBrowser : public QWidget {
		W_OBJECT(QFileFolderBrowser);

	public:
		bool m_bFileBrowser{true};

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

		connect(ui.cmbPath, &QComboBox::currentTextChanged, [this](const QString& text) {
			SigPathChanged(text);
		});

		connect(ui.cmbPath, &QComboBox::editTextChanged, [this](const QString& text) {
			SigPathChanged(text);
		});
	}

	QFileFolderBrowser::~QFileFolderBrowser() {
	}

};	// namespace biscuit::qt

