﻿module;

#include <QOpenGLWidget>
#include "biscuit/qt/macro.h"
#include "verdigris/wobjectcpp.h"
#include "verdigris/wobjectimpl.h"

export module biscuit.qt.MatView:Canvas;

export namespace biscuit::qt {

	class xMatViewCanvas : public QOpenGLWidget {
		friend class xMatView;
		W_OBJECT(xMatViewCanvas)

	public:
		using this_t = xMatViewCanvas;
		using base_t = QOpenGLWidget;

	protected:
		std::function<void(xMatViewCanvas*)> m_fnInitializeGL;
		std::function<void(xMatViewCanvas*)> m_fnPaintGL;
		std::function<void(xMatViewCanvas*, QMouseEvent* event)> m_fnMousePress;
		std::function<void(xMatViewCanvas*, QMouseEvent* event)> m_fnMouseRelease;
		std::function<void(xMatViewCanvas*, QMouseEvent* event)> m_fnMouseMove;
		std::function<void(xMatViewCanvas*, QWheelEvent* event)> m_fnWheel;

	public:
		xMatViewCanvas(QWidget* parent): QOpenGLWidget(parent) {}
		~xMatViewCanvas() {}

	protected:
		virtual void initializeGL() override {
			base_t::initializeGL();
			if (m_fnInitializeGL)
				m_fnInitializeGL(this);
		}
		virtual void paintGL() override {
			if (m_fnPaintGL)
				m_fnPaintGL(this);
		}

		virtual void mousePressEvent(QMouseEvent *event) override {
			if (m_fnMousePress)
				m_fnMousePress(this, event);
		}
		virtual void mouseReleaseEvent(QMouseEvent *event) override {
			if (m_fnMouseRelease)
				m_fnMouseRelease(this, event);
		}
		virtual void mouseMoveEvent(QMouseEvent *event) override {
			if (m_fnMouseMove)
				m_fnMouseMove(this, event);
		}
		virtual void wheelEvent(QWheelEvent* event) override {
			if (m_fnWheel)
				m_fnWheel(this, event);
		}
	};

	W_OBJECT_IMPL(xMatViewCanvas);

}	// namespace biscuit::qt
