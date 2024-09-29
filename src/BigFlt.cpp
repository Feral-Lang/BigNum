#include "BigFltType.hpp"
#include "BigIntType.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

Var *bigFltNewNative(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
		     const StringMap<AssnArgData> &assn_args)
{
	if(!args[1]->is<VarFlt>() && !args[1]->is<VarStr>() && !args[1]->is<VarBigInt>() &&
	   !args[1]->is<VarBigFlt>())
	{
		vm.fail(loc,
			"argument 1 to bignum.newFlt() must be of "
			"type 'flt', 'str', 'bigint', or 'bigflt', found: ",
			vm.getTypeName(args[1]));
		return nullptr;
	}

	if(args[1]->is<VarFlt>()) {
		return vm.makeVar<VarBigFlt>(loc, as<VarFlt>(args[1])->get());
	}
	if(args[1]->is<VarStr>()) {
		return vm.makeVar<VarBigFlt>(loc, as<VarStr>(args[1])->get().c_str());
	}
	if(args[1]->is<VarBigInt>()) {
		return vm.makeVar<VarBigFlt>(loc, as<VarBigInt>(args[1])->get());
	}
	return vm.makeVar<VarBigFlt>(loc, as<VarBigFlt>(args[1])->getSrc());
}

#define ARITHF_FUNC(fn, name, namez)                                                             \
	Var *bigFlt##fn(Interpreter &vm, ModuleLoc loc, Span<Var *> args,                        \
			const StringMap<AssnArgData> &assn_args)                                 \
	{                                                                                        \
		if(args[1]->is<VarBigInt>()) {                                                   \
			VarBigFlt *res =                                                         \
			vm.makeVar<VarBigFlt>(loc, as<VarBigFlt>(args[0])->get());               \
			mpfr_##namez(res->get(), res->getSrc(), as<VarBigInt>(args[1])->get(),   \
				     mpfr_get_default_rounding_mode());                          \
			return res;                                                              \
		} else if(args[1]->is<VarBigFlt>()) {                                            \
			VarBigFlt *res =                                                         \
			vm.makeVar<VarBigFlt>(loc, as<VarBigFlt>(args[0])->get());               \
			mpfr_##name(res->get(), res->getSrc(), as<VarBigFlt>(args[1])->getSrc(), \
				    mpfr_get_default_rounding_mode());                           \
			return res;                                                              \
		}                                                                                \
		vm.fail(loc, "expected int or flt arg for flt " STRINGIFY(name) ", found: ",     \
			vm.getTypeName(args[1]));                                                \
		return nullptr;                                                                  \
	}

#define ARITHF_ASSN_FUNC(fn, name, namez)                                                         \
	Var *bigFltAssn##fn(Interpreter &vm, ModuleLoc loc, Span<Var *> args,                     \
			    const StringMap<AssnArgData> &assn_args)                              \
	{                                                                                         \
		if(args[1]->is<VarBigInt>()) {                                                    \
			mpfr_##namez(                                                             \
			as<VarBigFlt>(args[0])->get(), as<VarBigFlt>(args[0])->getSrc(),          \
			as<VarBigInt>(args[1])->get(), mpfr_get_default_rounding_mode());         \
			return args[0];                                                           \
		} else if(args[1]->is<VarBigFlt>()) {                                             \
			mpfr_##name(                                                              \
			as<VarBigFlt>(args[0])->get(), as<VarBigFlt>(args[0])->getSrc(),          \
			as<VarBigFlt>(args[1])->getSrc(), mpfr_get_default_rounding_mode());      \
			return args[0];                                                           \
		}                                                                                 \
		vm.fail(loc, "expected int or flt arg for flt " STRINGIFY(name) "-assn, found: ", \
			vm.getTypeName(args[1]));                                                 \
		return nullptr;                                                                   \
	}

#define LOGICF_FUNC(name, checksym)                                                               \
	Var *bigFlt##name(Interpreter &vm, ModuleLoc loc, Span<Var *> args,                       \
			  const StringMap<AssnArgData> &assn_args)                                \
	{                                                                                         \
		if(args[1]->is<VarBigFlt>()) {                                                    \
			return mpfr_cmp(as<VarBigFlt>(args[0])->get(),                            \
					as<VarBigFlt>(args[1])->get()) checksym 0                 \
			       ? vm.getTrue()                                                     \
			       : vm.getFalse();                                                   \
		}                                                                                 \
		vm.fail(loc, "expected int or flt arg for flt " STRINGIFY(name) "-assn, found: ", \
			vm.getTypeName(args[1]));                                                 \
		return nullptr;                                                                   \
	}

