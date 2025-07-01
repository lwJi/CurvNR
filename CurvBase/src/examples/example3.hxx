//======================================================================
//  multipatch_runtime.hpp  –  ONE header, three geometries
//
//  • PatchMode::Cartesian   (1 Cartesian block)
//  • PatchMode::Spherical   (1 spherical shell)
//  • PatchMode::CubeSphere  (6 wedges  +  1 Cartesian core = 7 patches)
//
//  All functions carry CCTK_HOST / CCTK_DEVICE so the same interface
//  works from host and GPU kernels.
//======================================================================
#pragma once
#include <array>
#include <cmath>
#include <cstddef>

//----------------------------------
//  GPU/host qualifier alias
//----------------------------------
#ifndef CCTK_HOST
# define CCTK_HOST
# define CCTK_DEVICE
#endif
#define HD CCTK_HOST CCTK_DEVICE

//----------------------------------
//  Small helpers
//----------------------------------
using Coord = std::array<double,3>;
template<class T> HD constexpr T sq(T x){ return x*x; }

//======================================================================
//  1.  Meta data for each patch type
//======================================================================
struct CartesianMeta { };                         // nothing for now

struct SphericalMeta { double r_min, r_max; };    // inner / outer radius

enum class Face { PX,NX,PY,NY,PZ,NZ };
struct CubedSphereMeta { Face face; double r_in, r_out; };

//======================================================================
//  2.  PatchMap + Patch POD
//======================================================================
struct PatchMap {
  HD Coord (*local_to_global)(const Coord&, const void*) = nullptr;
  HD Coord (*global_to_local)(const Coord&, const void*) = nullptr;
  HD bool  (*is_valid_local) (const Coord&, const void*) = nullptr;
};

struct Patch { PatchMap map{}; void* meta=nullptr; };

//======================================================================
//  3.  MultiPatch container (fixed capacity, trivially-copyable)
//======================================================================
template<std::size_t N>
class MultiPatch {
  Patch       patches_[N]{};
  std::size_t count_{0};

public:
  HD std::size_t add_patch(const Patch& p)
  { return (count_<N)?(patches_[count_]=p,count_++):N; }

  HD std::size_t size() const { return count_; }

  HD Coord local_to_global(std::size_t id,const Coord& l) const
  { return patches_[id].map.local_to_global(l, patches_[id].meta); }

  HD Coord global_to_local(const Coord& g,std::size_t& id_out) const
  {
    for(std::size_t i=0;i<count_;++i){
      Coord l = patches_[i].map.global_to_local(g, patches_[i].meta);
      if(patches_[i].map.is_valid_local(l, patches_[i].meta)){
        id_out=i; return l;
      }
    }
    id_out=static_cast<std::size_t>(-1);
    return {0,0,0};
  }
};

//======================================================================
//  4.  Geometry-specific mappings + validity predicates
//======================================================================
//-- Cartesian ----------------------------------------------------------
HD inline Coord cart_l2g(const Coord& l,const void*) { return l; }
HD inline Coord cart_g2l(const Coord& g,const void*) { return g; }
HD inline bool  cart_valid(const Coord&,const void*) { return true; } // whole ℝ³

//-- Spherical shell ----------------------------------------------------
HD inline Coord sph_l2g(const Coord& l,const void* m)
{
  const auto* p=static_cast<const SphericalMeta*>(m);
  double ρ=l[0], θ=l[1], φ=l[2];
  double r=p->r_min+ρ*(p->r_max-p->r_min);
  return { r*std::sin(φ)*std::cos(θ),
           r*std::sin(φ)*std::sin(θ),
           r*std::cos(φ) };
}
HD inline Coord sph_g2l(const Coord& g,const void* m)
{
  const auto* p=static_cast<const SphericalMeta*>(m);
  double x=g[0],y=g[1],z=g[2];
  double r=std::sqrt(sq(x)+sq(y)+sq(z));
  return { (r-p->r_min)/(p->r_max-p->r_min), std::atan2(y,x), std::acos(z/r) };
}
HD inline bool  sph_valid(const Coord& l,const void*){
  return (l[0]>=0&&l[0]<=1)&&(l[1]>=0&&l[1]<=2*M_PI)&&(l[2]>=0&&l[2]<=M_PI);
}

