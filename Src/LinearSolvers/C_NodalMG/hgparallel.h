#ifndef _HGPARALLEL_H_
#define _HGPARALLEL_H_

#include <MultiFab.H>

inline bool is_remote(const MultiFab& r, int igrid)
{
    if (ParallelDescriptor::MyProc() == r.DistributionMap()[igrid]) return false;
    return true;
}

inline bool is_local(const MultiFab& r, int igrid)
{
    return ! is_remote(r, igrid);
}

#ifdef BL_USE_NEW_HFILES
#include <list>
using namespace std;
#else
#include <list.h>
#endif

class task
{
public:
    virtual ~task() {}
    virtual bool ready() = 0;
};

class task_fab : public task
{
public:
    virtual const FArrayBox& fab() const = 0;
};

class task_copy : public task
{
public:
    task_copy(FArrayBox* fab_, const MultiFab& mf_, int grid_, const Box& bx);
    task_copy(MultiFab* mf, int dgrid, const MultiFab& smf_, int sgrid, const Box& bx);
    task_copy(MultiFab* mf, int dgrid, const Box& db, const MultiFab& smf, int sgrid, const Box& sb);
    virtual ~task_copy();
    virtual bool ready();
private:
    FArrayBox* m_fab;
    MultiFab* m_mf;
    const MultiFab& m_smf;
    int m_dgrid;
    const int m_sgrid;
    const Box m_bx;
    const Box s_bx;
    bool m_ready;
};

class task_list
{
public:
    void add_task(task* t);
    void execute();
private:
    list<task*> tasks;
};

class task_fab_get : public task_fab
{
public:
    task_fab_get(const MultiFab& r_, int grid_);
    task_fab_get(const MultiFab& r_, int grid_, const Box& bx);
    virtual ~task_fab_get();
    virtual const FArrayBox& fab() const;
    virtual bool ready();
private:
    const MultiFab& r;
    const int grid;
    const Box bx;
};

class level_interface;
class amr_boundary_class;

class task_fill_patch : public task_fab
{
public:
    task_fill_patch(const Box& region_,
	const MultiFab& r_, const level_interface& lev_interface_, const amr_boundary_class* bdy_, int idim_ = 0, int index_ = 0);
    task_fill_patch(const Box& region_, int ncomp_, 
	const MultiFab& r_, const level_interface& lev_interface_, const amr_boundary_class* bdy_, int idim_ = 0, int index_ = 0);
    virtual ~task_fill_patch();
    virtual const FArrayBox& fab() const;
    virtual bool ready();
private:
    bool fill_patch_blindly();
    bool fill_exterior_patch_blindly();
    void fill_patch();
    bool newed;
    FArrayBox* target;
    const Box region;
    const MultiFab& r;
    const level_interface& lev_interface;
    const amr_boundary_class* bdy;
    const int idim;
    const int index;
    task_list tl;
};



#endif

