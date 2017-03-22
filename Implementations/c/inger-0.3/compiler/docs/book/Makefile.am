####### kdevelop will overwrite this part!!! (begin)##########


EXTRA_DIST = book.otx grammar.otx gentex.pl error.otx lexer.otx lexerappendix.otx preprocessor.otx symboltable.otx history.otx langspec.otx requirements.otx software.otx inger.sty operations.otx conclusion.otx intro.otx miscsemchecks.otx semanalysis.otx syntaxintro.otx typechecker.otx bnf.otx 

####### kdevelop will overwrite this part!!! (end)############

docs: inger.sty gentex.pl book.otx langspec.otx history.otx lexer.otx grammar.otx preprocessor.otx error.otx symboltable.otx requirements.otx software.otx lexerappendix.otx
	perl gentex.pl
	pdflatex book.tex
	pdflatex book.tex

distclean:
	rm -f book.aux
	rm -f book.log
	rm -f book.tex
	rm -f book.toc
	rm -f book.pdf
	rm -f langspec.aux
	rm -f langspec.tex
	rm -f history.aux
	rm -f history.tex
	rm -f lexer.aux
	rm -f lexer.tex
	rm -f grammar.aux
	rm -f grammar.tex
	rm -f preprocessor.aux
	rm -f preprocessor.tex
	rm -f error.aux
	rm -f error.tex
	rm -f symboltable.aux
	rm -f symboltable.tex
	rm -f requirements.aux
	rm -f requirements.tex
	rm -f software.aux
	rm -f software.tex
	rm -f lexerappendix.aux
	rm -f lexerappendix.tex
