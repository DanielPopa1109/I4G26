"""Compatibility entry point for the former provisional integration gate."""
from pathlib import Path
import runpy
runpy.run_path(str(Path(__file__).with_name('check_source_integration.py')),run_name='__main__')