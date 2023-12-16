#include "BigFltType.hpp"
#include "BigIntType.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// Functions ////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

Var *bigIntNewNative(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		     const Map<String, AssnArgData> &assn_args)
{
	if(!args[1]->is<VarInt>() && !args[1]->is<VarStr>() && !args[1]->is<VarBigInt>() &&
	   !args[1]->is<VarBigFlt>())
	{
		vm.fail(loc,
			"argument 1 to bignum.newInt() must be of "
			"type 'int', 'str', 'bigint', or 'bigflt', found: ",
			vm.getTypeName(args[1]));
		return nullptr;
	}

	if(args[1]->is<VarInt>()) {
		return vm.makeVar<VarBigInt>(loc, as<VarInt>(args[1])->get());
	}
	if(args[1]->is<VarStr>()) {
		return vm.makeVar<VarBigInt>(loc, as<VarStr>(args[1])->get().c_str());
	}
	if(args[1]->is<VarBigInt>()) {
		return vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[1])->get());
	}
	return vm.makeVar<VarBigFlt>(loc, as<VarBigFlt>(args[1])->getSrc());
}

#define ARITHI_FUNC(fn, name)                                                                    \
	Var *bigInt##fn(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,                 \
			const Map<String, AssnArgData> &assn_args)                               \
	{                                                                                        \
		if(args[1]->is<VarBigInt>()) {                                                   \
			VarBigInt *res =                                                         \
			vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());            \
			mpz_##name(res->get(), res->getSrc(), as<VarBigInt>(args[1])->getSrc()); \
			return res;                                                              \
		} else if(args[1]->is<VarBigFlt>()) {                                            \
			VarBigInt *res =                                                         \
			vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());            \
			mpz_t tmp;                                                               \
			mpz_init(tmp);                                                           \
			mpfr_get_z(tmp, as<VarBigFlt>(args[1])->getSrc(),                        \
				   mpfr_get_default_rounding_mode());                            \
			mpz_##name(res->get(), res->getSrc(), tmp);                              \
			mpz_clear(tmp);                                                          \
			return res;                                                              \
		}                                                                                \
		vm.fail(loc,                                                                     \
			"expected int or float argument for int " STRINGIFY(name) ", found: ",   \
			vm.getTypeName(args[1]));                                                \
		return nullptr;                                                                  \
	}

#define ARITHI_ASSN_FUNC(fn, name)                                                               \
	Var *bigIntAssn##fn(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,             \
			    const Map<String, AssnArgData> &assn_args)                           \
	{                                                                                        \
		if(args[1]->is<VarBigInt>()) {                                                   \
			mpz_##name(as<VarBigInt>(args[0])->get(),                                \
				   as<VarBigInt>(args[0])->getSrc(),                             \
				   as<VarBigInt>(args[1])->getSrc());                            \
			return args[0];                                                          \
		} else if(args[1]->is<VarBigFlt>()) {                                            \
			mpz_t tmp;                                                               \
			mpz_init(tmp);                                                           \
			mpfr_get_z(tmp, as<VarBigFlt>(args[1])->getSrc(),                        \
				   mpfr_get_default_rounding_mode());                            \
			mpz_##name(as<VarBigInt>(args[0])->get(),                                \
				   as<VarBigInt>(args[0])->getSrc(), tmp);                       \
			mpz_clear(tmp);                                                          \
			return args[0];                                                          \
		}                                                                                \
		vm.fail(                                                                         \
		loc, "expected int or float argument for int " STRINGIFY(name) "-assn, found: ", \
		vm.getTypeName(args[1]));                                                        \
		return nullptr;                                                                  \
	}

#define LOGICI_FUNC(fn, name, sym)                                                         \
	Var *bigInt##fn(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,           \
			const Map<String, AssnArgData> &assn_args)                         \
	{                                                                                  \
		if(args[1]->is<VarBigInt>()) {                                             \
			return mpz_cmp(as<VarBigInt>(args[0])->getSrc(),                   \
				       as<VarBigInt>(args[1])->getSrc()) sym 0             \
			       ? vm.getTrue()                                              \
			       : vm.getFalse();                                            \
		}                                                                          \
		vm.fail(loc, "expected int argument for int " STRINGIFY(name) ", found: ", \
			vm.getTypeName(args[1]));                                          \
		return nullptr;                                                            \
	}

