#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_test.h"

class test : public QMainWindow {
	Q_OBJECT
public:
	//biscuit::qt::xMatView* m_view{};

public:
	test(QWidget* parent = nullptr);
	~test();

private:
	Ui::testClass ui;
};
