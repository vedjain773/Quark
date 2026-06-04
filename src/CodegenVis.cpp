#include "CodegenVis.hpp"
#include "Expression.hpp"
#include <iostream>

void CodegenVis::initModule(std::string fileName) {
  Context = std::make_unique<llvm::LLVMContext>();
  Module = std::make_unique<llvm::Module>(fileName, *Context);
  Builder = std::make_unique<llvm::IRBuilder<>>(*Context);
}

llvm::Value *CodegenVis::LogErrorV(std::string errMsg) {
  std::cout << errMsg << "\n";
  return nullptr;
}

llvm::Type *CodegenVis::tkToType(TypeKind typek) {
  switch (typek.tk) {
  case TypeKindE::INT: {
    return llvm::Type::getInt32Ty(*Context);
  } break;

  case TypeKindE::CHAR: {
    return llvm::Type::getInt8Ty(*Context);
  } break;

  case TypeKindE::VOID: {
    return llvm::Type::getVoidTy(*Context);
  } break;

  case TypeKindE::POINTER: {
    return llvm::PointerType::get(*Context, 0);
  } break;
  }

  return nullptr;
}

llvm::AllocaInst *CodegenVis::CreateEntryBlockAlloca(llvm::Function *function,
                                                     std::string varname,
                                                     TypeKind tk) {
  llvm::StringRef VarName(varname);
  llvm::IRBuilder<> TmpB(&function->getEntryBlock(),
                         function->getEntryBlock().begin());
  return TmpB.CreateAlloca(tkToType(tk), nullptr, VarName);
}

llvm::Value *CodegenVis::handlePointerArithmetic(llvm::Value *left,
                                                 llvm::Value *right,
                                                 TypeKindE tkE, Operators Op) {
  llvm::IRBuilder<> *Bldr = (Builder).get();

  if (Op == Operators::MINUS) {
    right = Bldr->CreateNeg(right);
  }

  return Bldr->CreateGEP(tkToType(getTypeStruct(tkE)), left, right, "gep");
}

llvm::Value *CodegenVis::handleBinOp(llvm::Value *left, llvm::Value *right,
                                     Operators Op, TypeKind infType) {
  llvm::IRBuilder<> *Bldr = (Builder).get();

  switch (Op) {
  case Operators::PLUS: {
    return Bldr->CreateAdd(left, right, "add", false, true);
  } break;

  case Operators::MINUS: {
    return Bldr->CreateSub(left, right, "sub", false, true);
  } break;

  case Operators::MULT: {
    return Bldr->CreateMul(left, right, "mul", false, true);
  } break;

  case Operators::DIVIDE: {
    return Bldr->CreateSDiv(left, right, "sdiv", false);
  } break;

  case Operators::MODULUS: {
    return Bldr->CreateSRem(left, right, "srem");
  } break;

  case Operators::GREATER: {
    llvm::Value *gt = Bldr->CreateICmpSGT(left, right, "compSGT");
    return Bldr->CreateZExt(gt, tkToType(infType), "ext");
  } break;

  case Operators::GREATER_EQUALS: {
    llvm::Value *ge = Bldr->CreateICmpSGE(left, right, "compSGE");
    return Bldr->CreateZExt(ge, tkToType(infType), "ext");
  } break;

  case Operators::LESS: {
    llvm::Value *lt = Bldr->CreateICmpSLT(left, right, "compSLT");
    return Bldr->CreateZExt(lt, tkToType(infType), "ext");
  } break;

  case Operators::LESS_EQUALS: {
    llvm::Value *le = Bldr->CreateICmpSLE(left, right, "compSLE");
    return Bldr->CreateZExt(le, tkToType(infType), "ext");
  } break;

  case Operators::EQUALS: {
    llvm::Value *ee = Bldr->CreateICmpEQ(left, right, "compEE");
    return Bldr->CreateZExt(ee, tkToType(infType), "ext");
  } break;

  case Operators::NOT_EQUALS: {
    llvm::Value *ne = Bldr->CreateICmpNE(left, right, "compNE");
    return Bldr->CreateZExt(ne, tkToType(infType), "ext");
  } break;

  case Operators::AND: {
    llvm::Value *booland = Bldr->CreateAnd(left, right, "and");
    return Bldr->CreateZExt(booland, tkToType(infType), "ext");
  } break;

  case Operators::OR: {
    llvm::Value *boolor = Bldr->CreateOr(left, right, "or");
    return Bldr->CreateZExt(boolor, tkToType(infType), "ext");
  } break;

  default:
    return left;
  }
}

void CodegenVis::pushScope() { scopes.emplace_back(); }

void CodegenVis::popScope() {
  assert(!scopes.empty() && "Popping empty scope!");
  scopes.pop_back();
}

void CodegenVis::insertName(std::string name, llvm::AllocaInst *alloca) {
  scopes[scopes.size() - 1].insert({name, alloca});
}

llvm::AllocaInst *CodegenVis::lookup(std::string name) {
  for (int i = scopes.size() - 1; i >= 0; i--) {
    if (scopes[i].count(name)) {
      return scopes[i][name];
    }
  }

  return nullptr;
}

void CodegenVis::emitObj(std::string Filename) {
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();

  std::string TargetTriple = llvm::sys::getDefaultTargetTriple();
  Module->setTargetTriple(TargetTriple);

  std::string Error;
  auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

  if (!Target) {
    llvm::errs() << Error;
    return;
  }

  std::string CPU = "generic";
  std::string Features = "";

  llvm::TargetOptions opt{};

  auto RM = std::optional<llvm::Reloc::Model>(llvm::Reloc::PIC_);

  auto TheTargetMachine =
      Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

  Module->setDataLayout(TheTargetMachine->createDataLayout());

  std::error_code EC;
  llvm::raw_fd_ostream dest(Filename, EC, llvm::sys::fs::OF_None);

  if (EC) {
    llvm::errs() << "Could not open file: " << EC.message();
    return;
  }

  llvm::legacy::PassManager pass;

  auto FileType = llvm::CodeGenFileType::ObjectFile;

  if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    llvm::errs() << "TheTargetMachine can't emit a file of this type";
    return;
  }

  pass.run(*Module);
  dest.flush();
}
