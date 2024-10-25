module;

// 2024-10-22. biscuit. from mocha

export module biscuit.shape.dxf_loader;
import std;
import biscuit;
import biscuit.shape.color_table;
import biscuit.shape_basic;
import biscuit.shape.entities;
import dime.biscuit;

using namespace std::literals;

#if 0
export namespace biscuit::shape {

	class xDXFProgressThread;
	class IDXFLoader {
	public:
		virtual ~IDXFLoader() {}

	public:
		virtual std::optional<xDrawing> Load(std::filesystem::path const& path, callback_progress_t funcCallback = nullptr) = 0;
		virtual bool Save(std::filesystem::path const& path, layers_t const& layers, callback_progress_t funcCallback = nullptr) = 0;
	};


	class xDXFLoaderDime : public IDXFLoader {
	public:
		virtual ~xDXFLoaderDime() {}

	public:
		virtual std::optional<xDrawing> Load(std::filesystem::path const& path, callback_progress_t funcCallback = nullptr);
		virtual bool Save(std::filesystem::path const& path, layers_t const& layers, callback_progress_t funcCallback = nullptr);

		color_t GetColor(dimeState const* pState, dimeEntity const* pEntity);
		color_t GetColor(dimeLayer const* pLayer);

	protected:
		TContainerMap<std::string, xGroup, std::deque> m_blocks;
		//std::pair<std::string, xGroup*> m_currentBlock;
		xGroup* m_pCurrentBlock{};

		int AddEntity(layers_t& layers, dimeState const* const, dimeEntity *);
	protected:
		int m_nItem{};
		int m_iItemPos{};
		static int dimePosCallbackFunc(float, void *);
		int dimePosCallbackFunc(float rate);

	};


}	// namespace biscuit::shape
namespace biscuit::shape {

	point_t operator * (const dimeMatrix& m, point_t const& pt) {
		point_t result;
		double W = pt.x*m[3][0] + pt.y*m[3][1] + pt.z*m[3][2] + m[3][3];
		result.x = (pt.x*m[0][0] + pt.y*m[0][1] + pt.z*m[0][2] + m[0][3])/W;
		result.y = (pt.x*m[1][0] + pt.y*m[1][1] + pt.z*m[1][2] + m[1][3])/W;
		result.z = (pt.x*m[2][0] + pt.y*m[2][1] + pt.z*m[2][2] + m[2][3])/W; 
		return result;
	}

	color_t xDXFLoaderDime::GetColor(dimeState const* pState, dimeEntity const* pEntity) {
		if (!pEntity)
			return {};
		size_t iCR{};
		if (pEntity->getColorNumber() == 256) {
			if ( ("0"s == pEntity->getLayerName()) and pState and pState->getCurrentInsert()) {
				iCR = pState->getCurrentInsert()->getLayer()->getColorNumber();
			} else {
				iCR = pEntity->getLayer()->getColorNumber();
			}
		} else {
			iCR = pEntity->getColorNumber();
		}
		if (iCR >= s_tblColor.size())
			iCR = 0;

		return s_tblColor[iCR];
	}

	color_t xDXFLoaderDime::GetColor(dimeLayer const* pLayer) {
		if (!pLayer)
			return {};
		size_t iCR = pLayer->getColorNumber();
		if (iCR >= s_tblColor.size())
			iCR = 0;
		return s_tblColor[iCR];
	}

