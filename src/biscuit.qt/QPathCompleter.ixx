module;

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "verdigris/wobjectcpp.h"
#include "verdigris/wobjectimpl.h"

#include <QtWidgets/QCompleter>
#include <QtGui/QFileSystemModel>

export module biscuit.qt.QPathCompleter;
import std;
import biscuit;
import biscuit.qt.utils;

using namespace std::literals;

export namespace biscuit::qt {

	class QPathCompleter : public QCompleter {
		W_OBJECT(QPathCompleter)
	public:
		using this_t = QPathCompleter;
		using base_t = QCompleter;

	protected:
		QFileSystemModel m_model;

	public:
		QPathCompleter(QObject *parent);
		QPathCompleter(QObject *parent, std::filesystem::path const& root, bool bCaseSensitive = false);
		~QPathCompleter();

		bool Init(std::filesystem::path const& root, bool bCaseSensitive = false);
	};

	W_OBJECT_IMPL(QPathCompleter);

	QPathCompleter::QPathCompleter(QObject* parent) : QCompleter(parent), m_model(parent) {
	}

	QPathCompleter::QPathCompleter(QObject* parent, std::filesystem::path const& root, bool bCaseSensitive) : QPathCompleter(parent) {
		Init(root, bCaseSensitive);
	}

	QPathCompleter::~QPathCompleter() {
	}

	bool QPathCompleter::Init(std::filesystem::path const& root, bool bCaseSensitive) {
		m_model.setRootPath(ToQString(root));
		setModel(&m_model);

		setCompletionMode(QCompleter::PopupCompletion);
		setCaseSensitivity(bCaseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive);

		return true;
	}


}	// namespace biscuit::qt
