PROJECT(mqtt-tests C)

SET(MQTT_TEST_BROKER "tcp://localhost:1883" CACHE STRING "Hostname of a test MQTT broker to use")
SET(MQTT_WS_TEST_BROKER "ws://localhost:1883" CACHE STRING "WebSocket connection to a test MQTT broker")
SET(MQTT_TEST_PROXY "tcp://localhost:1884" CACHE STRING "Hostname of the test proxy to use")
SET(MQTT_WS_TEST_PROXY "tcp://localhost:1884" CACHE STRING "WebSocket connection of a test proxy to use")
SET(MQTT_SSL_HOSTNAME "localhost" CACHE STRING "Hostname of a test SSL MQTT broker to use")

SET(CERTDIR ${CMAKE_SOURCE_DIR}/test/ssl)


IF (WIN32)
	SET(LIBS_SYSTEM ws2_32)
ELSEIF (UNIX)
	IF(CMAKE_SYSTEM_NAME MATCHES "Linux")
		SET(LIBS_SYSTEM c dl pthread)
	ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
		SET(LIBS_SYSTEM compat pthread)
	ELSE()
		SET(LIBS_SYSTEM c pthread)
	ENDIF()
ENDIF()

ADD_EXECUTABLE(
	thread
	thread.c ../src/Thread.c
)

SET_TARGET_PROPERTIES(
  thread PROPERTIES
  COMPILE_DEFINITIONS "NOSTACKTRACE;NOLOG_MESSAGES" 
)

TARGET_LINK_LIBRARIES(
	thread 
	${LIBS_SYSTEM}
)

IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test1-static
		test1.c
	)
	TARGET_LINK_LIBRARIES(
		test1-static
		paho-mqtt3c-static
	)
	
	ADD_TEST(
		NAME test1-1-single-thread-client-static
		COMMAND "test1-static" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test1-2-multithread-callbacks-static
		COMMAND "test1-static" "--test_no" "2" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test1-3-connack-return-codes-static
		COMMAND "test1-static" "--test_no" "3" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test1-4-client-persistence-static
		COMMAND "test1-static" "--test_no" "4" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test1-5-disconnect-with-quiesce-static
		COMMAND "test1-static" "--test_no" "5" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test1-6-connlost-will-message-static
		COMMAND "test1-static" "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test1-7-connlost-binary-will-message-static
		COMMAND "test1-static" "--test_no" "7" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	SET_TESTS_PROPERTIES(
		test1-1-single-thread-client-static
		test1-2-multithread-callbacks-static
		test1-3-connack-return-codes-static
		test1-4-client-persistence-static
		test1-5-disconnect-with-quiesce-static
		test1-6-connlost-will-message-static
		test1-7-connlost-binary-will-message-static
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_SHARED)
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
	
	ADD_TEST(
        NAME test1-1-ws-single-thread-client
        COMMAND "test1" "--test_no" "1" "--connection" ${MQTT_WS_TEST_BROKER}
    )
    
    ADD_TEST(
        NAME test1-2-ws-multithread-callbacks
        COMMAND "test1" "--test_no" "2" "--connection" ${MQTT_WS_TEST_BROKER}
    )
    
    ADD_TEST(
        NAME test1-3-ws-connack-return-codes
        COMMAND "test1" "--test_no" "3" "--connection" ${MQTT_WS_TEST_BROKER}
    )
    
    ADD_TEST(
        NAME test1-4-ws-client-persistence
        COMMAND "test1" "--test_no" "4" "--connection" ${MQTT_WS_TEST_BROKER}
    )
    
    ADD_TEST(
        NAME test1-5-ws-disconnect-with-quiesce
        COMMAND "test1" "--test_no" "5" "--connection" ${MQTT_WS_TEST_BROKER}
    )
    
    ADD_TEST(
        NAME test1-6-ws-connlost-will-message
        COMMAND "test1" "--test_no" "6" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test1-7-ws-connlost-binary-will-message
        COMMAND "test1" "--test_no" "7" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    SET_TESTS_PROPERTIES(
        test1-1-ws-single-thread-client
        test1-2-ws-multithread-callbacks
        test1-3-ws-connack-return-codes
        test1-4-ws-client-persistence
        test1-5-ws-disconnect-with-quiesce
        test1-6-ws-connlost-will-message
        test1-7-ws-connlost-binary-will-message
        PROPERTIES TIMEOUT 540
    )
ENDIF()



IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test15-static
		test15.c
	)
	TARGET_LINK_LIBRARIES(
		test15-static
		paho-mqtt3c-static
	)
	
	ADD_TEST(
		NAME test15-1-single-thread-client-static
		COMMAND "test15-static" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-2-multithread-callbacks-static
		COMMAND "test15-static" "--test_no" "2" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-3-connack-return-codes-static
		COMMAND "test15-static" "--test_no" "3" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-4-client-persistence-static
		COMMAND "test15-static" "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-5-disconnect-with-quiesce-static
		COMMAND "test15-static" "--test_no" "5" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-6-connlost-will-message-static
		COMMAND "test15-static" "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-7-connlost-binary-will-message-static
		COMMAND "test15-static" "--test_no" "7" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)

	SET_TESTS_PROPERTIES(
		test15-1-single-thread-client-static
		test15-2-multithread-callbacks-static
		test15-3-connack-return-codes-static
		test15-4-client-persistence-static
		test15-5-disconnect-with-quiesce-static
		test15-6-connlost-will-message-static
		test15-7-connlost-binary-will-message-static
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_SHARED)
	ADD_EXECUTABLE(
		test15
		test15.c
	)
	TARGET_LINK_LIBRARIES(
		test15
		paho-mqtt3c
	)

	ADD_TEST(
		NAME test15-1-single-thread-client
		COMMAND "test15" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-2-multithread-callbacks
		COMMAND "test15" "--test_no" "2" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-3-connack-return-codes
		COMMAND "test15" "--test_no" "3" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-4-client-persistence
		COMMAND "test15" "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-5-disconnect-with-quiesce
		COMMAND "test15" "--test_no" "5" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-6-connlost-will-message
		COMMAND "test15" "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test15-7-connlost-binary-will-message
		COMMAND "test15" "--test_no" "7" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)

	SET_TESTS_PROPERTIES(
		test15-1-single-thread-client
		test15-2-multithread-callbacks
		test15-3-connack-return-codes
		test15-4-client-persistence
		test15-5-disconnect-with-quiesce
		test15-6-connlost-will-message
		test15-7-connlost-binary-will-message
		PROPERTIES TIMEOUT 540
	)
	
	ADD_TEST(
        NAME test15-1-ws-single-thread-client
        COMMAND "test15" "--test_no" "1" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test15-2-ws-multithread-callbacks
        COMMAND "test15" "--test_no" "2" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test15-3-ws-connack-return-codes
        COMMAND "test15" "--test_no" "3" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test15-4-ws-client-persistence
        COMMAND "test15" "--test_no" "4" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test15-5-ws-disconnect-with-quiesce
        COMMAND "test15" "--test_no" "5" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test15-6-ws-connlost-will-message
        COMMAND "test15" "--test_no" "6" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test15-7-ws-connlost-binary-will-message
        COMMAND "test15" "--test_no" "7" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )

    SET_TESTS_PROPERTIES(
        test15-1-ws-single-thread-client
        test15-2-ws-multithread-callbacks
        test15-3-ws-connack-return-codes
        test15-4-ws-client-persistence
        test15-5-ws-disconnect-with-quiesce
        test15-6-ws-connlost-will-message
        test15-7-ws-connlost-binary-will-message
        PROPERTIES TIMEOUT 540
    )
ENDIF()

IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test2-static
		test2.c
	)
	
	TARGET_LINK_LIBRARIES(
		test2-static
		paho-mqtt3c-static
	)
	
	ADD_TEST(
		NAME test2-1-multiple-threads-single-client-static
		COMMAND test2-static "--connection" ${MQTT_TEST_BROKER}
	)
	
	SET_TESTS_PROPERTIES(
		test2-1-multiple-threads-single-client-static
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_SHARED)
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

	ADD_TEST(
		NAME test2-1-ws-multiple-threads-single-client
		COMMAND test2 "--connection" ${MQTT_WS_TEST_BROKER}
	)
	
	SET_TESTS_PROPERTIES(
	    test2-1-multiple-threads-single-client
		test2-1-ws-multiple-threads-single-client
		PROPERTIES TIMEOUT 540
	)