	std::optional<xDrawing> xDXFLoaderDime::Load(std::filesystem::path const& path, callback_progress_t funcCallback) {
		xDrawing drawing;

		std::ifstream f(path, std::ios_base::binary);

		FILE* fp = std::fopen((char const*)path.u8string().c_str(), "rb");
		if (!fp)
			return {};

		do {
			dimeInput in;
			if (!in.setFileHandle(fp))
				break;
			if (funcCallback) {
				in.setCallback([funcCallback](float r, void*) -> int { funcCallback((int)(r*100), false, false); return 1; }, this);
			}
		#if 0
			m_bShowProgress = bShowProgress;
			// Progress Bar
			if (bShowProgress && m_pDXFThread && m_pDXFThread->m_pDLG && m_pDXFThread->m_pDLG->m_hWnd) {
				m_pDXFThread->m_pDLG->ShowWindow(SW_SHOW);
				m_pDXFThread->m_pDLG->SetCurrentePos(0, 100);
			}

			dimeModel model;
			model.read(&in);

			int nItem = 0;

			int n = model.getNumLayers();
			for (int i = 0; i <  n; i++) {
				model.getLayer(i);
				const dimeLayer* pLayer = model.getLayer(i);

				CShapeCollection* pCollection = new CShapeCollection(GetColor(pLayer), U8toString(pLayer->getLayerName()));

				shapeCollections.Push(pCollection);
			}

			m_pCurrentBlock = NULL;
			m_blocks.DeleteAll();
			T_DIME_CALLBACK data;
			data.pThis = this;
			data.pShapeCollections = &shapeCollections;
			dimeEntitiesSection *es = (dimeEntitiesSection*) model.findSection("ENTITIES");
			m_nItem = es ? es->getNumEntities() : 0;
			m_iItemPos = 0;
			//model.traverseEntities(dimeCallbackFunc, &data, true, false);
			model.traverseEntities(dimeCallbackFunc, &data, false, true);
			m_pCurrentBlock = NULL;
			m_blocks.DeleteAll();

			rectBound.SetRect(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX);
			for (int i = 0; i < shapeCollections.N(); i++) {
				shapeCollections[i].GetBoundingRect(rectBound, FALSE);
			}

			// Close Progress Bar
			// Progress Bar
			if (/*bShowProgress && */m_pDXFThread && m_pDXFThread->m_pDLG && m_pDXFThread->m_pDLG->m_hWnd) {
				m_pDXFThread->m_pDLG->ShowWindow(SW_HIDE);
			}

	#endif
		} while (false);

		return {};
	}

