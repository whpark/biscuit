#pragma once

#include "biscuit/config.h"
#include "biscuit/macro.h"
#include "biscuit/qt/macro.h"

#include <QtWidgets/QCompleter>
#include <QtGui/QFileSystemModel>

namespace biscuit::qt { class BSC__QT_CLASS QPathCompleter ; }

class biscuit::qt::QPathCompleter : public QCompleter {
	Q_OBJECT
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
