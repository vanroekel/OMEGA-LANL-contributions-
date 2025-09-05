# © 2025. Triad National Security, LLC. All rights reserved.
# This program was produced under U.S. Government contract 89233218CNA000001 for Los Alamos National Laboratory (LANL), which is operated by Triad National Security, LLC for the U.S. Department of Energy/National Nuclear Security Administration. All rights in the program are reserved by Triad National Security, LLC, and the U.S. Department of Energy/National Nuclear Security Administration. The Government is granted for itself and others acting on its behalf a nonexclusive, paid-up, irrevocable worldwide license in this material to reproduce, prepare. derivative works, distribute copies to the public, perform publicly and display publicly, and to permit others to do so.

# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

import os
from datetime import date

# -- Project information -----------------------------------------------------

project = "Omega"
copyright = f"{date.today().year}, Energy Exascale Earth System Model Project"
author = "E3SM Development Team"

# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.
if 'DOCS_VERSION' in os.environ:
    version = os.environ.get('DOCS_VERSION')
    release = version
else:
    # The short X.Y.Z version.
    version = 'develop'
    # The full version, including alpha/beta/rc tags.
    release = 'develop'

master_doc = "index"
language = "en"

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "myst_parser",
    "sphinx_rtd_theme",
    "sphinx.ext.intersphinx",
    "sphinx.ext.viewcode",
    "sphinx.ext.mathjax",
]

numfig = True
# Add any paths that contain templates here, relative to this directory.
templates_path = ["_templates"]

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ["_build", "Thumbs.db", ".DS_Store", "README.md",
                    "**/README.md"]

intersphinx_mapping = {
    'matplotlib': ('http://matplotlib.org/stable', None),
    'numpy': ('https://numpy.org/doc/stable', None),
    'polaris': ('https://e3sm-project.github.io/polaris/main', None),
    'python': ('https://docs.python.org', None),
    'scipy': ('http://docs.scipy.org/doc/scipy/reference', None),
    "sphinx": ("https://www.sphinx-doc.org/en/master", None),
    'xarray': ('http://xarray.pydata.org/en/stable', None)
}

# -- MyST settings ---------------------------------------------------

myst_enable_extensions = [
    'colon_fence',
    'deflist',
    'dollarmath'
]
myst_number_code_blocks = ["typescript"]
myst_heading_anchors = 3
myst_footnote_transition = True
myst_dmath_double_inline = True
myst_enable_checkboxes = True
suppress_warnings = ['myst.header']
myst_heading_start_level = 1

# -- HTML output -------------------------------------------------

html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ["_static"]

html_sidebars = {
    "**": [
        "versions.html",
    ],
}

smv_tag_whitelist = r"^\d+\.\d+.\d+$"  # Include tags like "tags/2.5.0"
smv_branch_whitelist = "main"
smv_remote_whitelist = r"^(origin|upstream)$"  # Use branches from origin
