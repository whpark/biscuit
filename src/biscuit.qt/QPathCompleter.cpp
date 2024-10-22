#include "biscuit/qt/QPathCompleter.h"

import std;
import biscuit;
import biscuit.qt.utils;

namespace biscuit::qt {

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
