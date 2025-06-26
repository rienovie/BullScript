" vim syntax file for basm
" BullASM using the Bull compiler
"
" Not complete but is better than nothing
"
" Created by Rienovie

if exists("b:current_syntax")
  finish
endif

syn keyword Repeat create define fn
syn keyword StorageClass multi const 8b 16b 32b 64b inline
syn keyword Conditional condition contents
syn keyword Function jump increment add subtract decrement add move push pop
syn keyword Label label

syn region Comment start=";" keepend end="$"

let b:current_syntax = "basm"