ARITHF_FUNC(Add, add, add_z)
ARITHF_FUNC(Sub, sub, sub_z)
ARITHF_FUNC(Mul, mul, mul_z)
ARITHF_FUNC(Div, div, div_z)

ARITHF_ASSN_FUNC(Add, add, add_z)
ARITHF_ASSN_FUNC(Sub, sub, sub_z)
ARITHF_ASSN_FUNC(Mul, mul, mul_z)
ARITHF_ASSN_FUNC(Div, div, div_z)

LOGICF_FUNC(LT, <)
LOGICF_FUNC(GT, >)
LOGICF_FUNC(LE, <=)
LOGICF_FUNC(GE, >=)

Var *bigFltEQ(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
	      const StringMap<AssnArgData> &assn_args)
{
	if(!args[1]->is<VarBigFlt>()) return vm.getFalse();
	return mpfr_cmp(as<VarBigFlt>(args[0])->get(), as<VarBigFlt>(args[1])->get()) == 0
	       ? vm.getTrue()
	       : vm.getFalse();
}

Var *bigFltNE(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
	      const StringMap<AssnArgData> &assn_args)
{
	if(!args[1]->is<VarBigFlt>()) return vm.getTrue();
	return mpfr_cmp(as<VarBigFlt>(args[0])->get(), as<VarBigFlt>(args[1])->get()) != 0
	       ? vm.getTrue()
	       : vm.getFalse();
}

Var *bigFltPreInc(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
		  const StringMap<AssnArgData> &assn_args)
{
	mpfr_add_ui(as<VarBigFlt>(args[0])->get(), as<VarBigFlt>(args[0])->getSrc(), 1,
		    mpfr_get_default_rounding_mode());
	return args[0];
}

Var *bigFltPostInc(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
		   const StringMap<AssnArgData> &assn_args)
{
	VarBigFlt *res = vm.makeVar<VarBigFlt>(loc, as<VarBigFlt>(args[0])->get());
	mpfr_add_ui(as<VarBigFlt>(args[0])->get(), as<VarBigFlt>(args[0])->getSrc(), 1,
		    mpfr_get_default_rounding_mode());
	return res;
}

Var *bigFltPreDec(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
		  const StringMap<AssnArgData> &assn_args)
{
	mpfr_sub_ui(as<VarBigFlt>(args[0])->get(), as<VarBigFlt>(args[0])->getSrc(), 1,
		    mpfr_get_default_rounding_mode());
	return args[0];
}

Var *bigFltPostDec(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
		   const StringMap<AssnArgData> &assn_args)
{
	VarBigFlt *res = vm.makeVar<VarBigFlt>(loc, as<VarBigFlt>(args[0])->get());
	mpfr_sub_ui(as<VarBigFlt>(args[0])->get(), as<VarBigFlt>(args[0])->getSrc(), 1,
		    mpfr_get_default_rounding_mode());
	return res;
}

Var *bigFltUSub(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
		const StringMap<AssnArgData> &assn_args)
{
	VarBigFlt *res = vm.makeVar<VarBigFlt>(loc, as<VarBigFlt>(args[0])->get());
	mpfr_neg(res->get(), as<VarBigFlt>(args[0])->getSrc(), mpfr_get_default_rounding_mode());
	return res;
}

Var *bigFltRound(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
		 const StringMap<AssnArgData> &assn_args)
{
	VarBigInt *res = vm.makeVar<VarBigInt>(loc, 0);
	mpfr_get_z(res->get(), as<VarBigFlt>(args[0])->getSrc(), MPFR_RNDN);
	return res;
}

Var *bigFltPow(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
	       const StringMap<AssnArgData> &assn_args)
{
	if(!args[1]->is<VarBigInt>()) {
		vm.fail(loc, "power must be an integer, found: ", vm.getTypeName(args[1]));
		return nullptr;
	}
	VarBigFlt *res = vm.makeVar<VarBigFlt>(loc, 0);
	mpfr_pow_si(res->get(), as<VarBigFlt>(args[0])->getSrc(),
		    mpz_get_si(as<VarBigInt>(args[1])->get()), MPFR_RNDN);
	return res;
}

Var *bigFltRoot(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
		const StringMap<AssnArgData> &assn_args)
{
	if(!args[1]->is<VarBigInt>()) {
		vm.fail(loc, "root must be an integer, found: ", vm.getTypeName(args[1]));
		return nullptr;
	}
	VarBigFlt *res = vm.makeVar<VarBigFlt>(loc, 0);
#if MPFR_VERSION_MAJOR >= 4
	mpfr_rootn_ui(res->get(), as<VarBigFlt>(args[0])->get(),
		      mpz_get_ui(as<VarBigInt>(args[1])->get()), MPFR_RNDN);
#else
	mpfr_root(res->get(), as<VarBigFlt>(args[0])->get(), mpz_get_ui(INT(args[1])->get()),
		  MPFR_RNDN);
#endif // MPFR_VERSION_MAJOR
	return res;
}

