#pragma once

#include "Common.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////// BigInt class //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

class VarBigInt : public Var
{
	mpz_t val;

public:
	VarBigInt(const ModuleLoc *loc, int64_t _val);
	VarBigInt(const ModuleLoc *loc, mpz_srcptr _val);
	VarBigInt(const ModuleLoc *loc, mpfr_srcptr _val);
	VarBigInt(const ModuleLoc *loc, const char *_val);
	~VarBigInt();

	Var *copy(const ModuleLoc *loc) override;
	void set(Var *from) override;

	inline mpz_ptr get() { return val; }
	// mpz_srcptr is basically 'const mpz_ptr'
	inline mpz_srcptr getSrc() { return val; }
};