	int xDXFLoaderDime::AddEntity(layers_t& pShapeCollections, const dimeState* const pState, dimeEntity* pEntity) {
	#if 0
		if (pShapeCollections->IsEmpty())
			return false;

		//{
		//	m_iItemPos++;
		//	if ((m_iItemPos % _max(5, (m_nItem/100))) == 0) {
		//		if (m_bShowProgress && m_pDXFThread && m_pDXFThread->m_pDLG)
		//			m_pDXFThread->m_pDLG->SetCurrentePos(50 + (int)((double)m_iItemPos/m_nItem), 100);
		//		TRACE("m_nItemPos : %d\n", m_iItemPos);
		//	}
		//}

		CString strName(U8toString(pEntity->getLayerName()));

		if ( (m_bStrictToLayerName ? TRUE : strName == _T("0")) && pState->getCurrentInsert())
			strName = U8toString(pState->getCurrentInsert()->getLayerName());
		else
			strName = U8toString(pEntity->getLayerName());

		if (pEntity->getLayer()->getFlags() & FLAG_DELETED) {
			return false;
		}

		if (strcmp(pEntity->getLayerName(), "63") == 0) {
			TRACE("Layer : %s\n", pEntity->getLayerName());
		}

		TRefPointer<CShapeCollection> rCollection = pShapeCollections->SearchByValue(strName);
		if (!rCollection) {
			//return false;
			rCollection = pShapeCollections->SearchByValue(_T("__Unknown__"));
			if (!rCollection) {
				rCollection = new CShapeCollection;
				rCollection->m_strName = _T("__Unknown__");
				pShapeCollections->Push(rCollection);
			}
			if (!rCollection)
				return false;
		}

		TRefPointer<CShapeObject> rObject;

		switch (pEntity->typeId()) {
		case dimeBase::dimeBaseType :
			break;
		case dimeBase::dimeRecordType :
			break;
		case dimeBase::dimeStringRecordType :
			break;
		case dimeBase::dimeFloatRecordType :
			break;
		case dimeBase::dimeDoubleRecordType :
			break;
		case dimeBase::dimeInt8RecordType :
			break;
		case dimeBase::dimeInt16RecordType :
			break;
		case dimeBase::dimeInt32RecordType :
			break;
		case dimeBase::dimeHexRecordType :
			break;
		case dimeBase::dimeRecordHolderType :
			break;
		case dimeBase::dimeClassType :
			break;
		case dimeBase::dimeUnknownClassType :
			break;
		case dimeBase::dimeObjectType :
			break;
		case dimeBase::dimeUnknownObjectType :
			break;
		case dimeBase::dimeEntityType :
			break;
		case dimeBase::dimeUnknownEntityType :
			{
				dimeUnknownEntity* pDimeUnknownEntity = dynamic_cast<dimeUnknownEntity*>(pEntity);
				if (!pDimeUnknownEntity)
					break;
				if (strcmp(pDimeUnknownEntity->getEntityName(), "ENDBLK") == 0) {
					m_pCurrentBlock = NULL;
				}
			}
			break;
		case dimeBase::dimePolylineType :
			{
				dimePolyline* pDimeLine = dynamic_cast<dimePolyline*>(pEntity);
				if (!pDimeLine)
					break;
				CShapePolyLine* pObject = new CShapePolyLine(GetColor(pState, pEntity));
				int nVertices = pDimeLine->getNumCoordVertices();
				for (int i = 0; i < nVertices; i++) {
					const dimeVertex* pVertex = pDimeLine->getCoordVertex(i);
					const dimeVec3f& p0 = pVertex->getCoords();
					double dBulge = pVertex->getBulge();
					pObject->AddPoint(pState->getMatrix() * CPoint2d(p0.x, p0.y), dBulge);
					pObject->m_bLoop = pDimeLine->getFlags() & dimePolyline::CLOSED;
				}
				rObject = pObject;
			}
			break;
		case dimeBase::dimeVertexType :
			break;
		case dimeBase::dimeFaceEntityType :
			break;
		case dimeBase::dimeExtrusionEntityType :
			break;
		case dimeBase::dime3DFaceType :
			break;
		case dimeBase::dimeSolidType :
			break;
		case dimeBase::dimeTraceType :
			break;
		case dimeBase::dimeLineType :
			{
				dimeLine* pDimeLine = dynamic_cast<dimeLine*>(pEntity);
				if (!pDimeLine)
					break;
				const dimeVec3f& p0 = pDimeLine->getCoords(0);
				const dimeVec3f& p1 = pDimeLine->getCoords(1);
				const dimeVec3f& pE = pDimeLine->getExtrusionDir();
				CShapeLine* pObject = new CShapeLine(GetColor(pState, pEntity));
				CPoint2d pt0(p0.x, p0.y), pt1(p1.x, p1.y);

				pt0 = pState->getMatrix() * pt0;
				pt1 = pState->getMatrix() * pt1;
				//if (pState) {
				//	CPoint2d ptShift(pState->getMatrix()[0][3], pState->getMatrix()[1][3]);
				//	pt0 += ptShift;
				//	pt1 += ptShift;
				//}

				pObject->AddPoint(pt0);
				pObject->AddPoint(pt1);

				rObject = pObject;
			}
			break;
		case dimeBase::dimeMTextType :
			{
				dimeMText* pDimeText = dynamic_cast<dimeMText*>(pEntity);
				if (!pDimeText)
					break;

				CShapeText* pText = new CShapeText(GetColor(pState, pEntity));
				pText->m_dHeight = pDimeText->dHeight;
				pText->m_dWidth = pDimeText->dWidth;

				rad_t dAngle(pDimeText->dRotationalAngle);		// in Radian
				if (pDimeText->eDrawingDirection == dimeMText::TD_T2B) {	// Top to Bottom
					dAngle -= rad_t(M_PI/2);
				}
				if (dAngle != 0.0_rad)
					pText->m_ct.RotateM(dAngle);
				pText->m_ct.SetOffset(pState->getMatrix() * CPoint2d(pDimeText->ptOrigin.x, pDimeText->ptOrigin.y));

				// {\fArial|b0|i0|c0|p34;12345g\PCBA\P1234567890abcdefghijk}
				// Text & Font
				CStringW strW;
				ConvUTF8ToUnicode(pDimeText->GetText(), strW);
				CStringW strText;
				CString strFont;// = _T("Tahoma");
				for (int i = 1; i < strW.GetLength(); i++) {
					wchar_t c = strW.GetAt(i);
					if (c == _T('\\')) {
						i++;
						wchar_t c2;
						if (strW.GetLength() > i)
							c2 = strW.GetAt(i);
						else
							break;

						if (c2 == 'f') {
							i++;
							if (strW.GetLength() <= i)
								break;
							int iNext = strW.Find(_T(';'), i);
							if (iNext <= 0)
								iNext = i;
							CStringW strParam = strW.Mid(i, iNext-i);
							TList<CStringW> strsParam;
							SplitString(strParam, _T('|'), strsParam);

							if (strFont.IsEmpty() && strsParam.N()) {
								strFont = strsParam[0];
							} else {
							}
							i = iNext;
						} else if ( c2 == 'P') {
							strText += _T('\n');
						}

						continue;
					}

					strText += c;
				}

				if (strText.GetLength() > 1)
					strText = strText.Left(strText.GetLength()-1);

				if (!strFont.IsEmpty()) {
					_tcscpy_s(pText->m_font.lfFaceName, strFont);
				}

				// Inserting Point
				/**
				 * Line spacing factor. 0.25 .. 4.0  
				 */
				//double lineSpacingFactor;
				pText->m_dLineSpacingFactor = pDimeText->dLineSpacingFactor;

				// Alignment
				switch (pDimeText->eAlign) {
				/**
				 * Attachment point.
				 *
				 * 1 = Top left, 2 = Top center, 3 = Top right,
				 * 4 = Middle left, 5 = Middle center, 6 = Middle right,
				 * 7 = Bottom left, 8 = Bottom center, 9 = Bottom right
				 */
				case 1 : pText->m_dwAlign = DT_LEFT  |DT_TOP;     break;
				case 2 : pText->m_dwAlign = DT_CENTER|DT_TOP;     break;
				case 3 : pText->m_dwAlign = DT_RIGHT |DT_TOP;     break;

				case 4 : pText->m_dwAlign = DT_LEFT  |DT_VCENTER; break;
				case 5 : pText->m_dwAlign = DT_CENTER|DT_VCENTER; break;
				case 6 : pText->m_dwAlign = DT_RIGHT |DT_VCENTER; break;

				case 7 : pText->m_dwAlign = DT_LEFT  |DT_BOTTOM;  break;
				case 8 : pText->m_dwAlign = DT_CENTER|DT_BOTTOM;  break;
				case 9 : pText->m_dwAlign = DT_RIGHT |DT_BOTTOM;  break;

				}

				pText->SetString(strText);

				rObject = pText;
			}
			break;
		case dimeBase::dimeTextType :
			{
				dimeText* pDimeText = dynamic_cast<dimeText*>(pEntity);
				if (!pDimeText)
					break;
				CShapeText* pText = new CShapeText(GetColor(pState, pEntity));
				pText->m_dHeight = pDimeText->getHeight() * 1.1;
				pText->m_dWidth = pDimeText->getWidth();
				pText->m_dLineSpacingFactor = 1.0;

				dimeVec3f ptOrigin, ptSecond;
				pDimeText->getOrigin(ptOrigin);
				if (!pDimeText->getSecond(ptSecond))
					ptSecond = ptOrigin;

				CStringW strW;
				ConvUTF8ToUnicode(pDimeText->getTextString(), strW);
				CStringW strText(strW);

				// wScale
				if (pDimeText->getWScale() != 0.0) {
					pText->m_ct.m_mat(1, 0) *= pDimeText->getWScale();
					pText->m_ct.m_mat(1, 1) *= pDimeText->getWScale();
				}

				/*! 0 = default, 2 = Backwards, 4 = Upside down */
				//textGenerationFlags;
				if (pDimeText->getTextGeneration() & BIT(1)) {
					pText->m_ct.FlipMX();
					//pText->m_ct.SetShift(0, 0);	// Don't need to.
				}
				if (pDimeText->getTextGeneration() & BIT(2)) {
					pText->m_ct.FlipMY();
					pText->m_ct.SetShift(0, pText->m_dHeight);
				}

				// Alignment
				pText->SetString(strText);

				// Alignment
				pText->m_dwAlign = DT_LEFT|DT_VCENTER;
				CPoint2d ptInsert(ptSecond.x, ptSecond.y);
				int vJustification = pDimeText->getVJust();
				switch (pDimeText->getHJust()) {
				case 0 : pText->m_dwAlign = DT_LEFT;   break;
				case 1 : pText->m_dwAlign = DT_CENTER; break;
				case 2 : pText->m_dwAlign = DT_RIGHT;  break;
				case 3 : pText->m_dwAlign = DT_LEFT;  vJustification = 0; break;
				case 4 : pText->m_dwAlign = DT_CENTER;vJustification = 0; break;
				case 5 : pText->m_dwAlign = DT_RIGHT; vJustification = 0; break;
				}
				switch (vJustification) {
				case 0 : pText->m_dwAlign |= DT_BOTTOM;  ptInsert.SetPoint(ptSecond.x, ptSecond.y); break;	// base line
				case 1 : pText->m_dwAlign |= DT_BOTTOM;  break;							// bottom
				case 2 : pText->m_dwAlign |= DT_VCENTER; break;
				case 3 : pText->m_dwAlign |= DT_TOP;     break;
				}

				ptInsert = pState->getMatrix() * ptInsert;
				rad_t dAngle(deg_t(pDimeText->getRotation()));
				if (dAngle != 0.0_rad) {
					pText->m_ct.SetOffset(ptInsert);
					pText->m_ct.RotateM(dAngle);
				} else {
					pText->m_ct.SetOffset(ptInsert);
				}

				pText->SetString(strText);

				rObject = pText;
			}
			break;
		case dimeBase::dimePointType :
			{
				dimePoint* pDimePoint = dynamic_cast<dimePoint*>(pEntity);
				if (pDimePoint) {
					CShapeDot* pObject = new CShapeDot(GetColor(pState, pEntity));
					if (pState) {
						pObject->m_pt = pState->getMatrix() * CPoint2d(pDimePoint->getCoords().x, pDimePoint->getCoords().y);
					} else {
						pObject->m_pt.x = pDimePoint->getCoords().x;
						pObject->m_pt.y = pDimePoint->getCoords().y;
					}
					rObject = pObject;
				}
			}
			break;
		case dimeBase::dimeBlockType :
			{
				dimeBlock* pDimeBlock = dynamic_cast<dimeBlock*>(pEntity);
				if (!pDimeBlock)
					break;

				CShapeGroup* pBlock = NULL;
				int iBlock = m_blocks.FindByValue(CString(pDimeBlock->getName()));
				if (iBlock < 0) {
					pBlock = new CShapeGroup(GetColor(pState, pEntity));
					TRefPointer<CShapeObject> rBlock(pBlock);
					pBlock->m_strName = pDimeBlock->getName();
					pBlock->m_pt.x = pDimeBlock->getBasePoint().x;
					pBlock->m_pt.y = pDimeBlock->getBasePoint().y;
					if (m_pCurrentBlock)
						m_pCurrentBlock->AddObject(rBlock);
					else
						m_blocks.Push((CShapeGroup*)rBlock.Detach());
				} else {
					pBlock = &m_blocks[iBlock];
				}

				m_pCurrentBlock = pBlock;
			}
			break;
		case dimeBase::dimeInsertType :
			{
				dimeInsert* pDimeInsert = dynamic_cast<dimeInsert*>(pEntity);
				if (!pDimeInsert)
					break;

				int iLayer = pShapeCollections->FindByValue(U8toString(pDimeInsert->getLayerName()));
				if (iLayer >= 0)
					rCollection = pShapeCollections->GetItem(iLayer);

				CShapeGroup* pBlock = NULL;
				int iBlock = m_blocks.FindByValue(CString(pDimeInsert->getBlock()->getName()));
				if (iBlock < 0)
					break;
				pBlock = &m_blocks[iBlock];

				for (int x = 0; x < pDimeInsert->GetColumnCount(); x++) {
					for (int y = 0; y < pDimeInsert->GetRowCount(); y++) {
						CShapeGroup block;
						block = m_blocks[iBlock];

						if ( (pDimeInsert->getScale().x != 1.0) || (pDimeInsert->getScale().y != 1.0) )
							block.Resize(pDimeInsert->getScale().x, pDimeInsert->getScale().y, block.m_pt);

						if (pDimeInsert->getRotAngle() != 0.0)
							block.Rotate(rad_t(deg_t(pDimeInsert->getRotAngle())), block.m_pt);	// To do : Check Rad or Deg

						CPoint2d ptOffset(pDimeInsert->getInsertionPoint().x, pDimeInsert->getInsertionPoint().y);
						ptOffset += CPoint2d(pDimeInsert->GetColumnSpacing() * x, pDimeInsert->GetRowSpacing() * y);
						ptOffset += CPoint2d(pDimeInsert->getBlock()->getBasePoint().x, pDimeInsert->getBlock()->getBasePoint().y);

						block.AddOffset(ptOffset);

						block.m_objects.First();
						while (block.m_objects.N())
							rCollection->m_objects.Push(block.m_objects.PopFirst());
					}
				}
			}
			break;
		case dimeBase::dimeCircleType :
			{
				dimeCircle* pDimeCircle = dynamic_cast<dimeCircle*>(pEntity);
				if (!pDimeCircle)
					break;

				CShapeCircle* pObject = new CShapeCircle(GetColor(pState, pEntity));
				CPoint2d pt;
				pt.x = pDimeCircle->getCenter().x;
				pt.y = pDimeCircle->getCenter().y;
				if (pState) {
					pt = pState->getMatrix() * pt;
				}
				pObject->m_ptCenter = pt;
				pObject->m_dRadius = pDimeCircle->getRadius();

				if (pDimeCircle->getExtrusionDir().z < 0) {
					pObject->FlipX(CPoint2d(0, 0));
				}

				rObject = pObject;
			}
			break;
		case dimeBase::dimeArcType :
			{
				dimeArc* pDimeArc = dynamic_cast<dimeArc*>(pEntity);
				if (!pDimeArc)
					break;

				CShapeArc* pObject = new CShapeArc(GetColor(pState, pEntity));
				dimeVec3f ptCenter;
				pDimeArc->getCenter(ptCenter);
				CPoint2d pt;
				pt.x = ptCenter.x;
				pt.y = ptCenter.y;
				if (pState) {
					pt = pState->getMatrix() * pt;
				}
				pObject->m_ptCenter = pt;

				pObject->m_dRadius = pDimeArc->getRadius();
				double dT0			= pDimeArc->getStartAngle();
				double dT1			= pDimeArc->getEndAngle();
				if ( pState->getCurrentInsert() && (pState->getCurrentInsert()->getRotAngle() != 0.0) ) {
					dT0 += pState->getCurrentInsert()->getRotAngle();
					dT1 += pState->getCurrentInsert()->getRotAngle();
				}
				dT0 *= M_PI/ 180.;
				dT1 *= M_PI/ 180.;
				if (dT1 < dT0)
					dT1 += M_PI*2;
				pObject->m_dT0		= dT0;
				pObject->m_dTLength	= dT1-dT0;
				if (pDimeArc->getExtrusionDir().z < 0) {
					pObject->FlipX(CPoint2d(0, 0));
				}
				rObject = pObject;
			}
			break;
		case dimeBase::dimeLWPolylineType :
			{
				dimeLWPolyline* pDimeLine = dynamic_cast<dimeLWPolyline*>(pEntity);
				if (!pDimeLine)
					break;

				CShapePolyLine* pObject = new CShapePolyLine(GetColor(pState, pEntity));
				int nVertices = pDimeLine->getNumVertices();
				for (int i = 0; i < nVertices; i++) {
					double x = pDimeLine->getXCoords()[i];
					double y = pDimeLine->getYCoords()[i];
					double dBulge = pDimeLine->getBulges()[i];
					if (pState)
						pObject->AddPoint(pState->getMatrix() * CPoint2d(x, y), dBulge);
					else
						pObject->AddPoint(CPoint2d(x, y), dBulge);
					pObject->m_bLoop = pDimeLine->getFlags() & dimePolyline::CLOSED;	// same w/ dimePolyline
				}
				if (pDimeLine->getExtrusionDir().z < 0) {
					pObject->FlipX(CPoint2d(0, 0));
				}
				rObject = pObject;
			}
			break;
		case dimeBase::dimeEllipseType :
			{
				dimeEllipse* pDimeEllipse = dynamic_cast<dimeEllipse*>(pEntity);
				if (!pDimeEllipse)
					break;

				CShapeEllipse* pObject = new CShapeEllipse(GetColor(pState, pEntity));
				CPoint2d pt;
				pt.x = pDimeEllipse->getCenter().x;
				pt.y = pDimeEllipse->getCenter().y;
				if (pState) {
					pt = pState->getMatrix() * pt;
				}
				pObject->m_ptCenter		= pt;
				pObject->m_dRadius1		= sqrt(_sqr(pDimeEllipse->getMajorAxisEndpoint().x) + _sqr(pDimeEllipse->getMajorAxisEndpoint().y));
				pObject->m_dRadius2		= pObject->m_dRadius1 * pDimeEllipse->getMinorMajorRatio();
				pObject->m_dTFirstAxis	= atan2(pDimeEllipse->getMajorAxisEndpoint().y, pDimeEllipse->getMajorAxisEndpoint().x);
				pObject->m_dT0			= pDimeEllipse->getStartParam();
				pObject->m_dTLength		= pDimeEllipse->getEndParam() - pDimeEllipse->getStartParam();

				if ( pState->getCurrentInsert() && (pState->getCurrentInsert()->getRotAngle() != 0.0) ) {
					pObject->m_dT0 += pState->getCurrentInsert()->getRotAngle() * M_PI / 180.;
				}

				if (pDimeEllipse->getExtrusionDir().z < 0) {
					pObject->FlipX(CPoint2d(0, 0));
				}

				rObject = pObject;
			}
			break;
		case dimeBase::dimeSplineType :
			{
				dimeSpline* pDimeSpline = dynamic_cast<dimeSpline*>(pEntity);
				if (!pDimeSpline)
					break;

				CShapeSpline* pObject = new CShapeSpline(GetColor(pState, pEntity));

				int i, n = pDimeSpline->getNumControlPoints();
				for (i = 0; i < n; i++) {
					const dimeVec3f p = pDimeSpline->getControlPoint(i);
					CPoint2d pt(p.x, p.y);
					if (pState)
						pt = pState->getMatrix() * pt;
					pObject->m_pts.push_back(pt);
				}

				n = pDimeSpline->getNumKnots();
				for (i = 0; i < n; i++) {
					pObject->m_dKnots.push_back(pDimeSpline->getKnotValue(i));
				}

				n = pDimeSpline->getNumFitPoints();
				for (i = 0; i < n; i++) {
					const dimeVec3f p = pDimeSpline->getFitPoint(i);
					CPoint2d pt(p.x, p.y);
					if (pState)
						pt = pState->getMatrix() * pt;
					pObject->m_ptsFit.push_back(pt);
				}

				n = pDimeSpline->getNumWeights();
				for (i = 0; i < n; i++) {
					pObject->m_dWeights.push_back(pDimeSpline->getWeight(i));
				}

				pObject->m_nDegree			= pDimeSpline->getDegree();
				pObject->m_dKnotTolerance	= pDimeSpline->getKnotTolerance();
				pObject->m_dFitTolerance		= pDimeSpline->getFitPointTolerance();
				pObject->m_dCPTolerance		= pDimeSpline->getControlPointTolerance();
				pObject->m_eFlags			= pDimeSpline->getFlags();

				rObject = pObject;
			}
			break;
		case dimeBase::dimeSectionType :
			break;
		case dimeBase::dimeUnknownSectionType :
			break;
		case dimeBase::dimeEntitiesSectionType :
			break;
		case dimeBase::dimeBlocksSectionType :
			break;
		case dimeBase::dimeTablesSectionType :
			break;
		case dimeBase::dimeHeaderSectionType :
			break;
		case dimeBase::dimeClassesSectionType :
			break;
		case dimeBase::dimeObjectsSectionType :
			break;
		case dimeBase::dimeTableType :
			break;
		case dimeBase::dimeTableEntryType :
			break;
		case dimeBase::dimeUnknownTableType :
			break;
		case dimeBase::dimeUCSTableType :
			break;
		case dimeBase::dimeLayerTableType :
			break;
		default :
			break;
		}

		if (rObject.GetPointer()) {
			if ( !(pState->getFlags() & dimeState::EXPLODE_INSERTS) && m_pCurrentBlock )
				m_pCurrentBlock->AddObject(rObject);
			else
				rCollection->AddObject(rObject);
		}
	#endif
		return true;
	}

}
#endif

