#ifndef _GUM_DTEX_C2_STRATEGY_H_
#define _GUM_DTEX_C2_STRATEGY_H_

#include <SM_Rect.h>
#include <CU_Singleton.h>
#include <multitask/Thread.h>

#include <map>
#include <set>
#include <vector>

#include <stdint.h>

namespace mt { class Mutex; }

namespace gum
{

class DTexC2Strategy
{
public:
	void OnC2QueryFail(uint32_t id, int tex_id, int tex_w, int tex_h, const sm::i16_rect& region);

	void Update();

	void Clear();

	void SetMaxC2Edge(int max_edge) { m_max_c2_edge = max_edge; }

private:
	struct Symbol
	{
		int16_t count;

		int16_t tex_id;
		int16_t tex_w, tex_h;
		
		sm::i16_rect region;

		Symbol() : count(0), tex_id(0), tex_w(0), tex_h(0) {}
	};

	class Package
	{
	public:
		Package(int pkg_id, int sym_count);
		~Package();

		void AddCount(int id, int tex_id, int tex_w, int tex_h, const sm::i16_rect& region);

		int GetSingleMaxCount() const { return m_single_max_count; }
		int GetDiffSprCount() const { return m_diff_spr_count; }
		int GetTotCount() const { return m_tot_count; }

		void Load();

		void Clear();
	
	private:
		int m_pkg_id;

		int m_single_max_count;
		int m_diff_spr_count;
		int m_tot_count;

		int m_sym_count;
		Symbol* m_symbols;

	}; // Package

private:
	void LoadPackage(Package* pkg);

	void AddLoadPkgTask(int pkg_id);
	void FlushLoadingTask();

private:
	std::map<int, Package*> m_pkgs;

	int m_max_no_update_count;
	int m_no_update_count;
	float m_discount;

	int m_single_max_count;
	int m_diff_spr_count;
	int m_tot_count;

	int m_max_c2_edge;

	mt::Mutex m_loading_tasks_mutex;
	std::vector<int> m_loading_tasks;

	SINGLETON_DECLARATION(DTexC2Strategy)

}; // DTexC2Strategy

}

#endif // _GUM_DTEX_C2_STRATEGY_H_