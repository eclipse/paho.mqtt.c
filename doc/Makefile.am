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

if PAHO_BUILD_DOC
DoxyfileV3ClientAPI.stamp:
	$(DOXYGEN) DoxyfileV3ClientAPI
	echo Timestamp > DoxyfileV3ClientAPI.stamp

DoxyfileV3AsyncAPI.stamp:
	$(DOXYGEN) DoxyfileV3AsyncAPI
	echo Timestamp > DoxyfileV3AsyncAPI.stamp

DoxyfileV3ClientInternal.stamp:
	$(DOXYGEN) DoxyfileV3ClientInternal
	echo Timestamp > DoxyfileV3ClientInternal.stamp

CLEANFILES = DoxyfileV3ClientAPI.stamp DoxyfileV3AsyncAPI.stamp DoxyfileV3ClientInternal.stamp

all-local: DoxyfileV3ClientAPI.stamp DoxyfileV3AsyncAPI.stamp DoxyfileV3ClientInternal.stamp
clean-local:
	rm -rf $(top_srcdir)/docs/man
endif
