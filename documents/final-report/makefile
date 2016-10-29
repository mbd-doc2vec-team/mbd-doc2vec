all: final-report.pdf

clean:
	rm -f final-report-blx.bib final-report.aux final-report.bcf final-report.log \
				final-report.run.xml final-report.pdf references.bib final-report.bbl \
				final-report.blg final-report-references.bib


final-report.pdf: final-report.tex
	pdflatex final-report
	bibtex final-report
	pdflatex final-report
	pdflatex final-report

