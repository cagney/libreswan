/testing/guestbin/swan-prep --46
ipsec start
../../guestbin/wait-until-pluto-started
ipsec whack --impair suppress-retransmits
ipsec auto --add road
echo "initdone"
