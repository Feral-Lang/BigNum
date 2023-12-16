#include "Common.hpp"

#include "BigIntType.hpp"

gmp_randstate_t rngstate;

Var *rngSeed(Interpreter &vm, const ModuleLoc *loc, Span<Var *> args,
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

INIT_MODULE(Common)
{
	gmp_randinit_default(rngstate);
	VarModule *mod = vm.getCurrModule();
	mod->addNativeFn("seed", rngSeed, 1);
	return true;
}

DEINIT_MODULE(Common) { gmp_randclear(rngstate); }
