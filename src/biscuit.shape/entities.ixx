module;

//////////////////////////////////////////////////////////////////////
//
// entities.h:
//
// PWH
// 2017.07.20
// 2021.05.27
// 2021.08.11 Compare
// 2023-06-09 분리
// 2024-10-10. biscuit.
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/*

	1. CShapeObject 에서 변경 내용 :

		class name :

			CShapeObject	-> xShape
			CShapeLine		-> xLine
			CShapePolyLine	-> xPolyline
			...

		Container :

			TList -> boost::ptr_deque

		Point / Line : x, y, z 3차원으로 변경

			typedef xSize3d					size2d_t;
			typedef xPoint3d				point_t;
			typedef xRect3d					rect_t;
			typedef std::vector<xLine>		s_lines;

*/
///////////////////////////////////////////////////////////////////////////////

export module biscuit.shape.entities;
export import biscuit.shape.entities.layer;
export import biscuit.shape.entities.block;
export import biscuit.shape.entities.dot;
export import biscuit.shape.entities.circle;
export import biscuit.shape.entities.arc;
export import biscuit.shape.entities.line;
export import biscuit.shape.entities.polyline;
export import biscuit.shape.entities.text;
export import biscuit.shape.entities.spline;
export import biscuit.shape.entities.drawing;
export import biscuit.shape.entities.insert;
export import biscuit.shape.entities.hatch;

