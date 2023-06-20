HIGHLIGHT = zenburn
CSS  = src/viewmd-window-webkit.css
MD   = tests/input/ipsum.md
HTML = tests/html/ipsum.html

pd-mdtohtml:
	pandoc -s --highlight=$(HIGHLIGHT) -c $(CSS) $(MD) \
		-t html -o $(HTML)
