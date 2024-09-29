#pragma once

#include "Common.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// BigFlt class //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

class VarBigFlt : public Var
{
	mpfr_t val;

public:
	VarBigFlt(ModuleLoc loc, long double _val);
	VarBigFlt(ModuleLoc loc, mpfr_srcptr _val);
	VarBigFlt(ModuleLoc loc, mpz_srcptr _val);
	VarBigFlt(ModuleLoc loc, const char *_val);
	~VarBigFlt();

	Var *copy(ModuleLoc loc) override;
	void set(Var *from) override;

	inline mpfr_ptr get() { return val; }
	// mpfr_srcptr is basically 'const mpfr_ptr'
	inline mpfr_srcptr getSrc() { return val; }
};
