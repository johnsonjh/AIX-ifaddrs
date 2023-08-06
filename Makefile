getifaddrs:
	rm -f libgetifaddrs.a
	gcc -shared -maix32 -O2 getifaddrs.c -o libcompat-getifaddrs.so.1 -Wl,-bmaxdata:0x80000000 -fPIC -Wl,-blibpath:/opt/freeware/lib:/usr/lib
	/usr/bin/ar -qc -X32 libcompat-getifaddrs.a libcompat-getifaddrs.so.1
	gcc -shared -maix64 -O2 getifaddrs.c -o libcompat-getifaddrs.so.1 -fPIC -Wl,-blibpath:/opt/freeware/lib:/opt/freeware/lib64::/usr/lib
	/usr/bin/ar -qc -X64 libcompat-getifaddrs.a libcompat-getifaddrs.so.1

check: getifaddrs
	gcc -maix32 test.c  -o test_32 -I. -lcompat-getifaddrs -L.
	echo "getifaddrs 32 bits"
	./test_32
	gcc -maix32 test.c  -o test_64 -I. -lcompat-getifaddrs -L.
	echo "getifaddrs 64 bits"
	./test_64
	echo "Compare with ifconfig result."
	ifconfig -a

clean:
	rm -f *.so.* *.a test_*
