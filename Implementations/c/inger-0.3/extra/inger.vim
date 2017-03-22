" Vim syntax file
" Language:	Inger
" Maintainer:	F.J.F. Benders
" Last Change:	2002 Nov 12

 :syntax clear
" A bunch of useful Inger keywords
  syntax keyword	iKeywords	break case continue default do else extern goto if label module return start switch while
  syntax keyword	iIdentifiers    bool char float int string void untyped

" Detect strings
  syntax region iString start=/"/ skip=/\\"/ end=/"/

" Comments
  syntax region iCommentLine  start="//" skip="\\$" end="$"
  syntax region iCommentBlock start="/\*" end="\*/"

" Include
  syntax region iInclude start="#" skip="\\ " end=" "

" Numbers and floats
  syntax match iNumber "\<\d\+"

" floating point number, with dot, optional exponent
  syntax match iFloat "\d\+\.\d*\(e[-+]\=\d\+\)\=[fl]\="

" floating point number, starting with a dot, optional exponent
  syntax match iFloat "\.\d\+\(e[-+]\=\d\+\)\=[fl]\=\>"

" floating point number, without dot, with exponent
  syntax match iFloat "\d\+e[-+]\=\d\+[fl]\=\>"

" 


  highlight link iKeywords	Type
  highlight link iIdentifiers 	Type
  highlight link iString	String
  highlight link iCommentLine	Comment
  highlight link iCommentBlock	Comment
  highlight link iInclude       Include
  highlight link iNumber        Number
  highlight link iFloat		Float


let b:current_syntax = "inger"
