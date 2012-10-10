B := build/

DIR = @mkdir -p $(@D)
LATEX = TEXINPUTS=$(@D): pdflatex -halt-on-error -interaction=nonstopmode \
	-output-directory $(@D) -shell-escape
REF_WARNING := "Rerun to get cross-references right"

docs: $Bdesign.pdf $Bevaluation.pdf

$B%.pdf: %.tex
	$(DIR)
	$(LATEX) $^
	@while grep $(REF_WARNING) $(@:.pdf=.log) > /dev/null; do \
		echo "** Rerunning pdflatex for cross-references **"; \
		$(LATEX) $^; \
	done

clean:
	@rm -rfv $B

.PHONY: docs
