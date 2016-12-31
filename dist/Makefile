VERSION=1.1.0

rpm-prep:
	mkdir -p ${HOME}/rpmbuild/SOURCES/
	tar --transform="s/\./paho-c-${VERSION}/" -cf ${HOME}/rpmbuild/SOURCES/paho-c-${VERSION}.tar.gz --exclude=./build --exclude=.git --exclude=*.bz ./ --gzip

rpm: rpm-prep
	rpmbuild -ba dist/paho-c.spec
