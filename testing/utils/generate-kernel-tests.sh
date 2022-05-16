#!/bin/sh

# missing esn

east_init()
{
cat <<EOF
/testing/guestbin/swan-prep --46
ipsec start
../../guestbin/wait-until-pluto-started
ipsec auto --add eastnet-westnet-ikev2
ipsec whack --impair suppress-retransmits
echo "initdone"
EOF
}

west_init()
{
    cat <<EOF
# note swan-prep does not yet support BSD
../../guestbin/netbsd-prep.sh
ipsec start
ipsec auto --add eastnet-westnet-ikev2
echo "initdone"
EOF
}

west_run()
{
    cat <<EOF
../../guestbin/ping-once.sh --down -4 -I 192.0.1.254 192.0.2.254
ipsec auto --up eastnet-westnet-ikev2
setkey -DP
../../guestbin/ping-once.sh --up -4 -I 192.0.1.254 192.0.2.254
setkey -D
../../guestbin/ping-once.sh --big --up -4 -I 192.0.1.254 192.0.2.254
setkey -D
dmesg | grep ipsec
EOF
}

ipsec_conf()
{

    type=$(expr ${mode} : '\(.*\)-.*')
    case ${type} in
	tunnel ) tc='' ;;
	transport ) tc='#' ;;
    esac
    case ${mode} in
	*in4 | *ipv4 )
	    left=192.1.2.45
	    right=192.1.2.23
	    ;;
	*in6 | *ipv6 )
	    left=2001:db8:1:2::45
	    right=2001:db8:1:2::23
	    ;;
    esac
    case ${mode} in
	*4in* | *ipv4 )
	    leftsubnet=192.0.1.0/24
	    rightsubnet=192.0.2.0/24
	    ;;
	*6in* | *ipv6 )
	    leftsubnet=2001:db8:0:1::/64
	    rightsubnet=2001:db8:0:2::/64
	    ;;
    esac
    case ${child} in
	ah ) alg=sha1 ;;
	esp ) alg=aes-sha1 ;;
    esac

    cat <<EOF
# /etc/ipsec.conf - Libreswan IPsec configuration file

version 2.0

config setup
	# put the logs in /tmp for the UMLs, so that we can operate
	# without syslogd, which seems to break on UMLs
	logfile=/tmp/pluto.log
	logtime=no
	logappend=no
	plutodebug=all
	dumpdir=/tmp

conn eastnet-westnet-ikev2
	left=${left}
	leftid="@west"
	${tc}leftsubnet=${leftsubnet}
	right=${right}
	rightid="@east"
	${tc}rightsubnet=${rightsubnet}
	authby=secret
	ike=aes-sha1
	${child}=${alg}
	replay-window=64
	type=${type}
	compress=${compress}
	phase2=${child}
EOF
}

ipsec_secrets()
{
    cat <<EOF
@east @west : PSK "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"
EOF
}

for kernel in pfkeyv2 ; do
    for mode in transport-ipv4 transport-ipv6 tunnel-4in4 tunnel-4in6 tunnel-6in4 tunnel-6in6 ; do
	for child in esp ah ; do
	    for compress in yes no ; do
		d=testing/pluto/${kernel}-${mode}-${child}-ikev2
		echo $d
		mkdir -p ${d}
		east_init > $d/01-east-init.sh
		west_init > $d/02-west-init.sh
		west_run > $d/03-west-init.sh
		ipsec_conf > $d/ipsec.conf
		ipsec_secrets > $d/ipsec.secrets
	    done
	done
    done
done
