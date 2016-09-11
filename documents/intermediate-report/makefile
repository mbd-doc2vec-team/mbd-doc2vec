all: intermediate-report.pdf

clean:
	rm -f intermediate-report-blx.bib intermediate-report.aux intermediate-report.bcf intermediate-report.log \
				intermediate-report.run.xml intermediate-report.pdf references.bib intermediate-report.bbl \
				intermediate-report.blg intermediate-report-references.bib

intermediate-report.pdf: intermediate-report.tex
	pdflatex intermediate-report
	bibtex intermediate-report
	pdflatex intermediate-report
	pdflatex intermediate-report

