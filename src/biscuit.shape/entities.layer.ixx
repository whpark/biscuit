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
// 2024-10-14. biscuit.
//
///////////////////////////////////////////////////////////////////////////////

#include "biscuit/macro.h"
#include "biscuit/dependencies_fmt.h"
#include "biscuit/dependencies_eigen.h"
#include "biscuit/dependencies_glaze.h"
#include "biscuit/dependencies_units.h"
#include "biscuit/dependencies_cereal.h"

#include "shape_macro.h"

export module biscuit.shape.entities.layer;
import std;
import biscuit;
import biscuit.shape.shape;

export namespace biscuit::shape {

	class xLayer : public xShape {
	public:
		string_t m_name;
		int m_flags{};
		shapes_t m_shapes;
		bool m_bUse{true};

	public:
		~xLayer() override = default;

	public:
		BSC__SHAPE_BASE_DEFINITION(xLayer, xShape, eSHAPE::layer);
		BSC__SHAPE_ARCHIVE_MEMBER(xLayer, xShape, 1u, m_name, m_flags, m_bUse, m_shapes);

		virtual std::optional<line_t> GetStartEndPoint() const override {
			if (m_shapes.empty())
				return {};
			auto r0 = m_shapes.front().GetStartEndPoint();
			if (!r0)
				return {};
			auto r1 = m_shapes.back().GetStartEndPoint();
			if (!r1)
				return {};
			return line_t{r0->pt0, r1->pt1};
		}
		virtual void FlipX() override { for (auto& shape : m_shapes) shape.FlipX(); }
		virtual void FlipY() override { for (auto& shape : m_shapes) shape.FlipY(); }
		virtual void FlipZ() override { for (auto& shape : m_shapes) shape.FlipZ(); }
		virtual void Reverse() override {
			std::ranges::reverse(m_shapes.Base());
			for (auto& shape : m_shapes) {
				shape.Reverse();
			}
		}
		virtual void Transform(ct_t const& ct, bool bRightHanded) override {
			for (auto& shape : m_shapes)
				shape.Transform(ct, bRightHanded);
		}
		virtual bool UpdateBounds(rect_t& bounds) const override {
			bool r{};
			for (auto& shape : m_shapes)
				r |= shape.UpdateBounds(bounds);
			return r;
		}
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			for (auto& shape : m_shapes) {
				shape.Draw(canvas);
			}
		}
		virtual bool DrawROI(ICanvas& canvas, rect_t const& roi) const override {
			bool result{};
			for (auto& shape : m_shapes) {
				result |= shape.DrawROI(canvas, roi);
			}
			return result;
		}
		//virtual void PrintOut(std::wostream& os) const override {
		//	xShape::PrintOut(os);
		//	for (auto& shape : m_shapes) {
		//		shape.PrintOut(os);
		//	}
		//}

		//virtual bool LoadFromCADJson(json_t& _j) override {
		//	xShape::LoadFromCADJson(_j);
		//	using namespace std::literals;
		//	gtl::bjson j(_j);
		//	m_name = j["name"];	// not a "layer"
		//	m_flags = j["flags"];
		//	m_bUse = j["plotF"];
		//	return true;
		//}

		void clear() {
			m_shapes.clear();
			m_strLineType.clear();
			m_flags = {};
			m_bUse = true;
			m_name.clear();
		}

		virtual void Sort_Loop();
		bool IsLoop(double dMinGap = 1.e-3) const;

		inline static bool RemoveConnectedComponents(std::deque<xShape const*>& shapes, xShape const* pSeed, double const dMinDistance = 0.000'1) {
			if (!pSeed)
				return false;

			std::deque<xShape const*> lst_found;
			lst_found.push_back(pSeed);

			while (lst_found.size()) {

				auto* p0 = lst_found.front();
				lst_found.pop_front();

				auto r = p0->GetStartEndPoint();
				if (!r)
					continue;
				auto const& [pt0, pt1] = *r;

				for (size_t i{}; i < shapes.size(); i++) {
					auto const* pNext = shapes[i];
					auto pts = pNext->GetStartEndPoint();
					if (!pts)
						continue;
					if ((pt0.GetDistance(pts->pt0) <= dMinDistance) or (pt0.GetDistance(pts->pt1) <= dMinDistance) or
						(pt1.GetDistance(pts->pt0) <= dMinDistance) or (pt1.GetDistance(pts->pt1) <= dMinDistance)) {
						lst_found.push_back(pNext);
						shapes.erase(shapes.begin()+i--);
					}
				}

				if (shapes.empty())
					break;
			}

			return true;
		}

	protected:
		friend class xDrawing;
		sLineType* pLineType{};

	};

	using layers_t = std::list<xLayer>;


}	// namespace biscuit::shape;

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xLayer);

