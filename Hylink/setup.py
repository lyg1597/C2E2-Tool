from distutils.core import setup
import py2exe

setup(
    name = 'hylink',
    description = 'Hybrid Link',
    version = '0.4',

    windows = [
                  {
                      'script': 'C:\Documents and Settings\Karthik Manamcheri\workspace\HyLink1\src\main.py',
                  }
              ],

    options = {
                  'py2exe': {
                      'packages':'encodings',
                      'includes': 'cairo, pango, pangocairo, atk, gobject, gio',
                  }
              },
#
#    data_files=[
#                   'handytool.glade',
#                   'readme.txt'
#               ]
)