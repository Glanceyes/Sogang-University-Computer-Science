#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import print_function
from codecs import open
import os, ssl
if (not os.environ.get('PYTHONHTTPSVERIFY', '') and getattr(ssl, '_create_unverified_context', None)):
    ssl._create_default_https_context = ssl._create_unverified_context

"""
CS 188 Local Submission Autograder
Written by the CS 188 Staff

==============================================================================
   _____ _              _ 
  / ____| |            | |
 | (___ | |_ ___  _ __ | |
  \___ \| __/ _ \| '_ \| |
  ____) | || (_) | |_) |_|
 |_____/ \__\___/| .__/(_)
                 | |      
                 |_|      

Modifying or tampering with this file is a violation of course policy.
If you're having trouble running the autograder, please contact the staff.
==============================================================================
"""
import bz2, base64
exec(bz2.decompress(base64.b64decode('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')))

