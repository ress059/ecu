# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------
project = "Embedded C Utilities (ECU)"
copyright = "2024, Ian Ress"
author = "Ian Ress"


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
            "sphinx_rtd_theme",
            "sphinx.ext.todo",
            "sphinx.ext.extlinks",
            "sphinx.ext.autodoc",
            "sphinx.ext.graphviz",
            "sphinx.ext.todo",
            "sphinx_tabs.tabs",
            "hoverxref.extension"
]

hoverxref_roles = [
    "term"
]

hoverxref_role_types = {
    "term": "tooltip"
}

# Add any paths that contain templates here, relative to this directory.
# templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []

todo_include_todos = True


# -- Options for HTML output -------------------------------------------------
html_logo = "_static/images/logo.svg"
html_favicon = "_static/images/favicon.svg"
html_theme = "sphinx_rtd_theme"
html_theme_options = {
    "logo_only": True,
    "prev_next_buttons_location": None,
    "navigation_depth": 5
}
html_title = "ECU Project Documentation"
html_show_sourcelink = False
html_show_sphinx = False
html_static_path = ["_static"]
html_css_files = [
    "css/custom.css",
]
