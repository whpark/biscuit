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
#include "biscuit/dependencies/glaze.h"

#include "shape_macro.h"

export module biscuit.shape.entities.drawing;
import std;
import "biscuit/dependencies/fmt.hxx";
import "biscuit/dependencies/cereal.hxx";
import "biscuit/dependencies/eigen.hxx";
import biscuit;
import biscuit.shape_basic;
import biscuit.shape.entities.shape;
import biscuit.shape.entities.layer;
import biscuit.shape.entities.block;
import biscuit.shape.canvas;

export namespace biscuit::shape {

	class xDrawing : public xShape {
	public:
		using variable_t = std::variant<int, double, point_t, string_t>;
		std::map<std::string, variable_t> m_vars;
		std::deque<sLineType> m_line_types;
		//boost::ptr_deque<xBlock> blocks;
		rect_t m_bounds;
		layers_t m_layers;
		//TSmartPtrContainer<xLayer, TCloneablePtr, std::deque> m_layers;

	public:
		BSC__SHAPE_BASE_DEFINITION(xDrawing, xShape, eSHAPE::drawing);
		BSC__SHAPE_ARCHIVE_MEMBER(xDrawing, xShape, 1u, m_vars, m_line_types, m_layers);

		//virtual point_t PointAt(double t) const override { throw std::exception{GTL__FUNCSIG "not here."}; return point_t {}; }	// no PointAt();
		virtual std::optional<line_t> GetStartEndPoint() const override {
			if (m_layers.empty())
				return {};
			auto r0 = m_layers.front().second.GetStartEndPoint();
			if (!r0)
				return {};
			auto r1 = m_layers.back().second.GetStartEndPoint();
			if (!r1)
				return {};
			return line_t{ r0->pt0, r1->pt1 };
		}
		virtual void FlipX() override { for (auto& [name, layer] : m_layers) layer.FlipX(); }
		virtual void FlipY() override { for (auto& [name, layer] : m_layers) layer.FlipY(); }
		virtual void FlipZ() override { for (auto& [name, layer] : m_layers) layer.FlipZ(); }
		virtual void Reverse() override {
			std::ranges::reverse(m_layers);
			for (auto& [name, layer] : m_layers) {
				layer.Reverse();
			}
		}
		virtual void Transform(ct_t const& ct, bool bRightHanded) override {
			for (auto& [name, layer] : m_layers)
				layer.Transform(ct, bRightHanded);
		}
		virtual bool UpdateBounds(rect_t& rect) const override {
			bool r{};
			for (auto& [name, layer] : m_layers)
				r |= layer.UpdateBounds(rect);
			return r;
		}
		virtual void Draw(ICanvas& canvas) const override {
			xShape::Draw(canvas);
			for (auto& [name, layer] : m_layers) {
				layer.Draw(canvas);
			}
		}
		virtual bool DrawROI(ICanvas& canvas, rect_t const& rectROI) const override {
			bool result{};
			for (auto& [name, layer] : m_layers) {
				result |= layer.DrawROI(canvas, rectROI);
			}
			return result;
		}
		//virtual void PrintOut(std::wostream& os) const override {
		//	for (auto& [name, layer] : m_layers) {
		//		layer.PrintOut(os);
		//	}
		//}

	#if 0
		bool operator == (xDrawing const& B) const {
			if ((base_t const&)*this != (base_t const&)B)
				return false;
			bool bEQ = (m_vars == B.m_vars);
			bEQ &= (m_line_types == B.m_line_types);
			bEQ &= (m_rectBoundary == B.m_rectBoundary);
			bEQ &= (m_layers == B.m_layers);
			if (m_layers.size() == B.m_layers.size()) {
				for (size_t i{}; i < m_layers.size(); i++) {
					auto const& layerA = m_layers[i];
					auto const& layerB = B.m_layers[i];
					if (layerA.m_shapes.size() != layerB.m_shapes.size())
						return false;
					for (size_t iShape{}; iShape < layerA.m_shapes.size(); iShape++) {
						auto const& shapeA = layerA.m_shapes[iShape];
						auto const& shapeB = layerB.m_shapes[iShape];
						if ((xShape const&)shapeA != (xShape const&)shapeB)
							return false;
						if (shapeA != shapeB)
							return false;
					}
					if (layerA != layerB)
						return false;
				}
			}
			if (!bEQ)
				return false;
			return true;
		}
	#endif
		xLayer& Layer(string_t const& name) {
			return m_layers[name];
			//if (auto iter = std::ranges::find_if(m_layers, [&name](xLayer const& layer) -> bool { return name == layer.m_name; }); iter != m_layers.end())
			//	return *iter;
			//throws std::runtime_error(BSC__FUNCSIG "not found.");
		}
		xLayer const& Layer(string_t const& name) const {
			return m_layers[name];
			//if (auto iter = std::ranges::find_if(m_layers, [&name](xLayer const& layer) -> bool { return name == layer.m_name; }); iter != m_layers.end())
			//	return *iter;
			//throws std::runtime_error(BSC__FUNCSIG "not found.");
		}

		bool AddEntity(std::unique_ptr<xShape> rShape, std::map<string_t, xLayer*> const& mapLayers, std::map<string_t, xBlock*> const& mapBlocks, rect_t& bounds);

	public:
		//virtual bool LoadFromCADJson(json_t& _j) override;

		void clear() {
			m_vars.clear();
			m_line_types.clear();
			m_layers.clear();
		}
	};

}

BSC__SHAPE_EXPORT_ARCHIVE_REGISTER(biscuit::shape::xDrawing);

