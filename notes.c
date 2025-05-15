// Documentation
1. doxygen
2. doxysphinx build doc/ doc/build Doxyfile
3. sphinx-build -b html doc/ doc/build

pip install sphinx_rtd_theme
pip install doxysphinx
pip install sphinxcontrib-doxylink
pip install sphinx-tabs
pip install sphinx-hoverxref



clang-format --Werror --dry-run -fallback-style=none --ferror-limit=0 --files=files to process --style=file 
