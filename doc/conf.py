#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os

extensions = ['breathe']
breathe_projects = { 'unistdx': os.getenv('MESON_BUILD_ROOT') + '/xml' }
breathe_default_project = 'unistdx'
breathe_default_members = ('members')
templates_path = ['_templates']
source_suffix = ['.rst']
master_doc = 'index'
project = 'Unistdx'
copyright = '2018, Ivan Gankevich'
author = 'Ivan Gankevich'
version = '3.2'
release = '3.2'
language = None
exclude_patterns = []
pygments_style = 'sphinx'
todo_include_todos = False
html_theme = 'alabaster'
# html_theme_options = {}
html_static_path = ['_static']
html_sidebars = {
    '**': [
        'relations.html',  # needs 'show_related': True theme option to display
        'searchbox.html',
    ]
}
htmlhelp_basename = 'Unistdxdoc'
latex_elements = {
    # 'papersize': 'letterpaper',
    # 'pointsize': '10pt',
    # 'preamble': '',
    # 'figure_align': 'htbp',
}
latex_documents = [
    (master_doc, 'Unistdx.tex', 'Unistdx Documentation',
     'Ivan Gankevich', 'manual'),
]
man_pages = [
    (master_doc, 'unistdx', 'Unistdx Documentation',
     [author], 1)
]
texinfo_documents = [
    (master_doc, 'Unistdx', 'Unistdx Documentation',
     author, 'Unistdx', 'One line description of project.',
     'Miscellaneous'),
]
