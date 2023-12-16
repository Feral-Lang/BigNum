#include "BigFltType.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// VarBigFlt /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBigFlt::VarBigFlt(const ModuleLoc *loc, long double _val)
	: Var(loc, typeID<VarBigFlt>(), false, false)
{
	mpfr_init_set_ld(val, _val, mpfr_get_default_rounding_mode());
}
VarBigFlt::VarBigFlt(const ModuleLoc *loc, mpfr_srcptr _val)
	: Var(loc, typeID<VarBigFlt>(), false, false)
{
	mpfr_init_set(val, _val, mpfr_get_default_rounding_mode());
}
VarBigFlt::VarBigFlt(const ModuleLoc *loc, mpz_srcptr _val)
	: Var(loc, typeID<VarBigFlt>(), false, false)
{
	mpfr_init_set_z(val, _val, mpfr_get_default_rounding_mode());
}
VarBigFlt::VarBigFlt(const ModuleLoc *loc, const char *_val)
	: Var(loc, typeID<VarBigFlt>(), false, false)
{
	mpfr_init_set_str(val, _val, 0, mpfr_get_default_rounding_mode());
}
VarBigFlt::~VarBigFlt() { mpfr_clear(val); }
Var *VarBigFlt::copy(const ModuleLoc *loc) { return new VarBigFlt(loc, val); }
void VarBigFlt::set(Var *from)
{
	mpfr_set(val, as<VarBigFlt>(from)->get(), mpfr_get_default_rounding_mode());
}