ARITHI_FUNC(Add, add)
ARITHI_FUNC(Sub, sub)
ARITHI_FUNC(Mul, mul)
ARITHI_FUNC(Mod, mod)

ARITHI_ASSN_FUNC(Add, add)
ARITHI_ASSN_FUNC(Sub, sub)
ARITHI_ASSN_FUNC(Mul, mul)
ARITHI_ASSN_FUNC(Mod, mod)

LOGICI_FUNC(LT, lt, <)
LOGICI_FUNC(GT, gt, >)
LOGICI_FUNC(LE, le, <=)
LOGICI_FUNC(GE, ge, >=)

Var *bigIntEq(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
	      const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		return mpz_cmp(as<VarBigInt>(args[0])->getSrc(),
			       as<VarBigInt>(args[1])->getSrc()) == 0
		       ? vm.getTrue()
		       : vm.getFalse();
	}
	return vm.getFalse();
}

Var *bigIntNe(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
	      const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		return mpz_cmp(as<VarBigInt>(args[0])->getSrc(),
			       as<VarBigInt>(args[1])->getSrc()) != 0
		       ? vm.getTrue()
		       : vm.getFalse();
	}
	return vm.getTrue();
}

Var *bigIntDiv(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
	       const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		// rhs == 0
		if(mpz_get_ui(as<VarBigInt>(args[1])->getSrc()) == 0) {
			vm.fail(loc, "division by zero");
			return nullptr;
		}
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_div(res->get(), res->getSrc(), as<VarBigInt>(args[1])->getSrc());
		return res;
	} else if(args[1]->is<VarBigFlt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_t tmp;
		mpz_init(tmp);
		mpfr_get_z(tmp, as<VarBigFlt>(args[1])->getSrc(), mpfr_get_default_rounding_mode());
		// rhs == 0
		if(mpz_get_ui(tmp) == 0) {
			vm.fail(loc, "division by zero");
			mpz_clear(tmp);
			return nullptr;
		}
		mpz_div(res->get(), res->getSrc(), tmp);
		mpz_clear(tmp);
		return res;
	}
	vm.fail(loc, "expected int or float argument for int " STRINGIFY(name) ", found: ",
		vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntAssnDiv(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		   const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		// rhs == 0
		if(mpz_get_ui(as<VarBigInt>(args[1])->getSrc()) == 0) {
			vm.fail(loc, "division by zero");
			return nullptr;
		}
		mpz_div(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(),
			as<VarBigInt>(args[1])->getSrc());
		return args[0];
	} else if(args[1]->is<VarBigFlt>()) {
		mpz_t tmp;
		mpz_init(tmp);
		mpfr_get_z(tmp, as<VarBigFlt>(args[1])->getSrc(), mpfr_get_default_rounding_mode());
		// rhs == 0
		if(mpz_get_ui(tmp) == 0) {
			vm.fail(loc, "division by zero");
			mpz_clear(tmp);
			return nullptr;
		}
		mpz_div(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(), tmp);
		mpz_clear(tmp);
		return args[0];
	}
	vm.fail(loc, "expected int or float argument for int " STRINGIFY(name) "-assn, found: ",
		vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntBAnd(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_and(res->get(), as<VarBigInt>(args[0])->getSrc(),
			as<VarBigInt>(args[1])->getSrc());
		return res;
	}
	vm.fail(loc, "expected int argument for int bitwise and, found: ", vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntBOr(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
	       const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_ior(res->get(), as<VarBigInt>(args[0])->getSrc(),
			as<VarBigInt>(args[1])->getSrc());
		return res;
	}
	vm.fail(loc, "expected int argument for int bitwise or, found: ", vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntBXOr(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_xor(res->get(), as<VarBigInt>(args[0])->getSrc(),
			as<VarBigInt>(args[1])->getSrc());
		return res;
	}
	vm.fail(loc, "expected int argument for int bitwise xor, found: ", vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntBNot(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		const Map<String, AssnArgData> &assn_args)
{
	VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
	mpz_com(res->get(), as<VarBigInt>(args[0])->getSrc());
	return res;
}

Var *bigIntBAndAssn(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		    const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		mpz_and(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(),
			as<VarBigInt>(args[1])->getSrc());
		return args[0];
	}
	vm.fail(loc,
		"expected int argument for int "
		"bitwise and-assn, found: ",
		vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntBOrAssn(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		   const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		mpz_ior(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(),
			as<VarBigInt>(args[1])->getSrc());
		return args[0];
	}
	vm.fail(loc,
		"expected int argument for int bitwise or-assn, found: ", vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntBXOrAssn(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		    const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		mpz_xor(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(),
			as<VarBigInt>(args[1])->getSrc());
		return args[0];
	}
	vm.fail(loc, "expected int argument for int bitwise xor, found: ", vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntBNotAssn(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		    const Map<String, AssnArgData> &assn_args)
{
	mpz_com(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc());
	return args[0];
}

Var *bigIntLShift(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		  const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_mul_2exp(res->get(), as<VarBigInt>(args[0])->getSrc(),
			     mpz_get_si(as<VarBigInt>(args[1])->getSrc()));
		return res;
	} else if(args[1]->is<VarBigFlt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_t tmp;
		mpz_init(tmp);
		mpfr_get_z(tmp, as<VarBigFlt>(args[1])->getSrc(), mpfr_get_default_rounding_mode());
		mpz_mul_2exp(res->get(), as<VarBigInt>(args[0])->getSrc(), mpz_get_si(tmp));
		mpz_clear(tmp);
		return res;
	}
	vm.fail(loc,
		"expected bigint or float argument "
		"for bigint leftshift, found: ",
		vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntRShift(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		  const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_div_2exp(res->get(), as<VarBigInt>(args[0])->getSrc(),
			     mpz_get_si(as<VarBigInt>(args[1])->getSrc()));
		return res;
	} else if(args[1]->is<VarBigFlt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_t tmp;
		mpz_init(tmp);
		mpfr_get_z(tmp, as<VarBigFlt>(args[1])->getSrc(), mpfr_get_default_rounding_mode());
		mpz_div_2exp(res->get(), as<VarBigInt>(args[0])->getSrc(), mpz_get_si(tmp));
		mpz_clear(tmp);
		return res;
	}
	vm.fail(loc,
		"expected int or float argument "
		"for int rightshift, found: ",
		vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntLShiftAssn(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		      const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		mpz_mul_2exp(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(),
			     mpz_get_si(as<VarBigInt>(args[1])->getSrc()));
		return args[0];
	} else if(args[1]->is<VarBigFlt>()) {
		mpz_t tmp;
		mpz_init(tmp);
		mpfr_get_z(tmp, as<VarBigFlt>(args[1])->getSrc(), mpfr_get_default_rounding_mode());
		mpz_mul_2exp(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(),
			     mpz_get_si(tmp));
		mpz_clear(tmp);
		return args[0];
	}
	vm.fail(loc, "expected int or float argument for int leftshift-assign, found: ",
		vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntRShiftAssn(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		      const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		mpz_div_2exp(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(),
			     mpz_get_si(as<VarBigInt>(args[1])->getSrc()));
		return args[0];
	} else if(args[1]->is<VarBigFlt>()) {
		mpz_t tmp;
		mpz_init(tmp);
		mpfr_get_z(tmp, as<VarBigFlt>(args[1])->getSrc(), mpfr_get_default_rounding_mode());
		mpz_div_2exp(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(),
			     mpz_get_si(tmp));
		mpz_clear(tmp);
		return args[0];
	}
	vm.fail(loc, "expected int or float argument for int rightshift-assign, found: ",
		vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntPow(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
	       const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_pow_ui(res->get(), res->getSrc(), mpz_get_ui(as<VarBigInt>(args[1])->getSrc()));
		return res;
	} else if(args[1]->is<VarBigFlt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_t tmp;
		mpz_init(tmp);
		mpfr_get_z(tmp, as<VarBigFlt>(args[1])->getSrc(), mpfr_get_default_rounding_mode());
		mpz_pow_ui(res->get(), res->getSrc(), mpz_get_ui(tmp));
		mpz_clear(tmp);
		return res;
	}
	vm.fail(loc,
		"expected int or float argument for int power, found: ", vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntRoot(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		const Map<String, AssnArgData> &assn_args)
{
	if(args[1]->is<VarBigInt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_root(res->get(), res->getSrc(), mpz_get_ui(as<VarBigInt>(args[1])->getSrc()));
		return res;
	} else if(args[1]->is<VarBigFlt>()) {
		VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
		mpz_t tmp;
		mpz_init(tmp);
		mpfr_get_z(tmp, as<VarBigFlt>(args[1])->getSrc(), mpfr_get_default_rounding_mode());
		mpz_root(res->get(), res->getSrc(), mpz_get_ui(tmp));
		mpz_clear(tmp);
		return res;
	}
	vm.fail(loc,
		"expected int or float argument for int root, found: ", vm.getTypeName(args[1]));
	return nullptr;
}

Var *bigIntPreInc(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		  const Map<String, AssnArgData> &assn_args)
{
	mpz_add_ui(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(), 1);
	return args[0];
}

Var *bigIntPostInc(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		   const Map<String, AssnArgData> &assn_args)
{
	VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
	mpz_add_ui(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(), 1);
	return res;
}

Var *bigIntPreDec(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		  const Map<String, AssnArgData> &assn_args)
{
	mpz_sub_ui(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(), 1);
	return args[0];
}

Var *bigIntPostDec(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		   const Map<String, AssnArgData> &assn_args)
{
	VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
	mpz_sub_ui(as<VarBigInt>(args[0])->get(), as<VarBigInt>(args[0])->getSrc(), 1);
	return res;
}

Var *bigIntUSub(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		const Map<String, AssnArgData> &assn_args)
{
	VarBigInt *res = vm.makeVar<VarBigInt>(loc, as<VarBigInt>(args[0])->getSrc());
	mpz_neg(res->get(), res->getSrc());
	return res;
}

Var *bigIntPopCnt(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		  const Map<String, AssnArgData> &assn_args)
{
	return vm.makeVar<VarBigInt>(loc, mpz_popcount(as<VarBigInt>(args[0])->getSrc()));
}

Var *bigIntToInt(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		 const Map<String, AssnArgData> &assn_args)
{
	return vm.makeVar<VarBigInt>(loc, mpz_get_si(as<VarBigInt>(args[0])->get()));
}

Var *bigIntToStr(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		 const Map<String, AssnArgData> &assn_args)
{
	typedef void (*gmp_freefunc_t)(void *, size_t);

	char *_res  = mpz_get_str(NULL, 10, as<VarBigInt>(args[0])->getSrc());
	VarStr *res = vm.makeVar<VarStr>(loc, _res);

	gmp_freefunc_t freefunc;
	mp_get_memory_functions(NULL, NULL, &freefunc);
	freefunc(_res, strlen(_res) + 1);

	return res;
}

// Iterator

class VarBigIntIterator : public Var
{
	mpz_t begin, end, step, curr;
	bool started;
	bool reversed;

public:
	VarBigIntIterator(const ModuleLoc *loc);
	VarBigIntIterator(const ModuleLoc *loc, mpz_srcptr _begin, mpz_srcptr _end,
			  mpz_srcptr _step);
	~VarBigIntIterator();

	Var *copy(const ModuleLoc *loc);
	void set(Var *from);

	bool next(mpz_ptr val);

	inline void setReversed(mpz_srcptr step) { reversed = mpz_cmp_si(step, 0) < 0; }
	inline mpz_ptr getBegin() { return begin; }
	inline mpz_ptr getEnd() { return end; }
	inline mpz_ptr getStep() { return step; }
	inline mpz_ptr getCurr() { return curr; }
};

VarBigIntIterator::VarBigIntIterator(const ModuleLoc *loc)
	: Var(loc, typeID<VarBigIntIterator>(), false, false), started(false), reversed(false)
{
	mpz_init(begin);
	mpz_init(end);
	mpz_init(step);
	mpz_init(curr);
}
VarBigIntIterator::VarBigIntIterator(const ModuleLoc *loc, mpz_srcptr _begin, mpz_srcptr _end,
				     mpz_srcptr _step)
	: Var(loc, typeID<VarBigIntIterator>(), false, false), started(false),
	  reversed(mpz_cmp_si(_step, 0) < 0)
{
	mpz_init_set(begin, _begin);
	mpz_init_set(end, _end);
	mpz_init_set(step, _step);
	mpz_init_set(curr, _begin);
}
VarBigIntIterator::~VarBigIntIterator() { mpz_clears(begin, end, step, curr, NULL); }

Var *VarBigIntIterator::copy(const ModuleLoc *loc)
{
	return new VarBigIntIterator(loc, begin, end, step);
}
void VarBigIntIterator::set(Var *from)
{
	VarBigIntIterator *f = as<VarBigIntIterator>(from);
	mpz_set(begin, f->begin);
	mpz_set(end, f->end);
	mpz_set(step, f->step);
	mpz_set(curr, f->curr);
	started	 = f->started;
	reversed = f->reversed;
}

bool VarBigIntIterator::next(mpz_ptr val)
{
	if(reversed) {
		if(mpz_cmp(curr, end) <= 0) return false;
	} else {
		if(mpz_cmp(curr, end) >= 0) return false;
	}
	if(!started) {
		mpz_init(val);
		mpz_set(val, curr);
		started = true;
		return true;
	}
	mpz_t tmp;
	mpz_init(tmp);
	mpz_add(tmp, curr, step);
	if(reversed) {
		if(mpz_cmp(tmp, end) <= 0) {
			mpz_clear(tmp);
			return false;
		}
	} else {
		if(mpz_cmp(tmp, end) >= 0) {
			mpz_clear(tmp);
			return false;
		}
	}
	mpz_set(curr, tmp);
	mpz_init(val);
	mpz_set(val, curr);
	mpz_clear(tmp);
	return true;
}

Var *bigIntRange(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		 const Map<String, AssnArgData> &assn_args)
{
	Var *lhs_base  = args[1];
	Var *rhs_base  = args.size() > 2 ? args[2] : nullptr;
	Var *step_base = args.size() > 3 ? args[3] : nullptr;

	if(!lhs_base->is<VarBigInt>()) {
		vm.fail(lhs_base->getLoc(),
			"expected argument 1 to be of type int, found: ", vm.getTypeName(lhs_base));
		return nullptr;
	}
	if(rhs_base && !rhs_base->is<VarBigInt>()) {
		vm.fail(rhs_base->getLoc(),
			"expected argument 2 to be of type int, found: ", vm.getTypeName(rhs_base));
		return nullptr;
	}
	if(step_base && !step_base->is<VarBigInt>()) {
		vm.fail(step_base->getLoc(), "expected argument 3 to be of type int, found: ",
			vm.getTypeName(step_base));
		return nullptr;
	}

	mpz_t begin, end, step;
	mpz_inits(begin, end, step, NULL);
	if(args.size() > 2) mpz_set(begin, as<VarBigInt>(lhs_base)->getSrc());
	else mpz_set_si(begin, 0);
	if(rhs_base) mpz_set(end, as<VarBigInt>(rhs_base)->getSrc());
	else mpz_set(end, as<VarBigInt>(lhs_base)->getSrc());
	if(step_base) mpz_set(step, as<VarBigInt>(step_base)->getSrc());
	else mpz_set_si(step, 1);
	VarBigIntIterator *res = vm.makeVar<VarBigIntIterator>(loc, begin, end, step);
	mpz_clears(begin, end, step, NULL);
	return res;
}

Var *getBigIntIteratorNext(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
			   const Map<String, AssnArgData> &assn_args)
{
	VarBigIntIterator *it = as<VarBigIntIterator>(args[0]);
	mpz_t _res;
	if(!it->next(_res)) {
		return vm.getNil();
	}
	VarBigInt *res = vm.makeVar<VarBigInt>(loc, _res);
	mpz_clear(_res);
	res->setLoadAsRef();
	return res;
}

// RNG

Var *rngSeedInt(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
		const Map<String, AssnArgData> &assn_args)
{
	if(!args[1]->is<VarBigInt>()) {
		vm.fail(loc,
			"expected seed value to be a big int, found: ", vm.getTypeName(args[1]));
		return nullptr;
	}
	gmp_randseed(rngstate, as<VarBigInt>(args[1])->get());
	return vm.getNil();
}

// [0, to)
Var *rngGetInt(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
	       const Map<String, AssnArgData> &assn_args)
{
	if(!args[1]->is<VarBigInt>()) {
		vm.fail(loc,
			"expected upper bound to be an big int, found: ", vm.getTypeName(args[1]));
		return nullptr;
	}
	VarBigInt *res = vm.makeVar<VarBigInt>(loc, 0);
	mpz_urandomm(res->get(), rngstate, as<VarBigInt>(args[1])->get());
	return res;
}

INIT_MODULE(BigInt)
{
	VarModule *mod = vm.getCurrModule();

	mod->addNativeFn("newIntNative", bigIntNewNative, 1);

	// register the BigInt type (registerType)
	vm.registerType<VarBigInt>(loc, "BigInt");

	vm.addNativeTypeFn<VarBigInt>(loc, "+", bigIntAdd, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "-", bigIntSub, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "*", bigIntMul, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "/", bigIntDiv, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "%", bigIntMod, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "<<", bigIntLShift, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, ">>", bigIntRShift, 1);

	vm.addNativeTypeFn<VarBigInt>(loc, "+=", bigIntAssnAdd, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "-=", bigIntAssnSub, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "*=", bigIntAssnMul, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "/=", bigIntAssnDiv, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "%=", bigIntAssnMod, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "<<=", bigIntLShiftAssn, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, ">>=", bigIntRShiftAssn, 1);

	vm.addNativeTypeFn<VarBigInt>(loc, "**", bigIntPow, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "//", bigIntRoot, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "++x", bigIntPreInc, 0);
	vm.addNativeTypeFn<VarBigInt>(loc, "x++", bigIntPostInc, 0);
	vm.addNativeTypeFn<VarBigInt>(loc, "--x", bigIntPreDec, 0);
	vm.addNativeTypeFn<VarBigInt>(loc, "x--", bigIntPostDec, 0);

	vm.addNativeTypeFn<VarBigInt>(loc, "u-", bigIntUSub, 0);

	vm.addNativeTypeFn<VarBigInt>(loc, "<", bigIntLT, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, ">", bigIntGT, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "<=", bigIntLE, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, ">=", bigIntGE, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "==", bigIntEq, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "!=", bigIntNe, 1);

	vm.addNativeTypeFn<VarBigInt>(loc, "&", bigIntBAnd, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "|", bigIntBOr, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "^", bigIntBXOr, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "~", bigIntBNot, 0);

	vm.addNativeTypeFn<VarBigInt>(loc, "&=", bigIntBAndAssn, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "|=", bigIntBOrAssn, 1);
	vm.addNativeTypeFn<VarBigInt>(loc, "^=", bigIntBXOrAssn, 1);

	vm.addNativeTypeFn<VarBigInt>(loc, "popcnt", bigIntPopCnt, 0);

	vm.addNativeTypeFn<VarBigInt>(loc, "int", bigIntToInt, 0);
	vm.addNativeTypeFn<VarBigInt>(loc, "str", bigIntToStr, 0);

	// Iterator

	mod->addNativeFn("irange", bigIntRange, 1, true);
	// generate the type ID for int iterator (registerType)
	vm.registerType<VarBigIntIterator>(loc, "BigIntIterator");
	vm.addNativeTypeFn<VarBigIntIterator>(loc, "next", getBigIntIteratorNext, 0);

	// RNG

	mod->addNativeFn("getRandomIntNative", rngGetInt, 1);

	return true;
}