ENDIF()


IF (PAHO_WITH_SSL)

	IF (PAHO_BUILD_STATIC)
		ADD_EXECUTABLE(
			test3-static
			test3.c
		)
		TARGET_LINK_LIBRARIES(
			test3-static
			paho-mqtt3cs-static
		)
		
		ADD_TEST(
			NAME test3-1-ssl-conn-to-non-SSL-broker-static
			COMMAND test3-static "--test_no" "1" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test3-2as-mutual-ssl-auth-single-thread-static
			COMMAND test3-static "--test_no" "2" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test3-2am-mutual-ssl-auth-multi-thread-static
			COMMAND test3-static "--test_no" "3" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test3-2b-mutual-ssl-broker-missing-client-cert-static
			COMMAND test3-static "--test_no" "4" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test3-2c-mutual-ssl-client-missing-broker-cert-static
			COMMAND test3-static "--test_no" "5" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
            NAME test3-2e-ssl-with-ha-static
            COMMAND test3-static "--test_no" "12" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"   "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test3-2e-ws-ssl-with-ha-static
            COMMAND test3-static "--test_no" "12" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"   "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
		
		ADD_TEST(
			NAME test3-3as-broker-auth-server-cert-in-client-store-single-thread-static
			COMMAND test3-static "--test_no" "6" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test3-3am-broker-auth-server-cert-in-client-store-multi-thread-static
			COMMAND test3-static "--test_no" "7" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test3-3b-broker-auth-client-missing-broker-cert-static
			COMMAND test3-static "--test_no" "8" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test3-4s-broker-auth-accept-invalid-certificate-single-thread-static
			COMMAND test3-static "--test_no" "9" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test3-4m-broker-auth-accept-invalid-certificate-multi-thread-static
			COMMAND test3-static "--test_no" "10" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	"${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		#ADD_TEST(
		#	NAME test3-6-psk-ssl-auth-static
		#	COMMAND test3-static "--test_no" "11" "--hostname" ${MQTT_SSL_HOSTNAME}
		#)
		
		SET_TESTS_PROPERTIES(
			test3-1-ssl-conn-to-non-SSL-broker-static
			test3-2as-mutual-ssl-auth-single-thread-static
			test3-2am-mutual-ssl-auth-multi-thread-static
			test3-2b-mutual-ssl-broker-missing-client-cert-static
			test3-2c-mutual-ssl-client-missing-broker-cert-static
			test3-2e-ssl-with-ha-static
			test3-2e-ws-ssl-with-ha-static
			test3-3as-broker-auth-server-cert-in-client-store-single-thread-static
			test3-3am-broker-auth-server-cert-in-client-store-multi-thread-static
			test3-3b-broker-auth-client-missing-broker-cert-static
			test3-4s-broker-auth-accept-invalid-certificate-single-thread-static
			test3-4m-broker-auth-accept-invalid-certificate-multi-thread-static
		#	test3-6-psk-ssl-auth-static
			PROPERTIES TIMEOUT 540
		)
	ENDIF()
	
	IF (PAHO_BUILD_SHARED)
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
            NAME test3-2e-ssl-with-ha
            COMMAND test3 "--test_no" "12" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"   "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
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
		
		#ADD_TEST(
		#	NAME test3-6-psk-ssl-auth
		#	COMMAND test3 "--test_no" "11" "--hostname" ${MQTT_SSL_HOSTNAME}
		#)
		
		SET_TESTS_PROPERTIES(
			test3-1-ssl-conn-to-non-SSL-broker
			test3-2as-mutual-ssl-auth-single-thread
			test3-2am-mutual-ssl-auth-multi-thread
			test3-2b-mutual-ssl-broker-missing-client-cert
			test3-2c-mutual-ssl-client-missing-broker-cert
			test3-2e-ssl-with-ha
			test3-3as-broker-auth-server-cert-in-client-store-single-thread
			test3-3am-broker-auth-server-cert-in-client-store-multi-thread
			test3-3b-broker-auth-client-missing-broker-cert
			test3-4s-broker-auth-accept-invalid-certificate-single-thread
			test3-4m-broker-auth-accept-invalid-certificate-multi-thread
		#	test3-6-psk-ssl-auth
			PROPERTIES TIMEOUT 540
		)
		
		ADD_TEST(
            NAME test3-1-ws-ssl-conn-to-non-SSL-broker
            COMMAND test3 "--test_no" "1" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"  "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test3-2as-ws-mutual-ssl-auth-single-thread
            COMMAND test3 "--test_no" "2" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"  "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test3-2am-ws-mutual-ssl-auth-multi-thread
            COMMAND test3 "--test_no" "3" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"  "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test3-2b-ws-mutual-ssl-broker-missing-client-cert
            COMMAND test3 "--test_no" "4" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"  "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test3-2c-ws-mutual-ssl-client-missing-broker-cert
            COMMAND test3 "--test_no" "5" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"  "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
               
        ADD_TEST(
            NAME test3-2e-ws-ssl-with-ha
            COMMAND test3 "--test_no" "12" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"   "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test3-3as-ws-broker-auth-server-cert-in-client-store-single-thread
            COMMAND test3 "--test_no" "6" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"  "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test3-3am-ws-broker-auth-server-cert-in-client-store-multi-thread
            COMMAND test3 "--test_no" "7" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"  "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test3-3b-ws-broker-auth-client-missing-broker-cert
            COMMAND test3 "--test_no" "8" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"  "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test3-4s-ws-broker-auth-accept-invalid-certificate-single-thread
            COMMAND test3 "--test_no" "9" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"  "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test3-4m-ws-broker-auth-accept-invalid-certificate-multi-thread
            COMMAND test3 "--test_no" "10" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key" "${CERTDIR}/client.pem" "--server_key"  "${CERTDIR}/test-root-ca.crt"
        )
        
        #ADD_TEST(
        #   NAME test3-6-ws-psk-ssl-auth
        #   COMMAND test3 "--test_no" "11" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME}
        #)
        
        SET_TESTS_PROPERTIES(
            test3-1-ws-ssl-conn-to-non-SSL-broker
            test3-2as-ws-mutual-ssl-auth-single-thread
            test3-2am-ws-mutual-ssl-auth-multi-thread
            test3-2b-ws-mutual-ssl-broker-missing-client-cert
            test3-2c-ws-mutual-ssl-client-missing-broker-cert
            test3-2e-ws-ssl-with-ha
            test3-3as-ws-broker-auth-server-cert-in-client-store-single-thread
            test3-3am-ws-broker-auth-server-cert-in-client-store-multi-thread
            test3-3b-ws-broker-auth-client-missing-broker-cert
            test3-4s-ws-broker-auth-accept-invalid-certificate-single-thread
            test3-4m-ws-broker-auth-accept-invalid-certificate-multi-thread
        #   test3-6-psk-ssl-auth
            PROPERTIES TIMEOUT 540
        )
	ENDIF()

ENDIF()

IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test4-static
		test4.c
	)
	
	TARGET_LINK_LIBRARIES(
		test4-static
		paho-mqtt3a-static
	)
	
	ADD_TEST(
		NAME test4-1-basic-connect-subscribe-receive-static
		COMMAND test4-static "--test_no" "1" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test4-2-connect-timeout-static
		COMMAND test4-static "--test_no" "2" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test4-3-multiple-client-objs-simultaneous-working-static
		COMMAND test4-static "--test_no" "3" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test4-4-send-receive-big-messages-static
		COMMAND test4-static "--test_no" "4" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test4-5-connack-return-codes-static
		COMMAND test4-static "--test_no" "5" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test4-6-ha-connections-static
		COMMAND test4-static "--test_no" "6" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test4-7-pending-tokens-static
		COMMAND test4-static "--test_no" "7" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test4-8-incomplete-commands-requests-static
		COMMAND test4-static "--test_no" "8" "--connection" ${MQTT_TEST_BROKER}
	)
	
	SET_TESTS_PROPERTIES(
		test4-1-basic-connect-subscribe-receive-static
		test4-2-connect-timeout-static
		test4-3-multiple-client-objs-simultaneous-working-static
		test4-4-send-receive-big-messages-static
		test4-5-connack-return-codes-static
		test4-6-ha-connections-static
		test4-7-pending-tokens-static
		test4-8-incomplete-commands-requests-static
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_SHARED)
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
ENDIF()

IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test45-static
		test45.c
	)
	
	TARGET_LINK_LIBRARIES(
		test45-static
		paho-mqtt3a-static
	)
	
	ADD_TEST(
		NAME test45-1-basic-connect-subscribe-receive-static
		COMMAND test45-static "--test_no" "1" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-2-connect-timeout-static
		COMMAND test45-static "--test_no" "2" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-3-multiple-client-objs-simultaneous-working-static
		COMMAND test45-static "--test_no" "3" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-4-send-receive-big-messages-static
		COMMAND test45-static "--test_no" "4" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-5-connack-return-codes-static
		COMMAND test45-static "--test_no" "5" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-6-ha-connections-static
		COMMAND test45-static "--test_no" "6" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-7-pending-tokens-static
		COMMAND test45-static "--test_no" "7" "--connection" ${MQTT_TEST_BROKER} 
	)
	
	ADD_TEST(
		NAME test45-8-incomplete-commands-requests-static
		COMMAND test45-static "--test_no" "8" "--connection" ${MQTT_TEST_BROKER}
	)
	
	SET_TESTS_PROPERTIES(
		test45-1-basic-connect-subscribe-receive-static
		test45-2-connect-timeout-static
		test45-3-multiple-client-objs-simultaneous-working-static
		test45-4-send-receive-big-messages-static
		test45-5-connack-return-codes-static
		test45-6-ha-connections-static
		test45-7-pending-tokens-static
		test45-8-incomplete-commands-requests-static
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_SHARED)
	ADD_EXECUTABLE(
		test45
		test45.c
	)
	TARGET_LINK_LIBRARIES(
		test45
		paho-mqtt3a
	)

	ADD_TEST(
		NAME test45-1-basic-connect-subscribe-receive
		COMMAND test45 "--test_no" "1" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-2-connect-timeout
		COMMAND test45 "--test_no" "2" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-3-multiple-client-objs-simultaneous-working
		COMMAND test45 "--test_no" "3" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-4-send-receive-big-messages
		COMMAND test45 "--test_no" "4" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-5-connack-return-codes
		COMMAND test45 "--test_no" "5" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-6-ha-connections
		COMMAND test45 "--test_no" "6" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test45-7-pending-tokens
		COMMAND test45 "--test_no" "7" "--connection" ${MQTT_TEST_BROKER} 
	)
	
	ADD_TEST(
		NAME test45-8-incomplete-commands-requests
		COMMAND test45 "--test_no" "8" "--connection" ${MQTT_TEST_BROKER}
	)
	
	SET_TESTS_PROPERTIES(
		test45-1-basic-connect-subscribe-receive
		test45-2-connect-timeout
		test45-3-multiple-client-objs-simultaneous-working
		test45-4-send-receive-big-messages
		test45-5-connack-return-codes
		test45-6-ha-connections
		test45-7-pending-tokens
		test45-8-incomplete-commands-requests
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_WITH_SSL)
	IF (PAHO_BUILD_STATIC)
		ADD_EXECUTABLE(
			test5-static
			test5.c
		)
		
		TARGET_LINK_LIBRARIES(
			test5-static
			paho-mqtt3as-static
		)
		
		ADD_TEST(
			NAME test5-1-ssl-connection-to-no-SSL-server-static
			COMMAND test5-static "--test_no" "1" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-1-ws-ssl-connection-to-no-SSL-server-static
			COMMAND test5-static "--test_no" "1" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2a-mutual-ssl-auth-certificates-in-place-static
			COMMAND test5-static "--test_no" "2" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2a-ws-mutual-ssl-auth-certificates-in-place-static
			COMMAND test5-static "--test_no" "2" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2b-mutual-ssl-auth-broker-missing-client-cert-static
			COMMAND test5-static "--test_no" "3" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2b-ws-mutual-ssl-auth-broker-missing-client-cert-static
			COMMAND test5-static "--test_no" "3" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2c-mutual-ssl-auth-client-missing-broker-cert-static
			COMMAND test5-static "--test_no" "4" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2c-ws-mutual-ssl-auth-client-missing-broker-cert-static
			COMMAND test5-static "--test_no" "4" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2d-mutual-ssl-auth-client-missing-client-cert-static
			COMMAND test5-static "--test_no" "5" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2d-ws-mutual-ssl-auth-client-missing-client-cert-static
			COMMAND test5-static "--test_no" "5" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-3a-server-auth-server-cert-in-client-store-static
			COMMAND test5-static "--test_no" "6" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-3a-ws-server-auth-server-cert-in-client-store-static
			COMMAND test5-static "--test_no" "6" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-3b-server-auth-client-missing-broker-cert-static
			COMMAND test5-static "--test_no" "7" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-3b-ws-server-auth-client-missing-broker-cert-static
			COMMAND test5-static "--test_no" "7" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-4-accept-invalid-certificates-static
			COMMAND test5-static "--test_no" "8" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-4-ws-accept-invalid-certificates-static
			COMMAND test5-static "--test_no" "8" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-6-multiple-connections-static
			COMMAND test5-static "--test_no" "9" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-6-ws-multiple-connections-static
			COMMAND test5-static "--test_no" "9" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-7-big-messages-static
			COMMAND test5-static "--test_no" "10" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt" 
		)
		
		ADD_TEST(
			NAME test5-7-ws-big-messages-static
			COMMAND test5-static "--test_no" "10" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt" 
		)
		
		ADD_TEST(
            NAME test5-2e-ssl-with-ha-static
            COMMAND test5-static "--test_no" "14" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"   "${CERTDIR}/client.pem" "--server_key" "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test5-2e-ws-ssl-with-ha-static
            COMMAND test5-static "--test_no" "14" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"    "${CERTDIR}/client.pem" "--server_key" "${CERTDIR}/test-root-ca.crt"
        )
		
		#ADD_TEST(
		#	NAME test5-8-psk-ssl-auth-static
		#	COMMAND test5-static "--test_no" "11" "--hostname" ${MQTT_SSL_HOSTNAME}
		#)
		
		IF (NOT WIN32)
			# OpenSSL CApath tests rely on Unix-type filesystem - can it work on Windows?
			ADD_TEST(
				NAME test5-5-server-verify-with-capath-static
				COMMAND test5-static "--test_no" "12" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--capath"	"${CERTDIR}/capath"
			)
			
			ADD_TEST(
				NAME test5-5-ws-server-verify-with-capath-static
				COMMAND test5-static "--test_no" "12" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--capath"	"${CERTDIR}/capath"
			)
			
			ADD_TEST(
				NAME test5-6-server-verify-with-capath-static
				COMMAND test5-static "--test_no" "13" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--capath"	"${CERTDIR}/notexist"
			)
			
			ADD_TEST(
				NAME test5-6-ws-server-verify-with-capath-static
				COMMAND test5-static "--test_no" "13" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--capath"	"${CERTDIR}/notexist"
			)
			
			SET_TESTS_PROPERTIES(
				test5-5-server-verify-with-capath-static
				test5-5-ws-server-verify-with-capath-static
				test5-6-server-verify-with-capath-static
				test5-6-ws-server-verify-with-capath-static
				PROPERTIES TIMEOUT 540
			)
		ENDIF ()
		
		
		SET_TESTS_PROPERTIES(
			test5-1-ssl-connection-to-no-SSL-server-static
			test5-1-ws-ssl-connection-to-no-SSL-server-static
			test5-2a-mutual-ssl-auth-certificates-in-place-static
			test5-2a-ws-mutual-ssl-auth-certificates-in-place-static
			test5-2b-mutual-ssl-auth-broker-missing-client-cert-static
			test5-2b-ws-mutual-ssl-auth-broker-missing-client-cert-static
			test5-2c-mutual-ssl-auth-client-missing-broker-cert-static
			test5-2c-ws-mutual-ssl-auth-client-missing-broker-cert-static
			test5-2d-mutual-ssl-auth-client-missing-client-cert-static
			test5-2d-ws-mutual-ssl-auth-client-missing-client-cert-static
			test5-2e-ssl-with-ha-static
			test5-2e-ws-ssl-with-ha-static
			test5-3a-server-auth-server-cert-in-client-store-static
			test5-3a-ws-server-auth-server-cert-in-client-store-static
			test5-3b-server-auth-client-missing-broker-cert-static
			test5-3b-ws-server-auth-client-missing-broker-cert-static
			test5-4-accept-invalid-certificates-static
			test5-4-ws-accept-invalid-certificates-static
			test5-6-multiple-connections-static
			test5-6-ws-multiple-connections-static
			test5-7-big-messages-static
			test5-7-ws-big-messages-static
		#	test5-8-psk-ssl-auth-static
			PROPERTIES TIMEOUT 540
		)
	ENDIF()
	
	IF (PAHO_BUILD_SHARED)
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
			COMMAND test5 "--test_no" "1" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-1-ws-ssl-connection-to-no-SSL-server
			COMMAND test5 "--test_no" "1" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2a-mutual-ssl-auth-certificates-in-place
			COMMAND test5 "--test_no" "2" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2a-ws-mutual-ssl-auth-certificates-in-place
			COMMAND test5 "--test_no" "2" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2b-mutual-ssl-auth-broker-missing-client-cert
			COMMAND test5 "--test_no" "3" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2b-ws-mutual-ssl-auth-broker-missing-client-cert
			COMMAND test5 "--test_no" "3" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2c-mutual-ssl-auth-client-missing-broker-cert
			COMMAND test5 "--test_no" "4" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2c-ws-mutual-ssl-auth-client-missing-broker-cert
			COMMAND test5 "--test_no" "4" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2d-mutual-ssl-auth-client-missing-client-cert
			COMMAND test5 "--test_no" "5" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-2d-ws-mutual-ssl-auth-client-missing-client-cert
			COMMAND test5 "--test_no" "5" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-3a-server-auth-server-cert-in-client-store
			COMMAND test5 "--test_no" "6" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-3a-ws-server-auth-server-cert-in-client-store
			COMMAND test5 "--test_no" "6" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-3b-server-auth-client-missing-broker-cert
			COMMAND test5 "--test_no" "7" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-3b-ws-server-auth-client-missing-broker-cert
			COMMAND test5 "--test_no" "7" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-4-accept-invalid-certificates
			COMMAND test5 "--test_no" "8" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-4-ws-accept-invalid-certificates
			COMMAND test5 "--test_no" "8" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-6-multiple-connections
			COMMAND test5 "--test_no" "9" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-6-ws-multiple-connections
			COMMAND test5 "--test_no" "9" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt"
		)
		
		ADD_TEST(
			NAME test5-7-big-messages
			COMMAND test5 "--test_no" "10" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt" 
		)
		
		ADD_TEST(
			NAME test5-7-ws-big-messages
			COMMAND test5 "--test_no" "10" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--server_key"	"${CERTDIR}/test-root-ca.crt" 
		)
		
		ADD_TEST(
            NAME test5-2e-ssl-with-ha
            COMMAND test5 "--test_no" "14" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"   "${CERTDIR}/client.pem" "--server_key" "${CERTDIR}/test-root-ca.crt"
        )
        
        ADD_TEST(
            NAME test5-2e-ws-ssl-with-ha
            COMMAND test5 "--test_no" "14" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"    "${CERTDIR}/client.pem" "--server_key" "${CERTDIR}/test-root-ca.crt"
        )
		
		#ADD_TEST(
		#	NAME test5-8-psk-ssl-auth
		#	COMMAND test5 "--test_no" "11" "--hostname" ${MQTT_SSL_HOSTNAME}
		#)
		
		IF (NOT WIN32)
			# OpenSSL CApath tests rely on Unix-type filesystem - can it work on Windows?
			ADD_TEST(
				NAME test5-5-server-verify-with-capath
				COMMAND test5 "--test_no" "12" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--capath"	"${CERTDIR}/capath"
			)
			
			ADD_TEST(
				NAME test5-5-ws-server-verify-with-capath
				COMMAND test5 "--test_no" "12" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--capath"	"${CERTDIR}/capath"
			)
			
			ADD_TEST(
				NAME test5-6-server-verify-with-capath
				COMMAND test5 "--test_no" "13" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--capath"	"${CERTDIR}/notexist"
			)
			
			ADD_TEST(
				NAME test5-6-ws-server-verify-with-capath
				COMMAND test5 "--test_no" "13" "--ws" "--hostname" ${MQTT_SSL_HOSTNAME} "--client_key"	 "${CERTDIR}/client.pem" "--capath"	"${CERTDIR}/notexist"
			)
			
			SET_TESTS_PROPERTIES(
				test5-5-server-verify-with-capath
				test5-5-ws-server-verify-with-capath
				test5-6-server-verify-with-capath
				test5-6-ws-server-verify-with-capath
				PROPERTIES TIMEOUT 540
			)
		ENDIF ()
	
		SET_TESTS_PROPERTIES(
			test5-1-ssl-connection-to-no-SSL-server
			test5-1-ws-ssl-connection-to-no-SSL-server
			test5-2a-mutual-ssl-auth-certificates-in-place
			test5-2a-ws-mutual-ssl-auth-certificates-in-place
			test5-2b-mutual-ssl-auth-broker-missing-client-cert
			test5-2b-ws-mutual-ssl-auth-broker-missing-client-cert
			test5-2c-mutual-ssl-auth-client-missing-broker-cert
			test5-2c-ws-mutual-ssl-auth-client-missing-broker-cert
			test5-2d-mutual-ssl-auth-client-missing-client-cert
			test5-2d-ws-mutual-ssl-auth-client-missing-client-cert
			test5-2e-ssl-with-ha
			test5-2e-ws-ssl-with-ha
			test5-3a-server-auth-server-cert-in-client-store
			test5-3a-ws-server-auth-server-cert-in-client-store
			test5-3b-server-auth-client-missing-broker-cert
			test5-3b-ws-server-auth-client-missing-broker-cert
			test5-4-accept-invalid-certificates
			test5-4-ws-accept-invalid-certificates
			test5-6-multiple-connections
			test5-6-ws-multiple-connections
			test5-7-big-messages
			test5-7-ws-big-messages
		#	test5-8-psk-ssl-auth
			PROPERTIES TIMEOUT 540
		)
	
	ENDIF()
