module;

//////////////////////////////////////////////////////////////////////
//
// shape_others.h:
//
// PWH
// 2017.07.20
// 2021.05.27
// 2021.08.11 Compare
// 2023-06-09 분리
// 2024-10-14. biscuit
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

#include "shape_macro.h"

export module biscuit.shape.entities.polyline;
import std;
import fmt;
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;
import biscuit.shape.entities.circle;
import biscuit.shape.entities.arc;
import biscuit.shape.entities.line;
import biscuit.shape.canvas;

export namespace biscuit::shape {

	//-----------------------------------------------------------------------------------------------------------------------------
	class xPolyline : public xShape {
	public:
		bool m_bLoop{};
		std::vector<polypoint_t> m_pts;

		BSC__SHAPE_BASE_DEFINITION(xPolyline, xShape, eSHAPE::polyline);
		BSC__SHAPE_ARCHIVE_MEMBER(xPolyline, xShape, 1u, m_bLoop, m_pts);

		//virtual point_t PointAt(double t) const override {};
		virtual std::optional<line_t> GetStartEndPoint() const override {
			if (m_pts.empty())
				return {};
			return line_t{ m_pts.front(), m_bLoop ? m_pts.front() : m_pts.back() };
		}
		virtual void FlipX() override { for (auto& pt : m_pts) { pt.x = -pt.x;  pt.Bulge() = -pt.Bulge(); } }
		virtual void FlipY() override { for (auto& pt : m_pts) { pt.y = -pt.y;  pt.Bulge() = -pt.Bulge(); } }
		virtual void FlipZ() override { for (auto& pt : m_pts) { pt.z = -pt.z;  pt.Bulge() = -pt.Bulge(); } }
		virtual void Reverse() override {
			if (m_bLoop) {
				if (m_pts.size()) {
					auto pt = m_pts.front();
					m_pts.erase(m_pts.begin());
					m_pts.push_back(pt);
				}
			}

			std::ranges::reverse(m_pts);

			if (m_pts.size() > 1) {
				auto b0 = m_pts.front().Bulge();
				for (size_t i{1}; i < m_pts.size(); i++) {
					m_pts[i-1].Bulge() = - m_pts[i].Bulge();
				}
				m_pts.back().Bulge() = - b0;

			}
		}
		virtual void Transform(ct_t const& ct, bool bRightHanded) override {
			for (auto& pt : m_pts) {
				(point_t&)pt = ct((point_t&)pt);
			}
			if (!bRightHanded) {
				for (auto& pt : m_pts) { pt.x = -pt.x;  pt.Bulge() = -pt.Bulge(); } 
			}
		};
		virtual bool UpdateBounds(rect_t& bounds) const override {
			bool bModified{};

			auto nPt = m_pts.size();
			if (!m_bLoop)
				nPt--;
			for (int iPt = 0; iPt < nPt; iPt++) {
				auto pt0 = m_pts[iPt];
				bModified |= bounds.UpdateBounds(pt0);
				if (pt0.Bulge() != 0.0) {
					auto iPt2 = (iPt+1) % m_pts.size();
					auto pt1 = m_pts[iPt2];
					xArc arc = xArc::GetFromBulge(pt0.Bulge(), pt0, pt1);
					bModified |= arc.UpdateBounds(bounds);
				}
			}

			return bModified;
		};
		virtual void Draw(ICanvas& canvas) const override;

		shapes_t Split() const;

		template < typename t_iterator >
		static std::unique_ptr<xPolyline> MergeShapeAsPolyline(t_iterator begin, t_iterator end, double dThreshold) {
			if (begin == end)
				return {};
			auto rPolyline = std::make_unique<xPolyline>();
			rPolyline->m_color = begin->m_color;
			rPolyline->m_bLoop = false;
			auto& pts = rPolyline->m_pts;
			if (auto ptSE = begin->GetStartEndPoint()) {
				pts.push_back(polypoint_t(ptSE->first));
			} else {
				return {};
			}

			for (auto pos = begin; pos != end; pos++) {
				xShape const& shape = *pos;
				auto const& ptBack = pts.back();
				if (shape.GetShapeType() == eSHAPE::line) {
					auto const& line = (xLine const&)shape;
					if (ptBack.GetDistance(line.m_pt0) > dThreshold) {
						continue;
					}
					pts.push_back(polypoint_t(line.m_pt1));
				} else if (shape.GetShapeType() == eSHAPE::arc_xy) {
					auto const& arc = (xArc const&)shape;
					auto ptC = arc.GetStartEndPoint();
					if (!ptC)
						continue;
					auto pt0 = ptC->first;
					auto pt1 = ptC->second;
					if (ptBack.GetDistance(pt0) > dThreshold) {
						continue;
					}
					pts.back().Bulge() = rad_t(arc.m_angle_length)/4.0_rad;
					pts.push_back(polypoint_t(pt1));
				}
				else {
					return {};	// if the source object includes not an line nor arc, cannot be reduced to polyline.
				}
			}
			if ( (pts.size() > 1) and point_t(pts.front()) == point_t(pts.back())) {
				pts.pop_back();
				rPolyline->m_bLoop = true;
			}
			return rPolyline;
		}
	};

	//-----------------------------------------------------------------------------------------------------------------------------
	class xPolylineLW : public xPolyline {
	public:
		BSC__SHAPE_BASE_DEFINITION(xPolylineLW, xPolyline, eSHAPE::lwpolyline);
		BSC__SHAPE_ARCHIVE_MEMBER(xPolylineLW, xPolyline, 1u);

		//virtual bool LoadFromCADJson(json_t& _j) override {
		//	xShape::LoadFromCADJson(_j);
		//	using namespace std::literals;
		//	gtl::bjson j(_j);

		//	auto jpts = j["vertlist"sv];
		//	auto apts = jpts.json().as_array();
		//	for (auto iter = apts.begin(); iter != apts.end(); iter++) {
		//		m_pts.push_back(polypoint_t{});
		//		m_pts.back() = PolyPointFrom(*iter);
		//	}

		//	m_bLoop = (j["flags"].value_or(0) & 1) != 0;
		//	return true;
		//}
	};

}

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xPolyline);
BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xPolylineLW);

