/*******************************************************************************
 * Copyright (c) 2020, 2020 Andreas Walter
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Andreas Walter - initially moved export declarations into separate fle
 *******************************************************************************/

#if !defined(EXPORTDECLARATIONS_H)
#define EXPORTDECLARATIONS_H

#if defined(WIN32) || defined(WIN64)
#   if defined(MQTT_EXPORTS)
#       define LIBMQTT_API __declspec(dllexport)
#   elif defined(MQTT_DIRECT)
#       define LIBMQTT_API
#   else
#       define LIBMQTT_API __declspec(dllimport)
#    endif
#else
#    if defined(MQTT_EXPORTS)
#       define LIBMQTT_API  __attribute__ ((visibility ("default")))
#    else
#       define LIBMQTT_API extern
#    endif
#endif

#endif
