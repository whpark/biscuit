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
		Point / Line : x, y, z (supports 3d, but coded in 2d)

*/
///////////////////////////////////////////////////////////////////////////////

export module biscuit.shape;
export import biscuit.shape_basic;
export import biscuit.shape.color_table;
export import biscuit.shape.entities;
export import biscuit.shape.canvas;
export import biscuit.shape.dxf_loader;

