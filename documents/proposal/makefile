all: proposal.pdf

clean:
	rm -f proposal-blx.bib proposal.aux proposal.bcf proposal.log \
	      proposal.run.xml proposal.pdf references.bib proposal.bbl \
	      proposal.blg proposal-references.bib

proposal.pdf: proposal.tex
	pdflatex proposal
	bibtex proposal
	pdflatex proposal
	pdflatex proposal

