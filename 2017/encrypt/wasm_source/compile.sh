clang -emit-llvm --target=wasm32 -Oz main.c -c -o tmp.bc
llc -march=wasm32 -asm-verbose=false  tmp.bc -o tmp.s
rm ./tmp.bc
../binaryen/bin/s2wasm ./tmp.s > tmp.wast
../wabt/bin/wast2wasm ./tmp.wast -o tmp.wasm
