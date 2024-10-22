module;

// 2024-10-22. biscuit. from mocha

#include <dime/config.h>
#include <dime/dime.h>

export module biscuit.shape.dxf_loader;
import std;
import biscuit;
import biscuit.shape.color_table;
import biscuit.shape.shape;
import biscuit.shape.entities;

export namespace biscuit::shape {

	class xDXFProgressThread;
	class IDXFLoader {
	private:
		static const color_t s_dxfColors[256];

	public:
		IDXFLoader();
		virtual ~IDXFLoader();

	protected:
		bool m_bShowProgress;
	public:
		struct sShapeResult {
			rect_t rectBound;
			shapes_t shapes;
		};
		virtual sShapeResult Load(std::filesystem::path const& path, callback_progress_t funcCallback = nullptr) = 0;
		virtual bool Save(std::filesystem::path const& path, layers_t const& layers, callback_progress_t funcCallback = nullptr) = 0;
	};


	class xDXFLoaderDime : public IDXFLoader {
	public:
		xDXFLoaderDime();
		virtual ~xDXFLoaderDime();

	public:
		virtual sShapeResult Load(std::filesystem::path const& path, callback_progress_t funcCallback = nullptr);
		virtual bool Save(std::filesystem::path const& path, layers_t const& layers, callback_progress_t funcCallback = nullptr);

		color_t GetColor(dimeState const* pState, dimeEntity const* pEntity);
		color_t GetColor(dimeLayer const* pLayer);

	protected:
		std::list<xGroup> m_blocks;
		xGroup* m_pCurrentBlock{};

		static bool dimeCallbackFunc(dimeState const* const, dimeEntity *, void *);
		bool dimeCallbackFunc(layers_t& layers, dimeState const* const, dimeEntity *);
	protected:
		int m_nItem;
		int m_iItemPos;
		static int dimePosCallbackFunc(float, void *);
		int dimePosCallbackFunc(float rate);

	};


}	// namespace biscuit::shape

