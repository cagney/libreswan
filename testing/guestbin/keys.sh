#!/bin/sh

set -xe

:
: disable FIPS
:

rm -f /etc/system-fips
/testing/guestbin/fipsoff

:
: Prepare output directory
:

rm -rf /tmp/x509
mkdir /tmp/x509

:
: Copy the scripts and files /tmp/x509
:

cp /testing/x509/nss-pw		      /tmp/x509
cp /testing/x509/openssl.cnf          /tmp/x509
cp /testing/x509/dist_certs.py	      /tmp/x509
cp /testing/x509/strongswan-ec-gen.sh /tmp/x509

:
: Generate the keys in /tmp/x509
:    - much faster than in /testing
:    - avoids p9fs write bug
:

/testing/x509/generate.sh /tmp/x509

{ cd /tmp/x509 && ./dist_certs.py ; }
{ cd /tmp/x509 && ./strongswan-ec-gen.sh ; }
/testing/x509/selfsigned.sh /tmp/x509/selfsigned

for host in east west road north nic ; do
    cp -v /tmp/x509/real/mainca/${host}.all.p12  /tmp/x509/pkcs12/mainca/${host}.p12
    cp -v /tmp/x509/real/mainca/${host}.end.cert /tmp/x509/certs/${host}.crt
    cp -v /tmp/x509/real/mainca/${host}.end.key  /tmp/x509/keys/${host}.key
done

:
: copy the certs from guest to host in a tar ball to avoid 9fs bug
:

rm -f /testing/x509/kvm-keys.tar
{ cd /tmp/x509 && tar cf kvm-keys.tar */ ; }
cp /tmp/x509/kvm-keys.tar /testing/x509/kvm-keys.tar
