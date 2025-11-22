" Vim syntax file
" Language: BASM
" ChatGPT Generated
" Latest Revision: 2025-11-16
if exists("b:current_syntax")
  finish
endif

" -------------------------------------------------------
" Comments
" -------------------------------------------------------
syn match basmComment ";.*$"

" -------------------------------------------------------
" Strings (double-quoted with escapes)
" -------------------------------------------------------
syn region basmString start=/"/ end=/"/ contains=basmEscape
syn match  basmEscape /\\[abfnrtv"\\]/

" -------------------------------------------------------
" Numbers
" -------------------------------------------------------
syn match basmNumber /\<\d\+\>/

" -------------------------------------------------------
" Types and modifiers
" -------------------------------------------------------
syn keyword basmType 8b 16b 32b 64b const output inline required

" -------------------------------------------------------
" Special prefixes
" -------------------------------------------------------
syn match basmSyscall /\$[A-Za-z_][A-Za-z0-9_]*/
syn match basmConst   /#[A-Za-z_][A-Za-z0-9_]*/

" -------------------------------------------------------
" Registers
" -------------------------------------------------------
syn match   basmRegister /\<reg[0-9]\{1,2}\>/
syn keyword basmRegister output

" -------------------------------------------------------
" Major defining keywords (separate highlight)
" -------------------------------------------------------
syn keyword basmDefKeyword create define

" -------------------------------------------------------
" Instruction keywords
" -------------------------------------------------------
syn keyword basmKeyword
      \ return call label jump jumpFalse evaluate
      \ move increment subtract multi while if entry

" -------------------------------------------------------
" Inputs after <- operator
" Highlight from just after <- until : or {
" -------------------------------------------------------
syn region basmInput start=/<-\s*/ end=/[:{]/ contained keepend
syn match  basmArrow /<-/ nextgroup=basmInput

" -------------------------------------------------------
" Operators & symbols
" -------------------------------------------------------
syn match basmOperator /<-/
syn match basmOperator /[{}()]/
syn match basmOperator /==\|!=\|>=\|<=\|>\|</
syn match basmOperator /+/
syn match basmOperator /-/
syn match basmOperator /*/
syn match basmOperator /=/

syn match basmMemoryKeyword /\<byte\>/
syn region basmMemory start=/\[/ end=/]/

" -------------------------------------------------------
" Blocks
" -------------------------------------------------------
syn region basmBlock start=/{/ end=/}/ transparent

" -------------------------------------------------------
" Highlight linking
" -------------------------------------------------------
hi def link basmComment       Comment
hi def link basmString        String
hi def link basmEscape        SpecialChar
hi def link basmNumber        Number
hi def link basmType          Type
hi def link basmDefKeyword    Define
hi def link basmKeyword       Keyword
hi def link basmRegister      Identifier
hi def link basmSyscall       Constant
hi def link basmConst         Constant
hi def link basmOperator      Operator
hi def link basmMemoryKeyword Keyword
hi def link basmMemory        Identifier
hi def link basmBlock         Statement

" NEW highlight:
hi def link basmArrow         Number
hi def link basmInput         String

let b:current_syntax = "basm"
