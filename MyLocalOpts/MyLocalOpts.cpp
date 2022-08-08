#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/ADT/APInt.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using namespace std;
using namespace llvm;

namespace {

class MyLocalOpts : public ModulePass {
std::map<std::string, int> hashTable;
std::map<std::string, int> VNTable;
std::map<std::string, llvm::Instruction * > VITable;
std::map<int,vector<std::string> > UVNTable;
vector<Instruction *> dc;      
vector<Instruction *> vdci;      
vector<Instruction *> vdi;      
vector<Instruction *> vi;	
vector<Value * > vec1;
vector<Value *> vec2;

int VN = 1;
  // strength reduction 
  bool reduceStrength(BasicBlock &BB) {
      for (BasicBlock::iterator BBI = BB.begin(); BBI != BB.end();BBI++) {
      // get instruction out of iterator
      Instruction &inst(*BBI);
      // if binary operator
      if (BinaryOperator *binOp = dyn_cast<BinaryOperator>(&inst)) {
	// get both operands
        Value *x(binOp->getOperand(0));
        Value *y(binOp->getOperand(1));
	// optimize if integer type
        if (IntegerType *intType = dyn_cast<IntegerType>(BBI->getType())) {
          ConstantInt *val;

          // optimize based on operator
          if(binOp->getOpcode()==Instruction::Mul) {
	     // if x is a constant
            if (ConstantInt::classof(x)) {
              // convert x into value of constant int
              val = dyn_cast<ConstantInt>(x);
		// if value of x is in power of 2 
              if (val->getValue().isPowerOf2()) {
		//find log2 of value of x                 
		int a = val->getValue().APInt::logBase2();
		// get a new constant intwith value of log2 of x 
		Value * v = ConstantInt::get(val->getContext(), APInt(intType->getBitWidth(), a));
		// create new shift left instruction 		
		Instruction *newInst = BinaryOperator::CreateShl(y, v);
		// replace mul by shl		
		errs()<<"replaced : "<< *BBI <<" with ";
                ReplaceInstWithInst(BB.getInstList(), BBI, newInst);
              	errs()<<*BBI<<" | Strength Reduction mul to shl.\n";
                }
            }
            // if y is a constant
            else if (ConstantInt::classof(y)) {
             // convert y into value of constant int
              val = dyn_cast<ConstantInt>(y);
              // if value of y is in power of 2 
              if (val->getValue().isPowerOf2()) {
		//find log2 of value of y                 
		int a = val->getValue().APInt::logBase2();
		// get a new constant intwith value of log2 of y 
		Value * v = ConstantInt::get(val->getContext(), APInt(intType->getBitWidth(), a));
		// create new shift left instruction 		
		Instruction *newInst = BinaryOperator::CreateShl(x, v);
		// replace mul by shl		
		errs()<<"replaced : "<< *BBI <<" with ";
                ReplaceInstWithInst(BB.getInstList(), BBI, newInst);
              	errs()<<*BBI<<" | Strength Reduction mul to shl.\n";
                }
            }

  }       // optimize based on operator
          if(binOp->getOpcode()==Instruction::SDiv) {
	     // if x is a constant
            if (ConstantInt::classof(x)) {
              // convert x into value of constant int
              val = dyn_cast<ConstantInt>(x);
		// if value of x is in power of 2 
              if (val->getValue().isPowerOf2()) {
		//find log2 of value of x                 
		int a = val->getValue().APInt::logBase2();
		// get a new constant intwith value of log2 of x 
		Value * v = ConstantInt::get(val->getContext(), APInt(intType->getBitWidth(), a));
		// create new shift right instruction 		
		Instruction *newInst = BinaryOperator::CreateLShr(y, v);
		// replace Div by shr	
		errs()<<"replaced : "<< *BBI <<" with ";
                ReplaceInstWithInst(BB.getInstList(), BBI, newInst);
              	errs()<<*BBI<<" | Strength Reduction div to shr.\n";
                }
            }
            // if y is a constant
            else if (ConstantInt::classof(y)) {
             // convert y into value of constant int
              val = dyn_cast<ConstantInt>(y);
              // if value of y is in power of 2 
              if (val->getValue().isPowerOf2()) {
		//find log2 of value of y                 
		
		int a = val->getValue().APInt::logBase2();
		// get a new constant intwith value of log2 of y 
		Value * v = ConstantInt::get(val->getContext(), APInt(intType->getBitWidth(), a));
		// create new shift right instruction 		
		Instruction *newInst = BinaryOperator::CreateLShr(x, v);
		// replace div by shr	
		errs()<<"replaced : "<< *BBI <<" with ";
                ReplaceInstWithInst(BB.getInstList(), BBI, newInst);
              	errs()<<*BBI<<" | Strength Reduction div to shr.\n";
                }
            }

  }
	}
	}
	}
    return true;

	}

BasicBlock & constantPropagation(BasicBlock &BB) {
	for (BasicBlock::iterator BBI = BB.begin(); BBI != BB.end();BBI++) {
      // get instruction out of iterator
      Instruction * inst = dyn_cast<Instruction>(BBI);
	if (inst->getOpcode() != Instruction::Store && inst->getOpcode() != Instruction::Ret){
	inst->setName("t");
	}
	}
      for (BasicBlock::iterator BBI = BB.begin(); BBI != BB.end();BBI++) {
      // get instruction out of iterator
      Instruction * inst = dyn_cast<Instruction>(BBI);
	if (inst->getOpcode() == Instruction::Store){
	  Value *v(inst->getOperand(0));
	  Value * v1(inst->getOperand(1));
	  ConstantInt *val;
	  if (ConstantInt::classof(v)) {
	  val = dyn_cast<ConstantInt>(v);
	  std::vector<Value *>::iterator it;
	  it = std::find(vec1.begin(),vec1.end(),v1);
	  if ( it != vec1.end()){
	    int i = it - vec1.begin();
	    vec2[i] = val;
	    vdci.push_back(vi[i]);
	    vi[i] = inst;
	  }
	  else{
	  vi.push_back(inst);
	  vec1.push_back(inst->getOperand(1));
	  vec2.push_back(val);	  
	  }
	  }
	}
	else if (inst->getOpcode() == Instruction::Mul){
	  Value * v(inst);
	  Value *v1(inst->getOperand(0));
	  Value * v2(inst->getOperand(1));
	  ConstantInt *val1 ,*val2 ;
	  if (ConstantInt::classof(v1) && ConstantInt::classof(v2)) {
	  val1 = dyn_cast<ConstantInt>(v1);
	  val2 = dyn_cast<ConstantInt>(v2);
	  int  val3 = val1->getSExtValue() * (val2->getSExtValue());
	  Value * val = ConstantInt::getSigned(inst->getType(), val3);
	  inst->replaceAllUsesWith(val);
	  errs()<<"replaced : "<< *inst <<" with "<<*val<<" | Constant Propagation .\n";
          dc.push_back(inst);
	  std::vector<Value *>::iterator it;
	  it = std::find(vec1.begin(),vec1.end(),v);
	  if ( it != vec1.end()){
	    int i = it - vec1.begin();
	    vec2[i] = val;
	    
	  }
	  else{
	  vec1.push_back(v);
	  vec2.push_back(val);	  
	  }
	  }
	}
	 else if (inst->getOpcode() == Instruction::Load){
		Value *ld(inst->getOperand(0));
		std::vector<Value *>::iterator it;
		it = std::find(vec1.begin(),vec1.end(),ld);
		if ( it != vec1.end()){
		inst->replaceAllUsesWith(vec2[it - vec1.begin()]);
		errs()<<"replaced : "<< *inst <<" with "<< *vec2[it - vec1.begin()]<<" | Constant Propagation .\n";
                dc.push_back(inst);
		}
	}
	
      }
    return BB;
  }

int deadcodeElimination(BasicBlock &BB){
int dcount=0;
//dc.clear();

errs()<< "\nDead Code Elimination :\n";
    for (BasicBlock::iterator BBI = BB.begin(); BBI != BB.end();BBI++) {
      // get instruction out of iterator
	Instruction * inst = dyn_cast<Instruction>(BBI);
	//dc = vdi;
	
	for ( Instruction * dcI : vdi){
	if (std::find(dc.begin(),dc.end(),dcI) == dc.end()){
	dc.push_back(dcI);
	dcount++;}}	
	if (inst->use_empty() && inst->getOpcode() != Instruction::Store && inst->getOpcode() != Instruction::Ret)
	{if (std::find(dc.begin(),dc.end(),inst) == dc.end()){
	dc.push_back(inst);
	dcount++;}}
	 else if (inst->getOpcode() == Instruction::Store){
		if (std::find(vdci.begin(),vdci.end(),inst) != vdci.end()){
		//dc.push_back(inst);
		dcount++;
		}
	}
	}
	errs()<<"start delete\n";
int n = dc.size();
for (int i = 0; i<n ;i++){Instruction *v = dc[i];
	errs()<<*v<<"\n";

	v->eraseFromParent();

	}
	errs()<<"end delete\n";
dc.clear();
return dcount;
}
std:: string getVName(Value * v){
	std::string s = "";
        if (v->hasName()){
            s = v->getName();
        }
        else {
            if (ConstantInt::classof(v)){
                ConstantInt * cv = dyn_cast<ConstantInt>(v);
                s = cv->getValue().toString(10,1);
            }
            else{
                //errs() <<*v<< "neither has name nor it is constant\n";
            }
        }
	return s;
    }
bool insertInVTable(Instruction * instI){
if (instI->getOpcode() == Instruction :: Load){
Value * ld(instI->getOperand(0));
auto i = VNTable.find(ld->getName());
if ( i != VNTable.end()){

VITable.insert(std::pair<std::string ,llvm::Instruction * >(instI->getName(),instI));
VNTable.insert(std::pair<std::string,int>(instI->getName(),i->second));
UVNTable[i->second].push_back(instI->getName());
if(UVNTable[i->second].size()>1){
string st_1 = UVNTable[i->second][0];
auto i1 = VITable.find(st_1);
if ( i1 != VITable.end()){

Instruction * instR = (VITable[st_1]);
Value * valR(instR);
if(instI->getType() == valR->getType()){
instI->replaceAllUsesWith(valR);
vdi.push_back(instI);
errs()<<"replaced:" <<*instI<<" with "<<*valR<<" | Copy Propagation .\n";}
}}
}
}

else if (instI->getOpcode() == Instruction::Store){
Value * st1(instI->getOperand(0));
Value * st2(instI->getOperand(1));
if(st2->hasName()){
auto i = VNTable.find(st2->getName());
if ( i != VNTable.end()){
	if( st1->hasName()){
	auto i1 = VNTable.find(st1->getName());
	if ( i1 != VNTable.end()){  
	   if(VNTable[st2->getName()] != VNTable[st1->getName()]){
		VNTable[st2->getName()] = VNTable[st1->getName()];}
	   else {vdi.push_back(instI);}}}
	else if(ConstantInt :: classof(st1)){VNTable[st2->getName()] = VN++;}

}

}}
else {
VNTable.insert(std::pair<std::string ,int>(instI->getName(),VN++));
VITable.insert(std::pair<std::string ,llvm::Instruction * >(instI->getName(),instI));
vector<std::string> vst ;
vst.push_back(instI->getName());
if (instI->getOpcode() != Instruction::Alloca )UVNTable.insert(std::pair<int,vector<std::string> >((--VN)++,vst));
}

return 0;
}

bool deleteInVNTable(std::string s1){

return 0;}

bool insertInHashTable(std::string s1,std::string s2,std::string s3,llvm::Instruction * instI){
auto fs1 = UVNTable.begin();
auto fs2 = UVNTable.begin();
string op1,op2;
vector<std::string> vst;
auto i = VNTable.find(s1);
if (i != VNTable.end()){
fs1 = UVNTable.find(i->second);
vst = fs1->second;
op1 = vst[0];
}else op1 = s1;

i = VNTable.find(s2);
if (i != VNTable.end()){
fs2 = UVNTable.find(i->second);
vst = fs2->second;
op2 = vst[0];//fs2->second;
}else op2 = s2;

string st = op1+s3+op2;

auto it = hashTable.find(st);
if (it != hashTable.end()){
string st_1 = UVNTable[it->second][0];
auto i1 = VITable.find(st_1);
if ( i1 != VITable.end()){

Instruction * instR = (VITable[st_1]);
Value * valR(instR);
if(instI->getType() == valR->getType()){
instI->replaceAllUsesWith(valR);
vdi.push_back(instI);
errs()<<"replaced:" <<*instI<<" with "<<*valR<<" |  Local Common Expression Elimination .\n";}
}
}
else{
	if (s3 == "+" || s3 == "*"){
	string st1 = op2+s3+op1;
	auto it1 = hashTable.find(st1);
	if(it1 != hashTable.end()){
string st_1 = UVNTable[it1->second][0];
auto i1 = VITable.find(st_1);
if ( i1 != VITable.end()){
Instruction * instR = (VITable[st_1]);
Value * valR(instR);
if(instI->getType() == valR->getType()){
instI->replaceAllUsesWith(valR);
vdi.push_back(instI);
errs()<<"replaced:" <<*instI<<" with "<<*valR<<" | Local Common Expression Elimination .\n";}
}

	}
	else {hashTable.insert(std::pair<std::string,int>(st,(--VN)++));
	}
	}
	else {hashTable.insert(std::pair<std::string,int>(st,(--VN)++));}
}
return 0;

}


bool localValueNumberCreator(BasicBlock &BB){
for (BasicBlock::iterator BBI = BB.begin(); BBI != BB.end();BBI++) {
      // get instruction out of iterator
      Instruction * inst = dyn_cast<Instruction>(BBI);
	if (inst->getOpcode() != Instruction::Store && inst->getOpcode() != Instruction::Ret){
	inst->setName("name");
	}
	}
for (BasicBlock::iterator BBI = BB.begin(); BBI != BB.end();BBI++) {
      // get instruction out of iterator
      Instruction * inst = dyn_cast<Instruction>(BBI);
      Value * val(inst);
	if (val->hasName()){
	insertInVTable(inst);
	}
	if (inst->getOpcode() == Instruction::Store){
	Value *op0(inst->getOperand(0));	
	Value *op1(inst->getOperand(1));
	string s1 = getVName(op0);	
	string s2 = getVName(op1);	
	insertInVTable(inst);
	}
	std::string st = "";
	if (inst->getOpcode() == Instruction::Add){
	Value *op0(inst->getOperand(0));	
	Value *op1(inst->getOperand(1));
	string s1 = op0->getName();	
	string s2 = op1->getName();	
	st = s1 + "+" + s2;	
	insertInHashTable(s1,s2,"+",inst);
	}	
	else if (inst->getOpcode() == Instruction::Mul){
	Value *op0(inst->getOperand(0));	
	Value *op1(inst->getOperand(1));
	string s1 = getVName(op0);	
	string s2 = getVName(op1);	
	st = s1 + "*" + s2;	
	insertInHashTable(s1,s2,"*",inst);
	
	}	
	else if (inst->getOpcode() == Instruction::Sub){
	Value *op0(inst->getOperand(0));	
	Value *op1(inst->getOperand(1));
	string s1 = getVName(op0);	
	string s2 = getVName(op1);	
	st = s1 + "-" + s2;	
	insertInHashTable(s1,s2,"-",inst);
	}	
	else if (inst->getOpcode() == Instruction::SDiv){
	Value *op0(inst->getOperand(0));	
	Value *op1(inst->getOperand(1));
	string s1 = getVName(op0);	
	string s2 = getVName(op1);	
	st = s1 + "/" + s2;	
	insertInHashTable(s1,s2,"/",inst);
	}
        
	
}/*
errs()<<"HashTable :\n";
for ( std::pair<std::string,int> entry : hashTable){
errs()<< entry.first <<"\t|\t'"<<entry.second<<"'\n";
}
errs()<<"ValTable :\n";
for ( std::pair<std::string,int> entry : VNTable){
errs()<< entry.first <<"\t|\t'"<<entry.second<<"'\n";
}
errs()<<"ValTable :\n";
for ( std::pair<std::string,llvm::Instruction * > entry : VITable){
errs()<< entry.first <<"\t|\t"<<*(entry.second)<<"\t|\t"<<*(entry.second)->getType()<<"\n";
}
errs()<<"UValTable :\n";
for ( std::pair<int,vector<std::string> >entry : UVNTable){
errs()<< entry.first <<"\t|\t'";
vector <std::string> vst = entry.second;
for( std::string s:vst){errs()<<s<<";";}
errs()<<"'\n";
}*/

return 0;
}
bool AllOpts(BasicBlock &BB){
	localValueNumberCreator(BB);
	constantPropagation(BB);
        deadcodeElimination(BB);
        reduceStrength(BB);
	
return 0;
}

public:
  static char ID;
  MyLocalOpts() : ModulePass(ID) {}
  ~MyLocalOpts() {}

  virtual bool runOnModule(Module &M) {
    errs() << "\nMODULE: " << M.getName() << "\n\n";
    // for every function in module
    for (Module::iterator MI = M.begin(); MI != M.end(); ++MI) {
      Function &F(*MI);
      errs() << "\nFUNCTION: " << F.getName() << "\n\n";
      // for every basic block in function
      for (Function::iterator FI = F.begin(); FI != F.end(); ++FI) {
        BasicBlock &BB(*FI);
        errs() << "\nBASIC BLOCK: " << BB.getName() << "\n\n";
        // print original code
        errs() << "\nORIGINAL CODE:\n\n";
        //BB.dump();
        errs() << "\nOPTIMIZATIONS:\n\n";
	AllOpts(BB);
	errs() << "\nFINAL CODE:\n\n";
        BB.dump();
      }
    }

    return true;
  }
};

char MyLocalOpts::ID = 0;
static RegisterPass<MyLocalOpts> X("MyLocalOpts", "My Local Optimizations");
} // namespace
