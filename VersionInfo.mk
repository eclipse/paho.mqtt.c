ifndef release.version
  release.version = 1.1.0
endif

build.level = $(shell date)

generated_sources := $(local-generated-sources-dir)
SRC := $(call my-dir)/src/VersionInfo.h.in
GEN := $(generated_sources)/VersionInfo.h

SED_COMMAND = sed \
    -e "s/@CLIENT_VERSION@/${release.version}/g" \
    -e "s/@BUILD_TIMESTAMP@/${build.level}/g"

$(GEN): PRIVATE_PATH := $(call my-dir)
$(GEN): PRIVATE_CUSTOM_TOOL = $(SED_COMMAND) $< > $@
$(GEN): $(SRC)
	$(transform-generated-source)

LOCAL_GENERATED_SOURCES += $(GEN)
