#include "BigIntType.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// VarBigInt /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

VarBigInt::VarBigInt(ModuleLoc loc, int64_t _val) : Var(loc, typeID<VarBigInt>(), false, false)
{
	mpz_init_set_si(val, _val);
}
VarBigInt::VarBigInt(ModuleLoc loc, mpz_srcptr _val) : Var(loc, typeID<VarBigInt>(), false, false)
{
	mpz_init_set(val, _val);
}
VarBigInt::VarBigInt(ModuleLoc loc, mpfr_srcptr _val) : Var(loc, typeID<VarBigInt>(), false, false)
{
	mpz_init(val);
	mpfr_get_z(val, _val, mpfr_get_default_rounding_mode());
}
VarBigInt::VarBigInt(ModuleLoc loc, const char *_val) : Var(loc, typeID<VarBigInt>(), false, false)
{
	mpz_init_set_str(val, _val, 0);
}
VarBigInt::~VarBigInt() { mpz_clear(val); }
Var *VarBigInt::copy(ModuleLoc loc) { return new VarBigInt(loc, val); }
void VarBigInt::set(Var *from) { mpz_set(val, as<VarBigInt>(from)->get()); }
