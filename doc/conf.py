#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import sphinx_bootstrap_theme

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
pygments_style = 'perldoc'
todo_include_todos = False
#html_theme = 'alabaster'
#html_theme = 'sphinx_rtd_theme'
html_theme = 'bootstrap'
html_theme_path = sphinx_bootstrap_theme.get_html_theme_path()
html_theme_options = {
        'navbar_sidebarrel': False,
        'navbar_pagenav': False,
        'bootswatch_theme': "cerulean",
        'bootstrap_version': "3",
        'navbar_links': [
            ("Reference", "api-reference"),
            ("Tutorials", "tutorials"),
            ("How-tos", "how-to-guides"),
            ("Advanced topics", "advanced-topics"),
            ],
        'nosidebar': True,
        }
html_static_path = ['_static']
html_show_sourcelink = False
def setup(app):
    app.add_stylesheet("unistdx.css")
