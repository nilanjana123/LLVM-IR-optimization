## LLVM Programming Assignment 

### Instruction 

  1. copy `MyLocalOpts` folder and paste in `llvm source directory ->./lib/Transforms`. 

  2. Edit `CMakeLists.txt` in `Transforms` folder. 
  3. Open terminal and change directory to llvm build directory. (in my case it is `cd llvm-6.0`).
  4. Run `make` command. 
  5. Run `./bin/opt -load ./lib/LLVMMyLocalOpts.so -MyLocalOpts ~/Desktop/TestCases/test1_IR.ll -o ~/Desktop/TestCases/test1_optimized_IR.bc
` command to generate optimized IR from unoptimized IR in `.bc` format. Here I placed my `TestCases` folder in `Desktop`.
  6. `llvm-dis test1_optimized_IR.bc -o test1_optimized_IR.ll` to convert `.bc` to `.ll` to make it human readable .
  7. You may run `./bin/opt -load ./lib/LLVMMyLocalOpts.so -MyLocalOpts ~/Desktop/TestCases/test1_IR.ll -o ~/Desktop/TestCases/test1_optimized_IR.ll
` command to generate optimized IR from unoptimized IR in `.ll` format.
  8. To generate `.ll` file from `.cpp` or .c file , use `clang -S -emit-llvm test1.cpp -o test1_IR.ll` .
  
#### LLVM Version used : 6.0.0
#### Clang Version used : 6.0.0
