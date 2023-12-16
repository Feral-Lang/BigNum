#pragma once

#include "Common.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// BigFlt class //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

class VarBigFlt : public Var
{
	mpfr_t val;

public:
	VarBigFlt(const ModuleLoc *loc, long double _val);
	VarBigFlt(const ModuleLoc *loc, mpfr_srcptr _val);
	VarBigFlt(const ModuleLoc *loc, mpz_srcptr _val);
	VarBigFlt(const ModuleLoc *loc, const char *_val);
	~VarBigFlt();

	Var *copy(const ModuleLoc *loc) override;
	void set(Var *from) override;

	inline mpfr_ptr get() { return val; }
	// mpfr_srcptr is basically 'const mpfr_ptr'
	inline mpfr_srcptr getSrc() { return val; }
};