//-- Cubed-sphere wedge --------------------------------------------------
HD inline Coord cube_l2g(const Coord& l,const void* m){
  const auto* p=static_cast<const CubedSphereMeta*>(m);
  double ξ=l[0],η=l[1],ρ=l[2];
  double r=p->r_in+ρ*(p->r_out-p->r_in);
  double d=std::sqrt(1+ξ*ξ+η*η);
  switch(p->face){
    case Face::PX: return { r/d,  r*ξ/d,  r*η/d};
    case Face::NX: return {-r/d, -r*ξ/d,  r*η/d};
    case Face::PY: return {-r*ξ/d,  r/d,  r*η/d};
    case Face::NY: return { r*ξ/d, -r/d,  r*η/d};
    case Face::PZ: return { r*ξ/d,  r*η/d,  r/d};
    case Face::NZ: return { r*ξ/d, -r*η/d, -r/d};
  } return {0,0,0};
}
HD inline Coord cube_g2l(const Coord& g,const void* m){
  const auto* p=static_cast<const CubedSphereMeta*>(m);
  double x=g[0],y=g[1],z=g[2], r=std::sqrt(sq(x)+sq(y)+sq(z));
  double ξ=0,η=0;
  switch(p->face){
    case Face::PX: ξ= y/x;  η= z/x; break; case Face::NX: ξ=-y/x; η= z/x; break;
    case Face::PY: ξ=-x/y;  η= z/y; break; case Face::NY: ξ= x/y; η= z/y; break;
    case Face::PZ: ξ= x/z;  η= y/z; break; case Face::NZ: ξ= x/z; η=-y/z; break;
  }
  double ρ=(r-p->r_in)/(p->r_out-p->r_in);
  return {ξ,η,ρ};
}
HD inline bool  cube_valid(const Coord& l,const void*){
  return (l[0]>=-1&&l[0]<=1)&&(l[1]>=-1&&l[1]<=1)&&(l[2]>=0&&l[2]<=1);
}

//======================================================================
//  5.  Helpers that create individual Patch objects
//======================================================================
inline Patch make_cart_patch(){
  static CartesianMeta meta{};
  return Patch{{cart_l2g, cart_g2l, cart_valid}, &meta};
}
inline Patch make_sph_patch(double r0,double r1){
  static SphericalMeta meta{r0,r1};
  return Patch{{sph_l2g, sph_g2l, sph_valid}, &meta};
}
inline Patch make_wedge(Face f,double r0,double r1){
  static CubedSphereMeta meta{f,r0,r1};
  return Patch{{cube_l2g, cube_g2l, cube_valid}, &meta};
}

//======================================================================
//  6.  High-level selector
//======================================================================
enum class PatchMode { Cartesian, Spherical, CubeSphere };

struct PatchParams {
  double r_min  = 1.0;   // spherical & wedge inner radius
  double r_max  = 2.0;   // spherical & wedge outer radius
  double cube_h = 1.0;   // edge of Cartesian core (unused by mapping)
};

//-- Wrapper that can hold 1-patch or 7-patch variants ---------------
struct AnyMultiPatch
{
  PatchMode mode{PatchMode::Cartesian};

  union {
    MultiPatch<1> mp1;
    MultiPatch<7> mp7;
  };

  AnyMultiPatch() : mp1{} {}                            // default Cartesian

  // uniform API
  HD std::size_t size() const
  { return (mode==PatchMode::CubeSphere)? mp7.size() : mp1.size(); }

  HD Coord local_to_global(std::size_t id,const Coord& l) const
  { return (mode==PatchMode::CubeSphere)? mp7.local_to_global(id,l)
                                        : mp1.local_to_global(id,l); }

  HD Coord global_to_local(const Coord& g,std::size_t& out) const
  { return (mode==PatchMode::CubeSphere)? mp7.global_to_local(g,out)
                                        : mp1.global_to_local(g,out); }
};

//-- Singleton accessor -----------------------------------------------
inline AnyMultiPatch& active_mp(){
  static AnyMultiPatch holder;
  return holder;
}

//-- Initialise once at program start ---------------------------------
inline void init_global_multipatch(PatchMode mode,const PatchParams& p={})
{
  AnyMultiPatch tmp;
  tmp.mode = mode;

  if(mode==PatchMode::Cartesian){
    tmp.mp1.add_patch(make_cart_patch());
  }
  else if(mode==PatchMode::Spherical){
    tmp.mp1.add_patch(make_sph_patch(p.r_min, p.r_max));
  }
  else { // CubeSphere: 6 wedges + central Cartesian at the end
    tmp.mp7.add_patch(make_wedge(Face::PX,p.r_min,p.r_max)); // id 0
    tmp.mp7.add_patch(make_wedge(Face::NX,p.r_min,p.r_max)); // id 1
    tmp.mp7.add_patch(make_wedge(Face::PY,p.r_min,p.r_max)); // id 2
    tmp.mp7.add_patch(make_wedge(Face::NY,p.r_min,p.r_max)); // id 3
    tmp.mp7.add_patch(make_wedge(Face::PZ,p.r_min,p.r_max)); // id 4
    tmp.mp7.add_patch(make_wedge(Face::NZ,p.r_min,p.r_max)); // id 5
    tmp.mp7.add_patch(make_cart_patch());                    // id 6 (core)
  }

  active_mp() = tmp;   // move into the global store
}


// How to use
#include "multipatch_runtime.hpp"
#include <iostream>

int main()
{
  // Pick geometry (e.g. from a parameter file)
  init_global_multipatch(PatchMode::CubeSphere,
                         {1.0, 2.0, 1.0});   // r_min, r_max, cube_h

  // Access anywhere
  auto& mp = active_mp();
  std::cout << "patch count = " << mp.size() << '\n';

  Coord logical{0.0, 0.0, 0.3};   // (ξ,η,ρ) on +Y wedge (id 2)
  Coord xyz = mp.local_to_global(2, logical);

  std::size_t pid;
  Coord back = mp.global_to_local(xyz, pid);

  std::cout << "xyz  = " << xyz[0] << ' ' << xyz[1] << ' ' << xyz[2] << '\n'
            << "came from patch " << pid << '\n';
}

