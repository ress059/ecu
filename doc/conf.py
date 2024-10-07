# Configuration file for the ECU documentation builder.

# ------------------------------- Path setup ------------------------------
import sys
from pathlib import Path

ECU_BASE_DIR = Path(__file__).resolve().parents[1]


# sys.path.append("/home/me/docproj/ext/breathe/") !!!! TODO Need to add breathe executable to path!!! How to do this in a non-jank way???



# -------------------------- Project information --------------------------
project = "Embedded C Utilities (ECU)"
copyright = "2024, Ian Ress"
author = "Ian Ress"


# -------------------------- General configuration --------------------------

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
            "sphinxcontrib.doxylink",
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


# -------------------------- Options for HTML output --------------------------
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


# -------------------------- Doxylink setup --------------------------
doxylink = {
    # "ecudoxygen" is the role name that you can later use in sphinx to reference this doxygen documentation.
    # First parameter is tagfile.
    # Second parameter is a relative path pointing from sphinx output directory to the doxygen output 
    # folder inside the output directory tree. Doxylink will use the tagfile to get the html file name 
    # of the symbol you want to link and then prefix it with this path to generate html links (<a>-tags).
    "ecudoxygen": (str(ECU_BASE_DIR / "doc" / "doxygen" / "html" / "tagfile.xml"), "doxygen/html")
}


# -- Breathe configuration -------------------------------------------------
# breathe_projects = {
#     "ECU": str(ECU_BASE_DIR / "doc" / "doxygen" / "xml")
# }

# breathe_default_project = "ECU"
