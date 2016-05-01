from distutils.core import setup, Extension

paho_mqtt3c = Extension('paho_mqtt3c',
              define_macros = [('NO_HEAP_TRACKING', '1')],
              sources = ['mqttclient_module.c', '../../src/LinkedList.c'],
					    libraries = ['paho-mqtt3c'],
						library_dirs = ['../../build/output'],
              include_dirs = ['../../src'])

paho_mqtt3a = Extension('paho_mqtt3a',
              define_macros = [('NO_HEAP_TRACKING', '1')],
              sources = ['mqttasync_module.c', '../../src/LinkedList.c'],
                        libraries = ['paho-mqtt3a'],
                        library_dirs = ['../../build/output'],
              include_dirs = ['../../src'])

setup (name = 'EclipsePahoMQTTClient',
       version = '1.0',
       description = 'Binding to the Eclipse Paho C clients',
       ext_modules = [paho_mqtt3c, paho_mqtt3a])
