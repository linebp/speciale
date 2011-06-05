all: basecase1.pdf basecase2.pdf basecase3.pdf basecase4.pdf alternation.pdf concatenation.pdf repetition.pdf

%.pdf : %.dot
	dot -Tpdf $< -o $@
