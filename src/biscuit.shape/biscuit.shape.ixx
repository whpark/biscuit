module;

//////////////////////////////////////////////////////////////////////
//
// shape.ixx:
//
// PWH
// 2017.07.20
// 2021.05.27
// 2024-10-10 biscuit
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

export module biscuit.shape;
export import biscuit.shape.color_table;
export import biscuit.shape.shape;
export import biscuit.shape.entities;

