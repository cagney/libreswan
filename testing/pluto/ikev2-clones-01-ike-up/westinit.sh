/testing/guestbin/swan-prep
ipsec start
../../guestbin/wait-until-pluto-started
ipsec add westnet-eastnet
ipsec whack --impair suppress_retransmits
ipsec whack --impair revival
echo "initdone"