Var *bigFltToFlt(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
		 const StringMap<AssnArgData> &assn_args)
{
	return vm.makeVar<VarFlt>(loc, mpfr_get_d(as<VarBigFlt>(args[0])->get(), MPFR_RNDN));
}

Var *bigFltToStr(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
		 const StringMap<AssnArgData> &assn_args)
{
	mpfr_exp_t expo;
	char *_res  = mpfr_get_str(NULL, &expo, 10, 0, as<VarBigFlt>(args[0])->getSrc(),
				   mpfr_get_default_rounding_mode());
	VarStr *res = vm.makeVar<VarStr>(loc, _res);
	mpfr_free_str(_res);
	if(res->get().empty() || expo == 0 || expo > 25) return res;
	auto last_zero_from_end = res->get().find_last_of("123456789");
	if(last_zero_from_end != String::npos) res->get().erase(last_zero_from_end + 1);
	if(expo > 0) {
		size_t sz = res->get().size();
		while(expo > sz) {
			res->get() += '0';
		}
		if(res->get()[0] == '-') ++expo;
		res->get().insert(expo, ".");
	} else {
		String pre_zero(-expo, '0');
		pre_zero.insert(pre_zero.begin(), '.');
		pre_zero.insert(pre_zero.begin(), '0');
		pre_zero += res->get();
		using namespace std;
		swap(res->get(), pre_zero);
	}
	return res;
}

// RNG

// [0.0, to]
Var *rngGetFlt(Interpreter &vm, ModuleLoc loc, Span<Var *> args,
	       const StringMap<AssnArgData> &assn_args)
{
	if(!args[1]->is<VarBigFlt>()) {
		vm.fail(loc,
			"expected upper bound to be a big float, found: ", vm.getTypeName(args[1]));
		return nullptr;
	}
	VarBigFlt *res = vm.makeVar<VarBigFlt>(loc, 0.0);
	mpfr_urandom(res->get(), rngstate, MPFR_RNDN);
	mpfr_mul(res->get(), res->getSrc(), as<VarBigFlt>(args[1])->getSrc(), MPFR_RNDN);
	return res;
}

INIT_MODULE(BigFlt)
{
	VarModule *mod = vm.getCurrModule();

	mod->addNativeFn("newFltNative", bigFltNewNative, 1);

	// register the BigFlt type (registerType)
	vm.registerType<VarBigFlt>(loc, "BigFlt");

	vm.addNativeTypeFn<VarBigFlt>(loc, "+", bigFltAdd, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, "-", bigFltSub, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, "*", bigFltMul, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, "/", bigFltDiv, 1);

	vm.addNativeTypeFn<VarBigFlt>(loc, "+=", bigFltAssnAdd, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, "-=", bigFltAssnSub, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, "*=", bigFltAssnMul, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, "/=", bigFltAssnDiv, 1);

	vm.addNativeTypeFn<VarBigFlt>(loc, "++x", bigFltPreInc, 0);
	vm.addNativeTypeFn<VarBigFlt>(loc, "x++", bigFltPostInc, 0);
	vm.addNativeTypeFn<VarBigFlt>(loc, "--x", bigFltPreDec, 0);
	vm.addNativeTypeFn<VarBigFlt>(loc, "x--", bigFltPostDec, 0);

	vm.addNativeTypeFn<VarBigFlt>(loc, "u-", bigFltUSub, 0);

	vm.addNativeTypeFn<VarBigFlt>(loc, "round", bigFltRound, 0);

	vm.addNativeTypeFn<VarBigFlt>(loc, "**", bigFltPow, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, "//", bigFltRoot, 1);

	vm.addNativeTypeFn<VarBigFlt>(loc, "<", bigFltLT, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, ">", bigFltGT, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, "<=", bigFltLE, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, ">=", bigFltGE, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, "==", bigFltEQ, 1);
	vm.addNativeTypeFn<VarBigFlt>(loc, "!=", bigFltNE, 1);

	vm.addNativeTypeFn<VarBigFlt>(loc, "flt", bigFltToFlt, 0);
	vm.addNativeTypeFn<VarBigFlt>(loc, "str", bigFltToStr, 0);

	// RNG

	mod->addNativeFn("getRandomFltNative", rngGetFlt, 1);

	return true;
}
