﻿#include "biscuit/qt/QCollapsibleGroupBox.h"

import std;
import biscuit;
import biscuit.qt.utils;

using namespace std::literals;

namespace biscuit::qt {
	QCollapsibleGroupBox::QCollapsibleGroupBox(QWidget* parent) : base_t(parent) {
		Init();
	}
	QCollapsibleGroupBox::QCollapsibleGroupBox(const QString& title, QWidget* parent) : base_t(title, parent) {
		Init();
	}
	QCollapsibleGroupBox::~QCollapsibleGroupBox() {
		m_animationsDeflate.removeAnimation(&m_aniPropMaxHeightD);
		m_animationsInflate.removeAnimation(&m_aniPropMaxHeight);
		m_animationsInflate.removeAnimation(&m_aniPropMaxHeightFinal);
		m_animationsInflate.removeAnimation(&m_aniPropMinHeightFinal);
	}

	void QCollapsibleGroupBox::Init() {
		PrepareAnimation();

		m_aniPropMaxHeight.setEasingCurve(QEasingCurve::OutExpo);
		m_aniPropMaxHeightD.setEasingCurve(QEasingCurve::OutExpo);

		m_aniPropMaxHeightD.setParent(this);
		m_aniPropMaxHeight.setParent(this);
		m_aniPropMaxHeightFinal.setParent(this);
		m_aniPropMinHeightFinal.setParent(this);

		m_aniPropMaxHeightD.setPropertyName("maximumHeight");
		m_aniPropMaxHeightD.setTargetObject(this);
		m_aniPropMaxHeight.setPropertyName("maximumHeight");
		m_aniPropMaxHeight.setTargetObject(this);
		m_aniPropMaxHeightFinal.setPropertyName("maximumHeight");
		m_aniPropMaxHeightFinal.setTargetObject(this);
		m_aniPropMaxHeightFinal.setDuration(0);
		m_aniPropMinHeightFinal.setPropertyName("minimumHeight");
		m_aniPropMinHeightFinal.setTargetObject(this);
		m_aniPropMinHeightFinal.setDuration(0);

		m_animationsDeflate.addAnimation(&m_aniPropMaxHeightD);

		m_animationsInflate.addAnimation(&m_aniPropMaxHeight);
		m_animationsInflate.addAnimation(&m_aniPropMaxHeightFinal);
		m_animationsInflate.addAnimation(&m_aniPropMinHeightFinal);

		connect(this, &QGroupBox::clicked, this, [this](bool checked) { Collapse(!checked); });
		connect(this, &QGroupBox::toggled, this, [this](bool checked) { Collapse(!checked); });
	}

	bool QCollapsibleGroupBox::PrepareAnimation(std::chrono::milliseconds durAnimation) {
		m_heightInflated = size().height();	// will be updated on deflation, too.
		m_heightMax0 = maximumHeight();
		m_heightMin0 = minimumHeight();
		m_durAnimation = durAnimation;
		return true;
	}

	bool QCollapsibleGroupBox::Collapse(bool bCollapse, std::optional<std::chrono::milliseconds> durAnimation) {
		auto dur = durAnimation.value_or(m_durAnimation).count();
		bool bNoAnimation = (dur <= 0);
		if (bNoAnimation) {
			m_animationsDeflate.stop();
			m_animationsInflate.stop();
		}

		if (m_bCollapsed == bCollapse)
			return true;
		m_bCollapsed = bCollapse;

		if (bCollapse) {
			if (this->isChecked()) {
				this->setChecked(false);
			}
			// Deflation
			this->setMinimumHeight(std::min(m_heightMin0, m_heightDeflated));

			if (bNoAnimation) {
				this->setMaximumHeight(m_heightDeflated);
			}
			else {
				// if already in deflation, do nothing
				if (m_animationsDeflate.state() == m_animationsDeflate.Running) {
					return true;
				}
				// stop inflation
				int h0 = size().height();
				auto dur = m_durAnimation.count();
				if (m_animationsInflate.state() == QAbstractAnimation::Running) {
					dur += (m_animationsInflate.currentTime() - m_animationsInflate.totalDuration());
					m_animationsInflate.stop();
				} else {
					// backup original height
					m_heightInflated = h0;
				}

				setMinimumHeight(std::min(m_heightMin0, m_heightDeflated));
				//this->setMaximumHeight(m_heightDeflated);
				m_aniPropMaxHeightD.setStartValue(h0);
				m_aniPropMaxHeightD.setEndValue(m_heightDeflated);
				m_aniPropMaxHeightD.setDuration(dur);
				m_animationsDeflate.start();
			}
		}
		else {
			if (!this->isChecked()) {
				this->setChecked(true);
			}
			// Inflation
			if (bNoAnimation) {
				this->setMaximumHeight(m_heightMax0);
				this->setMinimumHeight(m_heightMin0);
			}
			else {
				// if already in inflation, do nothing
				if (m_animationsInflate.state() == m_animationsInflate.Running) {
					return true;
				}
				// stop deflation
				int h0 = size().height();
				if (m_animationsDeflate.state() == m_animationsDeflate.Running) {
					// 남은 시간은 직전 animation의 남은 시간만큼 동작하도록. (rewinding), 직전 animation
					dur += (m_animationsDeflate.currentTime() - m_animationsDeflate.totalDuration());
					m_animationsDeflate.stop();
				}
				m_aniPropMaxHeight.setStartValue(h0);
				m_aniPropMaxHeight.setEndValue(m_heightInflated);
				m_aniPropMaxHeight.setDuration(dur);
				m_aniPropMaxHeightFinal.setStartValue(m_heightInflated);
				m_aniPropMaxHeightFinal.setEndValue(m_heightMax0);
				m_aniPropMinHeightFinal.setStartValue(m_heightDeflated);
				m_aniPropMinHeightFinal.setEndValue(m_heightMin0);
				m_animationsInflate.start();
			}
		}
		return true;
	}

	bool QCollapsibleGroupBox::IsCollapsed() const {
		//if (m_animationsDeflate.state() == m_animationsDeflate.Running) {
		//	return true;
		//}
		//if (m_animationsInflate.state() == m_animationsInflate.Running) {
		//	return false;
		//}
		return m_bCollapsed;
	}

} // namespace biscuit::qt
