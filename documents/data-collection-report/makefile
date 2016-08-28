all: data-collection-report.pdf

clean:
	rm -f data-collection-report-blx.bib data-collection-report.aux data-collection-report.bcf data-collection-report.log \
				data-collection-report.run.xml data-collection-report.pdf references.bib data-collection-report.bbl \
				data-collection-report.blg data-collection-report-references.bib


data-collection-report.pdf: data-collection-report.tex
	pdflatex data-collection-report
	bibtex data-collection-report
	pdflatex data-collection-report
	pdflatex data-collection-report

