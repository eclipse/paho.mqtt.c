#*******************************************************************************
#  Copyright (c) 2016
# 
#  All rights reserved. This program and the accompanying materials
#  are made available under the terms of the Eclipse Public License v1.0
#  and Eclipse Distribution License v1.0 which accompany this distribution. 
# 
#  The Eclipse Public License is available at 
#     http://www.eclipse.org/legal/epl-v10.html
#  and the Eclipse Distribution License is available at 
#    http://www.eclipse.org/org/documents/edl-v10.php.
# 
#  Contributors:
#     Guilherme Maciel Ferreira - initial implementation
#*******************************************************************************/

ACLOCAL_AMFLAGS = -I m4

###############################################################################
# Sub directories
###############################################################################

SUBDIRS  = src

if PAHO_BUILD_SAMPLES
SUBDIRS += src/samples
endif

if PAHO_BUILD_TEST
SUBDIRS += test
endif

SUBDIRS += doc