ENDIF()


IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test6-static
		test6.c
	)
	
	TARGET_LINK_LIBRARIES(
		test6-static
		paho-mqtt3a-static
	)
	
	ADD_TEST(
		NAME test6-restart-recovery-static
		COMMAND test6-static "--connection" ${MQTT_TEST_BROKER}
	)
	
	SET_TESTS_PROPERTIES(
		test6-restart-recovery-static
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_SHARED)
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
ENDIF()


IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test8-static
		test8.c
	)
	
	TARGET_LINK_LIBRARIES(
		test8-static
		paho-mqtt3a-static
	)
	
	ADD_TEST(
		NAME test8-1-basic-connect-subscribe-receive-static
		COMMAND test8-static "--test_no" "1" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test8-2-connect-timeout-static
		COMMAND test8-static "--test_no" "2" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test8-3-multiple-client-objects-simultaneous-working-static
		COMMAND test8-static "--test_no" "3" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test8-4-send-receive-big-messages-static
		COMMAND test8-static "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--size" "500000"
	)
	
	ADD_TEST(
		NAME test8-5a-all-ha-connections-out-of-service-static
		COMMAND test8-static "--test_no" "5" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test8-5b-all-ha-connections-out-of-service-except-the-last-one-static
		COMMAND test8-static "--test_no" "6" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test8-5c-all-ha-connections-out-of-service-except-the-first-one-static
		COMMAND test8-static "--test_no" "7" "--connection" ${MQTT_TEST_BROKER}
	)
	
	ADD_TEST(
		NAME test8-6-blocked-acks-static
		COMMAND test8-static "--test_no" "8" "--connection" ${MQTT_TEST_BROKER} "--size" "500000" 
	)
	
	SET_TESTS_PROPERTIES(
		test8-1-basic-connect-subscribe-receive-static
		test8-2-connect-timeout-static
		test8-3-multiple-client-objects-simultaneous-working-static
		test8-4-send-receive-big-messages-static
		test8-5a-all-ha-connections-out-of-service-static
		test8-5b-all-ha-connections-out-of-service-except-the-last-one-static
		test8-5c-all-ha-connections-out-of-service-except-the-first-one-static
		test8-6-blocked-acks-static
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_SHARED)
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
	
	ADD_TEST(
		NAME test8-6-blocked-acks
		COMMAND test8 "--test_no" "8" "--connection" ${MQTT_TEST_BROKER} "--size" "500000" 
	)
	
	SET_TESTS_PROPERTIES(
		test8-1-basic-connect-subscribe-receive
		test8-2-connect-timeout
		test8-3-multiple-client-objects-simultaneous-working
		test8-4-send-receive-big-messages
		test8-5a-all-ha-connections-out-of-service
		test8-5b-all-ha-connections-out-of-service-except-the-last-one
		test8-5c-all-ha-connections-out-of-service-except-the-first-one
		test8-6-blocked-acks
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test9-static
		test9.c
	)
	
	TARGET_LINK_LIBRARIES(
		test9-static
		paho-mqtt3a-static
	)
	
	ADD_TEST(
		NAME test9-1-offline-buffering-send-disconnected-static
		COMMAND test9-static "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test9-2-offline-buffering-send-disconnected-serverURIs-static
		COMMAND test9-static "--test_no" "2" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test9-3-offline-buffering-auto-reconnect-static
		COMMAND test9-static "--test_no" "3" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test9-4-offline-buffering-auto-reconnect-serverURIs-static
		COMMAND test9-static "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test9-5-offline-buffering-max-buffered-static
		COMMAND test9-static "--test_no" "5" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test9-6-offline-buffering-max-buffered-binary-will-static
		COMMAND test9-static "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
        NAME test9-8-offline-buffering-before-connect-static
        COMMAND test9-static "--test_no" "8" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
    )
      
    ADD_TEST(
        NAME test9-10-offline-buffering-delete-oldest-messages-static
        COMMAND test9-static "--test_no" "10" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
    )
	
	SET_TESTS_PROPERTIES(
		test9-1-offline-buffering-send-disconnected-static
		test9-2-offline-buffering-send-disconnected-serverURIs-static
		test9-3-offline-buffering-auto-reconnect-static
		test9-4-offline-buffering-auto-reconnect-serverURIs-static
		test9-5-offline-buffering-max-buffered-static
		test9-6-offline-buffering-max-buffered-binary-will-static
		test9-8-offline-buffering-before-connect-static
		test9-10-offline-buffering-delete-oldest-messages-static
		PROPERTIES TIMEOUT 540
	)
	
	 ADD_TEST(
        NAME test9-1ws-offline-buffering-send-disconnected-static
        COMMAND test9-static "--test_no" "1" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-2ws-offline-buffering-send-disconnected-serverURIs-static
        COMMAND test9-static "--test_no" "2" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY} 
    )
    
    ADD_TEST(
        NAME test9-3ws-offline-buffering-auto-reconnect-static
        COMMAND test9-static "--test_no" "3" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-4ws-offline-buffering-auto-reconnect-serverURIs-static
        COMMAND test9-static "--test_no" "4" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-5ws-offline-buffering-max-buffered-static
        COMMAND test9-static "--test_no" "5" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-6ws-offline-buffering-max-buffered-binary-will-static
        COMMAND test9-static "--test_no" "6" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-8ws-offline-buffering-before-connect-static
        COMMAND test9-static "--test_no" "8" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-10ws-offline-buffering-delete-oldest-messages-static
        COMMAND test9-static "--test_no" "10" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    SET_TESTS_PROPERTIES(
        test9-1ws-offline-buffering-send-disconnected-static
        test9-2ws-offline-buffering-send-disconnected-serverURIs-static
        test9-3ws-offline-buffering-auto-reconnect-static
        test9-4ws-offline-buffering-auto-reconnect-serverURIs-static
        test9-5ws-offline-buffering-max-buffered-static
        test9-6ws-offline-buffering-max-buffered-binary-will-static
        test9-8ws-offline-buffering-before-connect-static
        test9-10ws-offline-buffering-delete-oldest-messages-static
        PROPERTIES TIMEOUT 540
    )
ENDIF()
	
IF (PAHO_BUILD_SHARED)
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
	
	ADD_TEST(
        NAME test9-8-offline-buffering-before-connect
        COMMAND test9 "--test_no" "8" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-10-offline-buffering-delete-oldest-messages
        COMMAND test9 "--test_no" "10" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
    )
	
	SET_TESTS_PROPERTIES(
		test9-1-offline-buffering-send-disconnected
		test9-2-offline-buffering-send-disconnected-serverURIs
		test9-3-offline-buffering-auto-reconnect
		test9-4-offline-buffering-auto-reconnect-serverURIs
		test9-5-offline-buffering-max-buffered
		test9-6-offline-buffering-max-buffered-binary-will
		test9-8-offline-buffering-before-connect
		test9-10-offline-buffering-delete-oldest-messages
		PROPERTIES TIMEOUT 540
	)
	
	ADD_TEST(
        NAME test9-1-ws-offline-buffering-send-disconnected
        COMMAND test9 "--test_no" "1" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-2-ws-offline-buffering-send-disconnected-serverURIs
        COMMAND test9 "--test_no" "2" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-3-ws-offline-buffering-auto-reconnect
        COMMAND test9 "--test_no" "3" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-4-ws-offline-buffering-auto-reconnect-serverURIs
        COMMAND test9 "--test_no" "4" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-5-ws-offline-buffering-max-buffered
        COMMAND test9 "--test_no" "5" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-6-ws-offline-buffering-max-buffered-binary-will
        COMMAND test9 "--test_no" "6" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-8-ws-offline-buffering-before-connect
        COMMAND test9 "--test_no" "8" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test9-10-ws-offline-buffering-delete-oldest-messages
        COMMAND test9 "--test_no" "10" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    SET_TESTS_PROPERTIES(
        test9-1-ws-offline-buffering-send-disconnected
        test9-2-ws-offline-buffering-send-disconnected-serverURIs
        test9-3-ws-offline-buffering-auto-reconnect
        test9-4-ws-offline-buffering-auto-reconnect-serverURIs
        test9-5-ws-offline-buffering-max-buffered
        test9-6-ws-offline-buffering-max-buffered-binary-will
        test9-8-ws-offline-buffering-before-connect
        test9-10-ws-offline-buffering-delete-oldest-messages
        PROPERTIES TIMEOUT 540
    )
ENDIF()

IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test95-static
		test95.c
	)
	
	TARGET_LINK_LIBRARIES(
		test95-static
		paho-mqtt3a-static
	)
	
	ADD_TEST(
		NAME test95-1-offline-buffering-send-disconnected-static
		COMMAND test95-static "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test95-2-offline-buffering-send-disconnected-serverURIs-static
		COMMAND test95-static "--test_no" "2" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test95-3-offline-buffering-auto-reconnect-static
		COMMAND test95-static "--test_no" "3" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test95-4-offline-buffering-auto-reconnect-serverURIs-static
		COMMAND test95-static "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test95-5-offline-buffering-max-buffered-static
		COMMAND test95-static "--test_no" "5" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test95-6-offline-buffering-max-buffered-binary-will-static
		COMMAND test95-static "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	SET_TESTS_PROPERTIES(
		test95-1-offline-buffering-send-disconnected-static
		test95-2-offline-buffering-send-disconnected-serverURIs-static
		test95-3-offline-buffering-auto-reconnect-static
		test95-4-offline-buffering-auto-reconnect-serverURIs-static
		test95-5-offline-buffering-max-buffered-static
		test95-6-offline-buffering-max-buffered-binary-will-static
		PROPERTIES TIMEOUT 540
	)
	
	ADD_TEST(
        NAME test95-1ws-offline-buffering-send-disconnected-static
        COMMAND test95-static "--test_no" "1" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test95-2ws-offline-buffering-send-disconnected-serverURIs-static
        COMMAND test95-static "--test_no" "2" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test95-3ws-offline-buffering-auto-reconnect-static
        COMMAND test95-static "--test_no" "3" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test95-4ws-offline-buffering-auto-reconnect-serverURIs-static
        COMMAND test95-static "--test_no" "4" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test95-5ws-offline-buffering-max-buffered-static
        COMMAND test95-static "--test_no" "5" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test95-6ws-offline-buffering-max-buffered-binary-will-static
        COMMAND test95-static "--test_no" "6" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    SET_TESTS_PROPERTIES(
        test95-1ws-offline-buffering-send-disconnected-static
        test95-2ws-offline-buffering-send-disconnected-serverURIs-static
        test95-3ws-offline-buffering-auto-reconnect-static
        test95-4ws-offline-buffering-auto-reconnect-serverURIs-static
        test95-5ws-offline-buffering-max-buffered-static
        test95-6ws-offline-buffering-max-buffered-binary-will-static
        PROPERTIES TIMEOUT 540
    )
ENDIF()

IF (PAHO_BUILD_SHARED)
	ADD_EXECUTABLE(
		test95
		test95.c
	)
	
	TARGET_LINK_LIBRARIES(
		test95
		paho-mqtt3a
	)
	
	ADD_TEST(
		NAME test95-1-offline-buffering-send-disconnected
		COMMAND test95 "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test95-2-offline-buffering-send-disconnected-serverURIs
		COMMAND test95 "--test_no" "2" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test95-3-offline-buffering-auto-reconnect
		COMMAND test95 "--test_no" "3" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test95-4-offline-buffering-auto-reconnect-serverURIs
		COMMAND test95 "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test95-5-offline-buffering-max-buffered
		COMMAND test95 "--test_no" "5" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test95-6-offline-buffering-max-buffered-binary-will
		COMMAND test95 "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	SET_TESTS_PROPERTIES(
		test95-1-offline-buffering-send-disconnected
		test95-2-offline-buffering-send-disconnected-serverURIs
		test95-3-offline-buffering-auto-reconnect
		test95-4-offline-buffering-auto-reconnect-serverURIs
		test95-5-offline-buffering-max-buffered
		test95-6-offline-buffering-max-buffered-binary-will
		PROPERTIES TIMEOUT 540
	)
	
	ADD_TEST(
        NAME test95-1ws-offline-buffering-send-disconnected
        COMMAND test95 "--test_no" "1" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test95-2ws-offline-buffering-send-disconnected-serverURIs
        COMMAND test95 "--test_no" "2" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test95-3ws-offline-buffering-auto-reconnect
        COMMAND test95 "--test_no" "3" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test95-4ws-offline-buffering-auto-reconnect-serverURIs
        COMMAND test95 "--test_no" "4" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test95-5ws-offline-buffering-max-buffered
        COMMAND test95 "--test_no" "5" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
    
    ADD_TEST(
        NAME test95-6ws-offline-buffering-max-buffered-binary-will
        COMMAND test95 "--test_no" "6" "--connection" ${MQTT_WS_TEST_BROKER} "--proxy_connection" ${MQTT_WS_TEST_PROXY}
    )
       
    SET_TESTS_PROPERTIES(
        test95-1ws-offline-buffering-send-disconnected
        test95-2ws-offline-buffering-send-disconnected-serverURIs
        test95-3ws-offline-buffering-auto-reconnect
        test95-4ws-offline-buffering-auto-reconnect-serverURIs
        test95-5ws-offline-buffering-max-buffered
        test95-6ws-offline-buffering-max-buffered-binary-will
        PROPERTIES TIMEOUT 540
    )
ENDIF()

IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test10-static
		test10.c
	)
	
	TARGET_LINK_LIBRARIES(
		test10-static
		paho-mqtt3c-static
	)
	
	ADD_TEST(
		NAME test10-1-client_topic_aliases-static
		COMMAND "test10-static" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-2-server_topic_aliases-static
		COMMAND "test10-static" "--test_no" "2" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-3-subscription_ids-static
		COMMAND "test10-static" "--test_no" "3" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-4-flow_control-static
		COMMAND "test10-static" "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-5-error_handling-static
		COMMAND "test10-static" "--test_no" "5" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-6-qos_1_2_errors-static
		COMMAND "test10-static" "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-7-request_response-static
		COMMAND "test10-static" "--test_no" "7" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-8-subscribe_options-static
		COMMAND "test10-static" "--test_no" "8" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-9-shared_subscriptions-static
		COMMAND "test10-static" "--test_no" "9" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	SET_TESTS_PROPERTIES(
		test10-1-client_topic_aliases-static
		test10-2-server_topic_aliases-static
		test10-3-subscription_ids-static
		test10-4-flow_control-static
		test10-5-error_handling-static
		test10-6-qos_1_2_errors-static
		test10-7-request_response-static
		test10-8-subscribe_options-static
		test10-9-shared_subscriptions-static
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_SHARED)
	ADD_EXECUTABLE(
		test10
		test10.c
	)
	
	TARGET_LINK_LIBRARIES(
		test10
		paho-mqtt3c
	)

	ADD_TEST(
		NAME test10-1-client_topic_aliases
		COMMAND "test10" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-2-server_topic_aliases
		COMMAND "test10" "--test_no" "2" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-3-subscription_ids
		COMMAND "test10" "--test_no" "3" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-4-flow_control
		COMMAND "test10" "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-5-error_handling
		COMMAND "test10" "--test_no" "5" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-6-qos_1_2_errors
		COMMAND "test10" "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-7-request_response
		COMMAND "test10" "--test_no" "7" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-8-subscribe_options
		COMMAND "test10" "--test_no" "8" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test10-9-shared_subscriptions
		COMMAND "test10" "--test_no" "9" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	SET_TESTS_PROPERTIES(
		test10-1-client_topic_aliases
		test10-2-server_topic_aliases
		test10-3-subscription_ids
		test10-4-flow_control
		test10-5-error_handling
		test10-6-qos_1_2_errors
		test10-7-request_response
		test10-8-subscribe_options
		test10-9-shared_subscriptions
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test11-static
		test11.c
	)
	
	TARGET_LINK_LIBRARIES(
		test11-static
		paho-mqtt3a-static
	)
	
	ADD_TEST(
		NAME test11-1-client_topic_aliases-static
		COMMAND "test11-static" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-2-server_topic_aliases-static
		COMMAND "test11-static" "--test_no" "2" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-3-subscription_ids-static
		COMMAND "test11-static" "--test_no" "3" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-4-flow_control-static
		COMMAND "test11-static" "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-5-error_handling-static
		COMMAND "test11-static" "--test_no" "5" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-6-qos_1_2_errors-static
		COMMAND "test11-static" "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-7-request_response-static
		COMMAND "test11-static" "--test_no" "7" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-8-subscribe_options-static
		COMMAND "test11-static" "--test_no" "8" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-9-shared_subscriptions-static
		COMMAND "test11-static" "--test_no" "9" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	SET_TESTS_PROPERTIES(
		test11-1-client_topic_aliases-static
		test11-2-server_topic_aliases-static
		test11-3-subscription_ids-static
		test11-4-flow_control-static
		test11-5-error_handling-static
		test11-6-qos_1_2_errors-static
		test11-7-request_response-static
		test11-8-subscribe_options-static
		test11-9-shared_subscriptions-static
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_SHARED)
	ADD_EXECUTABLE(
		test11
		test11.c
	)
	
	TARGET_LINK_LIBRARIES(
		test11
		paho-mqtt3a
	)

	ADD_TEST(
		NAME test11-1-client_topic_aliases
		COMMAND "test11" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-2-server_topic_aliases
		COMMAND "test11" "--test_no" "2" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-3-subscription_ids
		COMMAND "test11" "--test_no" "3" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-4-flow_control
		COMMAND "test11" "--test_no" "4" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-5-error_handling
		COMMAND "test11" "--test_no" "5" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-6-qos_1_2_errors
		COMMAND "test11" "--test_no" "6" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-7-request_response
		COMMAND "test11" "--test_no" "7" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-8-subscribe_options
		COMMAND "test11" "--test_no" "8" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	ADD_TEST(
		NAME test11-9-shared_subscriptions
		COMMAND "test11" "--test_no" "9" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	SET_TESTS_PROPERTIES(
		test11-1-client_topic_aliases
		test11-2-server_topic_aliases
		test11-3-subscription_ids
		test11-4-flow_control
		test11-5-error_handling
		test11-6-qos_1_2_errors
		test11-7-request_response
		test11-8-subscribe_options
		test11-9-shared_subscriptions
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (NOT PAHO_HIGH_PERFORMANCE)
IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
		test_issue373-static
		test_issue373.c
	)
	TARGET_LINK_LIBRARIES(
		test_issue373-static
		paho-mqtt3a-static
	)
ENDIF()

IF (PAHO_BUILD_SHARED)
	ADD_EXECUTABLE(
		test_issue373
		test_issue373.c
	)
	TARGET_LINK_LIBRARIES(
		test_issue373
		paho-mqtt3a
	)
ENDIF()
ENDIF()


IF (PAHO_BUILD_STATIC)
	ADD_EXECUTABLE(
			test_sync_session_present-static
			test_sync_session_present.c
	)
	
	TARGET_LINK_LIBRARIES(
			test_sync_session_present-static
			paho-mqtt3c-static
	)
	
	ADD_TEST(
		NAME test_sync_session_present-1-client_session_present_after_reconnect-static
		COMMAND "test_sync_session_present-static" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	SET_TESTS_PROPERTIES(
		test_sync_session_present-1-client_session_present_after_reconnect-static
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_SHARED)
	ADD_EXECUTABLE(
			test_sync_session_present
			test_sync_session_present.c
	)
	
	TARGET_LINK_LIBRARIES(
			test_sync_session_present
			paho-mqtt3c
	)

	ADD_TEST(
		NAME test_sync_session_present-1-client_session_present_after_reconnect
		COMMAND "test_sync_session_present" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER} "--proxy_connection" ${MQTT_TEST_PROXY}
	)
	
	SET_TESTS_PROPERTIES(
		test_sync_session_present-1-client_session_present_after_reconnect
		PROPERTIES TIMEOUT 540
	)
ENDIF()

IF (PAHO_BUILD_STATIC)
    ADD_EXECUTABLE(
        test_connect_destroy-static
        test_connect_destroy.c
    )

   TARGET_LINK_LIBRARIES(
        test_connect_destroy-static
        paho-mqtt3c-static
    )
 
    ADD_TEST(
            NAME test_connect_destroy-1-execute-destroy-during-connect-static
            COMMAND "test_connect_destroy-static" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER}
    )
    
    SET_TESTS_PROPERTIES(
            test_connect_destroy-1-execute-destroy-during-connect-static
            PROPERTIES TIMEOUT 540
    )
ENDIF()


IF (PAHO_BUILD_SHARED)
    ADD_EXECUTABLE(
    	test_connect_destroy
    	test_connect_destroy.c
    )
    
    TARGET_LINK_LIBRARIES(
    	test_connect_destroy
    	paho-mqtt3c
    )
    
    ADD_TEST(
            NAME test_connect_destroy-1-execute-destroy-during-connect
            COMMAND "test_connect_destroy" "--test_no" "1" "--connection" ${MQTT_TEST_BROKER}
    )
    
    SET_TESTS_PROPERTIES(
            test_connect_destroy-1-execute-destroy-during-connect
            PROPERTIES TIMEOUT 540
    )
ENDIF()
