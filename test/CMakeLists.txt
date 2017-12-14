PROJECT(mqtt-tests C)

SET(MQTT_TEST_BROKER "tcp://localhost:1883" CACHE STRING "Hostname of a test MQTT broker to use")
SET(MQTT_TEST_PROXY "tcp://localhost:1884" CACHE STRING "Hostname of the test proxy to use")
SET(MQTT_SSL_HOSTNAME "localhost" CACHE STRING "Hostname of a test SSL MQTT broker to use")
IF (WIN32)
  SET(CERTDIR $ENV{APPVEYOR_BUILD_FOLDER}/test/ssl)
ELSE ()
  SET(CERTDIR $ENV{TRAVIS_BUILD_DIR}/test/ssl)
ENDIF ()

IF (PAHO_WITH_SSL)
    SET(OPENSSL_SEARCH_PATH "" CACHE PATH "Directory containing OpenSSL libraries and includes")

    IF (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
      SET(OPENSSL_SEARCH_PATH "/usr/local/opt/openssl")
    ENDIF (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")

    IF (WIN32)
      SET(OPENSSL_SEARCH_PATH "C:/OpenSSL-Win64")
    ENDIF ()

    FIND_PATH(OPENSSL_INCLUDE_DIR openssl/ssl.h
        HINTS ${OPENSSL_SEARCH_PATH}/include)

    MESSAGE(STATUS "OpenSSL hints: ${OPENSSL_SEARCH_PATH}")
    MESSAGE(STATUS "OpenSSL headers found at ${OPENSSL_INCLUDE_DIR}")

    INCLUDE_DIRECTORIES(
        ${OPENSSL_INCLUDE_DIR}
    )
ENDIF ()

ADD_EXECUTABLE(
	test1
	test1.c
)

TARGET_LINK_LIBRARIES(
	test1
	paho-mqtt3c
)

IF (WIN32)
	ADD_CUSTOM_COMMAND(
		TARGET test1 PRE_BUILD
		COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/dll-copy.cmake
		COMMENT "Copying DLLs to test directory"
	)
ENDIF()

ADD_TEST(
	NAME test1-1-single-thread-client
	COMMAND "test1" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test1-2-multithread-callbacks
	COMMAND "test1" "--test_no" "2" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test1-3-connack-return-codes
	COMMAND "test1" "--test_no" "3" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test1-4-client-persistence
	COMMAND "test1" "--test_no" "4" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test1-5-disconnect-with-quiesce
	COMMAND "test1" "--test_no" "5" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test1-6-connlost-will-message
	COMMAND "test1" "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
)

ADD_TEST(
	NAME test1-7-connlost-binary-will-message
	COMMAND "test1" "--test_no" "7" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
)

SET_TESTS_PROPERTIES(
	test1-1-single-thread-client
	test1-2-multithread-callbacks
	test1-3-connack-return-codes
	test1-4-client-persistence
	test1-5-disconnect-with-quiesce
	test1-6-connlost-will-message
	test1-7-connlost-binary-will-message
	PROPERTIES TIMEOUT 540
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
	NAME test2-1-multiple-threads-single-client
	COMMAND test2 "--connection" ${MQTT_TEST_BROKER}
)

SET_TESTS_PROPERTIES(
	test2-1-multiple-threads-single-client
	PROPERTIES TIMEOUT 540
)

IF (PAHO_WITH_SSL)
	ADD_EXECUTABLE(
		test3
		test3.c
	)

	TARGET_LINK_LIBRARIES(
		test3
		paho-mqtt3cs
	)

ADD_TEST(
	NAME test3-1-ssl-conn-to-non-SSL-broker
	COMMAND test3 "--test_no" "1" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test3-2as-mutual-ssl-auth-single-thread
	COMMAND test3 "--test_no" "2" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test3-2am-mutual-ssl-auth-multi-thread
	COMMAND test3 "--test_no" "3" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test3-2b-mutual-ssl-broker-missing-client-cert
	COMMAND test3 "--test_no" "4" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test3-2c-mutual-ssl-client-missing-broker-cert
	COMMAND test3 "--test_no" "5" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test3-3as-broker-auth-server-cert-in-client-store-single-thread
	COMMAND test3 "--test_no" "6" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test3-3am-broker-auth-server-cert-in-client-store-multi-thread
	COMMAND test3 "--test_no" "7" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test3-3b-broker-auth-client-missing-broker-cert
	COMMAND test3 "--test_no" "8" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test3-4s-broker-auth-accept-invalid-certificate-single-thread
	COMMAND test3 "--test_no" "9" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test3-4m-broker-auth-accept-invalid-certificate-multi-thread
	COMMAND test3 "--test_no" "10" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

SET_TESTS_PROPERTIES(
	test3-1-ssl-conn-to-non-SSL-broker
	test3-2as-mutual-ssl-auth-single-thread
	test3-2am-mutual-ssl-auth-multi-thread
	test3-2b-mutual-ssl-broker-missing-client-cert
	test3-2c-mutual-ssl-client-missing-broker-cert
	test3-3as-broker-auth-server-cert-in-client-store-single-thread
	test3-3am-broker-auth-server-cert-in-client-store-multi-thread
	test3-3b-broker-auth-client-missing-broker-cert
	test3-4s-broker-auth-accept-invalid-certificate-single-thread
	test3-4m-broker-auth-accept-invalid-certificate-multi-thread
	PROPERTIES TIMEOUT 540
)

ENDIF()

ADD_EXECUTABLE(
	test4
	test4.c
)

TARGET_LINK_LIBRARIES(
	test4
	paho-mqtt3a
)

ADD_TEST(
	NAME test4-1-basic-connect-subscribe-receive
	COMMAND test4 "--test_no" "1" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test4-2-connect-timeout
	COMMAND test4 "--test_no" "2" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test4-3-multiple-client-objs-simultaneous-working
	COMMAND test4 "--test_no" "3" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test4-4-send-receive-big-messages
	COMMAND test4 "--test_no" "4" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test4-5-connack-return-codes
	COMMAND test4 "--test_no" "5" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test4-6-ha-connections
	COMMAND test4 "--test_no" "6" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test4-7-pending-tokens
	COMMAND test4 "--test_no" "7" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test4-8-incomplete-commands-requests
	COMMAND test4 "--test_no" "8" "--connection" ${MQTT_TEST_BROKER}
)

SET_TESTS_PROPERTIES(
	test4-1-basic-connect-subscribe-receive
	test4-2-connect-timeout
	test4-3-multiple-client-objs-simultaneous-working
	test4-4-send-receive-big-messages
	test4-5-connack-return-codes
	test4-6-ha-connections
	test4-7-pending-tokens
	test4-8-incomplete-commands-requests
	PROPERTIES TIMEOUT 540
)

IF (PAHO_WITH_SSL)
	ADD_EXECUTABLE(
		test5
		test5.c
	)

	TARGET_LINK_LIBRARIES(
		test5
		paho-mqtt3as
	)

ADD_TEST(
	NAME test5-1-ssl-connection-to-no-SSL-server
	COMMAND test5 "--test_no" "1" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key" 	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test5-2a-multual-ssl-auth-certificates-in-place
	COMMAND test5 "--test_no" "2" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key" 	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test5-2b-multual-ssl-auth-broker-missing-client-cert
	COMMAND test5 "--test_no" "3" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key" 	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test5-2c-multual-ssl-auth-client-missing-broker-cert
	COMMAND test5 "--test_no" "4" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key" 	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test5-2d-multual-ssl-auth-client-missing-client-cert
	COMMAND test5 "--test_no" "5" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key" 	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test5-3a-server-auth-server-cert-in-client-store
	COMMAND test5 "--test_no" "6" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key" 	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test5-3b-server-auth-client-missing-broker-cert
	COMMAND test5 "--test_no" "7" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key" 	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

ADD_TEST(
	NAME test5-4-accept-invalid-certificates
	COMMAND test5 "--test_no" "8" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key" 	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
)

SET_TESTS_PROPERTIES(
	test5-1-ssl-connection-to-no-SSL-server
	test5-2a-multual-ssl-auth-certificates-in-place
	test5-2b-multual-ssl-auth-broker-missing-client-cert
	test5-2c-multual-ssl-auth-client-missing-broker-cert
	test5-2d-multual-ssl-auth-client-missing-client-cert
	test5-3a-server-auth-server-cert-in-client-store
	test5-3b-server-auth-client-missing-broker-cert
	test5-4-accept-invalid-certificates
	PROPERTIES TIMEOUT 540
)

ENDIF()

ADD_EXECUTABLE(
	test6
	test6.c
)

TARGET_LINK_LIBRARIES(
	test6
	paho-mqtt3a
)

ADD_TEST(
	NAME test6-restart-recovery
	COMMAND test6 "--connection" ${MQTT_TEST_BROKER}
)

SET_TESTS_PROPERTIES(
	test6-restart-recovery
	PROPERTIES TIMEOUT 540
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
	NAME test8-1-basic-connect-subscribe-receive
	COMMAND test8 "--test_no" "1" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test8-2-connect-timeout
	COMMAND test8 "--test_no" "2" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test8-3-multiple-client-objects-simultaneous-working
	COMMAND test8 "--test_no" "3" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test8-4-send-receive-big-messages
	COMMAND test8 "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--size" "500000"
)

ADD_TEST(
	NAME test8-5a-all-ha-connections-out-of-service
	COMMAND test8 "--test_no" "5" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test8-5b-all-ha-connections-out-of-service-except-the-last-one
	COMMAND test8 "--test_no" "6" "--connection" ${MQTT_TEST_BROKER}
)

ADD_TEST(
	NAME test8-5c-all-ha-connections-out-of-service-except-the-first-one
	COMMAND test8 "--test_no" "7" "--connection" ${MQTT_TEST_BROKER}
)

SET_TESTS_PROPERTIES(
	test8-1-basic-connect-subscribe-receive
	test8-2-connect-timeout
	test8-3-multiple-client-objects-simultaneous-working
	test8-4-send-receive-big-messages
        test8-5a-all-ha-connections-out-of-service
        test8-5b-all-ha-connections-out-of-service-except-the-last-one
        test8-5c-all-ha-connections-out-of-service-except-the-first-one
	PROPERTIES TIMEOUT 540
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
	NAME test9-1-offline-buffering-send-disconnected
	COMMAND test9 "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
)

ADD_TEST(
	NAME test9-2-offline-buffering-send-disconnected-serverURIs
	COMMAND test9 "--test_no" "2" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
)

ADD_TEST(
	NAME test9-3-offline-buffering-auto-reconnect
	COMMAND test9 "--test_no" "3" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
)

ADD_TEST(
	NAME test9-4-offline-buffering-auto-reconnect-serverURIs
	COMMAND test9 "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
)

ADD_TEST(
	NAME test9-5-offline-buffering-max-buffered
	COMMAND test9 "--test_no" "5" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
)

ADD_TEST(
	NAME test9-6-offline-buffering-max-buffered-binary-will
	COMMAND test9 "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
)

SET_TESTS_PROPERTIES(
	test9-1-offline-buffering-send-disconnected
	test9-2-offline-buffering-send-disconnected-serverURIs
	test9-3-offline-buffering-auto-reconnect
	test9-4-offline-buffering-auto-reconnect-serverURIs
	test9-5-offline-buffering-max-buffered
	test9-6-offline-buffering-max-buffered-binary-will
	PROPERTIES TIMEOUT 540
)
