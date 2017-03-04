SET(MQTT_TEST_BROKER "tcp://localhost:1883" CACHE STRING "Hostname of a test MQTT broker to use")
SET(MQTT_SSL_HOSTNAME "localhost" CACHE STRING "Hostname of a test SSL MQTT broker to use")

ADD_EXECUTABLE(
	test1
	test1.c
)

TARGET_LINK_LIBRARIES(
	test1
	paho-mqtt3c
)

ADD_TEST(
	NAME test1
	COMMAND "test1" "--connection" ${MQTT_TEST_BROKER}
)

ADD_EXECUTABLE(
	test2
	test2.c
)

TARGET_LINK_LIBRARIES(
	test2
	paho-mqtt3c
)

ADD_TEST(
	NAME test2
	COMMAND test2 "--connection" ${MQTT_TEST_BROKER}
)

ADD_EXECUTABLE(
	test3
	test3.c
)

TARGET_LINK_LIBRARIES(
	test3
	paho-mqtt3cs
)

ADD_TEST(
	NAME test3
	COMMAND test3 "--connection" ${MQTT_TEST_BROKER}
)

ADD_EXECUTABLE(
	test4
	test4.c
)

TARGET_LINK_LIBRARIES(
	test4
	paho-mqtt3a
)

ADD_TEST(
	NAME test4
	COMMAND test4 "--connection" ${MQTT_TEST_BROKER}
)


ADD_EXECUTABLE(
	test5
	test5.c
)

TARGET_LINK_LIBRARIES(
	test5
	paho-mqtt3as
)

ADD_TEST(
	NAME test5
	COMMAND test5 "--hostname" ${MQTT_SSL_HOSTNAME}
)

ADD_EXECUTABLE(
	test6
	test6.c
)

TARGET_LINK_LIBRARIES(
	test6
	paho-mqtt3a
)

ADD_TEST(
	NAME test6
	COMMAND test6 "--connection" ${MQTT_TEST_BROKER}
)

ADD_EXECUTABLE(
	test8
	test8.c
)

TARGET_LINK_LIBRARIES(
	test8
	paho-mqtt3a
)

ADD_TEST(
	NAME test8
	COMMAND test8 "--connection" ${MQTT_TEST_BROKER}
)

ADD_EXECUTABLE(
	test9
	test9.c
)

TARGET_LINK_LIBRARIES(
	test9
	paho-mqtt3a
)

ADD_TEST(
	NAME test9
	COMMAND test9 "--connection" ${MQTT_TEST_BROKER}
)